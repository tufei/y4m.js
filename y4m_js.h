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

#ifndef __Y4M_JS_H__
#define __Y4M_JS_H__

typedef struct y4m_js_buf_s
{
    int32_t offset;
    int32_t length;
    const uint8_t *raw_bytes;
} y4m_js_buf_t;

y4m_cb_reader_t *y4m_js_open_cb_reader(const uint8_t *bytes, int32_t length);

void y4m_js_close_cb_reader(y4m_cb_reader_t *r);

int y4m_js_decode_frame(y4m_cb_reader_t *r, y4m_stream_info_t *si, y4m_frame_info_t *fi, uint8_t *rgba);

#endif /* __Y4M_JS_H__ */
