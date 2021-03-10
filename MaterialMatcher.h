#pragma once

#include "RemoteFortressReader.pb.h"
#include "Types.h"
#include <map>
#include <string>

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
public:
    T* get(DFHack::t_matglossPair);
    int set(T input, std::string token, google::protobuf::RepeatedPtrField< ::RemoteFortressReader::MaterialDefinition >* matTokenList);
    void set_growth(T input, std::string token);

    //Wipes the slate clean
    void clear();
};

int FuzzyCompare(std::string source, std::string target);


//--------------------------------------------------------

template<class T>
T* MaterialMatcher<T>::get(DFHack::t_matglossPair matPair)
{
    if (matList.count(matPair))
        return &matList[matPair].item;
    else return NULL;
}

template<class T>
int MaterialMatcher<T>::set(T input, std::string token, google::protobuf::RepeatedPtrField< ::RemoteFortressReader::MaterialDefinition >* matTokenList)
{
    int count = 0;
    for (int i = 0; i < matTokenList->size(); i++)
    {
        int match = FuzzyCompare(token, matTokenList->Get(i).id());
        if (match < 0)
            continue;
        DFHack::t_matglossPair pair;
        pair.index = matTokenList->Get(i).mat_pair().mat_index();
        pair.type = matTokenList->Get(i).mat_pair().mat_type();
        if (matList.count(pair))
        {
            if (matList[pair].difference <= match) //dont't overwrite old ones that are equal.
                continue;
        }
        matList[pair].item = input;
        matList[pair].difference = match;
        count++;
    }
    return 0;
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
        DFHack::t_matglossPair pair;
        pair.type = -1;
        pair.index = int32_t(i);

        int match = FuzzyCompare(token, pp->id + ":BASE");
        if (match >= 0 && (!matList.count(pair) || matList.at(pair).difference > match))
        {
            matList[pair].item = input;
            matList[pair].difference = match;
        }

        for (size_t g = 0; g < pp->growths.size(); g++)
        {
            auto growth = pp->growths[g];
            if (!growth)
                continue;
            for (int l = 0; l < (sizeof(growth_locations) / sizeof(growth_locations[0])); l++)
            {
                pair.type = g * 10 + l;

                match = FuzzyCompare(token, pp->id + ":" + growth->id + ":" + growth_locations[l]);
                if (match >= 0 && (!matList.count(pair) || matList.at(pair).difference > match))
                {
                    matList[pair].item = input;
                    matList[pair].difference = match;
                }
            }
        }
    }
}

template<class T>
void MaterialMatcher<T>::clear()
{
    matList.clear();
}
