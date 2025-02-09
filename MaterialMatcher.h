#pragma once

#include "Types.h"
#include <map>
#include <string>

#include "MiscUtils.h"
#include "modules/Materials.h"

#include "df/builtin_mats.h"
#include "df/creature_raw.h"
#include "df/plant_growth.h"
#include "df/plant_raw.h"
#include "df/world.h"

template<class T>
class MaterialMatcher
{
private:
    struct MaterialMatch
    {
        T item;
        int difference;
    };
    std::map<DFHack::t_matglossPair, MaterialMatch> matList;
    void check_match(T input, std::string token, std::string id, int16_t type, int32_t index);
public:
    T* get(DFHack::t_matglossPair);
    void set_material(T input, std::string token);
    void set_growth(T input, std::string token);

    //Wipes the slate clean
    void clear();
};

int FuzzyCompare(std::string source, std::string target);


//--------------------------------------------------------

template<class T>
void MaterialMatcher<T>::check_match(T input, std::string token, std::string id, int16_t type, int32_t index)
{
    DFHack::t_matglossPair pair;
    pair.type = type;
    pair.index = index;

    int match = FuzzyCompare(token, id);
    if (match >= 0 && (!matList.count(pair) || matList.at(pair).difference > match))
    {
        matList[pair].item = input;
        matList[pair].difference = match;
    }
}

template<class T>
T* MaterialMatcher<T>::get(DFHack::t_matglossPair matPair)
{
    if (matList.count(matPair))
        return &matList[matPair].item;
    else return NULL;
}

// Simple helper function to check if there are any wildcards present
// If not, we can save ourselves a lot of trouble
inline bool contains_wildcard(const std::string &str)
{
    return (str.find_first_of("*?") != std::string::npos);
}

template<class T>
void MaterialMatcher<T>::set_material(T input, std::string token)
{
    using DFHack::MaterialInfo;
    using df::global::world;
    MaterialInfo mat;

    std::vector<std::string> tokens;
    split_string(&tokens, token, ":");
    bool match_id = (tokens.size() >= 2) && !contains_wildcard(tokens[1]);

    // If there are no wildcards anywhere, then just do a direct lookup
    if (!contains_wildcard(token))
    {
        if (mat.find(token))
            check_match(input, token, mat.getToken(), mat.type, mat.index);
        return;
    }

    // Only scan inorganics if the token looks like it's actually pointing at one
    if ((tokens[0] == "INORGANIC") || contains_wildcard(tokens[0]))
    {
        for (size_t i = 0; i < world->raws.inorganics.all.size(); i++)
        {
            mat.decode(0, i);
            check_match(input, token, mat.getToken(), 0, i);
        }
    }
    FOR_ENUM_ITEMS(builtin_mats, i)
    {
        int k = -1;
        if (i == df::builtin_mats::COAL)
            k = 1;
        for (int j = -1; j <= k; j++)
        {
            mat.decode(i, j);
            check_match(input, token, mat.getToken(), i, j);
        }
    }
    // Same for creatures - no point in evaluating something that won't possibly match
    if ((tokens[0] == "CREATURE") || contains_wildcard(tokens[0]))
    {
        for (size_t i = 0; i < world->raws.creatures.all.size(); i++)
        {
            auto creature = world->raws.creatures.all.at(i);
            // Furthermore, don't bother looking at creature materials if the creature itself is wrong
            if (match_id && (creature->creature_id != tokens[1]))
                continue;
            for (size_t j = 0; j < creature->material.size(); j++)
            {
                mat.decode(j + MaterialInfo::CREATURE_BASE, i);
                check_match(input, token, mat.getToken(), j + MaterialInfo::CREATURE_BASE, i);
            }
        }
    }
    // And the same goes for plants
    if ((tokens[0] == "PLANT") || contains_wildcard(tokens[0]))
    {
        for (size_t i = 0; i < world->raws.plants.all.size(); i++)
        {
            auto plant = world->raws.plants.all.at(i);
            // And skip plants that obviously don't match
            if (match_id && (plant->id != tokens[1]))
                continue;
            for (size_t j = 0; j < plant->material.size(); j++)
            {
                mat.decode(j + MaterialInfo::PLANT_BASE, i);
                check_match(input, token, mat.getToken(), j + MaterialInfo::PLANT_BASE, i);
            }
        }
    }
}

template<class T>
void MaterialMatcher<T>::set_growth(T input, std::string token)
{
    using df::global::world;

    static const char* growth_locations[] = {
        "TWIGS",
        "LIGHT_BRANCHES",
        "HEAVY_BRANCHES",
        "TRUNK",
        "ROOTS",
        "CAP",
        "SAPLING",
        "SHRUB"
    };

    std::vector<std::string> tokens;
    split_string(&tokens, token, ":");
    bool match_plant = (tokens.size() >= 1) && (!contains_wildcard(tokens[0]));
    bool match_growth = (tokens.size() >= 2) && (!contains_wildcard(tokens[1]));

    for (size_t i = 0; i < world->raws.plants.all.size(); i++)
    {
        auto pp = world->raws.plants.all.at(i);
        if (!pp)
            continue;
        if (match_plant && (pp->id != tokens[0]))
            continue;

        check_match(input, token, pp->id + ":BASE", -1, int32_t(i));

        for (size_t g = 0; g < pp->growths.size(); g++)
        {
            auto growth = pp->growths[g];
            if (!growth)
                continue;
            if (match_growth && (growth->id != tokens[1]))
                continue;

            for (int l = 0; l < int(sizeof(growth_locations) / sizeof(growth_locations[0])); l++)
            {
                check_match(input, token, pp->id + ":" + growth->id + ":" + growth_locations[l], g * 10 + l, int32_t(i));
            }
        }
    }
}

template<class T>
void MaterialMatcher<T>::clear()
{
    matList.clear();
}
