#pragma once
#include "TextAnalyzer.h"
#include "ChatGPTClient.h"
#include <QMainWindow>
#include <QTreeWidget>
#include <map>
#include <string>
#include <json.hpp>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onOpenFiles();
    void onShowAnalysisResults();
    void onShowPerformanceChart();
    void onDrawChart();
    void onSelectedFileChanged();
    void onAnalyzeInputText();
    void onGptButtonClicked();
    void onGptResultReady(const QString& result);
    void onGptError(const QString& error);

private:
    Ui::MainWindow* ui;
    std::map<std::string, std::string> loadedTexts;
    TextAnalyzer analyzer;
    nlohmann::json analysisResult;
    ChatGPTClient* gptClient = nullptr;

    void populateTree(const nlohmann::json& result);
};