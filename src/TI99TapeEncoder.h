#ifndef HAD_TI99_TAPE_ENCODER_H
#define HAD_TI99_TAPE_ENCODER_H

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

#include <string>
#include <vector>

#include "TZX.h"

class TI99TapeEncoder {
public:
    TI99TapeEncoder(const std::string &filename, bool use_data_block_) : tzx(filename), use_data_block(use_data_block_), first(true) { }

    void encode(const std::vector<uint8_t> &data) { encode(data.begin(), data.end()); }
    void encode(std::vector<uint8_t>::const_iterator start, std::vector<uint8_t>::const_iterator end);
    
private:
    TZX tzx;
    bool use_data_block;
    bool first;
    
    std::vector<uint8_t> data;
    std::vector<uint16_t> pulses;
    
    void add_byte(uint8_t byte);
    void add_block(std::vector<uint8_t>::const_iterator start, std::vector<uint8_t>::const_iterator end);
    
    static uint16_t ZERO_PULSE_LENGTH;
    static uint16_t ONE_PULSE_LENGTH;
    static uint16_t NUMBER_OF_SYNC_PULSES;
    
    static TZX::GeneralizedDataBlock::SymbolDefinitions pilot_symbols;
    static TZX::GeneralizedDataBlock::SymbolDefinitions data_symbols;
    static TZX::GeneralizedDataBlock::PilotData pilot_data;
};

#endif // HAD_TI99_TAPE_ENCODER_H
