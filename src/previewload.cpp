// previewload.cpp
// PreviewWidget: creating the web view, building the HTML page and loading it for a
// document.
// Code by uwuclxdy and Olivier Booklage, moved out of previewwidget.cpp.
// License: GPL-3.0-or-later

#include "previewpage.h"
#include "previewutil.h"
#include "previewwidget.h"
#include "settings.h"

#include <QCursor>
#include <QFileInfo>
#include <QJsonObject>
#include <QToolTip>
#include <QUrl>
#include <QWebEngineNewWindowRequest>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QWebEngineView>

#include <KLocalizedString>
#include <KTextEditor/Document>

namespace
{
// Default titles of the alerts and callouts, in the user's language. The page has no
// access to the translations, so it gets them from here.
QJsonObject calloutLabels()
{
    return QJsonObject{
        {QStringLiteral("note"), i18nc("@title GitHub alert", "Note")},
        {QStringLiteral("tip"), i18nc("@title GitHub alert", "Tip")},
        {QStringLiteral("important"), i18nc("@title GitHub alert", "Important")},
        {QStringLiteral("warning"), i18nc("@title GitHub alert", "Warning")},
        {QStringLiteral("caution"), i18nc("@title GitHub alert", "Caution")},
        {QStringLiteral("abstract"), i18nc("@title Obsidian callout", "Abstract")},
        {QStringLiteral("info"), i18nc("@title Obsidian callout", "Info")},
        {QStringLiteral("todo"), i18nc("@title Obsidian callout", "Todo")},
        {QStringLiteral("success"), i18nc("@title Obsidian callout", "Success")},
        {QStringLiteral("question"), i18nc("@title Obsidian callout", "Question")},
        {QStringLiteral("failure"), i18nc("@title Obsidian callout", "Failure")},
        {QStringLiteral("danger"), i18nc("@title Obsidian callout", "Danger")},
        {QStringLiteral("bug"), i18nc("@title Obsidian callout", "Bug")},
        {QStringLiteral("example"), i18nc("@title Obsidian callout", "Example")},
        {QStringLiteral("quote"), i18nc("@title Obsidian callout", "Quote")},
        {QStringLiteral("definition"), i18nc("@title teaching callout", "Definition")},
        {QStringLiteral("theorem"), i18nc("@title teaching callout", "Theorem")},
        {QStringLiteral("lemma"), i18nc("@title teaching callout", "Lemma")},
        {QStringLiteral("proposition"), i18nc("@title teaching callout", "Proposition")},
        {QStringLiteral("corollary"), i18nc("@title teaching callout", "Corollary")},
        {QStringLiteral("axiom"), i18nc("@title teaching callout", "Axiom")},
        {QStringLiteral("claim"), i18nc("@title teaching callout", "Claim")},
        {QStringLiteral("assumption"), i18nc("@title teaching callout", "Assumption")},
        {QStringLiteral("conjecture"), i18nc("@title teaching callout", "Conjecture")},
        {QStringLiteral("hypothesis"), i18nc("@title teaching callout", "Hypothesis")},
        {QStringLiteral("proof"), i18nc("@title teaching callout", "Proof")},
        {QStringLiteral("exercise"), i18nc("@title teaching callout", "Exercise")},
        {QStringLiteral("method"), i18nc("@title teaching callout", "Method")},
        {QStringLiteral("remark"), i18nc("@title teaching callout", "Remark")},
        {QStringLiteral("solution"), i18nc("@title teaching callout", "Solution")},
    };
}
} // namespace

