// previewutil.h
// Small helpers shared by the source files of the preview.
// Code by uwuclxdy and Olivier Booklage.
// License: GPL-3.0-or-later

#pragma once

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QUrl>

#include <KTextEditor/Document>

inline QString readAsset(const QString &path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) {
        return QString();
    }
    return QString::fromUtf8(f.readAll());
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

// A document holding one Mermaid diagram and no Markdown. The mode decides first: older
// syntax definitions give .mmd, also a MultiMarkdown extension, to Markdown.
inline bool isMermaid(KTextEditor::Document *doc)
{
    const QString mode = doc->highlightingMode();
    if (mode.compare(QLatin1String("Mermaid"), Qt::CaseInsensitive) == 0) {
        return true;
    }
    if (mode.compare(QLatin1String("Markdown"), Qt::CaseInsensitive) == 0) {
        return false;
    }
    const QString path = doc->url().path();
    return path.endsWith(QLatin1String(".mmd"), Qt::CaseInsensitive) || path.endsWith(QLatin1String(".mermaid"), Qt::CaseInsensitive);
}

// The diagram as a Markdown code block. An empty one stays empty: Mermaid would show an error.
// ponytail: a line of three backticks in the diagram ends the block; a longer fence if one does.
inline QString mermaidAsMarkdown(const QString &diagram)
{
    if (diagram.trimmed().isEmpty()) {
        return diagram;
    }
    return QLatin1String("```mermaid\n") + diagram + QLatin1String("\n```\n");
}

// "//host/share/x" or "\\host\share\x": a file on another machine under Windows. Looking
// it up contacts that machine and hands it the user's credentials. Read from the path,
// not from QUrl::host(): "//host@SSL@443/x" has no host for QUrl, yet Windows goes there.
inline bool isNetworkPath(const QString &path)
{
    return path.startsWith(QLatin1String("//")) || path.startsWith(QLatin1String("\\\\"));
}

// The file a clicked link means. The page resolves every link against the folder of the
// document (base), so "~/notes" arrives as "<folder>/~/notes", and a wiki link written
// without its extension names a file that is not there.
inline QUrl localLinkTarget(const QUrl &url, const QUrl &base)
{
    QString path = url.toLocalFile();
    // Left as written: looking it up while the pointer passes over it would reach the host.
    if (isNetworkPath(path)) {
        return url;
    }
    if (QFileInfo::exists(path)) {
        return url;
    }
    const QString tilde = base.toLocalFile() + QLatin1String("~/");
    if (path.startsWith(tilde)) {
        path = QDir::homePath() + QLatin1Char('/') + path.mid(tilde.size());
    }
    if (!QFileInfo::exists(path) && QFileInfo::exists(path + QLatin1String(".md"))) {
        path += QLatin1String(".md");
    }
    QUrl target = QUrl::fromLocalFile(path);
    target.setFragment(url.fragment());
    return target;
}

// What to show while the pointer is on a link: "#heading" for a heading of this page,
// the path of a local file, the address of anything else.
inline QString linkHint(const QUrl &url, const QUrl &base)
{
    if (url.hasFragment() && url.matches(base, QUrl::RemoveFragment)) {
        return QLatin1Char('#') + url.fragment();
    }
    if (url.isLocalFile()) {
        return localLinkTarget(url, base).toDisplayString(QUrl::PreferLocalFile);
    }
    return url.toDisplayString();
}
