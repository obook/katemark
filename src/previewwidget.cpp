// previewwidget.cpp
// PreviewWidget: building the bar, attaching a document, rendering.
// Code by uwuclxdy and Olivier Booklage.
// License: GPL-3.0-or-later

#include "previewwidget.h"
#include "previewpage.h"
#include "previewutil.h"
#include "settings.h"

#include <QAction>
#include <QCheckBox>
#include <QIcon>
#include <QLabel>
#include <QTimer>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWebEngineProfile>
#include <QWebEngineView>

#include <KLocalizedString>
#include <KTextEditor/Document>
#include <KTextEditor/MainWindow>
#include <KTextEditor/View>

namespace
{
// How long the text has to stay unchanged before the page renders it.
constexpr int RenderDebounceMs = 150;

constexpr int BarIconSizePx = 16;

// A page that has not answered for this long is said to be rendering, and the dots after
// the word move at this pace.
constexpr int BusyTickMs = 400;
constexpr int BusyLabelMarginPx = 8;
} // namespace

PreviewWidget::PreviewWidget(KTextEditor::MainWindow *mainWindow, KTextEditor::View *view, KTextEditor::Document *doc, QWidget *parent)
    : QWidget(parent)
    , m_mainWindow(mainWindow)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    createBar();
    layout->addWidget(m_bar);
    createWebView();
    layout->addWidget(m_web);

    m_debounce = new QTimer(this);
    m_debounce->setSingleShot(true);
    m_debounce->setInterval(RenderDebounceMs);
    connect(m_debounce, &QTimer::timeout, this, &PreviewWidget::render);

    connect(Settings::self(), &Settings::changed, this, &PreviewWidget::applyTheme);
    connect(Settings::self(), &Settings::changed, this, &PreviewWidget::applyMediaPolicy);
    connect(Settings::self(), &Settings::changed, this, &PreviewWidget::applyMacros);
    connect(Settings::self(), &Settings::changed, this, &PreviewWidget::applySyntax);

    setWindowIcon(QIcon::fromTheme(QStringLiteral("text-markdown")));
    applyMediaPolicy();
    attachDocument(doc, view);
}

// The bar above the preview: what the page renders on the left, the exports on the right
// once the window hands them over.
void PreviewWidget::createBar()
{
    m_bar = new QToolBar(this);
    m_bar->setIconSize(QSize(BarIconSizePx, BarIconSizePx));
    m_bar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    // A check box: a flat button that is off reads as a mere label.
    m_githubOnlyBox = new QCheckBox(i18n("GitHub only"), m_bar);
    m_githubOnlyBox->setToolTip(i18n("Render only what GitHub renders, without the CodiMD, Obsidian and MkDocs syntax"));
    m_githubOnlyBox->setChecked(Settings::self()->githubOnly());
    connect(m_githubOnlyBox, &QCheckBox::toggled, Settings::self(), &Settings::setGithubOnly);
    m_bar->addWidget(m_githubOnlyBox);
    // Empty while the page keeps up: see runJs().
    m_busyLabel = new QLabel(m_bar);
    m_busyLabel->setObjectName(QStringLiteral("busy"));
    m_busyLabel->setContentsMargins(BusyLabelMarginPx, 0, 0, 0);
    m_bar->addWidget(m_busyLabel);
    m_busyTimer = new QTimer(this);
    m_busyTimer->setInterval(BusyTickMs);
    connect(m_busyTimer, &QTimer::timeout, this, [this]() {
        m_busyDots = m_busyDots % 3 + 1;
        m_busyLabel->setText(i18nc("@info:status the preview is rendering a long document; one to three dots follow", "Rendering")
                             + QString(m_busyDots, QLatin1Char('.')));
    });
    auto *spacer = new QWidget(m_bar);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_bar->addWidget(spacer);
}

