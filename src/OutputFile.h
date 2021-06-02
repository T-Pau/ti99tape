#ifndef HAD_OUTPUT_FILE_H
#define HAD_OUTPUT_FILE_H

/*
 OutputFile.h -- write binary file.
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

#include <cstdio>
#include <string>
#include <vector>

class OutputFile {
public:
    OutputFile(const std::string &filename);
    ~OutputFile();

    void write_string(const std::string &string) { fputs(string.c_str(), f); };
    void write_8(uint8_t value) { putc(value, f); }
    void write_16(uint16_t value);
    void write_32(uint32_t value);
    void write_data(const std::vector<uint8_t> &data) { fwrite(data.data(), 1, data.size(), f); }

private:
    FILE *f;
};

#endif // HAD_OUTPUT_FILE_H
