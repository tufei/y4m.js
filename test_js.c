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
#include "y4m_js.h"

#define INPUT_PATH          "html/suzie_qcif.y4m"
#define OUTPUT_PATH         "suzie_qcif.rgba"

int main(void)
{
    y4m_cb_reader_t *reader;
    y4m_stream_info_t si;
    y4m_frame_info_t fi;
    FILE *input = fopen(INPUT_PATH, "rb");
    int32_t file_len = 0;
    uint8_t *buffer = NULL;
    uint8_t *rgba = NULL;
    int32_t width;
    int32_t height;
    int32_t decoded_frames = 0;
    int ret_value;

    fseek(input, 0, SEEK_END);
    file_len = ftell(input);
    rewind(input);

    buffer = malloc(file_len);
    fread(buffer, sizeof(uint8_t), file_len, input);
    fclose(input);

    reader = y4m_js_open_cb_reader(buffer, file_len);

    y4m_accept_extensions(1);
    y4m_init_stream_info(&si);
    y4m_init_frame_info(&fi);

    y4m_read_stream_header_cb(reader, &si);
    width = y4m_si_get_width(&si);
    height = y4m_si_get_height(&si);
    rgba = calloc(width * height * 4, sizeof(uint8_t));

    while(1) {
        ret_value = y4m_js_decode_frame(reader, &si, &fi, rgba);
        if(Y4M_OK != ret_value) {
            printf("return value: %d\n", ret_value);
            break;
        }
        decoded_frames += 1;
    }

    printf("decoded %d frames\n", decoded_frames);

    y4m_fini_frame_info(&fi);
    y4m_fini_stream_info(&si);

    free(buffer);
    y4m_js_close_cb_reader(reader);

    free(rgba);

    return 0;
}

