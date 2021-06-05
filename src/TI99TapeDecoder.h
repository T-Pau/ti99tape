#ifndef HAD_TI99_TAPE_DECODER_H
#define HAD_TI99_TAPE_DECODER_H

/*
 TI99TapeEncoder.h -- encode file in TI 99/4A tape format.
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

#include "Exception.h"
#include "Pulses.h"

class TI99TapeDecoder {
public:
    class DecodeException : public Exception {
    public:
        enum ErrorCode {
            OK,
            CRC_ERROR,
            ENCODING_ERROR,
            OUT_OF_DATA,
            NO_DATA,
            NO_SYNC
        };
        
        DecodeException(ErrorCode code, const std::string &message) : Exception(message), error(code) { }
        DecodeException(const DecodeException &other) : Exception(other.message), error(other.error) { }

        ErrorCode error;
    };

    TI99TapeDecoder(Pulses::Iterator begin, Pulses::Iterator end_) : pulse_iterator(begin), end(end_) { }
    
    std::vector<uint8_t> decode();
    
private:
    Pulses::Iterator pulse_iterator;
    Pulses::Iterator end;

    uint64_t zero_length;
    uint64_t long_pulse_threshold;
    
    std::vector<uint8_t> read_block();
    void read_block_sync();
    uint8_t read_byte();
    void read_data_mark();
    void read_sync();

    static const uint64_t SYNC_SKIP_BEGINNING;
    static const uint64_t SYNC_MINIMUM_COUNT;
};

#endif // HAD_TI99_TAPE_DECODER_H
