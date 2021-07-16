#pragma once
#include <string>

char *string_to_char_arr(std::string &ref)
{
    return &ref[0];
}