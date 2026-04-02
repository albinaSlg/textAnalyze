#pragma once
#include <string>
#include <map>
#include <set>
#include <vector>
#include <json.hpp>

struct TextMetrics {
    int wordCount = 0;
    int sentenceCount = 0;

    std::set<std::string> uniqueWords;
    std::map<std::string, int> wordFrequency;
    std::map<int, int> wordLengthDistribution;
    std::map<int, int> syllableDistribution;
    std::vector<int> sentenceLengths;

    std::map<std::string, int> letterFrequency;
};


inline void to_json(nlohmann::json& j, const TextMetrics& m) {
    // Преобразуем int-ключи в строки
    std::map<std::string, int> wordLengthMap;
    for (const auto& [k, v] : m.wordLengthDistribution)
        wordLengthMap[std::to_string(k)] = v;

    std::map<std::string, int> syllableMap;
    for (const auto& [k, v] : m.syllableDistribution)
        syllableMap[std::to_string(k)] = v;

    // Преобразуем set в вектор
    std::vector<std::string> uniqueWordsVec(m.uniqueWords.begin(), m.uniqueWords.end());

    j = {
        {"wordCount", m.wordCount},
        {"uniqueWords", uniqueWordsVec},
        {"wordFrequency", m.wordFrequency},
        {"wordLengthDistribution", wordLengthMap},
        {"syllableDistribution", syllableMap},
        {"sentenceCount", m.sentenceCount},
        {"sentenceLengths", m.sentenceLengths},
        {"letterFrequency", m.letterFrequency}
    };
}