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

#include "TI99TapeDecoder.h"

const uint64_t TI99TapeDecoder::SYNC_SKIP_BEGINNING = 10;
const uint64_t TI99TapeDecoder::SYNC_MINIMUM_COUNT = 200;

std::vector<uint8_t> TI99TapeDecoder::decode() {
    auto data = std::vector<uint8_t>();

    read_sync();
    
    auto number_of_blocks = read_byte();
    if (number_of_blocks != read_byte()) {
        // number of blocks mismatch
    }
    //printf("DEBUG: number of blocks: %u\n", number_of_blocks);
    
    for (uint8_t block = 0; block < number_of_blocks; block++) {
        std::vector<uint8_t> data0;
        std::vector<uint8_t> data1;
        auto status0 = DecodeException::OK;
        auto message0 = std::string("OK");
        auto status1 = DecodeException::OK;
        auto message1 = std::string("OK");
        
        try {
            data0 = read_block();
        }
        catch (DecodeException &ex) {
            status0 = ex.error;
            message0 = ex.message;
        }
        try {
            data1 = read_block();
        }
        catch (DecodeException &ex) {
            status1 = ex.error;
            message1 = ex.message;
        }
        
        //printf("DEBUG: read block %u: %s, %s\n", block, message0.c_str(), message1.c_str());

        if (status0 == DecodeException::OK) {
            if (status1 == DecodeException::OK) {
                // compare data?
            }
            data.insert(data.end(), data0.begin(), data0.end());
        }
        else if (status1 == DecodeException::OK) {
            data.insert(data.end(), data1.begin(), data1.end());
        }
        else {
            throw status0 <= status1 ? DecodeException(status0, message0) : DecodeException(status1, message1);
        }
    }
    
    //printf("DEBUG: got %zu bytes of data\n", data.size());
    return data;
}


std::vector<uint8_t> TI99TapeDecoder::read_block() {
    read_block_sync();
    
    std::vector<uint8_t> data;
    
    uint8_t checksum = 0;
    for (auto i = 0; i < 64; i++) {
        auto byte = read_byte();
        data.push_back(byte);
        checksum += byte;
    }
    
    if (read_byte() != checksum) {
        throw DecodeException(DecodeException::CRC_ERROR, "crc error in block");
    }
    
    return data;
}


void TI99TapeDecoder::read_block_sync() {
    auto count = 0;
    
    while (1) {
        auto pulse = *(pulse_iterator++);
        
        if (!pulse.is_pulse()) {
            throw DecodeException(DecodeException::NO_DATA, "end of data in block sync");
        }
        
        if (pulse.duration < long_pulse_threshold) {
            if (count > 56) {
                // Data sync is 8 00 bytes, which is 64 long pulses. Allow for up to 8 of them being consumed by the previous block read in error.
                try {
                    read_data_mark();
                    return;
                }
                catch (DecodeException &ex) { }
            }
            count = 0;
        }
        else {
            count += 1;
        }

    }
}


uint8_t TI99TapeDecoder::read_byte() {
    uint8_t byte = 0;
    
    for (auto i = 0; i < 8; i++) {
        auto pulse = *(pulse_iterator++);
        
        if (pulse.type != Pulse::NEGATIVE && pulse.type != Pulse::POSITIVE) {
            throw DecodeException(DecodeException::NO_DATA, "no pulse found");
        }
        
        auto bit = 0;
        if (pulse.duration < long_pulse_threshold) {
            auto pulse = *pulse_iterator;
            if (pulse.duration >= long_pulse_threshold) {
                throw DecodeException(DecodeException::NO_DATA, "lone short pulse"); // TODO: other code
            }
            pulse_iterator++;
            bit = 1;
        }
        byte |= bit << (7 - i);
    }
    
    return byte;
}


void TI99TapeDecoder::read_data_mark() {
    // Data mark is an FF byte, which is 16 short pulses. The first pulse has already been read to detect the sync end.
    for (auto i = 0; i < 15; i++) {
        auto pulse = *(pulse_iterator++);
        if (pulse.type != Pulse::POSITIVE && pulse.type != Pulse::NEGATIVE) {
            throw DecodeException(DecodeException::NO_DATA, "missing pulse in data mark");
        }
        if (pulse.duration >= long_pulse_threshold) {
            throw DecodeException(DecodeException::ENCODING_ERROR, "missing data mark");
        }
    }
}


void TI99TapeDecoder::read_sync() {
    uint64_t sync_length = 0;
    uint64_t sync_count = 0;

    while (1) {
        if (pulse_iterator == end) {
            throw DecodeException(DecodeException::NO_SYNC, "no sync found");
        }
        
        auto const &pulse = *(pulse_iterator++);
        
        switch (pulse.type) {
            case Pulse::SILENCE:
                // TODO: handle error: silence in middle of sync; reset and try again?
                break;
                
            case Pulse::NEGATIVE:
            case Pulse::POSITIVE:
                if (sync_count > SYNC_MINIMUM_COUNT) {
                    zero_length = sync_length / (sync_count - SYNC_SKIP_BEGINNING);
                    long_pulse_threshold = zero_length * 3 / 4;
                    if (pulse.duration < long_pulse_threshold) {
                        read_data_mark();
                        return;
                    }
                }
                if (sync_count >= SYNC_SKIP_BEGINNING) {
                    sync_length += pulse.duration;
                }
                sync_count += 1;
                break;
        }
    }
}
