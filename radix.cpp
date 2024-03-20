/* * https://github.com/rhymu8354/Base64/blob/main/src/Base32.cpp */
#include <map>
#include <stdint.h>
#include <sstream>
#include <fstream>
#include <vector>
#include <iostream>
#include <iterator>
#include <algorithm>
const char EncodingTable[33] = "ABCDE23FGHJK45LMNPQ67RSTUV89WXYZ";
const std::map< char, uint32_t > DecodingTable{
{'A',0},
{'B',1},
{'C',2},
{'D',3},
{'E',4},
{'2',5},
{'3',6},
{'F',7},
{'G',8},
{'H',9},
{'J',10},
{'K',11},
{'4',12},
{'5',13},
{'L',14},
{'M',15},
{'N',16},
{'P',17},
{'Q',18},
{'6',19},
{'7',20},
{'R',21},
{'S',22},
{'T',23},
{'U',24},
{'V',25},
{'8',26},
{'9',27},
{'W',28},
{'X',29},
{'Y',30},
{'Z',31},
};


std::string Encode( const std::vector< uint8_t >& data, bool omitPadding) 
{
    std::ostringstream output;
    size_t bits = 0;
    uint32_t buffer = 0;
    for (auto datum: data) {
        buffer <<= 8;
        buffer += (uint32_t)datum;
        bits += 8;
        while (bits >= 5) {
            output << EncodingTable[(buffer >> (bits - 5)) & 0x3f];
            buffer &= ~(0x1f << (bits - 5));
            bits -= 5;
        }
    }
    if ((data.size() % 5) == 1) {
        buffer <<= 2;
        output << EncodingTable[buffer & 0x1f];
        if (!omitPadding) {
            output << "======";
        }
    } else if ((data.size() % 5) == 2) {
        buffer <<= 4;
        output << EncodingTable[buffer & 0x1f];
        if (!omitPadding) {
            output << "====";
        }
    } else if ((data.size() % 5) == 3) {
        buffer <<= 1;
        output << EncodingTable[buffer & 0x1f];
        if (!omitPadding) {
            output << "===";
        }
    } else if ((data.size() % 5) == 4) {
        buffer <<= 3;
        output << EncodingTable[buffer & 0x1f];
        if (!omitPadding) {
            output << '=';
        }
    }
    return output.str();
}
std::string Encode( const std::string& data, bool omitPadding=0) 
{
    return Encode( std::vector< uint8_t >( data.begin(), data.end()), omitPadding);
}
std::string Decode(const std::vector< uint8_t >& data) 
{
    std::ostringstream output;
    uint32_t buffer = 0;
    size_t bits = 0;
    for (auto datum: data) 
    {
        const auto entry = DecodingTable.find(datum);
        uint32_t group = 0;
        if (entry != DecodingTable.end()) {
            group = entry->second;
        }
        buffer <<= 5;
        bits += 5;
        buffer += group;
        if (bits >= 8) {
            if (datum != '=') {
                output << (char)(buffer >> (bits - 8));
            }
            buffer &= ~(0xff << (bits - 8));
            bits -= 8;
        }
    }
    return output.str();
}
std::string Decode(const std::string& data) 
{
    return Decode( std::vector< uint8_t >( data.begin(), data.end()));
}

void convert_file(const std::string &in_filename, const std::string &out_filename)
{
    std::ifstream in(in_filename.c_str(),std::ios::binary | std::ios::ate);
    size_t len  = in.tellg();
    in.seekg(0, std::ios::beg);
    std::vector<uint8_t>  buf(len);
    if(in.read((char*)buf.data(), len) )
    {
        std::cout<<"=====================DATA ==================== with size "<<len<<'\n';
        std::copy(buf.begin(), buf.end(), std::ostream_iterator<unsigned char>(std::cout,""));std::cout<<'\n';
    }
    else
    {
        std::cout<<"Fail to read the data\n";
    }

    auto enc = Encode(buf,1);
    auto dec = Decode(enc);
    std::cout<<"=====================ENC ==================== with size "<<enc.length()<<'\n';
    std::cout<<enc<<'\n';
    std::cout<<"=====================DEC ==================== with size "<<dec.length()<<'\n';
    std::cout<<dec<<'\n';
}

int main(int argc, char *argv[])
{
    convert_file("2.txt", "");
    return 0;
    std::string src ="RAMASWAMYBA,ramaswamyba, welcome to hong";
    std::string enc = Encode(src,1);
    std::string dec = Decode(enc);
    std::cout<<src<<'\n';
    std::cout<<enc<<'\n';
    std::cout<<dec<<'\n';
    return 0;
}
