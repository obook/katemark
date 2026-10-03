// previewutil.h
// Small helpers shared by the source files of the preview.
// Code by uwuclxdy, moved out of previewwidget.cpp.
// License: GPL-3.0-or-later

#pragma once

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

inline QString readAsset(const QString &path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) {
        return QString();
    }
    return QString::fromUtf8(f.readAll());
}

// Make an arbitrary string safe to inline inside a <script> block.
inline QString shieldScript(QString js)
{
    return js.replace(QLatin1String("</script"), QLatin1String("<\\/script"), Qt::CaseInsensitive);
}

// Encode a string as a JavaScript string literal (incl. surrounding quotes).
inline QString jsLiteral(const QString &s)
{
    const QJsonDocument doc(QJsonArray{s});
    const QByteArray json = doc.toJson(QJsonDocument::Compact); // ["..."]
    return QString::fromUtf8(json.mid(1, json.size() - 2));
}

inline QString compactJson(const QJsonObject &obj)
{
    return QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact));
}
