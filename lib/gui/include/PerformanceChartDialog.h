#pragma once
#include <QDialog>
#include <map>
#include <string>

QT_BEGIN_NAMESPACE
namespace Ui { class PerformanceChartDialog; }
QT_END_NAMESPACE

class PerformanceChartDialog : public QDialog
{
    Q_OBJECT

public:
    PerformanceChartDialog(const std::map<std::string, std::string>& texts, QWidget* parent = nullptr);
    ~PerformanceChartDialog();

private:
    Ui::PerformanceChartDialog* ui;
    std::map<std::string, std::string> loadedTexts;

    void runPerformanceTestsAndWriteCSV();
    void loadChartData();
};
