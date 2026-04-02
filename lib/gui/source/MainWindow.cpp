#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "TextManager.h"
#include <QtCharts>
#include <QFileDialog>
#include <QMessageBox>
#include <QTreeWidgetItem>
#include <set>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QFile styleFile(":/style.qss");
    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qApp->setStyleSheet(styleFile.readAll());
    }
    connect(ui->openButton, &QPushButton::clicked, this, &MainWindow::onOpenFiles);
    connect(ui->showResultsButton, &QPushButton::clicked, this, &MainWindow::onShowAnalysisResults);
    connect(ui->showPerformanceButton, &QPushButton::clicked, this, &MainWindow::onShowPerformanceChart);
    connect(ui->drawButton, &QPushButton::clicked, this, &MainWindow::onDrawChart);
    connect(ui->fileComboBox, &QComboBox::currentIndexChanged, this, &MainWindow::onSelectedFileChanged);
    connect(ui->analyzeInputButton, &QPushButton::clicked, this, &MainWindow::onAnalyzeInputText);
    connect(gptClient, &ChatGPTClient::analysisReady, this, &MainWindow::onGptResultReady);
    connect(gptClient, &ChatGPTClient::errorOccurred, this, &MainWindow::onGptError);
    connect(ui->gptButton, &QPushButton::clicked, this, &MainWindow::onGptButtonClicked);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onGptButtonClicked()
{
    QString selectedFile = ui->fileComboBox->currentData().toString();
    if (selectedFile == "*" || !analysisResult.contains(selectedFile.toStdString())) {
        QMessageBox::warning(this, "Помилка", "Оберіть конкретний файл.");
        return;
    }

    QJsonObject metrics;
    const auto& stats = analysisResult[selectedFile.toStdString()];
    for (auto& [key, value] : stats.items()) {
        if (value.is_number()) {
            metrics[QString::fromStdString(key)] = value.get<double>();
        }
        else if (value.is_array() && key == "syllableDistribution") {
            int total = 0, weighted = 0;
            for (const auto& pair : value) {
                if (pair.is_array() && pair.size() == 2)
                    weighted += pair[0].get<int>() * pair[1].get<int>(), total += pair[1].get<int>();
            }
            if (total > 0)
                metrics["avg_syllables"] = double(weighted) / total;
        }
    }

    gptClient->analyzeTextWithMetrics(metrics);
}

void MainWindow::onGptResultReady(const QString& result)
{
    QMessageBox::information(this, "GPT-висновок", result);
}

void MainWindow::onGptError(const QString& error)
{
    QMessageBox::critical(this, "GPT-помилка", error);
}


void MainWindow::onAnalyzeInputText()
{
    QString input = ui->inputTextEdit->toPlainText();
    if (input.trimmed().isEmpty()) {
        QMessageBox::warning(this, "Порожній текст", "Введіть текст для аналізу.");
        return;
    }

    std::map<std::string, std::string> inputMap;
    inputMap["Введений текст"] = input.toStdString();

    analysisResult = analyzer.analyzeTextParallel(inputMap);
    populateTree(analysisResult);

    ui->fileComboBox->clear();
    ui->fileComboBox->addItem("Введений текст", "Введений текст");
    onSelectedFileChanged();
}

void MainWindow::onOpenFiles()
{
    QStringList files = QFileDialog::getOpenFileNames(this, "Оберіть текстові файли");
    loadedTexts.clear();

    for (const QString& file : files)
    {
        try {
            loadedTexts[file.toStdString()] = TextManager::readTextFromFile(file.toStdString())[file.toStdString()];
        }
        catch (const std::exception& e) {
            QMessageBox::warning(this, "Помилка", e.what());
        }
    }

    QMessageBox::information(this, "Готово", QString("Завантажено файлів: %1").arg(loadedTexts.size()));
}

void MainWindow::onShowAnalysisResults()
{
    if (loadedTexts.empty()) {
        QMessageBox::warning(this, "Немає файлів", "Спочатку оберіть файли.");
        return;
    }
    analysisResult = analyzer.analyzeTextParallel(loadedTexts);
    populateTree(analysisResult);

    ui->fileComboBox->clear();
    ui->fileComboBox->addItem("Усі файли", "*");
    for (auto& [fileName, stats] : analysisResult.items()) {
        ui->fileComboBox->addItem(QString::fromStdString(fileName), QString::fromStdString(fileName));
    }
    onSelectedFileChanged();
}

void MainWindow::onShowPerformanceChart()
{
    QMessageBox::information(this, "Інфо", "Графік продуктивності залишено без змін.");
}

void MainWindow::onSelectedFileChanged()
{
    QString selectedFile = ui->fileComboBox->currentData().toString();
    ui->chartComboBox->clear();

    if (selectedFile == "*") {
        ui->chartComboBox->addItem("Кількість слів", "wordCount");
        ui->chartComboBox->addItem("Унікальні слова", "uniqueWords");
        ui->chartComboBox->addItem("Кількість речень", "sentenceCount");
    }
    else {
        ui->chartComboBox->addItem("Довжина слів", "wordLengthDistribution");
        ui->chartComboBox->addItem("Склади в словах", "syllableDistribution");
        ui->chartComboBox->addItem("Довжина речень", "sentenceLengths");
    }
}

