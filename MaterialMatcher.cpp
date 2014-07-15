#include "MaterialMatcher.h"

// The main function that checks if two given strings match. The first
// string may contain wildcard characters
bool match(const char *first, const char * second)
{
    // If we reach at the end of both strings, we are done
    if (*first == '\0' && *second == '\0')
        return true;

    // Make sure that the characters after '*' are present in second string.
    // This function assumes that the first string will not contain two
    // consecutive '*'
    if (*first == '*' && *(first + 1) != '\0' && *second == '\0')
        return false;

    // If the first string contains '?', or current characters of both
    // strings match
    if (*first == '?' || *first == *second)
        return match(first + 1, second + 1);

    // If there is *, then there are two possibilities
    // a) We consider current character of second string
    // b) We ignore current character of second string.
    if (*first == '*')
        return match(first + 1, second) || match(first, second + 1);
    return false;
}

int FuzzyCompare(std::string source, std::string target)
{
    if (!match(source.c_str(), target.c_str()))
        return -1;
    int similarity = 0;
    for (int i = 0; i < source.size(); i++)
    {
        if (source[i] != '*')
            similarity++;
    }
    return target.size() - similarity;
}

template<class T>
T MaterialMatcher<T>::get(DFHack::t_matglossPair matPair)
{
    if (matList.count(matPair))
        return matList[matPair].item;
    else return noMat;
}

template<class T>
T MaterialMatcher<T>::get()
{
    return noMat;
}

template<class T>
void MaterialMatcher<T>::set(T input)
{
    noMat = input;
}

template<class T>
int MaterialMatcher<T>::set(T input, std::string token, google::protobuf::RepeatedPtrField< ::RemoteFortressReader::MaterialDefinition >* matTokenList)
{
    int count = 0;
    for (int i = 0; i < matTokenList->size(); i++)
    {
        int match = FuzzyCompare(tokan, matTokenList->Get(i).id());
        if (match < 0)
            continue;
        DFHack::t_matglossPair pair;
        pair.index = matTokenList->Get(i).mat_pair().mat_index();
        pair.type = matTokenList->Get(i).mat_pair().mat_index();
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
