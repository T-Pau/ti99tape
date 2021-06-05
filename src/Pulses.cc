/*
 Pulses.cc -- pulse recovery from audio data.
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

Pulses::Pulses(Wav wav_) : wav(wav_) {
    cutoff = wav.peek * 1 / 16;
}

std::string Pulse::type_name() const {
    switch (type) {
        
    case SILENCE:
        return "silence";
    case POSITIVE:
        return "positive";
    case NEGATIVE:
        return "negative";
    }
    
    return "invalid";
}

Pulses::Iterator::Iterator(const Pulses &pulses_, bool end) : pulses(pulses_), phase(START), count(0), current_pulse(Pulse::SILENCE, 0) {
    if (end) {
        current = pulses.wav.samples.end();
    }
    else {
        current = pulses.wav.samples.begin();
        next();
    }
}

void Pulses::Iterator::next() {
    // TODO: detect silence in the middle of the file
    
    while (current < pulses.wav.samples.end()) {
        auto sample = *current;
        current++;
        count += 1;
        
#if 0
        if (sample < -pulses.cutoff) {
            printf("%d < <\n", sample);
        }
        else if (sample <= pulses.cutoff) {
            printf("< %d <\n", sample);
        }
        else {
            printf("< < %d\n", sample);
        }
#endif

        switch (phase) {
        case START:
            if (sample > pulses.cutoff) {
                phase = PLUS_FALLING;
                if (count > 2) {
                    set_pulse(Pulse::SILENCE);
                    return;
                }
            }
            else if (sample < -pulses.cutoff) {
                phase = MINUS_RISING;
                if (count > 2) {
                    set_pulse(Pulse::SILENCE);
                    return;
                }
            }
            break;
            
        case PLUS_RISING:
            if (sample > pulses.cutoff) {
                phase = PLUS_FALLING;
            }
            else if (sample < -pulses.cutoff) {
                printf("ERROR: missing positive peak\n");
            }
            break;
            
        case PLUS_FALLING:
            if (sample < 0) {
                if (sample < -pulses.cutoff) {
                    phase = MINUS_RISING;
                }
                else {
                    phase = MINUS_FALLING;
                }
                set_pulse(Pulse::POSITIVE);
                return;
            }
            break;
            
        case MINUS_FALLING:
            if (sample < -pulses.cutoff) {
                phase = MINUS_RISING;
            }
            else if (sample > pulses.cutoff) {
                printf("ERROR: missing negative peak\n");
            }
            break;
            
        case MINUS_RISING:
            if (sample > 0) {
                if (sample > pulses.cutoff) {
                    phase = PLUS_FALLING;
                }
                else {
                    phase = PLUS_RISING;
                }
                set_pulse(Pulse::NEGATIVE);
                return;
            }
        }
    }

}

void Pulses::Iterator::set_pulse(Pulse::Type type) {
    current_pulse = Pulse(type, count * 3500000 / pulses.wav.sample_rate);
    // printf("PULSE: %s %llu\n", current_pulse.type_name().c_str(), current_pulse.duration);
    count = 0;
}
