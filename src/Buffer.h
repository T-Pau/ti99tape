#ifndef HAD_BUFFER_H
#define HAD_BUFFER_H

/*
 Buffer.h -- parsing of bounded data.
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

#include <vector>

class Buffer {
public:
    Buffer(const std::vector<uint8_t> &data_) : data(data_), start_position(0), end_position(data_.size()), current_position(0) { }
    Buffer(const std::vector<uint8_t> &data, size_t start, size_t length);

    bool at_end() const { return current_position == end_position; }
    
    Buffer get_buffer(size_t length);

    int8_t get_int8();
    int16_t get_int16();
    int32_t get_int32();
    int64_t get_int64();
    
    std::string get_string(size_t length);

    uint8_t get_uint8();
    uint16_t get_uint16();
    uint32_t get_uint32();
    uint64_t get_uint64();
    
    void skip(size_t bytes) { ensure_bytes(bytes); skip_unchecked(bytes); }
    
private:
    const std::vector<uint8_t> &data;
    size_t start_position;
    size_t end_position;
    size_t current_position;
    
    void ensure_bytes(size_t bytes) const;
    void skip_unchecked(size_t bytes) { current_position += bytes; }
};

#endif // HAD_BUFFER_H
