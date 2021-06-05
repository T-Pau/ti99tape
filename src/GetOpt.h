/*
 GetOpt.h -- parse command line arguments
 Copyright (C) 2020 Dieter Baron
 
 This file is part of gfx-convert, a graphics converter toolbox
 for 8-bit systems.
 The authors can be contacted at <gfx-convert@tpau.group>
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:
 1. Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in
 the documentation and/or other materials provided with the
 distribution.
 3. The names of the authors may not be used to endorse or promote
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

#ifndef hAD_GETOPT_H
#define hAD_GETOPT_H

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

class GetOpt {
public:
    enum ArgumentType {
        ARGUMENT_NONE,
        ARGUMENT_REQUIRED,
        ARGUMENT_OPTIONAL
    };
    
    class Option {
    public:
        char short_name;
        std::string long_name;
        ArgumentType argument_type;
        std::string argument_name;
        std::string default_argument;
        std::string description;
        
        Option(char short_name_, const std::string &long_name_, const std::string &description_) : short_name(short_name_), long_name(long_name_), argument_type(ARGUMENT_NONE), description(description_), sort_name(1, short_name) { }
        Option(char short_name_, const std::string &description_) : short_name(short_name_), argument_type(ARGUMENT_NONE), description(description_), sort_name(1, short_name) { }
        Option(const std::string &long_name_, const std::string &description_) : short_name('\0'), long_name(long_name_), argument_type(ARGUMENT_NONE), description(description_), sort_name(long_name) { }

        Option(char short_name_, const std::string &long_name_, ArgumentType argument_type_, const std::string &argument_name_, const std::string &description_, const std::string &default_argument_ = "") : short_name(short_name_), long_name(long_name_), argument_type(argument_type_), argument_name(argument_name_), description(description_), default_argument(default_argument_), sort_name(1, short_name) { }
        Option(char short_name_, ArgumentType argument_type_, const std::string &argument_name_, const std::string &description_, const std::string &default_argument_ = "") : short_name(short_name_), argument_type(argument_type_), argument_name(argument_name_), description(description_), default_argument(default_argument_), sort_name(1, short_name) { }
        Option(const std::string &long_name_, ArgumentType argument_type_, const std::string &argument_name_, const std::string &description_, const std::string &default_argument_ = "") : short_name('\0'), long_name(long_name_), argument_type(argument_type_), argument_name(argument_name_), description(description_), default_argument(default_argument_), sort_name(long_name) { }
        
        bool has_short() const { return short_name != '\0'; }
        bool has_long() const { return !long_name.empty(); }

        bool operator>(const Option &other) const { return sort_name > other.sort_name; }
        
    private:
        std::string sort_name;
    };
    
    GetOpt(const std::vector<Option> &options, const std::string &header, const std::string &footer);
    
    void print_usage(bool to_stderr = false) const;
    void print_help(bool to_stderr = false) const;
    
    bool parse(int argc, const char *argv[], bool print_errors = true);
    
    bool is_set(char name) const;
    bool is_set(const std::string &name) const;
    
    std::optional<std::string> option(char name) const;
    std::optional<std::string> option(const std::string &name) const;
    
    std::string program_name;
    std::vector<std::string> arguments;

private:
    std::vector<Option> options;
    std::string header;
    std::string footer;

    std::unordered_map<char, size_t> short_options;
    std::unordered_map<std::string, size_t> long_options;
    
    std::unordered_map<size_t, std::string> option_arguments;
};

#endif // hAD_GETOPT_H