void PreviewWidget::createWebView()
{
    // Each preview has its own off-the-record profile and guard, so it stays inside its
    // own document folder.
    m_guard = new LocalFileGuard(this);
    m_profile = new QWebEngineProfile(this);
    m_profile->setUrlRequestInterceptor(m_guard);

    m_web = new QWebEngineView(this);
    auto *page = new PreviewPage(m_profile, m_web);
    page->onLinkActivated = [this](const QUrl &url) {
        openLink(url);
    };
    // A link that asks for a window of its own (target="_blank", a middle click) opens
    // like any other, later as a click does: the question some links ask must not wait
    // inside Qt WebEngine's call.
    connect(page, &QWebEnginePage::newWindowRequested, this, [this](QWebEngineNewWindowRequest &request) {
        QMetaObject::invokeMethod(
            this,
            [this, url = request.requestedUrl()]() {
                openLink(url);
            },
            Qt::QueuedConnection);
    });
    // Where a link goes, shown while the pointer is on it. Leaving the link sends an
    // empty address, which hides the tip.
    connect(page, &QWebEnginePage::linkHovered, this, [this](const QString &address) {
        QToolTip::showText(QCursor::pos(), linkHint(QUrl(address), baseUrl()), m_web);
    });
    m_web->setPage(page);
    // The menu is built in showContextMenu() instead of being left to Qt WebEngine.
    m_web->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_web, &QWidget::customContextMenuRequested, this, &PreviewWidget::showContextMenu);
    connect(page, &QWebEnginePage::scrollPositionChanged, this, &PreviewWidget::syncFromPreview);
    connect(page, &QWebEnginePage::pdfPrintingFinished, this, &PreviewWidget::onPdfPrinted);
    // A page whose process died answers nothing more: an export would wait for ever.
    connect(page, &QWebEnginePage::renderProcessTerminated, this, [this]() {
        m_loaded = false;
        setIdle();
        endExport(false);
    });
    m_web->settings()->setAttribute(QWebEngineSettings::FocusOnNavigationEnabled, false);
    m_web->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
    // Watch the view for the lazily-created render widget so we can attach to it.
    m_web->installEventFilter(this);
    installInputFilter();
    connect(m_web, &QWebEngineView::loadFinished, this, &PreviewWidget::onLoadFinished);
}

void PreviewWidget::onLoadFinished(bool ok)
{
    if (!ok) {
        return;
    }
    m_loaded = true;
    installInputFilter();
    runJs(QStringLiteral("window.__setLabels(%1);").arg(compactJson(calloutLabels())));
    // While pictures from the web are kept out, the page names the setting that lets them
    // in, in place of each one: the same text as on the settings page (configpage.cpp).
    // A change of that setting reloads the page.
    const QString mediaHint = Settings::self()->loadRemoteMedia() ? QString() : i18n("Load media previews from remote URLs");
    runJs(QStringLiteral("window.__setRemoteMediaHint(%1);").arg(jsLiteral(mediaHint)));
    applyTheme();
    applyMacros();
    applySyntax();
    render();
}

QString PreviewWidget::buildHtml()
{
    const QString base = QStringLiteral(":/katemark/");
    QString html = readAsset(base + QStringLiteral("preview.html"));
    html.replace(QLatin1String("/*__GHMD_CSS__*/"), readAsset(base + QStringLiteral("css/github-markdown.css")));
    html.replace(QLatin1String("/*__BASE_CSS__*/"), readAsset(base + QStringLiteral("css/base.css")));
    html.replace(QLatin1String("/*__HLJS_LIGHT__*/"), readAsset(base + QStringLiteral("css/hljs-github.min.css")));
    html.replace(QLatin1String("/*__HLJS_DARK__*/"), readAsset(base + QStringLiteral("css/hljs-github-dark.min.css")));
    return html;
}

QUrl PreviewWidget::baseUrl() const
{
    if (m_url.isLocalFile()) {
        return m_url.adjusted(QUrl::RemoveFilename);
    }
    return QUrl(QStringLiteral("qrc:/katemark/"));
}

void PreviewWidget::loadPage()
{
    if (m_doc) {
        m_url = m_doc->url();
        m_mermaid = isMermaid(m_doc);
    }
    const QString root = m_url.isLocalFile() ? QFileInfo(m_url.toLocalFile()).absolutePath() : QString();
    m_guard->setRoot(root);
    m_loaded = false;
    setIdle(); // the page that goes away will not answer its scripts
    endExport(false); // an export cannot finish on a page that goes away
    // Qt paints this color until the page has its own; white would flash in a dark preview.
    m_web->page()->setBackgroundColor(pageBackground());
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

// Switching the mode between Markdown and Mermaid changes how the same text reads.
// Closing resets the mode once the url is cleared: that one is not a switch.
void PreviewWidget::onHighlightingModeChanged()
{
    if (m_doc->url().isEmpty() && !m_url.isEmpty()) {
        return;
    }
    m_mermaid = isMermaid(m_doc);
    scheduleRender();
}

void PreviewWidget::applyMediaPolicy()
{
    const bool remote = Settings::self()->loadRemoteMedia();
    m_web->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, remote);
    m_guard->setAllowRemote(remote);
    if (m_loaded && remote != m_pageAllowsRemoteMedia) {
        loadPage();
    }
    m_pageAllowsRemoteMedia = remote;
}
