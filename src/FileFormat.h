#ifndef HAD_FILE_FORMAT_H
#define HAD_FILE_FORMAT_H

/*
 FileFormat.h -- file format detector.
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
#include <unordered_map>
#include <vector>

#include "System.h"
    
class FileFormat {
public:
    enum Type {
        TI_TAPE,
        TZX,
        RAW,
        WAV,
        UNKNOWN
    };
    
    static std::string name(Type type);
    static Type by_contents(const std::vector<uint8_t> &data, System::Type system);
    static Type by_extension(const std::string &extension);
    static Type by_filename(const std::string &filename);
    static Type by_name(const std::string &name);

private:
    class Signature {
    public:
        Signature(int64_t offset_, const std::vector<uint8_t> &value_, Type type_) : offset(offset_), value(value_), type(type_) { }
        Signature(int64_t offset_, const std::string &value_, Type type_);

        int64_t offset;
        std::vector<uint8_t> value;
        Type type;
        
        bool matches(const std::vector<uint8_t> &data) const;
    };

    static std::unordered_map<std::string, Type> extensions;
    static std::unordered_map<std::string, Type> names;
    static std::vector<Signature> signatures;
};

#endif // HAD_FILE_FORMAT_H
