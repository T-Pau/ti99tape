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

#include "TI99TapeEncoder.h"

#include "Exception.h"


uint16_t TI99TapeEncoder::ZERO_PULSE_LENGTH = 2539;
uint16_t TI99TapeEncoder::NUMBER_OF_SYNC_PULSES = (768 * 8);

TZX::GeneralizedDataBlock::SymbolDefinitions TI99TapeEncoder::pilot_symbols = {
    TZX::GeneralizedDataBlock::SymbolDefinition(0, { ZERO_PULSE_LENGTH })
};

TZX::GeneralizedDataBlock::PilotData TI99TapeEncoder::pilot_data = {
    TZX::GeneralizedDataBlock::PilotRunLength(0, NUMBER_OF_SYNC_PULSES)
};

TZX::GeneralizedDataBlock::SymbolDefinitions TI99TapeEncoder::data_symbols = {
    TZX::GeneralizedDataBlock::SymbolDefinition(0, { ZERO_PULSE_LENGTH }),
    TZX::GeneralizedDataBlock::SymbolDefinition(0, { static_cast<uint16_t>(ZERO_PULSE_LENGTH / 2), static_cast<uint16_t>(ZERO_PULSE_LENGTH - (ZERO_PULSE_LENGTH / 2)) })
};

void TI99TapeEncoder::encode(std::vector<uint8_t>::const_iterator start, std::vector<uint8_t>::const_iterator end) {
    auto length = end - start;
    auto num_blocks = (length + 63) / 64;
    
    if (num_blocks > 255) {
        throw Exception("file too long");
    }
    
    if (!first) {
        // TODO: add silence between files
    }
    
    add_byte(0xff);
    add_byte(num_blocks);
    add_byte(num_blocks);
    
    for (auto i = 0; i < num_blocks; i++) {
        auto block_end = start < end - 64 ? start + 64 : end;
        
        add_block(start, block_end);
        add_block(start, block_end);
        
        start += 64;
    }
    
    if (use_data_block) {
        tzx.add_general_data(TZX::GeneralizedDataBlock(0, pilot_symbols, pilot_data, data_symbols, static_cast<uint32_t>(data.size() * 8), data));
    }
    else {
        tzx.add_pure_tone(ZERO_PULSE_LENGTH, NUMBER_OF_SYNC_PULSES);
        tzx.add_pulse_sequence(pulses);
    }
}


void TI99TapeEncoder::add_block(std::vector<uint8_t>::const_iterator start, std::vector<uint8_t>::const_iterator end) {
    auto checksum = 0;
    
    for (auto i = 0; i < 8; i++) {
        add_byte(0);
    }
    add_byte(0xff);
    for (auto i = 0; i < 64; i++) {
        uint8_t byte = 0;
        if (start < end) {
            byte = *start;
            start++;
        }

        add_byte(byte);
        checksum = (checksum + byte) & 0xff;
    }
    add_byte(checksum);
}


void TI99TapeEncoder::add_byte(uint8_t byte) {
    if (use_data_block) {
        data.push_back(byte);
    }
    else {
        for (size_t i = 0; i < 8; i++) {
            if (byte & (1 << (7-i))) {
                pulses.push_back(ZERO_PULSE_LENGTH / 2);
                pulses.push_back(ZERO_PULSE_LENGTH - (ZERO_PULSE_LENGTH / 2));
            }
            else {
                pulses.push_back(ZERO_PULSE_LENGTH);
            }
        }
    }
}
