#pragma once

#include <QPointer>
#include <QUrl>
#include <QWidget>

#include <functional>

#include <KTextEditor/Document>
#include <KTextEditor/View>

class QWebEngineView;
class QWebEngineProfile;
class QWebEngineUrlRequestInterceptor;
class QTimer;
class QAction;
class QKeyEvent;
class QKeySequence;
class QMouseEvent;

namespace KTextEditor
{
class MainWindow;
}

/**
 * The Markdown preview: a QWebEngineView fed by a self-contained HTML document.
 * Source text is pushed to the page on every change; theming is driven from the
 * active editor theme (Application mode) or GitHub's palette.
 *
 * The widget outlives its document: Kate destroys the document when its editor tab
 * closes, so the source text and url are mirrored here and the preview keeps showing
 * that copy until it is attached to another document.
 */
class PreviewWidget : public QWidget
{
    Q_OBJECT
public:
    PreviewWidget(KTextEditor::MainWindow *mainWindow, KTextEditor::View *view, KTextEditor::Document *doc, QWidget *parent = nullptr);
    ~PreviewWidget() override;

    // Still valid after the document is gone.
    QUrl documentUrl() const
    {
        return m_url;
    }

    void attachDocument(KTextEditor::Document *doc, KTextEditor::View *view);

    // A paused preview stops following the editor: nobody sees it, so rendering every
    // edit would be wasted work. Resuming renders the current text.
    void setPaused(bool paused);

    // Write the rendered document to a file, in GitHub's light look whatever the preview
    // shows: a dark page makes a poor printout. Both finish with exported().
    void exportPdf(const QString &path);
    void exportHtml(const QString &path);

Q_SIGNALS:
    void exported(const QString &path, bool ok);

public Q_SLOTS:
    void applyTheme();

protected:
    void changeEvent(QEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private Q_SLOTS:
    void scheduleRender();
    void onDocumentUrlChanged();
    void snapshotSource();
    void syncFromEditor();
    void syncFromPreview();

private:
    void render();
    void applyGithubLook(bool dark);
    void applyMacros();
    // 50 checks, 100 ms apart: an export waits at most 5 s for the page.
    void whenSettled(std::function<void()> then, int checksLeft = 50);
    void setView(KTextEditor::View *view);
    void runJs(const QString &code);
    void loadPage();
    void openLink(const QUrl &url);
    void applyMediaPolicy();
    QUrl baseUrl() const;
    static QString buildHtml();

    // Forward input the preview doesn't use back to Kate: QWebEngineView's render
    // widget swallows keys/mouse buttons before Kate's shortcut machinery sees them.
    void installInputFilter();
    void showContextMenu(const QPoint &position);
    bool forwardKeyEvent(QKeyEvent *event);
    bool forwardMouseEvent(QMouseEvent *event);
    QAction *kateActionFor(const QKeySequence &seq) const;

    QPointer<KTextEditor::MainWindow> m_mainWindow;
    QWebEngineView *m_web = nullptr;
    QWebEngineProfile *m_profile = nullptr;
    QWebEngineUrlRequestInterceptor *m_guard = nullptr;
    QTimer *m_debounce = nullptr;
    QPointer<KTextEditor::Document> m_doc;
    QPointer<KTextEditor::View> m_view;
    QPointer<QWidget> m_inputTarget;
    QUrl m_url;
    QString m_text;
    bool m_paused = false;
    bool m_loaded = false;
    bool m_remoteApplied = false;
};