void PreviewWidget::attachDocument(KTextEditor::Document *doc, KTextEditor::View *view)
{
    setView(view); // the same document can come back under another view (split views)
    if (doc && m_doc == doc) {
        return;
    }
    if (m_doc) {
        m_doc->disconnect(this);
    }
    m_doc = doc;
    m_reloadsLeft = ReloadsAfterCrash;
    if (doc) {
        connect(doc, &KTextEditor::Document::textChanged, this, &PreviewWidget::scheduleRender);
        connect(doc, &KTextEditor::Document::documentUrlChanged, this, &PreviewWidget::onDocumentUrlChanged);
        connect(doc, &KTextEditor::Document::aboutToClose, this, &PreviewWidget::snapshotSource);
        connect(doc, &KTextEditor::Document::highlightingModeChanged, this, &PreviewWidget::onHighlightingModeChanged);
    }
    // Reloading the page costs a blank flash and the diagrams already drawn. It is only
    // needed when the document's folder changes, because relative images and the file
    // guard depend on it; otherwise pushing the new text is enough.
    const QUrl previousBase = baseUrl();
    if (doc) {
        m_url = doc->url();
        m_mermaid = isMermaid(doc);
    }
    if (m_loaded && baseUrl() == previousBase) {
        render();
    } else {
        loadPage();
    }
}

// aboutToClose is the last moment the buffer still holds the document's text;
// closeUrl() empties it immediately afterwards.
void PreviewWidget::snapshotSource()
{
    if (m_doc) {
        m_text = m_doc->text();
    }
}

// Delete in this order: the view and its page, then the profile, then the guard the
// profile uses. Qt promises no order for deleting children, so it is done by hand.
PreviewWidget::~PreviewWidget()
{
    delete m_web;
    m_web = nullptr;
    delete m_profile;
    m_profile = nullptr;
    delete m_guard;
    m_guard = nullptr;
}

// The page runs its scripts one after the other, so an answer to the last one sent means
// it has caught up. While it has not, the bar says so after a moment: a long document
// takes seconds to render.
void PreviewWidget::runJs(const QString &code)
{
    if (!m_web || !m_loaded) {
        return;
    }
    const int script = ++m_scriptsSent;
    if (!m_busyTimer->isActive()) {
        m_busyTimer->start();
    }
    // The answer arrives later, when this widget may be gone: QPointer turns null then.
    const QPointer<PreviewWidget> self(this);
    m_web->page()->runJavaScript(code, [self, script](const QVariant &) {
        if (self) {
            self->onScriptAnswered(script);
        }
    });
}

void PreviewWidget::onScriptAnswered(int script)
{
    m_scriptsAnswered = qMax(m_scriptsAnswered, script);
    if (m_scriptsAnswered < m_scriptsSent) {
        m_busyTimer->start(); // the page answers: the wait starts over for the next script
    } else {
        setIdle();
    }
}

// Nothing is expected from the page any more: it has answered, or it is going away.
void PreviewWidget::setIdle()
{
    m_scriptsAnswered = m_scriptsSent;
    m_busyTimer->stop();
    m_busyDots = 0;
    m_busyLabel->clear();
}

void PreviewWidget::render()
{
    if (!m_loaded) {
        return;
    }
    if (m_doc) {
        m_text = m_doc->text();
    }
    const QString markdown = m_mermaid ? mermaidAsMarkdown(m_text) : m_text;
    runJs(QStringLiteral("window.__setMarkdown(%1);").arg(jsLiteral(markdown)));
    syncFromEditor();
}

// Hand the page the math macros of the settings; it renders again when they changed.
void PreviewWidget::applyMacros()
{
    runJs(QStringLiteral("window.__setMacros(%1);").arg(jsLiteral(Settings::self()->mathMacros())));
}

// Tell the page which syntax to read, and keep the check box in step with a change made
// from the bar of another window.
void PreviewWidget::applySyntax()
{
    const bool githubOnly = Settings::self()->githubOnly();
    m_githubOnlyBox->setChecked(githubOnly);
    runJs(QStringLiteral("window.__setGithubOnly(%1);").arg(githubOnly ? QStringLiteral("true") : QStringLiteral("false")));
}

void PreviewWidget::addExportActions(const QList<QAction *> &actions)
{
    addActions(actions); // showContextMenu() lists them
    m_bar->addActions(actions);
}

void PreviewWidget::scheduleRender()
{
    if (m_paused) {
        return;
    }
    // A page whose render process died stays blank: the next edit loads it again.
    if (!m_loaded && !m_web->page()->isLoading() && m_reloadsLeft > 0) {
        --m_reloadsLeft;
        loadPage();
        return;
    }
    m_debounce->start();
}

void PreviewWidget::setPaused(bool paused)
{
    if (m_paused == paused) {
        return;
    }
    m_paused = paused;
    if (paused) {
        m_debounce->stop();
    } else {
        render(); // catch up with the edits made meanwhile
    }
}
