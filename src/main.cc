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

#include "Pulses.h"
#include "Wav.h"

static void output_data(const std::vector<uint64_t> &data);
static void output_header();
static void output_sync(uint64_t length, uint64_t count);

#define T_LENGTH 3500000

int main(int argc, const char * argv[]) {
    try {
        auto wav = Wav(argv[1], Wav::LEFT);
        auto pulses = Pulses(wav);

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
                            output_header();
                            output_sync(zero_length * T_LENGTH / wav.sample_rate, sync_count);
                            in_sync = false;
                        }
                    }
                    
                    data.push_back(pulse.duration * T_LENGTH / wav.sample_rate);
                    if (data.size() == 255) {
                        output_data(data);
                        data.clear();
                    }
                    break;
                    
                case Pulses::END:
                    break;
            }
        }
        
        if (!data.empty()) {
            output_data(data);
        }
    }
    catch (std::exception &e) {
        fprintf(stderr, "ERROR: %s\n", e.what());
    }
}

static void output_header() {
    printf("ZXTape!%c%c%c", 0x1a, 1, 20);
}


static void print16(uint64_t value) {
    printf("%c%c", static_cast<int>(value & 0xff), static_cast<int>((value >> 8) & 0xff));
}

static void output_sync(uint64_t length, uint64_t count) {
    // Pure Tone block
    printf("%c", 0x12);
    print16(length);
    print16(count);
}


static void output_data(const std::vector<uint64_t> &data) {
    printf("%c%c", 0x13, static_cast<int>(data.size()));
    for (auto value : data) {
        print16(value);
    }
}
