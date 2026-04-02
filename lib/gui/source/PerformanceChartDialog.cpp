#include "PerformanceChartDialog.h"
#include "ui_PerformanceChartDialog.h"
#include "TextAnalyzer.h"
#include <QtCharts>
#include <fstream>
#include <chrono>
#include <iostream>

PerformanceChartDialog::PerformanceChartDialog(const std::map<std::string, std::string>& texts, QWidget* parent)
    : QDialog(parent), ui(new Ui::PerformanceChartDialog), loadedTexts(texts)
{
    ui->setupUi(this);
    runPerformanceTestsAndWriteCSV();
    loadChartData();
}

PerformanceChartDialog::~PerformanceChartDialog()
{
    delete ui;
}

void PerformanceChartDialog::runPerformanceTestsAndWriteCSV()
{
    TextAnalyzer analyzer;

    auto measure = [](auto&& func) {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(end - start).count();
    };


    double t1 = measure([&]() { analyzer.analyzeText(loadedTexts); });
    double t2 = measure([&]() { analyzer.analyzeTextParallel(loadedTexts); });
   //double t3 = measure([&]() {
   //    for (const auto& text : loadedTexts)
   //        analyzer.analyzeLargeTextBySentenceBlocks(text);
   //    });

    std::ofstream out("performance.csv");
    out << "method,time_sec\n";
    out << "Sequential," << t1 << "\n";
    out << "Parallel," << t2 << "\n";
    //out << "Block-based," << t3 << "\n";
}

void PerformanceChartDialog::loadChartData()
{
    QFile file("performance.csv");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    in.readLine(); // skip header

    QMap<QString, double> methodTime;
    while (!in.atEnd()) {
        auto parts = in.readLine().split(",");
        if (parts.size() != 2) continue;
        methodTime[parts[0]] = parts[1].toDouble();
    }

    // Выводим текстовые показатели
    QString summary;
    double base = methodTime.value("Sequential", 1.0);
    QBarSet* setTime = new QBarSet("Час виконання");
    QBarSet* setSpeedup = new QBarSet("Speedup");
    QStringList categories;

    for (auto it = methodTime.constBegin(); it != methodTime.constEnd(); ++it) {
        double time = it.value();
        double speedup = base / time;
        summary += QString("%1: %2 сек, Speedup: x%3\n")
            .arg(it.key())
            .arg(time, 0, 'f', 4)
            .arg(speedup, 0, 'f', 2);

        *setTime << time;
        *setSpeedup << speedup;
        categories << it.key();
    }
    ui->labelSummary->setText(summary);

    // График времени
    QBarSeries* seriesTime = new QBarSeries();
    seriesTime->append(setTime);
    QChart* chartTime = new QChart();
    chartTime->addSeries(seriesTime);
    chartTime->setTitle("Час виконання методів");

    QBarCategoryAxis* axisX1 = new QBarCategoryAxis();
    axisX1->append(categories);
    chartTime->addAxis(axisX1, Qt::AlignBottom);
    seriesTime->attachAxis(axisX1);

    QValueAxis* axisY1 = new QValueAxis();
    chartTime->addAxis(axisY1, Qt::AlignLeft);
    seriesTime->attachAxis(axisY1);

    chartTime->legend()->setVisible(true);
    chartTime->legend()->setAlignment(Qt::AlignBottom);
    ui->chartViewTime->setChart(chartTime);

    // График ускорения
    QBarSeries* seriesSpeedup = new QBarSeries();
    seriesSpeedup->append(setSpeedup);
    QChart* chartSpeedup = new QChart();
    chartSpeedup->addSeries(seriesSpeedup);
    chartSpeedup->setTitle("Прискорення (Speedup)");

    QBarCategoryAxis* axisX2 = new QBarCategoryAxis();
    axisX2->append(categories);
    chartSpeedup->addAxis(axisX2, Qt::AlignBottom);
    seriesSpeedup->attachAxis(axisX2);

    QValueAxis* axisY2 = new QValueAxis();
    chartSpeedup->addAxis(axisY2, Qt::AlignLeft);
    seriesSpeedup->attachAxis(axisY2);

    chartSpeedup->legend()->setVisible(true);
    chartSpeedup->legend()->setAlignment(Qt::AlignBottom);
    ui->chartViewSpeedup->setChart(chartSpeedup);
}
