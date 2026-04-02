#pragma once

#include "TextMetrics.h"
#include <map>
#include <set>
#include <string>
#include <vector>

class MetricCollector {
public:
    void setWordCount(int count) {
        metrics.wordCount = count;
    }

    void setUniqueWords(const std::set<std::string>& words) {
        metrics.uniqueWords = words;
    }

    void setWordFrequency(const std::map<std::string, int>& frequency) {
        metrics.wordFrequency = frequency;
    }

    void setWordLengthDistribution(const std::map<int, int>& distribution) {
        metrics.wordLengthDistribution = distribution;
    }

    void setSyllableDistribution(const std::map<int, int>& distribution) {
        metrics.syllableDistribution = distribution;
    }

    void setSentenceCount(int count) {
        metrics.sentenceCount = count;
    }

    void setSentenceLengths(const std::vector<int>& lengths) {
        metrics.sentenceLengths = lengths;
    }

    void setLetterFrequency(const std::map<std::string, int>& frequency) {
        metrics.letterFrequency = frequency;
    }

    const TextMetrics& extract() const {
        return metrics;
    }

    void merge(const MetricCollector& other) {
        metrics.wordCount += other.metrics.wordCount;
        metrics.sentenceCount += other.metrics.sentenceCount;

        metrics.uniqueWords.insert(
            other.metrics.uniqueWords.begin(), other.metrics.uniqueWords.end());

        for (const auto& [k, v] : other.metrics.wordFrequency)
            metrics.wordFrequency[k] += v;

        for (const auto& [k, v] : other.metrics.wordLengthDistribution)
            metrics.wordLengthDistribution[k] += v;

        for (const auto& [k, v] : other.metrics.syllableDistribution)
            metrics.syllableDistribution[k] += v;

        metrics.sentenceLengths.insert(
            metrics.sentenceLengths.end(),
            other.metrics.sentenceLengths.begin(),
            other.metrics.sentenceLengths.end());

        for (const auto& [k, v] : other.metrics.letterFrequency)
            metrics.letterFrequency[k] += v;
    }

private:
    TextMetrics metrics;
};
