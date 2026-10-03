#pragma once

#include <KTextEditor/ConfigPage>

class QCheckBox;
class QComboBox;
class QLabel;
class QPlainTextEdit;
class QPushButton;
class QNetworkAccessManager;

/**
 * Settings page shown under Kate's plugin configuration: choose between GitHub
 * colors and the active editor/system theme, plus the GitHub light/dark variant.
 */
class ConfigPage : public KTextEditor::ConfigPage
{
    Q_OBJECT
public:
    explicit ConfigPage(QWidget *parent = nullptr);

    QString name() const override;
    QString fullName() const override;
    QIcon icon() const override;

    void apply() override;
    void reset() override;
    void defaults() override;

private:
    void syncEnabled();
    void checkForUpdates();

    QComboBox *m_mode = nullptr;
    QComboBox *m_variant = nullptr;
    QCheckBox *m_remoteMedia = nullptr;
    QPlainTextEdit *m_macros = nullptr;
    QPushButton *m_checkButton = nullptr;
    QLabel *m_updateStatus = nullptr;
    QNetworkAccessManager *m_net = nullptr;
};
