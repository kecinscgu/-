#include <iostream>
#include <vector>

bool endsWith(std::vector<char> const& fullString, std::vector<char> const& ending);


int main()
{
    

    std::vector<char> *p_header = new std::vector<char>{1,1,1,1};
    std::vector<char> ending{1,1,1};



    std::cout << endsWith(*p_header, ending);
    
    delete p_header;
    return 0;
}

bool endsWith (std::vector<char> const& fullString, std::vector<char> const& ending) {
    if (fullString.size() >= ending.size()) {
        size_t pos = fullString.size() - ending.size();
        for (size_t i{pos}; i < fullString.size(); i++)
        {
            if (fullString[i] != ending[i-pos])
            {
                return false;
            }
        }
        return true;
    } else {
        return false;
    }
}