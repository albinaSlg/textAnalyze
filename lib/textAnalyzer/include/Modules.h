#pragma once
#include "IStatisticModule.h"
#include "TextUtils.h"
#include <sstream>
#include <iterator>
#include <regex>
#include <locale>
#include <codecvt>

int countSyllables(const std::string& word) {
    int count = 0;
    const std::string vowels_utf8[] = {
        "а", "е", "є", "и", "і", "ї", "о", "у", "ю", "я",
        "А", "Е", "Є", "И", "І", "Ї", "О", "У", "Ю", "Я"
    };

    for (const auto& vowel : vowels_utf8) {
        size_t pos = 0;
        while ((pos = word.find(vowel, pos)) != std::string::npos) {
            count++;
            pos += vowel.size();
        }
    }

    return count > 0 ? count : 1;
}

class WordCountModule : public IStatisticModule {
public:
    void analyze(const std::string& text, MetricCollector& collector) override {
        std::istringstream stream(text);
        std::string word;
        int count = 0;
        while (stream >> word) {
            word = cleanWord(word);
            if (!word.empty()) count++;
        }
        collector.setWordCount(count);
    }
};

class UniqueWordCountModule : public IStatisticModule {
public:
    void analyze(const std::string& text, MetricCollector& collector) override {
        std::set<std::string> unique;
        std::istringstream stream(text);
        std::string word;
        while (stream >> word) {
            word = cleanWord(word);
            if (!word.empty()) unique.insert(word);
        }
        collector.setUniqueWords(unique);
    }
};

class WordFrequencyModule : public IStatisticModule {
public:
    void analyze(const std::string& text, MetricCollector& collector) override {
        std::map<std::string, int> freq;
        std::istringstream stream(text);
        std::string word;
        while (stream >> word) {
            word = cleanWord(word);
            if (!word.empty()) freq[word]++;
        }
        collector.setWordFrequency(freq);
    }
};

class WordLengthDistributionModule : public IStatisticModule {
public:
    void analyze(const std::string& text, MetricCollector& collector) override {
        std::map<int, int> distribution;
        std::istringstream stream(text);
        std::string word;
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
        while (stream >> word) {
            word = cleanWord(word);
            if (!word.empty()) {
                int len = static_cast<int>(conv.from_bytes(word).size());
                distribution[len]++;
            }
        }
        collector.setWordLengthDistribution(distribution);
    }
};

class SyllableCountModule : public IStatisticModule {
public:
    void analyze(const std::string& text, MetricCollector& collector) override {
        std::map<int, int> distribution;
        std::istringstream stream(text);
        std::string word;
        while (stream >> word) {
            word = cleanWord(word);
            if (!word.empty()) {
                int count = countSyllables(word);
                distribution[count]++;
            }
        }
        collector.setSyllableDistribution(distribution);
    }
};

class SentenceCountModule : public IStatisticModule {
public:
    void analyze(const std::string& text, MetricCollector& collector) override {
        std::vector<int> lengths;
        std::regex splitter(R"([^.!?…]+)");
        auto begin = std::sregex_iterator(text.begin(), text.end(), splitter);
        auto end = std::sregex_iterator();

        for (auto it = begin; it != end; ++it) {
            std::string sentence = std::regex_replace(it->str(), std::regex("^\\s+|\\s+$"), "");
            if (sentence.empty()) continue;

            std::istringstream stream(sentence);
            std::string word;
            int count = 0;
            while (stream >> word) {
                word = cleanWord(word);
                if (!word.empty()) count++;
            }
            if (count > 0) lengths.push_back(count);
        }

        collector.setSentenceCount(static_cast<int>(lengths.size()));
        collector.setSentenceLengths(lengths);
    }
};