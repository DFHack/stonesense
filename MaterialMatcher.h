#pragma once

#include "RemoteFortressReader.pb.h"
#include "Types.h"
#include <map>
#include <string>

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
void MaterialMatcher<T>::clear()
{
    matList.clear();
}
