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

#include <algorithm>

#include "Exception.h"
#include "utility.h"

TZX::TZX(const std::string &filename) : file(filename) {
    file.write_string("ZXTape!");
    file.write_8(0x1a);
    file.write_8(1);
    file.write_8(20);
}


void TZX::add_general_data(const GeneralizedDataBlock &block) {
    file.write_8(0x19);
    file.write_32(block.block_length());
    file.write_16(block.pause_after);
    file.write_32(static_cast<uint32_t>(block.pilot_data.size()));
    file.write_8(block.number_of_pilot_symbol_pulses);
    file.write_8(static_cast<uint8_t>(block.pilot_symbols.size()));
    file.write_32(block.data_size);
    file.write_8(block.number_of_data_symbol_pulses);
    file.write_8(static_cast<uint8_t>(block.data_symbols.size()));
    write_symbol_definitions(block.number_of_pilot_symbol_pulses, block.pilot_symbols);
    for (const auto &entry : block.pilot_data) {
        file.write_8(entry.symbol);
        file.write_16(entry.repetitions);
    }
    write_symbol_definitions(block.number_of_data_symbol_pulses, block.data_symbols);
    file.write_data(block.data);
}

void TZX::add_pure_tone(uint16_t pulse_length, uint16_t repetitions) {
    file.write_8(0x12);
    file.write_16(pulse_length);
    file.write_16(repetitions);
}


void TZX::add_pulse_sequence(const std::vector<uint16_t> &pulses) {
    for (size_t i = 0; i < pulses.size(); i += 255) {
        uint16_t length = std::min(static_cast<size_t>(255), pulses.size() - i);
        file.write_8(0x13);
        file.write_8(length);
        for (size_t j = i; j < i + length; j++) {
            file.write_16(pulses[j]);
        }
    }
}


void TZX::write_symbol_definitions(uint8_t number_of_pulses, const GeneralizedDataBlock::SymbolDefinitions &symbols) {
    for (const auto &symbol : symbols) {
        file.write_8(symbol.flags);
        for (size_t i = 0; i < number_of_pulses; i++) {
            file.write_16(i >= symbol.pulse_lengths.size() ? 0 : symbol.pulse_lengths[i]);
        }
    }
}


TZX::GeneralizedDataBlock::GeneralizedDataBlock(uint16_t pause_after_, const SymbolDefinitions &pilot_symbols_, const PilotData &pilot_data_, SymbolDefinitions &data_symbols_, uint32_t data_size_, const std::vector<uint8_t> &data_) : pause_after(pause_after_), number_of_pilot_symbol_pulses(0), number_of_data_symbol_pulses(0), data_size(data_size_) {
    if (!pilot_data_.empty())  {
        pilot_data = pilot_data_;
        pilot_symbols = pilot_symbols_;

        if (pilot_symbols.size() > 256) {
            throw Exception("too many pilot symmbols");
        }
        number_of_pilot_symbol_pulses = get_number_of_pulses(pilot_symbols);
    }
    if (data_size > 0) {
        data = data_;
        data_symbols = data_symbols_;

        auto bits_per_sybmol = number_of_bits(data_symbols.size());
        if (bits_per_sybmol > 8) {
            throw Exception("too many data symbols");
        }
        if (data_size > UINT32_MAX) {
            throw Exception("data stream too long");
        }
        auto number_of_data_bytes = (bits_per_sybmol * data_size + 7) / 8;
        if (data.size() < number_of_data_bytes) {
            throw Exception("too little data");
        }
        else if (data.size() > number_of_data_bytes) {
            throw Exception("too much data");
        }
        number_of_data_symbol_pulses = get_number_of_pulses(data_symbols);
    }
}


uint8_t TZX::GeneralizedDataBlock::get_number_of_pulses(const SymbolDefinitions &symbols) const {
    uint8_t number_of_pulses = 0;
    
    for (const auto &symbol : symbols) {
        if (symbol.pulse_lengths.size() > 255) {
            throw Exception("too many pulses in pilot symbol");
        }
        number_of_pulses = std::max(number_of_pulses, static_cast<uint8_t>(symbol.pulse_lengths.size()));
    }
    
    return number_of_pulses;
}


uint32_t TZX::GeneralizedDataBlock::block_length() const {
    size_t length = 14
        + pilot_symbols.size() * (1 + number_of_pilot_symbol_pulses * 2)
        + data_symbols.size() * (1 + number_of_data_symbol_pulses * 2)
        + pilot_data.size() * 3
        + data.size();
    
    if (length > UINT32_MAX) {
        throw Exception("block too long");
    }
    
    return static_cast<uint32_t>(length);
}