void MainWindow::populateTree(const nlohmann::json& result)
{
    ui->treeWidget->clear();
    for (auto& [fileName, stats] : result.items()) {
        QTreeWidgetItem* fileItem = new QTreeWidgetItem();
        fileItem->setText(0, QString::fromStdString(fileName));
        for (auto& [key, value] : stats.items()) {
            QTreeWidgetItem* statItem = new QTreeWidgetItem();
            statItem->setText(0, QString::fromStdString(key));
            statItem->setText(1, QString::fromStdString(value.dump()));
            fileItem->addChild(statItem);
        }
        ui->treeWidget->addTopLevelItem(fileItem);
    }
    ui->treeWidget->expandAll();
}

void MainWindow::onDrawChart()
{
    QString selectedFile = ui->fileComboBox->currentData().toString();
    QString selectedMetric = ui->chartComboBox->currentData().toString();

    QChart* chart = new QChart();

    if (selectedFile == "*") {
        QBarSet* set = new QBarSet(selectedMetric);
        QStringList categories;

        for (auto& [fileName, stats] : analysisResult.items()) {
            if (selectedMetric == "uniqueWords" && stats[selectedMetric.toStdString()].is_array()) {
                std::set<std::string> unique;
                for (const auto& val : stats[selectedMetric.toStdString()]) {
                    if (val.is_string()) unique.insert(val.get<std::string>());
                }
                *set << static_cast<int>(unique.size());
                categories << QString::fromStdString(fileName);
            }
            else if (stats.contains(selectedMetric.toStdString()) && stats[selectedMetric.toStdString()].is_number()) {
                double val = stats[selectedMetric.toStdString()].get<double>();
                *set << val;
                categories << QString::fromStdString(fileName);
            }
        }

        QBarSeries* series = new QBarSeries();
        series->append(set);
        chart->addSeries(series);

        QBarCategoryAxis* axisX = new QBarCategoryAxis();
        axisX->append(categories);
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);

        QValueAxis* axisY = new QValueAxis();
        chart->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisY);

        chart->setTitle(QString("Порівняння: %1").arg(selectedMetric));
    }
    else {
        auto it = analysisResult.find(selectedFile.toStdString());
        if (it == analysisResult.end() || !it->is_object()) {
            QMessageBox::warning(this, "Помилка", "Файл не знайдено в результатах.");
            return;
        }

        const auto& stats = *it;
        auto metricIt = stats.find(selectedMetric.toStdString());
        if (metricIt == stats.end()) {
            QMessageBox::warning(this, "Помилка", "Метрика не знайдена.");
            return;
        }

        const auto& value = *metricIt;

        if ((selectedMetric == "wordLengthDistribution" || selectedMetric == "syllableDistribution") && value.is_object()) {
            std::map<int, int> sorted;
            for (const auto& [keyStr, countJson] : value.items()) {
                try {
                    int key = std::stoi(keyStr);
                    if (key >= 1 && key <= 30) {
                        int count = countJson.get<int>();
                        sorted[key] = count;
                    }
                }
                catch (...) {
                    // игнорируем некорректные ключи
                }
            }

            QBarSet* set = new QBarSet(selectedMetric);
            QStringList categories;
            int maxY = 0;

            for (const auto& [length, count] : sorted) {
                *set << count;
                categories << QString::number(length);
                if (count > maxY) maxY = count;
            }

            QBarSeries* series = new QBarSeries();
            series->append(set);
            chart->addSeries(series);

            QBarCategoryAxis* axisX = new QBarCategoryAxis();
            axisX->append(categories);
            chart->addAxis(axisX, Qt::AlignBottom);
            series->attachAxis(axisX);

            QValueAxis* axisY = new QValueAxis();
            axisY->setRange(0, maxY + maxY * 0.1);
            chart->addAxis(axisY, Qt::AlignLeft);
            series->attachAxis(axisY);

            chart->setTitle(QString("%1 → Кількість").arg(selectedMetric));
        }
        else if (selectedMetric == "sentenceLengths" && value.is_array()) {
            std::map<int, int> frequency;

            for (const auto& item : value) {
                if (item.is_number()) {
                    int len = item.get<int>();
                    if (len >= 1 && len <= 100)
                        frequency[len]++;
                }
            }

            QBarSet* barSet = new QBarSet("Довжина речень");
            QStringList categories;
            int maxY = 0;

            for (const auto& [length, count] : frequency) {
                *barSet << count;
                categories << QString::number(length);
                if (count > maxY) maxY = count;
            }

            QBarSeries* series = new QBarSeries();
            series->append(barSet);
            chart->addSeries(series);
            chart->setTitle("Частота довжин речень");

            QBarCategoryAxis* axisX = new QBarCategoryAxis();
            axisX->append(categories);
            chart->addAxis(axisX, Qt::AlignBottom);
            series->attachAxis(axisX);

            QValueAxis* axisY = new QValueAxis();
            axisY->setRange(0, maxY + maxY * 0.1);
            chart->addAxis(axisY, Qt::AlignLeft);
            series->attachAxis(axisY);
        }
    }

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    ui->chartView->setChart(chart);
}
