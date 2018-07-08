#pragma once
#include "common.h"

#define KEYMOD_NONE 0

struct action_name_mapper {
    std::string name;
    void (*func)(uint32_t);
};

std::string parseStrFromLine( std::string keyword, std::string line );
int parseIntFromLine( std::string keyword, std::string line );
