#include "settings.h"

#include <KConfigGroup>
#include <KSharedConfig>

static QString groupName()
{
    return QStringLiteral("Katemark");
}

Settings::Settings(QObject *parent)
    : QObject(parent)
{
    load();
}

Settings *Settings::self()
{
    static Settings instance;
    return &instance;
}

void Settings::load()
{
    KConfigGroup cfg(KSharedConfig::openConfig(), groupName());
    const QString mode = cfg.readEntry("Theme", QStringLiteral("github"));
    m_mode = (mode == QLatin1String("application")) ? Application : GitHub;

    const QString variant = cfg.readEntry("GithubVariant", QStringLiteral("auto"));
    if (variant == QLatin1String("light")) {
        m_ghVariant = Light;
    } else if (variant == QLatin1String("dark")) {
        m_ghVariant = Dark;
    } else {
        m_ghVariant = Auto;
    }
    m_loadRemoteMedia = cfg.readEntry("LoadRemoteMedia", false);
    m_mathMacros = cfg.readEntry("MathMacros", QString());
}

void Settings::save() const
{
    KConfigGroup cfg(KSharedConfig::openConfig(), groupName());
    cfg.writeEntry("Theme", m_mode == Application ? QStringLiteral("application") : QStringLiteral("github"));
    const char *variant = m_ghVariant == Light ? "light" : m_ghVariant == Dark ? "dark" : "auto";
    cfg.writeEntry("GithubVariant", QString::fromLatin1(variant));
    cfg.writeEntry("LoadRemoteMedia", m_loadRemoteMedia);
    cfg.writeEntry("MathMacros", m_mathMacros);
    cfg.sync();
}

void Settings::setMode(Mode mode)
{
    if (m_mode == mode) {
        return;
    }
    m_mode = mode;
    save();
    Q_EMIT changed();
}

void Settings::setGhVariant(GhVariant variant)
{
    if (m_ghVariant == variant) {
        return;
    }
    m_ghVariant = variant;
    save();
    Q_EMIT changed();
}

void Settings::setLoadRemoteMedia(bool enabled)
{
    if (m_loadRemoteMedia == enabled) {
        return;
    }
    m_loadRemoteMedia = enabled;
    save();
    Q_EMIT changed();
}

void Settings::setMathMacros(const QString &macros)
{
    if (m_mathMacros == macros) {
        return;
    }
    m_mathMacros = macros;
    save();
    Q_EMIT changed();
}
