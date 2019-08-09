#include "ShapeSelGroupBox.h"
#include "ShapeSelGroupBoxPlugin.h"

#include <QtPlugin>

ShapeSelGroupBoxPlugin::ShapeSelGroupBoxPlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void ShapeSelGroupBoxPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool ShapeSelGroupBoxPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *ShapeSelGroupBoxPlugin::createWidget(QWidget *parent)
{
    return new ShapeSelGroupBox(parent);
}

QString ShapeSelGroupBoxPlugin::name() const
{
    return QLatin1String("ShapeSelGroupBox");
}

QString ShapeSelGroupBoxPlugin::group() const
{
    return QLatin1String("");
}

QIcon ShapeSelGroupBoxPlugin::icon() const
{
    return QIcon(QLatin1String(":/Icon/sector.png"));
}

QString ShapeSelGroupBoxPlugin::toolTip() const
{
    return QLatin1String("");
}

QString ShapeSelGroupBoxPlugin::whatsThis() const
{
    return QLatin1String("");
}

bool ShapeSelGroupBoxPlugin::isContainer() const
{
    return false;
}

QString ShapeSelGroupBoxPlugin::domXml() const
{
    return QLatin1String("<widget class=\"ShapeSelGroupBox\" name=\"shapeSelGroupBox\">\n</widget>\n");
}

QString ShapeSelGroupBoxPlugin::includeFile() const
{
    return QLatin1String("ShapeSelGroupBox.h");
}
#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(ShapeSelGroupboxPlugin, ShapeSelGroupBoxPlugin)
#endif // QT_VERSION < 0x050000
