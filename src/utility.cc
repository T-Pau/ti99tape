/*
 utility.cc -- utility functions.
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

#include "utility.h"

#include <filesystem>
#include <fstream>

std::vector<uint8_t> get_file_contents(const std::string &filename) {
    auto file = std::ifstream(filename, std::ios::binary);
    auto data = std::vector<uint8_t>();
    data.reserve(std::filesystem::file_size(filename));
    data.insert(data.end(), (std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
    
    return data;
}


size_t number_of_bits(uint64_t value) {
    size_t i = 0;
    while (value > (1 << i)) {
        i++;
    }
    
    return i;
}


void write_file(const std::string &filename, const std::vector<uint8_t> &data) {
    auto file = std::ofstream(filename, std::ios::binary);
    file.write(reinterpret_cast<const char *>(data.data()), data.size());
}
