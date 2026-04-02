#include "AnalysisResultDialog.h"
#include "ui_AnalysisResultDialog.h"
#include <QtCharts>
#include <QTreeWidgetItem>
#include <QMessageBox>
#include <set>

AnalysisResultDialog::AnalysisResultDialog(const nlohmann::json& result, QWidget* parent)
    : QDialog(parent), ui(new Ui::AnalysisResultDialog), resultJson(result)
{
    ui->setupUi(this);
    populateTree(result);

    ui->fileComboBox->addItem("Усі файли", "*");
    for (auto& [fileName, stats] : result.items()) {
        ui->fileComboBox->addItem(QString::fromStdString(fileName), QString::fromStdString(fileName));
    }

    connect(ui->drawButton, &QPushButton::clicked, this, &AnalysisResultDialog::drawChart);
    connect(ui->fileComboBox, &QComboBox::currentIndexChanged,
        this, &AnalysisResultDialog::onFileChanged);
}

AnalysisResultDialog::~AnalysisResultDialog()
{
    delete ui;
}

void AnalysisResultDialog::onFileChanged()
{
    QString selectedFile = ui->fileComboBox->currentData().toString();
    ui->chartComboBox->clear();

    if (selectedFile == "*") {
        ui->chartComboBox->addItem("Кількість слів", "wordCount");
        ui->chartComboBox->addItem("Унікальні слова", "uniqueWords");
        ui->chartComboBox->addItem("Кількість речень", "sentenceCount");
    }
    else {
        ui->chartComboBox->addItem("Довжина слів", "lengthDistribution");
        ui->chartComboBox->addItem("Склади в словах", "syllableDistribution");
        ui->chartComboBox->addItem("Довжина речень", "sentenceLengths");
    }
}

void AnalysisResultDialog::populateTree(const nlohmann::json& result)
{
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
    ui->chartComboBox->addItem("Кількість слів", "wordCount");
    ui->chartComboBox->addItem("Унікальні слова", "uniqueWords");
    ui->chartComboBox->addItem("Кількість речень", "sentenceCount");
}

void AnalysisResultDialog::drawChart()
{
    QString selectedFile = ui->fileComboBox->currentData().toString();
    QString selectedMetric = ui->chartComboBox->currentData().toString();

    QChart* chart = new QChart();

    if (selectedFile == "*") {
        QBarSet* set = new QBarSet(selectedMetric);
        QStringList categories;

        for (auto& [fileName, stats] : resultJson.items()) {
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
        auto it = resultJson.find(selectedFile.toStdString());
        if (it == resultJson.end() || !it->is_object()) {
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

        if (selectedMetric == "lengthDistribution" || selectedMetric == "syllableDistribution") {
            if (!value.is_array()) {
                QMessageBox::warning(this, "Помилка", "Очікується масив пар.");
                return;
            }

            QBarSet* set = new QBarSet(selectedMetric);
            QStringList categories;

            for (const auto& pair : value) {
                if (pair.is_array() && pair.size() == 2 &&
                    pair[0].is_number() && pair[1].is_number()) {

                    int length = pair[0].get<int>();
                    int count = pair[1].get<int>();

                    *set << count;
                    categories << QString::number(length);
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

            chart->setTitle(QString("%1 → Кількість").arg(selectedMetric));
        }
        else if (selectedMetric == "sentenceLengths" && value.is_array()) {
            std::map<int, int> frequency;

            for (const auto& item : value) {
                if (item.is_number()) {
                    int len = item.get<int>();
                    frequency[len]++;
                }
            }

            QBarSet* barSet = new QBarSet("Довжина речень");
            QStringList categories;
            for (const auto& [length, count] : frequency) {
                *barSet << count;
                categories << QString::number(length);
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
            chart->addAxis(axisY, Qt::AlignLeft);
            series->attachAxis(axisY);
        }
    }

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    ui->chartView->setChart(chart);
}

