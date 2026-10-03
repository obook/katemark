// previewload.cpp
// PreviewWidget: building the HTML page and loading it for a document.
// Code by uwuclxdy, moved out of previewwidget.cpp.
// License: GPL-3.0-or-later

#include "previewpage.h"
#include "previewutil.h"
#include "previewwidget.h"
#include "settings.h"

#include <QFileInfo>
#include <QUrl>
#include <QWebEngineSettings>
#include <QWebEngineView>

#include <KTextEditor/Document>

QString PreviewWidget::buildHtml()
{
    const QString base = QStringLiteral(":/katdown/");
    QString html = readAsset(base + QStringLiteral("preview.html"));
    html.replace(QLatin1String("/*__GHMD_CSS__*/"), readAsset(base + QStringLiteral("css/github-markdown.css")));
    html.replace(QLatin1String("/*__BASE_CSS__*/"), readAsset(base + QStringLiteral("css/base.css")));
    html.replace(QLatin1String("/*__HLJS_LIGHT__*/"), readAsset(base + QStringLiteral("css/hljs-github.min.css")));
    html.replace(QLatin1String("/*__HLJS_DARK__*/"), readAsset(base + QStringLiteral("css/hljs-github-dark.min.css")));
    html.replace(QLatin1String("/*__MARKDOWN_IT__*/"), shieldScript(readAsset(base + QStringLiteral("js/markdown-it.min.js"))));
    html.replace(QLatin1String("/*__HLJS_JS__*/"), shieldScript(readAsset(base + QStringLiteral("js/highlight.min.js"))));
    html.replace(QLatin1String("/*__JS_YAML__*/"), shieldScript(readAsset(base + QStringLiteral("js/js-yaml.min.js"))));
    html.replace(QLatin1String("/*__PREVIEW_JS__*/"), shieldScript(readAsset(base + QStringLiteral("js/preview.js"))));
    return html;
}

QUrl PreviewWidget::baseUrl() const
{
    if (m_url.isLocalFile()) {
        return m_url.adjusted(QUrl::RemoveFilename);
    }
    return QUrl(QStringLiteral("qrc:/katdown/"));
}

void PreviewWidget::loadPage()
{
    if (m_doc) {
        m_url = m_doc->url();
    }
    const QString root = m_url.isLocalFile() ? QFileInfo(m_url.toLocalFile()).absolutePath() : QString();
    static_cast<LocalFileGuard *>(m_guard)->setRoot(root);
    m_loaded = false;
    m_web->setHtml(buildHtml(), baseUrl());
}

// Closing a document clears its url before anything announces the close, so an
// unfiltered reload here would blank the page and drop the document folder the
// frozen content still resolves its images against. A rename or Save As always
// lands on a non-empty url.
void PreviewWidget::onDocumentUrlChanged()
{
    if (m_doc && m_doc->url().isEmpty() && !m_url.isEmpty()) {
        return;
    }
    loadPage();
}

void PreviewWidget::applyMediaPolicy()
{
    const bool remote = Settings::self()->loadRemoteMedia();
    m_web->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, remote);
    static_cast<LocalFileGuard *>(m_guard)->setAllowRemote(remote);
    if (m_loaded && remote != m_remoteApplied) {
        loadPage();
    }
    m_remoteApplied = remote;
}
