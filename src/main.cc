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
#include "FileFormat.h"
#include "GetOpt.h"
#include "Pulses.h"
#include "System.h"
#include "TI99TapeDecoder.h"
#include "TI99TapeEncoder.h"
#include "TZX.h"
#include "utility.h"
#include "Wav.h"

#define T_LENGTH 3500000

static void convert(System::Type system, FileFormat::Type input_format, FileFormat::Type output_format, const std::vector<uint8_t> &contents, const std::string &outfile);
static void convert_wav(Pulses &pulses, TZX &tzx);
static void encode_ti(std::vector<uint8_t>::const_iterator begin, std::vector<uint8_t>::const_iterator end, TZX &tzx);

int main(int argc, const char * argv[]) {
    auto options = GetOpt({
        GetOpt::Option('F', "format", GetOpt::ARGUMENT_REQUIRED, "format", "specify output format"),
        GetOpt::Option('s', "system", GetOpt::ARGUMENT_REQUIRED, "system", "specify computer system"),
        GetOpt::Option('h', "help", "display this help message and exit")
    }, "ti99tape by Dieter Baron", "Report bugs to ti99tape@tpau.group");
    
    options.parse(argc, argv);

    if (options.is_set("help")) {
        options.print_help();
        exit(0);
    }
    
    if (options.arguments.size() != 2) {
        options.print_usage(true);
        exit(1);
    }
    
    try {
        std::string infile = options.arguments[0];
        std::string outfile = options.arguments[1];
        
        auto output_format = FileFormat::UNKNOWN;
        auto output_format_name = options.option("format");
        if (output_format_name.has_value()) {
            output_format = FileFormat::by_name(output_format_name.value());
        }
        else {
            output_format = FileFormat::by_filename(outfile);
        }
        if (output_format == FileFormat::WAV) {
            throw Exception("Writing WAV files is not supported.");
        }
        if (output_format == FileFormat::TI_TAPE) {
            throw Exception("Writing TI-Tape files is not supported.");
        }

        auto system = System::UNKNOWN;
        auto system_name = options.option("system");
        if (system_name.has_value()) {
            system = System::by_name(system_name.value());
        }

        // TODO: check that output_format / system combination is valid
        
        auto data = get_file_contents(infile);
        
        auto input_format = FileFormat::by_contents(data, system);
        
        if (input_format == FileFormat::TZX) {
            throw Exception("reading TZX files not supported yet");
        }
        
        convert(system, input_format, output_format, data, outfile);

     }
    catch (std::exception &e) {
        fprintf(stderr, "ERROR: %s\n", e.what());
    }
}


static void convert_wav(Pulses &pulses, TZX &tzx) {
    std::vector<uint16_t> data;

    for (auto pulse : pulses) {
        switch (pulse.type) {
            case Pulse::SILENCE:
                // TODO: add silence block
                break;
        
            case Pulse::POSITIVE:
            case Pulse::NEGATIVE:
                data.push_back(pulse.duration);
                break;
        }
    }
    
    tzx.add_pulse_sequence(data);
}


static void convert(System::Type system, FileFormat::Type input_format, FileFormat::Type output_format, const std::vector<uint8_t> &data, const std::string &outfile) {
    // TODO: check that input_format / output_format / system combination is valid
    
    switch (input_format) {
        case FileFormat::UNKNOWN:
        case FileFormat::RAW:
            switch (output_format) {
                case FileFormat::TZX: {
                    auto tzx = TZX(outfile);
                    
                    switch (system) {
                        case System::TI99_4A:
                            encode_ti(data.begin(), data.end(), tzx);
                            return;
                            
                        default:
                            break;
                    }
                    break;
                }
                    
                default:
                    break;
            }
            break;
            
        case FileFormat::TI_TAPE: {
            switch (output_format) {
                case FileFormat::TZX: {
                    auto tzx = TZX(outfile);
                    
                    switch (system) {
                        case System::TI99_4A:
                            encode_ti(data.begin() + 20, data.end(), tzx);
                            break;
                            
                        default:
                            break;
                    }
                    break;
                }
                    
                default:
                    break;
            }
            break;
            
        }
            
        case FileFormat::WAV: {
            auto wav = Wav(data, Wav::RIGHT);
            auto pulses = Pulses(wav);
            
            switch (output_format) {
                case FileFormat::TZX: {
                    auto tzx = TZX(outfile);
                    
                    switch (system) {
                        case System::TI99_4A: {
                            auto decoder = TI99TapeDecoder(pulses.begin(), pulses.end());
                            auto data = decoder.decode();
                            encode_ti(data.begin(), data.end(), tzx);
                            return;
                        }
                            
                        default:
                            convert_wav(pulses, tzx);
                            break;
                    }
                    break;
                }
                    
                case FileFormat::RAW: {
                    switch (system) {
                        case System::TI99_4A: {
                            auto decoder = TI99TapeDecoder(pulses.begin(), pulses.end());
                            auto data = decoder.decode();
                            write_file(outfile, data);
                            return;
                        }
                            
                        default:
                            break;
                    }
                }
                default:
                    break;
            }
            break;
        }
            
        default:
            break;
    }

    throw Exception("cannot convert " + System::name(system) + " " + FileFormat::name(input_format) + " to " + FileFormat::name(output_format));
}

static void encode_ti(std::vector<uint8_t>::const_iterator begin, std::vector<uint8_t>::const_iterator end, TZX &tzx) {
    auto encoder = TI99TapeEncoder(tzx, false);
    encoder.encode(begin, end);
}
