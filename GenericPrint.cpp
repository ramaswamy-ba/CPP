#include <format>
#include <iostream>
//generic formatted print with std:::vformat
constexpr void print(const std::string_view str_fmt, auto&&... args) 
{
    std::fputs(std::vformat(str_fmt, std::make_format_args(args...)).c_str(), stdout);
}

int main() 
{
    int num {42};
    print("The  number is {}\n",  num);
}
