#pragma once
#include <QDialog>
#include <json.hpp>

QT_BEGIN_NAMESPACE
namespace Ui { class AnalysisResultDialog; }
QT_END_NAMESPACE

class AnalysisResultDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AnalysisResultDialog(const nlohmann::json& result, QWidget* parent = nullptr);
    ~AnalysisResultDialog();

private slots:
    void drawChart();
    void onFileChanged();  

private:
    Ui::AnalysisResultDialog* ui;
    nlohmann::json resultJson;

    void populateTree(const nlohmann::json& result);
};
