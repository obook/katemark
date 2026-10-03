// previewexport.cpp
// PreviewWidget: exporting the rendered document to PDF or to a standalone HTML file.
// Author: Olivier Booklage
// Date: October 2026
// License: GPL-3.0-or-later

#include "previewtheme.h"
#include "previewutil.h"
#include "previewwidget.h"

#include <QFileInfo>
#include <QJsonObject>
#include <QLocale>
#include <QMimeDatabase>
#include <QPageLayout>
#include <QPageSize>
#include <QPointer>
#include <QRegularExpression>
#include <QSaveFile>
#include <QTimer>
#include <QUrl>
#include <QWebEnginePage>
#include <QWebEngineView>

namespace
{
// How often an export checks whether the page has finished drawing.
constexpr int SettleIntervalMs = 100;

// Margin around an exported PDF page.
constexpr qreal ExportMarginMm = 15;

// Replace every picture stored on this computer by its content, so that the exported
// file can be moved or sent on its own. A picture that cannot be read keeps its address.
QString embedLocalImages(const QString &html)
{
    static const QRegularExpression localPicture(QStringLiteral("(<img\\b[^>]*?\\bsrc=\")(file://[^\"]+)(\")"));
    QString result;
    qsizetype copied = 0;
    QRegularExpressionMatchIterator pictures = localPicture.globalMatch(html);
    while (pictures.hasNext()) {
        const QRegularExpressionMatch picture = pictures.next();
        // The page wrote the address as HTML: "&" comes as "&amp;".
        const QString address = picture.captured(2).replace(QLatin1String("&amp;"), QLatin1String("&"));
        const QString path = QUrl(address).toLocalFile();
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly)) {
            continue;
        }
        const QString type = QMimeDatabase().mimeTypeForFile(path).name();
        const QString content = QString::fromLatin1(file.readAll().toBase64());
        result += html.mid(copied, picture.capturedStart(2) - copied);
        result += QStringLiteral("data:%1;base64,%2").arg(type, content);
        copied = picture.capturedEnd(2);
    }
    return result + html.mid(copied);
}

// A page that stands on its own: the rendered article with the stylesheets it needs and
// no script. Image paths stay relative to the document's folder.
QString standaloneHtml(const QString &title, const QString &body)
{
    const QString base = QStringLiteral(":/katdown/css/");
    QString css = readAsset(base + QStringLiteral("github-markdown.css")) + readAsset(base + QStringLiteral("base.css"))
        + readAsset(base + QStringLiteral("hljs-github.min.css"));
    if (body.contains(QLatin1String("class=\"katex"))) {
        css += readAsset(base + QStringLiteral("katex.min.css"));
    }
    QString vars = QStringLiteral("color-scheme:light;");
    const QJsonObject light = githubVars(false);
    for (auto it = light.begin(); it != light.end(); ++it) {
        vars += it.key() + QLatin1Char(':') + it.value().toString() + QLatin1Char(';');
    }
    return QStringLiteral("<!DOCTYPE html>\n<html data-pv-scheme=\"light\" style=\"%1\">\n<head>\n<meta charset=\"utf-8\">\n"
                          "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n<title>%2</title>\n<style>%3</style>\n"
                          "</head>\n<body>\n<article class=\"markdown-body\" id=\"content\">%4</article>\n</body>\n</html>\n")
        .arg(vars, title.toHtmlEscaped(), css, body);
}
} // namespace

// Run then once the page has finished drawing: switching look redraws the diagrams, and
// an export taken before that would catch them half done. Gives up waiting after 5 s.
void PreviewWidget::whenSettled(std::function<void()> then, int checksLeft)
{
    // The answer arrives later, when this widget may be gone: QPointer turns null then.
    const QPointer<PreviewWidget> self(this);
    m_web->page()->runJavaScript(QStringLiteral("window.__settled()"), [self, then, checksLeft](const QVariant &settled) {
        if (!self) {
            return;
        }
        if (settled.toBool() || checksLeft <= 0) {
            then();
        } else {
            QTimer::singleShot(SettleIntervalMs, self, [self, then, checksLeft]() {
                self->whenSettled(then, checksLeft - 1);
            });
        }
    });
}

void PreviewWidget::exportPdf(const QString &path)
{
    if (!m_loaded) {
        Q_EMIT exported(path, false);
        return;
    }
    applyGithubLook(false);
    whenSettled([this, path]() {
        const QPageSize size(QLocale().measurementSystem() == QLocale::MetricSystem ? QPageSize::A4 : QPageSize::Letter);
        m_web->page()->printToPdf(path, QPageLayout(size, QPageLayout::Portrait, QMarginsF(ExportMarginMm, ExportMarginMm, ExportMarginMm, ExportMarginMm), QPageLayout::Millimeter));
    });
}

void PreviewWidget::exportHtml(const QString &path)
{
    if (!m_loaded) {
        Q_EMIT exported(path, false);
        return;
    }
    applyGithubLook(false);
    whenSettled([this, path]() {
        m_web->page()->runJavaScript(QStringLiteral("window.__exportBody()"), [self = QPointer<PreviewWidget>(this), path](const QVariant &body) {
            if (!self) {
                return;
            }
            const QString title = QFileInfo(self->m_url.fileName()).completeBaseName();
            const QByteArray html = standaloneHtml(title, embedLocalImages(body.toString())).toUtf8();
            // QSaveFile writes to a temporary file and replaces the target only on
            // commit(), so a failed export never leaves a half-written file behind.
            QSaveFile file(path);
            bool ok = false;
            if (file.open(QIODevice::WriteOnly)) {
                file.write(html);
                ok = file.commit();
            }
            self->applyTheme();
            Q_EMIT self->exported(path, ok);
        });
    });
}
