/*
 * Y4M Javascript decoder
 *
 * Copyright (c) 2015 Yufei Yuan
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
window['Y4MDecoder'] = function(ctx) {
    this.ctx = ctx;
    this['imageData'] = null;
    this['onload'] = null;
    this['frames'] = null;
    this['loop_count'] = 0;
}

window['Y4MDecoder'].prototype = {

malloc: Module['cwrap']('malloc', 'number', [ 'number' ]),

free: Module['cwrap']('free', 'void', [ 'number' ]),

y4m_js_open_cb_reader: Module['cwrap']('y4m_js_open_cb_reader', 'number', [ 'array', 'number' ]),

y4m_js_close_cb_reader: Module['cwrap']('y4m_js_close_cb_reader', 'void', [ 'number' ]),

y4m_js_decode_frame: Module['cwrap']('y4m_js_decode_frame', 'number', [ 'number', 'number', 'number', 'number' ]),

y4m_accept_extensions: Module['cwrap']('y4m_accept_extensions', 'number', [ 'number' ]),

y4m_init_stream_info: Module['cwrap']('y4m_init_stream_info', 'void', [ 'number' ]),

y4m_init_frame_info: Module['cwrap']('y4m_init_frame_info', 'void', [ 'number' ]),

y4m_read_stream_header_cb: Module['cwrap']('y4m_read_stream_header_cb', 'number', [ 'number', 'number' ]),

y4m_fini_stream_info: Module['cwrap']('y4m_fini_stream_info', 'void', [ 'number' ]),

y4m_fini_frame_info: Module['cwrap']('y4m_fini_frame_info', 'void', [ 'number' ]),

load: function(url)
{
    var request = new XMLHttpRequest();
    var this1 = this;

    request.open("get", url, true);
    request.responseType = "arraybuffer";
    request.onload = function(event) {
        this1._onload(request, event);
    };
    request.send();
},

_onload: function(request, event)
{
    var data = request.response;
    var array = new Uint8Array(data);
    var reader, si, fi, w, h, cimg, frame_count, rgba, stat;
    var heap8, heap16, heap32, dst, i, p, duration, frames;
    var offset;

    //console.log("loaded " + data.byteLength + " bytes");

    reader = this.y4m_js_open_cb_reader(array, array.length);

    this.y4m_accept_extensions(1);

    si = this.malloc((9 + 32) * 4);
    this.y4m_init_stream_info(si);

    fi = this.malloc((4 + 32) * 4);
    this.y4m_init_frame_info(fi);

    if (this.y4m_read_stream_header_cb(reader, si) != 0) {
        console.log("could not decode file");
        return;
    }

    /* extract the image info */
    heap8 = Module['HEAPU8'];
    heap16 = Module['HEAPU16'];
    heap32 = Module['HEAPU32'];

    w = heap32[si >> 2];
    h = heap32[(si + 4) >> 2];
    duration = (heap32[(si + 16) >> 2] * 1000) / heap32[(si + 12) >> 2];

    console.log("image: w=" + w + " h=" + h + " t=" + duration);

    frame_count = 0;
    frames = [];
    rgba = this.malloc(w * h * 4);

    for (;;) {
        /* select RGBA32 output */
        cimg = this.ctx.createImageData(w, h);
        dst = cimg.data;

        stat = this.y4m_js_decode_frame(reader, si, fi, rgba);
        if (stat != 0) {
            console.log("cannot decode image: " + stat);
            break;
        }

        for (p = 0; p < w * h; p = (p + 1) | 0) {
            for (i = 0; i < 3; i = (i + 1) | 0) {
                dst[(p << 2) + i] = heap8[(rgba + (p << 2) + i) | 0] | 0;
            }
            dst[(p << 2) + 3] = 255;
        }
        frames[frame_count++] = { 'img': cimg, 'duration': duration };
    }

    console.log('decoded ' + frame_count + ' images');

    this.free(rgba);

    this.y4m_fini_frame_info(fi);
    this.free(fi);

    this.y4m_fini_stream_info(si);
    this.free(si);

    this.y4m_js_close_cb_reader(reader);

    this['frames'] = frames;
    this['imageData'] = frames[0]['img'];

    if (this['onload']) {
        this['onload']();
    }
}

};

window.onload = function() {
    var i, n, el, tab, tab1, url, dec, canvas, id, style, ctx, dw, dh;

    /* put all images to load in a separate array */
    tab = document.images;
    n = tab.length;
    tab1 = [];
    for (i = 0; i < n; i++) {
        el = tab[i];
        url = el.src;
        if (url.substr(-4, 4).toLowerCase() == ".y4m") {
            tab1[tab1.length] = el;
        }
    }

    /* change the tags to canvas */
    n = tab1.length;
    for (i = 0; i < n; i++) {
        el = tab1[i];
        url = el.src;
        canvas = document.createElement("canvas");

        if (el.id) {
            canvas.id = el.id;
        }
        if (el.className) {
            canvas.className = el.className;
        }

        /* handle simple attribute cases to resize the canvas */
        dw = el.getAttribute("width") | 0;
        if (dw) {
            canvas.style.width = dw + "px";
        }
        dh = el.getAttribute("height") | 0;
        if (dh) {
            canvas.style.height = dh + "px";
        }

        el.parentNode.replaceChild(canvas, el);

        ctx = canvas.getContext("2d");
        dec = new Y4MDecoder(ctx);
        dec.onload = (function(canvas, ctx) {
            var dec = this;
            var frames = this['frames'];
            var imageData = frames[0]['img'];
            function next_frame() {
                var frame_index = dec.frame_index;

                /* compute next frame index */
                if (++frame_index >= frames.length) {
                    if (dec['loop_count'] == 0 ||
                        dec.loop_counter < dec['loop_count']) {
                        frame_index = 0;
                        dec.loop_counter++;
                    } else {
                        frame_index = -1;
                    }
                }
                if (frame_index >= 0) {
                    dec.frame_index = frame_index;
                    ctx.putImageData(frames[frame_index]['img'], 0, 0);
                    setTimeout(next_frame, frames[frame_index]['duration']);
                }
            };

            /* resize the canvas to the image size */
            canvas.width = imageData.width;
            canvas.height = imageData.height;

            /* draw the image */
            ctx.putImageData(imageData, 0, 0);

            /* if it is an animation, add a timer to display the next frame */
            if (frames.length > 1) {
                dec.frame_index = 0;
                dec.loop_counter = 0;
                setTimeout(next_frame, frames[0]['duration']);
            }
        }).bind(dec, canvas, ctx);
        dec.load(url);
    }
};

