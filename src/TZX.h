#ifndef HAD_TZX_H
#define HAD_TZX_H

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

#include <vector>

#include "OutputFile.h"

class TZX {
public:
    class GeneralizedDataBlock {
    public:
        class SymbolDefinition {
        public:
            SymbolDefinition(uint8_t flags_, const std::vector<uint16_t> &pulse_lengths_) : flags(flags_), pulse_lengths(pulse_lengths_) { }

            uint8_t flags;
            std::vector<uint16_t> pulse_lengths;
        };
        
        class PilotRunLength {
        public:
            PilotRunLength(uint8_t symbol_, uint16_t repetitions_) : symbol(symbol_), repetitions(repetitions_) { }
            
            uint8_t symbol;
            uint16_t repetitions;
        };
        
        typedef std::vector<SymbolDefinition> SymbolDefinitions;
        typedef std::vector<PilotRunLength> PilotData;
        
        GeneralizedDataBlock(uint16_t paus_after, const SymbolDefinitions &pilot_symbols, const PilotData &pilot_data, SymbolDefinitions &data_symbols, uint32_t data_size, const std::vector<uint8_t> &data);
        
        uint16_t pause_after;

        uint8_t number_of_pilot_symbol_pulses;
        SymbolDefinitions pilot_symbols;
        PilotData pilot_data;

        uint8_t number_of_data_symbol_pulses;
        SymbolDefinitions data_symbols;
        uint32_t data_size;
        std::vector<uint8_t> data;

        uint32_t block_length() const;

    private:
        uint8_t get_number_of_pulses(const SymbolDefinitions &symbols) const;
    };

    TZX(const std::string &filename);
    
    void add_general_data(const GeneralizedDataBlock &block);
    void add_pure_tone(uint16_t pulse_length, uint16_t repetitions);
    void add_pulse_sequence(const std::vector<uint16_t> &pulses);

private:
    OutputFile file;
    
    void write_symbol_definitions(uint8_t number_of_pulses, const GeneralizedDataBlock::SymbolDefinitions &symbols);
};

#endif // HAD_TZX_H
