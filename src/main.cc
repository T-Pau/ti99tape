/*
 main.cc -- main program
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

#include <filesystem>
#include <fstream>

#include "Exception.h"
#include "Pulses.h"
#include "TZX.h"
#include "Wav.h"

#define T_LENGTH 3500000

static void convert_titape(const std::string &infile, const std::string &outfile);
static void convert_wav(const std::string &infile, const std::string &outfile);

int main(int argc, const char * argv[]) {
    if (argc != 3) {
        fprintf(stderr, "%s: infile outfile\n", argv[0]);
        exit(1);
    }
    
    std::string infile = argv[1];
    std::string outfile = argv[2];
    
    try {
        std::string extension = std::filesystem::path(infile).extension();
        if (strcasecmp(extension.c_str(), ".wav") == 0) {
            convert_wav(infile, outfile);
        }
        else if (strcasecmp(extension.c_str(), ".titape") == 0) {
            convert_titape(infile, outfile);
        }
        else {
            throw Exception("unknown extension '" + extension + "'");
        }
    }
    catch (std::exception &e) {
        fprintf(stderr, "ERROR: %s\n", e.what());
    }
}


void convert_wav(const std::string &infile, const std::string &outfile) {
    auto wav = Wav(infile, Wav::LEFT);
    auto pulses = Pulses(wav);
    auto tzx = TZX(outfile);

    auto in_sync = true;
    uint64_t sync_length = 0;
    uint64_t sync_count = 0;
    uint64_t zero_length = 0;
    auto data = std::vector<uint64_t>();
    
    for (auto pulse : pulses) {
        switch (pulse.type) {
            case Pulses::SILENCE:
                // TODO: handle in middle of file
                break;
                
            case Pulses::POSITIVE:
            case Pulses::NEGATIVE:
                if (in_sync) {
                    if (sync_count < 10 || pulse.duration >= zero_length * 3 / 4) {
                        sync_length += pulse.duration;
                        sync_count += 1;
                        zero_length = sync_length / sync_count;
                        break;
                    }
                    else {
                        tzx.sync(zero_length * T_LENGTH / wav.sample_rate, sync_count);
                        in_sync = false;
                    }
                }
                
                tzx.pulse(pulse.duration * T_LENGTH / wav.sample_rate);
                break;
                
            case Pulses::END:
                break;
        }
    }
}

#define PULSE_ZERO 2539
#define PULSE_ONE 1269
#define SYNC_PULSES (768 * 8)

void add_byte(TZX &tzx, uint8_t byte) {
    for (auto i = 0; i < 8; i++) {
        if (byte & (1 << (7-i))) {
            tzx.pulse(PULSE_ONE);
            tzx.pulse(PULSE_ONE);
        }
        else {
            tzx.pulse(PULSE_ZERO);
        }
    }
}

void add_block(TZX &tzx, std::vector<uint8_t>::iterator start, std::vector<uint8_t>::iterator end) {
    auto checksum = 0;
    
    for (auto i = 0; i < 8; i++) {
        add_byte(tzx, 0);
    }
    add_byte(tzx, 0xff);
    for (auto i = 0; i < 64; i++) {
        uint8_t byte = 0;
        if (start < end) {
            byte = *start;
            start++;
        }

        add_byte(tzx, byte);
        checksum = (checksum + byte) & 0xff;
    }
    add_byte(tzx, checksum);
}

void convert_titape(const std::string &infile, const std::string &outfile) {
    auto file = std::ifstream(infile, std::ios::binary);
    auto data = std::vector<uint8_t>((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
    auto tzx = TZX(outfile);
    
    tzx.sync(PULSE_ZERO, SYNC_PULSES);
    
    auto num_blocks = ((data.size() - 20) + 63) / 64;
    
    if (num_blocks > 255) {
        throw Exception("file too long");
    }
    
    add_byte(tzx, 0xff);
    add_byte(tzx, num_blocks);
    add_byte(tzx, num_blocks);
    
    for (auto i = 0; i < num_blocks; i++) {
        auto offset = 20 + i * 64;
        auto start = data.begin() + offset;
        auto end = offset > data.size() ? data.end() : start + 64;
        
        add_block(tzx, start, end);
        add_block(tzx, start, end);
    }
}
