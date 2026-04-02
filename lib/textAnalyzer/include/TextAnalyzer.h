#pragma once
#include "IStatisticModule.h"
#include <vector>
#include <map>
#include <memory>
#include <json.hpp>
#include <ThreadPool.h>

enum class ModuleName
{
	WordCountModule,
	UniqueWordCountModule,
	WordFrequencyModule,
	WordLengthDistributionModule,
	SyllableCountModule,
	SentenceCountModule,
	SentenceLengthModule,
	AverageWordLengthModule,
	AverageSentenceLengthInWordsModule,
	TypeTokenRatioModule,
	LetterFrequencyModule
};

class TextAnalyzer
{
public:
	TextAnalyzer();
	~TextAnalyzer() = default;
	TextAnalyzer(const TextAnalyzer& p) = delete;
	TextAnalyzer(TextAnalyzer&& p) = delete;

public:
    void addModule(ModuleName moduleName, std::unique_ptr <IStatisticModule> module);

	nlohmann::json analyzeText(const std::map<std::string, std::string>& texts);

	nlohmann::json analyzeTextBy(ModuleName moduleName, const std::map<std::string, std::string>& texts);

	nlohmann::json analyzeTextParallel(const std::map<std::string, std::string>& texts);

	nlohmann::json analyzeLargeTextBySentenceBlocks(const std::pair<std::string, std::string>& text);

private:
	void mergeJson(nlohmann::json& target, const nlohmann::json& source);
	nlohmann::json analyzeSingleText(const std::pair<std::string, std::string>& text);
	nlohmann::json analyzeSingleTextBy(ModuleName moduleName, const std::pair<std::string, std::string>& text);

public:
    std::map<ModuleName, std::unique_ptr<IStatisticModule>> modules;
	ThreadPool pool;
};

