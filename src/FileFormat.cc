/*
 FileFormat.cc -- file format detector.
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

#include "FileFormat.h"

#include <algorithm>
#include <filesystem>

#include "Exception.h"

std::unordered_map<std::string, FileFormat::Type> FileFormat::extensions = {
    { "tzx", TZX },
    { "wav", WAV }
};

std::unordered_map<std::string, FileFormat::Type> FileFormat::names = {
    { "raw", RAW },
    { "raw data", RAW },
    { "tzx", TZX },
    { "wav", WAV },
    { "unknown", UNKNOWN }
};

std::vector<FileFormat::Signature> FileFormat::signatures = {
    Signature(0, "TI-TAPE", TI_TAPE),
    Signature(0, "RIFF", WAV),
    Signature(0, "ZXTape!\x1a", TZX)
};


std::string FileFormat::name(Type type) {
    switch (type) {
        case RAW:
            return "raw data";
        case TI_TAPE:
            return "TI-Tape";
        case TZX:
            return "TZX";
        case WAV:
            return "WAV";
        case UNKNOWN:
            return "unknown";
    }
    
    return "invalid";
}

FileFormat::Type FileFormat::by_contents(const std::vector<uint8_t> &data, System::Type system) {
    for (const auto &signature : signatures) {
        if (signature.matches(data)) {
            return signature.type;
        }
    }
    return RAW;
}


FileFormat::Type FileFormat::by_filename(const std::string &filename) {
    auto extension = std::filesystem::path(filename).extension().string();
    if (extension.empty()) {
        return RAW;
    }
    if (extension[0] == '.') {
        extension = extension.substr(1);
    }
    return by_extension(extension);
}


FileFormat::Type FileFormat::by_extension(const std::string &extension) {
    // TODO: lowercase extension
    
    auto it = extensions.find(extension);
    
    if (it == extensions.end()) {
        return RAW;
    }
    
    return it->second;
}

FileFormat::Type FileFormat::by_name(const std::string &name) {
    // TOOD: lowercase name
    auto it = names.find(name);
    
    if (it == names.end()) {
        throw Exception("unknown system '" + name + "'");
    }
    
    return it->second;
}


FileFormat::Signature::Signature(int64_t offset_, const std::string &value_, Type type_) : offset(offset_), type(type_) {
    value.insert(value.end(), value_.begin(), value_.end());
}


bool FileFormat::Signature::matches(const std::vector<uint8_t> &data) const {
    uint64_t start;
    
    if (offset >= 0) {
        start = static_cast<uint64_t>(offset);
    }
    else {
        if (-offset > data.size()) {
            return false;
        }
        start = data.size() - offset;
    }
    
    if (start + value.size() > data.size()) {
        return false;
    }
    
    return std::equal(value.begin(), value.end(), data.begin() + start);
}
