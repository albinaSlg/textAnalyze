#include "TextAnalyzer.h"
#include "TextManager.h"
#include <Windows.h>
#include <iostream>
#include <locale>  

// Фабрика для створення модулів аналізу
//class StatisticsFactory {
//public:
//    static std::shared_ptr<IStatisticModule> createModule(int choice) {
//        switch (choice) {
//        case 1: return std::make_shared<WordCountModule>();
//        case 2: return std::make_shared<UniqueWordCountModule>();
//        case 3: return std::make_shared<WordFrequencyModule>();
//        case 4: return std::make_shared<WordLengthDistributionModule>();
//        case 5: return std::make_shared<SyllableCountModule>();
//        case 6: return std::make_shared<SentenceCountModule>();
//        case 7: return std::make_shared<SentenceLengthModule>();
//        default: return nullptr;
//        }
//    }
//
//    static void showAvailableModules() {
//        std::cout << "Доступні методи аналізу:\n";
//        std::cout << "1 - Кількість слів\n";
//        std::cout << "2 - Кількість унікальних слів\n";
//        std::cout << "3 - Частотний розподіл слів\n";
//        std::cout << "4 - Розподіл слів за довжиною (у символах)\n";
//        std::cout << "5 - Розподіл слів за довжиною у складах\n";
//        std::cout << "6 - Кількість речень та їх розподіл за довжиною\n";
//        std::cout << "7 - Розподіл речень за довжиною у символах та словах\n";
//    }
//};
//
//
//
//
//// Основний клас аналізу тексту
//

void validateTextUtf8(const std::string& text) {
    try {
        nlohmann::json test = text;  // это выбросит исключение, если text невалиден
    }
    catch (const std::exception& e) {
        std::cerr << "INVALID TEXT: " << e.what() << std::endl;
        throw; // или exit(1);
    }
}

// Головна функція з меню вибору
int main() {

    /*try {
        std::string filename;
        std::cout << "Введіть ім'я файлу для аналізу: ";
        std::cin >> filename;

        std::string text = TextManager::readTextFromFile(filename);

        TextAnalyzer analyzer;
        int choice;

        do {
            StatisticsFactory::showAvailableModules();
            std::cout << "Введіть номер методу аналізу (0 - завершити вибір): ";
            std::cin >> choice;

            if (choice != 0) {
                auto module = StatisticsFactory::createModule(choice);
                if (module) {
                    analyzer.addModule(module);
                }
                else {
                    std::cout << "Невірний вибір, спробуйте ще раз.\n";
                }
            }
        } while (choice != 0);

        std::cout << "\nРезультати аналізу:\n";
        analyzer.analyzeText(text);

    }
    catch (const std::exception& e) {
        std::cerr << "Помилка: " << e.what() << std::endl;
    }*/
    //std::locale loc;
    SetConsoleCP(65001); // встановлення кодування Windows-1251 в  потік введення
    SetConsoleOutputCP(65001);

    auto text = TextManager::readTextFromFile("D:/diplom/files/23.txt");//, "D:/diplom/files/text2.txt", "D:/diplom/files/text3.txt");
    //std::map<std::string, std::string> testTexts = {
    //{"test.txt", "apple apple apple apple apple apple apple apple apple apple apple apple apple apple apple apple apple apple apple apple apple"} // строка из 103 символов
    //};

    //std::map<std::string, std::string> testTexts1 = {
    //    {"test1.txt", "Привіт"}
    //};

    //for (auto t : text)
    //    std::cout << t.second;
    //    validateTextUtf8(t.second); // где content — это текст из файла

    TextAnalyzer analyzer;
    nlohmann::json report = analyzer.analyzeText(text);
    //nlohmann::json report = analyzer.analyzeTextBy(ModuleName::WordCountModule, text);

   std::cout << report.dump(4);

    //std::string word = "Привіт";
    //std::cout << "Word: " << word << std::endl;
    //std::cout << "Cleaned: " << cleanWord(word) << std::endl;
    //std::cout << "Syllables: " << countSyllables(word) << std::endl;

    //std::string word2 = "всім";
    //std::cout << "Syllables (всім): " << countSyllables(word2) << std::endl;


    //TextAnalyzer analyzer;
    //std::cout << analyzer.analyzeTextParallel(text).dump(4);
    //analyzer.analyzeTextParallel(text);
    //std::cout << analyzer.analyzeText(text).dump(4);
    //std::cout << analyzer.analyzeTextBy(ModuleName::WordCountModule, text).dump(4);

    //auto texts = TextManager::readTextFromFile("D:/diplom/files/text.txt", "D:/diplom/files/text2.txt", "D:/diplom/files/text3.txt");

    //for (const auto& [filename, content] : texts) {
    //    std::cout << "Файл: " << filename << "\n---\n" << content << "\n\n";
    //}

    return 0;
}
