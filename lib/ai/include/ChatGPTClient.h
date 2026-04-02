#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>

class ChatGPTClient : public QObject {
    Q_OBJECT
public:
    explicit ChatGPTClient(const QString& apiKey, QObject* parent = nullptr);
    void analyzeTextWithMetrics(const QJsonObject& metrics);

signals:
    void analysisReady(const QString& result);
    void errorOccurred(const QString& message);

private slots:
    void onReplyFinished();

private:
    QNetworkAccessManager* manager;
    QString apiKey;
};
