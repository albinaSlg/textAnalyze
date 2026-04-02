#include "gtest/gtest.h"
#include "TextAnalyzer.h"
#include "TextManager.h"
#include <chrono>
#include <iostream>

class PerformanceTest : public ::testing::Test {
protected:
    std::map<std::string, std::string> texts;
    TextAnalyzer analyzer;

    void SetUp() override {
        texts = TextManager::readTextFromFile("D:/diplom/files/test1.txt", "D:/diplom/files/test2.txt"); // Подставь нужные файлы
    }

    double measureTime(const std::function<void()>& func) {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(end - start).count();
    }
};

TEST_F(PerformanceTest, SequentialVsParallel) {
    double timeSequential = measureTime([this]() {
        analyzer.analyzeText(texts);
        });

    double timeParallel = measureTime([this]() {
        analyzer.analyzeTextParallel(texts);
        });

    std::cout << "Sequential analyze: " << timeSequential << " sec\n";
    std::cout << "Parallel analyze: " << timeParallel << " sec\n";
    std::cout << "Speedup: " << timeSequential / timeParallel << "\n";

    EXPECT_LT(timeParallel, timeSequential * 0.95); // Ожидаем хотя бы 5% ускорение
}

