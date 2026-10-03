#pragma once

#include <QObject>
#include <QString>

/**
 * Plugin-wide settings, persisted in the application's KSharedConfig under the
 * group "Katemark". A single instance broadcasts changed() so every open
 * preview re-themes at once.
 */
class Settings : public QObject
{
    Q_OBJECT
public:
    enum Mode {
        GitHub,      // GitHub's own light/dark palette
        Application, // match the active editor/system theme
    };
    enum GhVariant {
        Auto,  // follow the application palette (dark vs light)
        Light,
        Dark,
    };

    static Settings *self();

    Mode mode() const
    {
        return m_mode;
    }
    GhVariant ghVariant() const
    {
        return m_ghVariant;
    }
    bool loadRemoteMedia() const { return m_loadRemoteMedia; }
    // Render only what GitHub renders: no CodiMD, Obsidian or MkDocs syntax.
    bool githubOnly() const
    {
        return m_githubOnly;
    }
    // \newcommand lines defined before every document.
    QString mathMacros() const
    {
        return m_mathMacros;
    }

    void setMode(Mode mode);
    void setGhVariant(GhVariant variant);
    void setLoadRemoteMedia(bool enabled);
    void setMathMacros(const QString &macros);
    void setGithubOnly(bool enabled);

    void load();
    void save() const;

Q_SIGNALS:
    void changed();

private:
    explicit Settings(QObject *parent = nullptr);

    Mode m_mode = GitHub;
    GhVariant m_ghVariant = Auto;
    bool m_loadRemoteMedia = false;
    QString m_mathMacros;
    bool m_githubOnly = false;
};
