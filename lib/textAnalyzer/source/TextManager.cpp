#include "TextManager.h"
#include <fstream>

void TextManager::insertFile(std::map<std::string, std::string>& map, const std::string& filename)
{
    std::ifstream file(filename);
    if (!file)
        throw std::runtime_error("Не вдалося відкрити файл: " + filename);

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    if (content.empty()) {
        throw std::runtime_error("Файл порожній або не прочитан: " + filename);
    }
    map[filename] = std::move(content);
}
