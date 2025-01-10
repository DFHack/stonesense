#pragma once
#include "common.h"

constexpr auto KEYMOD_NONE = 0;

struct action_name_mapper {
    std::string name;
    void (*func)(uint32_t);
};

bool loadConfigFile();
