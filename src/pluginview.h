#pragma once

#include <QObject>
#include <QPointer>

#include <functional>

#include <KXMLGUIClient>

class PreviewWidget;
class QAction;
class QIcon;
class QKeySequence;

namespace KTextEditor
{
class Document;
class MainWindow;
class Plugin;
class View;
}

/**
 * One instance per Kate main window. Owns a tool view docked on the right, like Kate's
 * own document preview: the source stays in the editor area and the preview beside it
 * follows the active Markdown document.
 */
class PluginView : public QObject, public KXMLGUIClient
{
    Q_OBJECT
public:
    PluginView(KTextEditor::Plugin *plugin, KTextEditor::MainWindow *mainWindow);
    ~PluginView() override;

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    // What an editing action does to the view it is given.
    using EditFunction = std::function<void(KTextEditor::View *)>;

    static bool isMarkdown(KTextEditor::Document *doc);
    KTextEditor::View *markdownView() const;
    QAction *addAction(const QString &name, const QString &text, const QIcon &icon);
    void addEditAction(const QString &name, const QString &text, const QString &iconName, const QKeySequence &shortcut, const EditFunction &edit);
    void addEditActions();
    void showMessage(const QString &text, bool isError);
    void togglePreview();
    void syncPreview();
    void onViewChanged(KTextEditor::View *view);
    void pasteImage();
    void exportPreview(bool pdf);
    void onExported(const QString &path, bool ok);
    void onToolViewShown(bool shown);

    KTextEditor::MainWindow *m_mainWindow = nullptr;
    QPointer<QWidget> m_toolView;
    QPointer<PreviewWidget> m_preview;
    QAction *m_exportPdf = nullptr;
    QAction *m_exportHtml = nullptr;
};
