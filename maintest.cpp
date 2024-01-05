#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>

int main(int argc, char* argv[])
{
    std::ifstream ifs("/Users/marc-andrelord/Downloads/chatGPT.jpg");
    (void) argv;
    (void) argc;

    if(!ifs)
    {
        return -1;
    }

    std::vector<char> data = std::vector<char>(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());

    ifs.clear();
    ifs.seekg(0, std::ios::beg);

    //If you really need it in a string you can initialize it the same way as the vector
    std::string data2 = std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());

    std::cout << data2 << std::endl;

    //for(std::vector<char>::iterator it = data.begin(); it != data.end(); ++it) {
    //    std::cout << *it;
    //}

    //std::for_each(data.begin(), data.end(), [](char c) { std::cout << c; });

    //std::cin.get();
    return 0;
}