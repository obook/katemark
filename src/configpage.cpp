#include "configpage.h"
#include "settings.h"

#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QUrl>
#include <QVBoxLayout>

#include <KLocalizedString>

namespace
{
const QString repoUrl = QStringLiteral("https://github.com/obook/katemark");

QString currentVersion()
{
    return QStringLiteral(KATEMARK_VERSION);
}

// Compare dotted numeric versions ("0.10.0" > "0.9.1"). Returns -1/0/1. Any non-numeric
// suffix on a part is dropped by toInt(), which is fine: /releases/latest never returns a
// pre-release tag, so the inputs here are plain x.y.z.
int compareVersions(const QString &a, const QString &b)
{
    const QStringList pa = a.split(QLatin1Char('.'));
    const QStringList pb = b.split(QLatin1Char('.'));
    const int n = pa.size() > pb.size() ? pa.size() : pb.size();
    for (int i = 0; i < n; ++i) {
        const int va = i < pa.size() ? pa.at(i).toInt() : 0;
        const int vb = i < pb.size() ? pb.at(i).toInt() : 0;
        if (va != vb) {
            return va < vb ? -1 : 1;
        }
    }
    return 0;
}
} // namespace

ConfigPage::ConfigPage(QWidget *parent)
    : KTextEditor::ConfigPage(parent)
{
    auto *outer = new QVBoxLayout(this);
    auto *form = new QFormLayout();
    outer->addLayout(form);

    m_mode = new QComboBox(this);
    m_mode->addItem(i18n("GitHub"), Settings::GitHub);
    m_mode->addItem(i18n("Match editor / system theme"), Settings::Application);
    form->addRow(i18n("Style:"), m_mode);

    m_variant = new QComboBox(this);
    m_variant->addItem(i18n("Follow system (auto)"), Settings::Auto);
    m_variant->addItem(i18n("Light"), Settings::Light);
    m_variant->addItem(i18n("Dark"), Settings::Dark);
    form->addRow(i18n("GitHub variant:"), m_variant);

    auto *hint = new QLabel(
        i18n("\"GitHub\" uses GitHub's own colors. \"Match editor / system theme\" recolors the "
             "same layout from the active editor theme, so the preview blends with the rest of Kate."),
        this);
    hint->setWordWrap(true);
    hint->setEnabled(false);
    outer->addWidget(hint);

    m_remoteMedia = new QCheckBox(i18n("Load media previews from remote URLs"), this);
    outer->addWidget(m_remoteMedia);

    auto *remoteHint = new QLabel(i18n("Off by default so the preview stays fully offline. Enable to let images load over http(s)."), this);
    remoteHint->setWordWrap(true);
    remoteHint->setEnabled(false);
    outer->addWidget(remoteHint);

    outer->addWidget(new QLabel(i18n("Math macros:"), this));
    m_macros = new QPlainTextEdit(this);
    m_macros->setPlaceholderText(QStringLiteral("\\newcommand{\\vect}[1]{\\overrightarrow{#1}}"));
    // Four lines are enough to show a few macros; the field scrolls beyond that.
    m_macros->setFixedHeight(4 * m_macros->fontMetrics().lineSpacing() + 12);
    outer->addWidget(m_macros);

    auto *macrosHint = new QLabel(i18n("The \\newcommand lines written here are available in the formulas of every document."), this);
    macrosHint->setWordWrap(true);
    macrosHint->setEnabled(false);
    outer->addWidget(macrosHint);

    m_net = new QNetworkAccessManager(this);

    auto *updateRow = new QHBoxLayout();
    m_checkButton = new QPushButton(i18n("Check for updates"), this);
    m_checkButton->setIcon(QIcon::fromTheme(QStringLiteral("system-software-update")));
    updateRow->addWidget(m_checkButton);
    m_updateStatus = new QLabel(i18n("Installed version: %1", currentVersion()), this);
    m_updateStatus->setTextFormat(Qt::RichText);
    m_updateStatus->setOpenExternalLinks(true);
    m_updateStatus->setTextInteractionFlags(Qt::TextBrowserInteraction);
    updateRow->addWidget(m_updateStatus, 1);
    outer->addLayout(updateRow);

    outer->addStretch();

    reset();

    connect(m_remoteMedia, &QCheckBox::toggled, this, [this]() { Q_EMIT changed(); });
    connect(m_macros, &QPlainTextEdit::textChanged, this, [this]() {
        Q_EMIT changed();
    });
    connect(m_mode, &QComboBox::currentIndexChanged, this, [this]() {
        syncEnabled();
        Q_EMIT changed();
    });
    connect(m_variant, &QComboBox::currentIndexChanged, this, [this]() {
        Q_EMIT changed();
    });
    connect(m_checkButton, &QPushButton::clicked, this, &ConfigPage::checkForUpdates);
}

