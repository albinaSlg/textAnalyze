#include "TextAnalyzer.h"
#include "Modules.h"
#include "MetricCollector.h"
#include <iostream>
#include <thread>
#include <regex>
#include <unordered_set>

TextAnalyzer::TextAnalyzer()
    : pool(4)
{
    addModule(ModuleName::WordCountModule, std::make_unique<WordCountModule>());
    addModule(ModuleName::UniqueWordCountModule, std::make_unique<UniqueWordCountModule>());
    addModule(ModuleName::WordFrequencyModule, std::make_unique<WordFrequencyModule>());
    addModule(ModuleName::WordLengthDistributionModule, std::make_unique<WordLengthDistributionModule>());
    addModule(ModuleName::SyllableCountModule, std::make_unique<SyllableCountModule>());
    addModule(ModuleName::SentenceCountModule, std::make_unique<SentenceCountModule>());
    addModule(ModuleName::SentenceLengthModule, std::make_unique<SentenceLengthModule>());
    addModule(ModuleName::LetterFrequencyModule, std::make_unique<LetterFrequencyModule>());
}

void TextAnalyzer::addModule(ModuleName moduleName, std::unique_ptr<IStatisticModule> module)
{
    if (module)
        modules.emplace(moduleName, std::move(module));
}

nlohmann::json TextAnalyzer::analyzeSingleText(const std::pair<std::string, std::string>& text)
{
    MetricCollector collector;
    for (const auto& module : modules)
    {
            module.second->analyze(text.second, collector);
    }

    return nlohmann::json{ {text.first, collector.extract()} };
}

nlohmann::json TextAnalyzer::analyzeSingleTextBy(ModuleName moduleName ,const std::pair<std::string, std::string>& text)
{
    auto module = modules.find(moduleName);
    if (module != modules.end())
    {
        nlohmann::json result;

        MetricCollector collector;
        module->second->analyze(text.second, collector);
        result[text.first] = collector.extract();

        return result;
    }
}

nlohmann::json TextAnalyzer::analyzeText(const std::map<std::string, std::string>& texts)
{
    nlohmann::json result;

    for (const auto& text : texts)
    {
        result.update(analyzeSingleText(text));
    }

    return result;
}

nlohmann::json TextAnalyzer::analyzeTextBy(ModuleName moduleName, const std::map<std::string, std::string>& texts)
{
    auto module = modules.find(moduleName);
    if (module != modules.end())
    {
        nlohmann::json result;

        for (const auto& text : texts)
        {
            MetricCollector collector;
            module->second->analyze(text.second, collector);
            result[text.first] = collector.extract();
        }   

        return result;
    }
}

nlohmann::json TextAnalyzer::analyzeTextParallel(const std::map<std::string, std::string>& texts)
{
    nlohmann::json fullReport;
    MultiFuture<nlohmann::json> multi;

    constexpr size_t THRESHOLD = 1000;

    for (const auto& text : texts)
    {
        //if (text.second.size() > THRESHOLD) {
        //    // ќбрабатываем синхронно большим методом Ч сразу, без submit
        //    nlohmann::json result = analyzeLargeTextBySentenceBlocks(text);
        //    fullReport.update(result);
        //}
        //else {
             //ћелкий файл Ч асинхронно в пуле
            auto fut = pool.submit([&, text]() -> nlohmann::json {
                std::cout << "Start analyzing: " << text.first
                    << " in thread " << std::this_thread::get_id() << "\n";

               // nlohmann::json fileResult = analyzeSingleTextBy(ModuleName::WordCountModule, text);
                nlohmann::json fileResult = analyzeSingleText(text);

                std::cout << "Finished analyzing: " << text.first
                    << " in thread " << std::this_thread::get_id() << "\n";

                return fileResult;
                });

            multi.pushFuture(fut.share());
        //}
    }

    for (const auto& result : multi.get())
        fullReport.update(result);

    return fullReport;
}

nlohmann::json TextAnalyzer::analyzeLargeTextBySentenceBlocks(const std::pair<std::string, std::string>& text)
{
    std::regex sentenceEnd(R"(([^.!?]+[.!?]?)(?=\s|$))");
    std::sregex_iterator begin(text.second.begin(), text.second.end(), sentenceEnd), end;
    std::vector<std::string> sentences;

    for (auto it = begin; it != end; ++it) {
        std::string s = it->str();
        if (!s.empty()) sentences.push_back(s);
    }

    if (sentences.empty()) {
        return nlohmann::json{ {text.first, {}} };
    }

    size_t blockCount = std::min(pool.threadsCount(), sentences.size());
    size_t blockSize = static_cast<size_t>(std::ceil((double)sentences.size() / blockCount));

    std::vector<std::string> blocks;

    for (size_t i = 0; i < sentences.size(); i += blockSize) {
        std::string part;
        for (size_t j = i; j < std::min(i + blockSize, sentences.size()); ++j) {
            part += sentences[j] + " ";
        }
        blocks.push_back(part);
    }

    MultiFuture<MetricCollector> multi;

    for (const auto& part : blocks) {
        auto fut = pool.submit([=, this]() -> MetricCollector {
            std::ostringstream log;
            log << "Analyzing block (sentences=" << part.size() << ") in thread " << std::this_thread::get_id() << "\n";
            std::cout << log.str();



            MetricCollector partial;
            //auto it = modules.find(ModuleName::WordCountModule);
            //it->second->analyze(part, partial);
            for (const auto& [_, module] : modules)
                module->analyze(part, partial);

            return partial;
        });

        multi.pushFuture(fut.share());
    }

    MetricCollector finalCollector;
    for (auto& partial : multi.get()) {
        finalCollector.merge(partial);
    }

    return nlohmann::json{ {text.first, finalCollector.extract()} };
}







