/*
 Wav.h -- create TZX files.
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

#include "TZX.h"

#include "Exception.h"

TZX::TZX(const std::string &filename) {
    f = fopen(filename.c_str(), "wb");
    
    if (f == NULL) {
        throw Exception("can't create output file");
    }
    
    fprintf(f, "ZXTape!%c%c%c", 0x1a, 1, 20);
}


TZX::~TZX() {
    flush_data();
    fclose(f);
}


void TZX::sync(uint64_t length, uint64_t count) {
    // Pure Tone block
    fprintf(f, "%c", 0x12);
    print16(length);
    print16(count);
}


void TZX::pulse(uint64_t pulse_length) {
    data.push_back(pulse_length);
    if (data.size() == 255) {
        flush_data();
    }
}


void TZX::flush_data() {
    if (!data.empty()) {
        fprintf(f, "%c%c", 0x13, static_cast<int>(data.size()));
        for (auto value : data) {
            print16(value);
        }
        data.clear();
    }
}


void TZX::print16(uint64_t value) {
    fprintf(f, "%c%c", static_cast<int>(value & 0xff), static_cast<int>((value >> 8) & 0xff));
}