QString ConfigPage::name() const
{
    return i18n("Katemark");
}

QString ConfigPage::fullName() const
{
    return i18n("Katemark");
}

QIcon ConfigPage::icon() const
{
    return QIcon::fromTheme(QStringLiteral("text-markdown"), QIcon::fromTheme(QStringLiteral("view-preview")));
}

void ConfigPage::syncEnabled()
{
    const bool github = m_mode->currentData().toInt() == Settings::GitHub;
    m_variant->setEnabled(github);
}

void ConfigPage::checkForUpdates()
{
    m_checkButton->setEnabled(false);
    m_updateStatus->setText(i18n("Checking..."));

    QNetworkRequest req(QUrl(QStringLiteral("https://api.github.com/repos/obook/katemark/releases/latest")));
    req.setRawHeader("Accept", "application/vnd.github+json");
    req.setRawHeader("User-Agent", "katemark");
    req.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

    QNetworkReply *reply = m_net->get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        m_checkButton->setEnabled(true);

        if (reply->error() != QNetworkReply::NoError) {
            m_updateStatus->setText(i18n("Update check failed: %1", reply->errorString()));
            return;
        }

        const QJsonObject obj = QJsonDocument::fromJson(reply->readAll()).object();
        const QString tag = obj.value(QStringLiteral("tag_name")).toString();
        if (tag.isEmpty()) {
            m_updateStatus->setText(i18n("Update check failed: no published release found."));
            return;
        }

        const QString latest = tag.startsWith(QLatin1Char('v')) ? tag.mid(1) : tag;
        if (compareVersions(latest, currentVersion()) > 0) {
            QString url = obj.value(QStringLiteral("html_url")).toString();
            if (url.isEmpty()) {
                url = repoUrl + QStringLiteral("/releases/latest");
            }
            m_updateStatus->setText(i18n("Update available: %1 (installed %2). <a href=\"%3\">Release notes</a>", tag, currentVersion(), url));
        } else {
            m_updateStatus->setText(i18n("Up to date (installed %1).", currentVersion()));
        }
    });
}

void ConfigPage::apply()
{
    Settings *s = Settings::self();
    s->setMode(static_cast<Settings::Mode>(m_mode->currentData().toInt()));
    s->setGhVariant(static_cast<Settings::GhVariant>(m_variant->currentData().toInt()));
    s->setLoadRemoteMedia(m_remoteMedia->isChecked());
    s->setMathMacros(m_macros->toPlainText());
}

void ConfigPage::reset()
{
    Settings *s = Settings::self();
    m_mode->setCurrentIndex(m_mode->findData(s->mode()));
    m_variant->setCurrentIndex(m_variant->findData(s->ghVariant()));
    m_remoteMedia->setChecked(s->loadRemoteMedia());
    m_macros->setPlainText(s->mathMacros());
    syncEnabled();
}

void ConfigPage::defaults()
{
    m_mode->setCurrentIndex(m_mode->findData(Settings::GitHub));
    m_variant->setCurrentIndex(m_variant->findData(Settings::Auto));
    m_remoteMedia->setChecked(false);
    m_macros->clear();
    syncEnabled();
}
