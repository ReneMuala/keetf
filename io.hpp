/*  KEEFT - TCP/IP FILE TRANSFER TOOL
 *  © Landia (Rene Muala)
 *
 */

#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "config.hpp"

namespace keeft {
    typedef enum 
        { _H, _V,_B, _R, _S, __IPv4, __IPv6, _L, __only_IPv4, __only_IPv6, _P, _K, _C, _NULL } in_option;
    
    void log(const char * );
    void reset_get_opts();
    bool handle_opt(in_option option);
    ;; in_option convert_str_to_option(const char * str);
    void get_value(const char * value);
    void parse_args(int argc, char **argv);
    void print_config();
    void print_addresses(std::vector<std::string> addresses, const char * type);
    void list_machine_addresses(bool print_v4s, bool print_v6s);
    
    void print_progress(double, double);
    
    void print_version();
    
    void load_command_options();
    void print_command_options();
    
}
