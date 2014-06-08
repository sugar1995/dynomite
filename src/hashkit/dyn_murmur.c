/*
 * Dynomite - A thin, distributed replication layer for multi non-distributed storages.
 * Copyright (C) 2014 Netflix, Inc.
 */ 

/*
 * twemproxy - A fast and lightweight proxy for memcached protocol.
 * Copyright (C) 2011 Twitter, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * "Murmur" hash provided by Austin, tanjent@gmail.com
 * http://murmurhash.googlepages.com/
 *
 * Note - This code makes a few assumptions about how your machine behaves -
 *
 * 1. We can read a 4-byte value from any address without crashing
 * 2. sizeof(int) == 4
 *
 * And it has a few limitations -
 * 1. It will not work incrementally.
 * 2. It will not produce the same results on little-endian and big-endian
 *  machines.
 *
 *  Updated to murmur2 hash - BP
 */

#include <dyn_core.h>
#include <dyn_token.h>

rstatus_t 
hash_murmur(const char *key, size_t length, struct dyn_token *token)
{
    /*
     * 'm' and 'r' are mixing constants generated offline.  They're not
     * really 'magic', they just happen to work well.
     */

    const unsigned int m = 0x5bd1e995;
    const uint32_t seed = (0xdeadbeef * (uint32_t)length);
    const int r = 24;


    /* Initialize the hash to a 'random' value */

    uint32_t h = seed ^ (uint32_t)length;

    /* Mix 4 bytes at a time into the hash */

    const unsigned char * data = (const unsigned char *)key;

    while (length >= 4) {
        unsigned int k = *(unsigned int *)data;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        length -= 4;
    }

    /* Handle the last few bytes of the input array */

    switch(length) {
    case 3:
        h ^= ((uint32_t)data[2]) << 16;

    case 2:
        h ^= ((uint32_t)data[1]) << 8;

    case 1:
        h ^= data[0];
        h *= m;

    default:
        break;
    };

    /*
     * Do a few final mixes of the hash to ensure the last few bytes are
     * well-incorporated.
     */

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    size_dyn_token(token, 1);
    set_int_dyn_token(token, h);

    return DN_OK;
}
