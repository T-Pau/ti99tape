#ifndef HAD_PULSES_H
#define HAD_PULSES_H

/*
 Pulses.h -- pulse recovery from audio data.
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

#include <cinttypes>

#include "Wav.h"

class Pulse {
public:
    enum Type {
        SILENCE,
        POSITIVE,
        NEGATIVE
    };
    Pulse(Type type_, uint64_t duration_) : type(type_), duration(duration_) { }
    
    bool is_pulse() const { return type == POSITIVE || type == NEGATIVE; }
    std::string type_name() const;
    
    Type type;
    uint64_t duration;
};

class Pulses {
public:
    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = int64_t;
        using value_type        = Pulse;
        using pointer           = Pulse *;  // or also value_type*
        using reference         = Pulse &;  // or also value_type&

        Iterator(const Pulses &pulses, bool end = false);
        
        reference operator*() const { return const_cast<Pulse &>(current_pulse); }
        pointer operator->() { return &current_pulse; }
        
        // Prefix increment
        Iterator& operator++() { next(); return *this; }
        
        // Postfix increment
        Iterator operator++(int) { Iterator tmp = *this; next(); return tmp; }
        
        friend bool operator== (const Iterator& a, const Iterator& b) { return a.current == b.current; };
        friend bool operator!= (const Iterator& a, const Iterator& b) { return a.current != b.current; };

    private:
        enum Phase {
            START,
            PLUS_RISING,
            PLUS_FALLING,
            MINUS_FALLING,
            MINUS_RISING
        };
        
        const Pulses &pulses;
        
        Phase phase;
        uint64_t count;
        std::vector<int16_t>::const_iterator current;
        
        Pulse current_pulse;
        
        void next();
        void set_pulse(Pulse::Type type);
    };
    
    Pulses(Wav wav);
    
    Iterator begin() { return Iterator(*this); }
    Iterator end() { return Iterator(*this, true); }
        
private:
    Wav wav;
    int32_t cutoff;
};

#endif // HAD_PULSES_H
