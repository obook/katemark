#include "plugin.h"
#include "configpage.h"
#include "pluginview.h"

#include <KPluginFactory>

K_PLUGIN_FACTORY_WITH_JSON(KatemarkPluginFactory, "katemark.json", registerPlugin<KatemarkPlugin>();)

KatemarkPlugin::KatemarkPlugin(QObject *parent, const QVariantList &args)
    : KTextEditor::Plugin(parent)
{
    Q_UNUSED(args);
}

QObject *KatemarkPlugin::createView(KTextEditor::MainWindow *mainWindow)
{
    return new PluginView(this, mainWindow);
}

int KatemarkPlugin::configPages() const
{
    return 1;
}

KTextEditor::ConfigPage *KatemarkPlugin::configPage(int number, QWidget *parent)
{
    if (number != 0) {
        return nullptr;
    }
    return new ConfigPage(parent);
}

#include "plugin.moc"
