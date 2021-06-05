/*
 Wav.cc -- read WAV files with PCM audio.
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

#include "Wav.h"

#include "Buffer.h"
#include "Exception.h"

Wav::Wav(const std::vector<uint8_t> &data, Mixdown mixdown) {
    auto buffer = Buffer(data);
    
    auto magic = buffer.get_string(4);
    if (magic != "RIFF") {
        printf("magic: %s\n", magic.c_str());
        throw Exception("not a WAV file");
    }
    
    auto size = buffer.get_uint32();
    
    magic = buffer.get_string(4);
    if (magic != "WAVE") {
        printf("format: %s\n", magic.c_str());
        throw Exception("not a WAV file");
    }
    
    auto chunks = buffer.get_buffer(size - 4);
    
    uint16_t channels = 0;
    uint16_t sample_size = 0;
    
    while (!chunks.at_end()) {
        auto id = chunks.get_string(4);
        auto chunk_size = chunks.get_uint32();
        auto chunk_data = chunks.get_buffer(chunk_size);
        
        if (id == "fmt ") {
            if (chunk_data.get_uint16() != 1) {
                throw Exception("not a PCM WAV file");
            }
            channels = chunk_data.get_uint16();
            sample_rate = chunk_data.get_uint32();
            chunk_data.skip(6); // byte rate and block align
            sample_size = chunk_data.get_uint16();
            
            if (sample_size != 8 && sample_size != 16) {
                throw Exception("unsupported sample size");
            }
            sample_size /= 8;
            if (channels != 1 && channels != 2) {
                throw Exception("unsupported number of channels");
            }
        }
        else if (id == "data") {
            if (channels == 0) {
                throw Exception("missing fmt chunk");
            }
            
            auto num_samples = chunk_size / (channels * sample_size);
            
            if (channels == 1) {
                mixdown = LEFT;
            }
            peek = 0;
            samples.resize(num_samples);
            
            for (size_t i = 0; i < num_samples; i++) {
                switch (mixdown) {
                    case RIGHT:
                        if (channels == 2) {
                            chunk_data.skip(sample_size);
                        }
                        // fallthrough
                    case LEFT:
                        switch (sample_size) {
                            case 1: {
                                uint8_t sample8 = chunk_data.get_uint8();
                                samples[i] = (static_cast<int16_t>(sample8) * 0x101) - 0x8000;
                                break;
                            }
                            case 2:
                                samples[i] = chunk_data.get_int16();
                                break;
                        }
                        if (mixdown == LEFT && channels == 2) {
                            chunk_data.skip(sample_size);
                        }
                        break;
                        
                    case BOTH:
                        switch (sample_size) {
                            case 1:
                                samples[i] = (static_cast<int16_t>(chunk_data.get_uint8()) + static_cast<int16_t>(chunk_data.get_uint8()) * 0x80) - 0x8000;
                                break;
                            case 2:
                                samples[i] = (chunk_data.get_int16() + chunk_data.get_int16()) / 2;
                                break;
                        }
                        break;
                }
                
                if (abs(samples[i]) > peek) {
                    peek = abs(samples[i]);
                }
            }
        }
    }
}
