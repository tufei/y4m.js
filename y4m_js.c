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

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "yuv4mpeg.h"
#include "colorspace.h"
#include "subsample.h"
#include "y4m_js.h"

static ssize_t y4m_js_buf_read(void *data, void *buf, size_t len)
{
    y4m_js_buf_t *jb = (y4m_js_buf_t *)data;
    size_t rem_buf_len = jb->length - jb->offset;

    if(rem_buf_len >= len) {
        memcpy(buf, &jb->raw_bytes[jb->offset], len);
        jb->offset += len;

        return 0;
    } else {
        return len - rem_buf_len;
    }
}

y4m_cb_reader_t *y4m_js_open_cb_reader(const uint8_t *bytes, int32_t length)
{
    y4m_cb_reader_t *r = NULL;
    y4m_js_buf_t *jb = NULL;

    if(NULL == (r = (y4m_cb_reader_t *)malloc(sizeof(y4m_cb_reader_t)))) {
        goto reader_error;
    }

    if(NULL == (jb = malloc(sizeof(y4m_js_buf_t))))
    {
        goto buffer_error;
    }

    jb->offset = 0;
    jb->length = length;
    jb->raw_bytes = bytes;

    r->data = jb;
    r->read = y4m_js_buf_read;

    return r;

buffer_error:
    free(r);
reader_error:
    return NULL;
}

void y4m_js_close_cb_reader(y4m_cb_reader_t *r)
{
    if(r) {
        if(r->data) {
            free(r->data);
        }
        free(r);
    }
}

int y4m_js_decode_frame(y4m_cb_reader_t *r, y4m_stream_info_t *si, y4m_frame_info_t *fi, uint8_t *rgba)
{
    int i, j;
    uint8_t *ycbcr[3];
    int32_t plane_length[3];
    int32_t width;
    int32_t height;
    int32_t ret_value;

    if(NULL == r || NULL == si || NULL == fi || NULL == rgba) {
        return Y4M_UNKNOWN;
    }

    if(3 != y4m_si_get_plane_count(si)) {
        return Y4M_UNKNOWN;
    }

    width = y4m_si_get_plane_width(si, 0);
    height = y4m_si_get_plane_height(si, 0);

    for(i = 0; i < 3; ++i) {
        plane_length[i] = y4m_si_get_plane_length(si, i);
        if(NULL == (ycbcr[i] = malloc(width * height))) {
            return Y4M_UNKNOWN;
        }
    }

    ret_value = y4m_read_frame_cb(r, si, fi, ycbcr);
    if(Y4M_OK != ret_value) {
        return ret_value;
    }

    chroma_supersample(Y4M_CHROMA_420JPEG, ycbcr, width, height);
    convert_YCbCr_to_RGB(ycbcr, width * height);

    for(i = 0; i < width * height; ++i) {
        for(j = 0; j < 3; ++j) {
            rgba[i * 4 + j] = ycbcr[j][i];
        }
    }

    return Y4M_OK;
}

