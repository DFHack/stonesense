#pragma once

#include "Types.h"
#include <map>
#include <string>

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

template<class T>
void MaterialMatcher<T>::set_material(T input, std::string token)
{
    using DFHack::MaterialInfo;
    using df::global::world;
    MaterialInfo mat;
    for (size_t i = 0; i < world->raws.inorganics.size(); i++)
    {
        mat.decode(0, i);
        check_match(input, token, mat.getToken(), 0, i);
    }
    FOR_ENUM_ITEMS(builtin_mats, i)
    {
        int k = -1;
        if (i == builtin_mats::COAL)
            k = 1;
        for (int j = -1; j <= k; j++)
        {
            mat.decode(i, j);
            check_match(input, token, mat.getToken(), i, j);
        }
    }
    for (size_t i = 0; i < world->raws.creatures.all.size(); i++)
    {
        auto creature = world->raws.creatures.all.at(i);
        for (size_t j = 0; j < creature->material.size(); j++)
        {
            mat.decode(j + MaterialInfo::CREATURE_BASE, i);
            check_match(input, token, mat.getToken(), j + MaterialInfo::CREATURE_BASE, i);
        }
    }
    for (size_t i = 0; i < world->raws.plants.all.size(); i++)
    {
        auto plant = world->raws.plants.all.at(i);
        for (size_t j = 0; j < plant->material.size(); j++)
        {
            mat.decode(j + MaterialInfo::PLANT_BASE, i);
            check_match(input, token, mat.getToken(), j + MaterialInfo::PLANT_BASE, i);
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

    for (size_t i = 0; i < world->raws.plants.all.size(); i++)
    {
        auto pp = world->raws.plants.all.at(i);
        if (!pp)
            continue;

        check_match(input, token, pp->id + ":BASE", -1, int32_t(i));

        for (size_t g = 0; g < pp->growths.size(); g++)
        {
            auto growth = pp->growths[g];
            if (!growth)
                continue;
            for (int l = 0; l < (sizeof(growth_locations) / sizeof(growth_locations[0])); l++)
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
