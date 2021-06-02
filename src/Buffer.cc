/*
 Buffer.cc -- parsing of bounded data.
 Copyright (C) 2021 Dieter Baron
 
 This file is part of ti99tape, a utility to create TZX files
 for TI 99/4A tape images.
 The authors can be contacted at <ti99tape@tpau.group>
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:
 1. Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 2. The names of the authors may not be used to endorse or promote
 products derived from this software without specific prior
 written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
 OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Buffer.h"

#include "Exception.h"

Buffer::Buffer(const std::vector<uint8_t> &data_, size_t start, size_t length) : data(data_), start_position(start), end_position(start + length), current_position(start) {
    if (end_position > data.size()) {
        throw Exception("buffer overrun");
    }
}


Buffer Buffer::get_buffer(size_t length) {
    ensure_bytes(length);
    auto val = Buffer(data, current_position, length);
    skip_unchecked(length);
    return val;
}


int8_t Buffer::get_int8() {
    ensure_bytes(1);
    int8_t val = data[current_position];
    skip_unchecked(1);
    return val;
}


int16_t Buffer::get_int16() {
    ensure_bytes(2);
    int16_t val = data[current_position] | (data[current_position + 1] << 8);
    skip_unchecked(2);
    return val;
}


int32_t Buffer::get_int32() {
    ensure_bytes(4);
    int32_t val = data[current_position] | (data[current_position + 1] << 8) | (data[current_position + 2] << 16) | (data[current_position + 3] << 24);
    skip_unchecked(4);
    return val;
}


int64_t Buffer::get_int64() {
    ensure_bytes(8);
    int64_t val = data[current_position] | (data[current_position + 1] << 8) | (data[current_position + 2] << 16) | (data[current_position + 3] << 24) | (static_cast<int64_t>(data[current_position + 1]) << 32) | (static_cast<int64_t>(data[current_position + 2]) << 40) | (static_cast<int64_t>(data[current_position + 3]) << 48) | (static_cast<int64_t>(data[current_position + 1]) << 56);
    skip_unchecked(8);
    return val;
}


std::string Buffer::get_string(size_t length) {
    ensure_bytes(length);
    auto val = std::string(data.begin() + current_position, data.begin() + current_position + length);
    skip_unchecked(length);
    return val;
}


uint8_t Buffer::get_uint8() {
    ensure_bytes(1);
    uint8_t val = data[current_position];
    skip_unchecked(1);
    return val;
}


uint16_t Buffer::get_uint16() {
    ensure_bytes(2);
    uint16_t val = data[current_position] | (data[current_position + 1] << 8);
    skip_unchecked(2);
    return val;
}


uint32_t Buffer::get_uint32() {
    ensure_bytes(4);
    uint32_t val = data[current_position] | (data[current_position + 1] << 8) | (data[current_position + 2] << 16) | (data[current_position + 3] << 24);
    skip_unchecked(4);
    return val;
}


uint64_t Buffer::get_uint64() {
    ensure_bytes(8);
    uint64_t val = data[current_position] | (data[current_position + 1] << 8) | (data[current_position + 2] << 16) | (data[current_position + 3] << 24) | (static_cast<uint64_t>(data[current_position + 1]) << 32) | (static_cast<uint64_t>(data[current_position + 2]) << 40) | (static_cast<uint64_t>(data[current_position + 3]) << 48) | (static_cast<uint64_t>(data[current_position + 1]) << 56);
    skip_unchecked(8);
    return val;
}


void Buffer::ensure_bytes(size_t bytes) const {
    if (current_position + bytes > end_position) {
        throw Exception("buffer overrun");
    }
}
