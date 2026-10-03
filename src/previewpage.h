// previewpage.h
// The two guards around the web page: which requests it may make, and what a click
// on a link does.
// Code by uwuclxdy, moved out of previewwidget.cpp.
// License: GPL-3.0-or-later

#pragma once

#include <QDir>
#include <QFileInfo>
#include <QMetaObject>
#include <QUrl>
#include <QWebEnginePage>
#include <QWebEngineUrlRequestInfo>
#include <QWebEngineUrlRequestInterceptor>

#include <functional>

// Confine the page to its document folder: a malicious markdown (raw HTML + JS on a
// file:// origin) could otherwise read any local file via fetch/img/etc. Only assets
// under the canonical document root pass; remote requests are limited to images/media
// and only when the user opted in.
class LocalFileGuard : public QWebEngineUrlRequestInterceptor
{
public:
    using QWebEngineUrlRequestInterceptor::QWebEngineUrlRequestInterceptor;

    void setRoot(const QString &dir)
    {
        m_root = dir.isEmpty() ? QString() : QDir(dir).canonicalPath();
    }

    void setAllowRemote(bool allow)
    {
        m_allowRemote = allow;
    }

    void interceptRequest(QWebEngineUrlRequestInfo &info) override
    {
        const QUrl url = info.requestUrl();
        const QString scheme = url.scheme();
        if (scheme == QLatin1String("qrc") || scheme == QLatin1String("data") || scheme == QLatin1String("about") || scheme == QLatin1String("blob")) {
            return;
        }
        if (scheme == QLatin1String("file")) {
            const QString path = QFileInfo(url.toLocalFile()).canonicalFilePath();
            const bool ok = !m_root.isEmpty() && !path.isEmpty() && (path == m_root || path.startsWith(m_root + QLatin1Char('/')));
            if (!ok) {
                info.block(true);
            }
            return;
        }
        const bool isMedia = info.resourceType() == QWebEngineUrlRequestInfo::ResourceTypeImage
            || info.resourceType() == QWebEngineUrlRequestInfo::ResourceTypeMedia;
        if (!(m_allowRemote && isMedia)) {
            info.block(true);
        }
    }

private:
    QString m_root;
    bool m_allowRemote = false;
};

// Keep link clicks from turning the preview into a browser: same-document anchors
// scroll in place; everything else is handed back to the host via onLinkActivated.
class PreviewPage : public QWebEnginePage
{
public:
    using QWebEnginePage::QWebEnginePage;

    std::function<void(const QUrl &)> onLinkActivated;

    bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame) override
    {
        Q_UNUSED(isMainFrame);
        if (type == NavigationTypeLinkClicked) {
            if (url.hasFragment() && url.matches(this->url(), QUrl::RemoveFragment)) {
                return true;
            }
            if (onLinkActivated) {
                QMetaObject::invokeMethod(
                    this, [cb = onLinkActivated, u = url]() { cb(u); }, Qt::QueuedConnection);
            }
            return false;
        }
        return true;
    }
};
