/*
 * Copyright (c) 2015-2018 Nexenta Systems, inc.
 *
 * This file is part of EdgeFS Project
 * (see https://github.com/Nexenta/edgefs).
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */
/*
 * Copyright (c) 2014 DeNA Co., Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 * lib/file/templates.c.h is automatically generated from lib/file/_templates.h
 * with command:
 *   picotemplate.pl --conf=misc/picotemplate-conf.pl lib/file/_templates.c.h
 */

#include <limits.h>

static int cmpstrptr(const void *_x, const void *_y)
{
    const char *x = *(const char **)_x;
    const char *y = *(const char **)_y;
    return strcmp(x, y);
}

#if !defined(NAME_MAX) || defined(__linux__)
/* readdir(3) is known to be thread-safe on Linux and should be thread-safe on a platform that does not have a predefined value for
   NAME_MAX */
#define FOREACH_DIRENT(dp, dent)                                                                                                   \
    struct dirent *dent;                                                                                                           \
    while ((dent = readdir(dp)) != NULL)
#else
#define FOREACH_DIRENT(dp, dent)                                                                                                   \
    struct {                                                                                                                       \
        struct dirent d;                                                                                                           \
        char s[NAME_MAX + 1];                                                                                                      \
    } dent_;                                                                                                                       \
    struct dirent *dentp, *dent = &dent_.d;                                                                                        \
    int ret;                                                                                                                       \
    while ((ret = readdir_r(dp, dent, &dentp)) == 0 && dentp != NULL)
#endif /* FOREACH_DIRENT */

static h2o_buffer_t *build_dir_listing_html(h2o_mem_pool_t *pool, h2o_iovec_t path_normalized, DIR *dp)
{
    H2O_VECTOR(char *) files = {NULL};

    { /* build list of files */
        FOREACH_DIRENT(dp, dent)
        {
            if (strcmp(dent->d_name, ".") == 0 || strcmp(dent->d_name, "..") == 0)
                continue;
            h2o_vector_reserve(pool, &files, files.size + 1);
            files.entries[files.size++] = h2o_strdup(pool, dent->d_name, SIZE_MAX).base;
        }
        if (files.size > 1)
            qsort(files.entries, files.size, sizeof(files.entries[0]), cmpstrptr);
    }

    h2o_buffer_t *_;
    h2o_iovec_t path_normalized_escaped = h2o_htmlescape(pool, path_normalized.base, path_normalized.len);

    h2o_buffer_init(&_, &h2o_socket_buffer_prototype);

    {
        h2o_iovec_t _s = (h2o_iovec_init(H2O_STRLIT("<!DOCTYPE html>\n<TITLE>Index of ")));
        if (_s.len != 0 && _s.base[_s.len - 1] == '\n')
            --_s.len;
        h2o_buffer_reserve(&_, _s.len);
        memcpy(_->bytes + _->size, _s.base, _s.len);
        _->size += _s.len;
    }
    {
        h2o_iovec_t _s = (path_normalized_escaped);
        if (_s.len != 0 && _s.base[_s.len - 1] == '\n')
            --_s.len;
        h2o_buffer_reserve(&_, _s.len);
        memcpy(_->bytes + _->size, _s.base, _s.len);
        _->size += _s.len;
    }
    {
        h2o_iovec_t _s = (h2o_iovec_init(H2O_STRLIT("</TITLE>\n<H2>Index of ")));
        if (_s.len != 0 && _s.base[_s.len - 1] == '\n')
            --_s.len;
        h2o_buffer_reserve(&_, _s.len);
        memcpy(_->bytes + _->size, _s.base, _s.len);
        _->size += _s.len;
    }
    {
        h2o_iovec_t _s = (path_normalized_escaped);
        if (_s.len != 0 && _s.base[_s.len - 1] == '\n')
            --_s.len;
        h2o_buffer_reserve(&_, _s.len);
        memcpy(_->bytes + _->size, _s.base, _s.len);
        _->size += _s.len;
    }
    {
        h2o_iovec_t _s = (h2o_iovec_init(H2O_STRLIT("</H2>\n<UL>\n<LI><A HREF=\"..\">Parent Directory</A>\n")));
        if (_s.len != 0 && _s.base[_s.len - 1] == '\n')
            --_s.len;
        h2o_buffer_reserve(&_, _s.len);
        memcpy(_->bytes + _->size, _s.base, _s.len);
        _->size += _s.len;
    }

    size_t i;
    for (i = 0; i != files.size; ++i) {
        h2o_iovec_t link_escaped = h2o_uri_escape(pool, files.entries[i], strlen(files.entries[i]), NULL);
        link_escaped = h2o_htmlescape(pool, link_escaped.base, link_escaped.len);
        h2o_iovec_t label_escaped = h2o_htmlescape(pool, files.entries[i], strlen(files.entries[i]));
        {
            h2o_iovec_t _s = (h2o_iovec_init(H2O_STRLIT("<LI><A HREF=\"")));
            if (_s.len != 0 && _s.base[_s.len - 1] == '\n')
                --_s.len;
            h2o_buffer_reserve(&_, _s.len);
            memcpy(_->bytes + _->size, _s.base, _s.len);
            _->size += _s.len;
        }
        {
            h2o_iovec_t _s = (link_escaped);
            if (_s.len != 0 && _s.base[_s.len - 1] == '\n')
                --_s.len;
            h2o_buffer_reserve(&_, _s.len);
            memcpy(_->bytes + _->size, _s.base, _s.len);
            _->size += _s.len;
        }
        {
            h2o_iovec_t _s = (h2o_iovec_init(H2O_STRLIT("\">")));
            if (_s.len != 0 && _s.base[_s.len - 1] == '\n')
                --_s.len;
            h2o_buffer_reserve(&_, _s.len);
            memcpy(_->bytes + _->size, _s.base, _s.len);
            _->size += _s.len;
        }
        {
            h2o_iovec_t _s = (label_escaped);
            if (_s.len != 0 && _s.base[_s.len - 1] == '\n')
                --_s.len;
            h2o_buffer_reserve(&_, _s.len);
            memcpy(_->bytes + _->size, _s.base, _s.len);
            _->size += _s.len;
        }
        {
            h2o_iovec_t _s = (h2o_iovec_init(H2O_STRLIT("</A>\n")));
            if (_s.len != 0 && _s.base[_s.len - 1] == '\n')
                --_s.len;
            h2o_buffer_reserve(&_, _s.len);
            memcpy(_->bytes + _->size, _s.base, _s.len);
            _->size += _s.len;
        }
    }
    {
        h2o_iovec_t _s = (h2o_iovec_init(H2O_STRLIT("</UL>\n")));
        if (_s.len != 0 && _s.base[_s.len - 1] == '\n')
            --_s.len;
        h2o_buffer_reserve(&_, _s.len);
        memcpy(_->bytes + _->size, _s.base, _s.len);
        _->size += _s.len;
    }

    return _;
}
