#include "ChatGPTClient.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include <QNetworkRequest>
#include <QDebug>
#include <iostream>

ChatGPTClient::ChatGPTClient(const QString& apiKey, QObject* parent)
    : QObject(parent), apiKey(apiKey)
{
    manager = new QNetworkAccessManager(this);
}

void ChatGPTClient::analyzeTextWithMetrics(const QJsonObject& metrics)
{
    QString prompt = "Оціни текст на основі таких метрик:\n";
    for (auto it = metrics.begin(); it != metrics.end(); ++it) {
        prompt += it.key() + ": " + it.value().toVariant().toString() + "\n";
    }

    QJsonObject userMessage{
        {"role", "user"},
        {"content", prompt}
    };
    QJsonObject systemMessage{
        {"role", "system"},
        {"content", "Ти аналітик текстів. На основі статистичних метрик зроби короткий висновок про якість, складність та читабельність тексту, а також про його стиль. Якщо потрібно, зроби додаткові обчислення на основі отриманних даних"}
    };

    QJsonObject body;
    body["model"] = "gpt-3.5-turbo";
    body["messages"] = QJsonArray{ systemMessage, userMessage };

    QNetworkRequest request(QUrl("https://api.openai.com/v1/chat/completions"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + apiKey.toUtf8());

    QNetworkReply* reply = manager->post(request, QJsonDocument(body).toJson());
    connect(reply, &QNetworkReply::finished, this, &ChatGPTClient::onReplyFinished);
}

void ChatGPTClient::onReplyFinished()
{
    std::cout << "Gere" << std::endl;

    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    QByteArray data = reply->readAll();
    std::cout << "HTTP status code:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() << std::endl;
    std::cout << "Full reply:" << QString::fromUtf8(reply->readAll()).toStdString() << std::endl;

    if (reply->error() != QNetworkReply::NoError) {
        emit errorOccurred("Помилка запиту: " + reply->errorString());
        reply->deleteLater();
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QString content = doc["choices"].toArray()[0].toObject()["message"].toObject()["content"].toString();
    emit analysisReady(content);


    reply->deleteLater();
}
