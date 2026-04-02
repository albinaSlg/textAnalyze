#pragma once
#include <string>
#include <map>

class TextManager {
public:
    template<typename... Args>
    static std::map<std::string, std::string> readTextFromFile(const Args&... filenames) 
    {
        std::map<std::string, std::string> result;
        (insertFile(result, filenames), ...);
        return result;
    }

private:
    static void insertFile(std::map<std::string, std::string>& map, const std::string& filename);
};
