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
    T noMat;
public:
    T get(DFHack::t_matglossPair);
    T get();
    int set(T input, std::string token, google::protobuf::RepeatedPtrField< ::RemoteFortressReader::MaterialDefinition >* matTokenList);
    void set(T input);
};

int FuzzyCompare(std::string source, std::string target);
