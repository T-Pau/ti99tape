//
//  GetOpt.cpp
//  gfx-convert
//
//  Created by Dieter Baron on 19.11.20.
//  Copyright © 2020 T'Pau. All rights reserved.
//

#include "GetOpt.h"

#include <cstdio>
#include <getopt.h>

GetOpt::GetOpt(const std::vector<Option> &options_, const std::string &header_, const std::string &footer_) : options(options_), header(header_), footer(footer_) {
    // TODO: sort options
    for (auto i = 0; i < options.size(); i++) {
        auto &option = options[i];

        if (option.has_short()) {
            short_options[option.short_name] = i;
        }
        if (option.has_long()) {
            long_options[option.long_name] = i;
        }
    }
}

bool GetOpt::parse(int argc, const char **argv, bool print_errors) {
    option_arguments.clear();
    program_name = argv[0];
    arguments.clear();
    std::unordered_map<int, size_t> option_indices;

    struct option long_opts[long_options.size() + 1];
    std::string options_string;
    
    size_t long_opts_id = 0;
    
    for (auto i = 0; i < options.size(); i++) {
        auto &option = options[i];
        
        if (option.has_short()) {
            option_indices[option.short_name] = i;
            options_string += option.short_name;
            if (option.argument_type != ARGUMENT_NONE) {
                options_string += ":";
            }
        }
        if (option.has_long()) {
            auto &long_opt = long_opts[long_opts_id];
            
            switch (option.argument_type) {
            case ARGUMENT_NONE:
                long_opt.has_arg = no_argument;
                break;
            case ARGUMENT_OPTIONAL:
                long_opt.has_arg = optional_argument;
                break;
                
            case ARGUMENT_REQUIRED:
                long_opt.has_arg = required_argument;
                break;
            }
            
            long_opt.flag = NULL;
            long_opt.name = option.long_name.c_str();
            if (option.has_short()) {
                long_opt.val = option.short_name;
            }
            else {
                long_opt.val = 256 + i;
                option_indices[long_opt.val] = i;
            }
            
            long_opts_id++;
        }
    }
    
    long_opts[long_opts_id].name = NULL;

    auto ok = true;
    
    int c;
    while ((c = getopt_long(argc, const_cast<char * const *>(argv), options_string.c_str(), long_opts, NULL)) != EOF) {
        if (c == ':') {
            // TODO: missing argument
            ok = false;
            continue;
        }
        if (c == '?') {
            // TODO: unknown option
            ok = false;
            continue;
        }

        auto it = option_indices.find(c);
        if (it == option_indices.end()) {
            // TODO: unknown option
            ok = false;
            continue;
        }

        option_arguments[it->second] = optarg ? optarg : "";
    }
    
    while (optind < argc) {
        arguments.push_back(argv[optind]);
        optind++;
    }
    
    if (print_errors && !ok) {
        // TODO: print usage
    }
    
    return ok;
}

bool GetOpt::is_set(char name) const {
    auto it = short_options.find(name);
    
    if (it == short_options.end()) {
        return false;
    }
    
    return option_arguments.find(it->second) != option_arguments.end();
}

bool GetOpt::is_set(const std::string &name) const {
    auto it = long_options.find(name);
    
    if (it == long_options.end()) {
        return false;
    }
    
    return option_arguments.find(it->second) != option_arguments.end();
}

std::optional<std::string> GetOpt::option(char name) const {
    if (!is_set(name)) {
        return {};
    }
    
    return option_arguments.find(short_options.find(name)->second)->second;
}

std::optional<std::string> GetOpt::option(const std::string &name) const {
    if (!is_set(name)) {
        return {};
    }
    
    return option_arguments.find(long_options.find(name)->second)->second;
}


void GetOpt::print_help(bool to_stderr) const {
    print_usage(to_stderr);
    
    // TODO: help message
}

void GetOpt::print_usage(bool to_stderr) const {
    FILE *f = to_stderr ? stderr : stdout;
    
    fprintf(f, "Usage: %s ...\n", program_name.c_str());
}
