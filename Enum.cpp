
#include <stdio.h>
#include <iostream>
#include <map>
#include <string>

#include <map>
#include <string>

#define STRINGIFY(x) #x
#define ENUM_TO_STRING(x) STRINGIFY(x)

#define DECLARE_ENUM(NAME, ...)                                         \
    enum class NAME { __VA_ARGS__, COUNT };                             \
    const char* NAME##Strings[] = { ENUM_TO_STRING((__VA_ARGS__)) };    \
    std::string NAME##_to_string(NAME value)                            \
    {                                                                   \
        return NAME##Strings[static_cast<int>(value)];                  \
    }                                                                   \
                                                                        \
    NAME NAME##_to_enum(const std::string& str)                         \
    {                                                                   \
        for (int i = 0; i < static_cast<int>(NAME::COUNT); ++i)         \
            if (str == NAME##Strings[i])                                \
                return static_cast<NAME>(i);                            \
        return NAME::RED; /* Default value */                           \
    }

DECLARE_ENUM(Color, RED, GREEN, BLUE, BLACK)

int main() 
{
    std::cout<<"Test\n";
    std::cout<<"NumericValue "<<(int)Color_to_enum("GREEN")<<'\n';
    std::string str = Color_to_string(Color::GREEN);
    std::cout<<"Res "<<str<<'\n';
    
    std::cout<<"Values "<<Color_to_string(Color::GREEN)<<'\n';
    
    return 0;
}

