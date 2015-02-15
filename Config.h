#pragma once
#include "common.h"

#define KEYMOD_NONE 0

struct action_name_mapper {
    string name;
    void (*func)(uint32_t);
};

string parseStrFromLine( string keyword, string line );
int parseIntFromLine( string keyword, string line );