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
    int similarity = target.size() - source.size();
    for (int i = 0; i < source.size(); i++)
    {
        switch (source[i])
        {
        case '*':
            similarity += 2;
            break;
        case '?':
            similarity += 1;
            break;
        }
    }
    return similarity;
}
