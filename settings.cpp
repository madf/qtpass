#include "settings.h"

#include <QSettings>

using QtPass::Settings;

Settings::Window::Window(QSettings& src)
{
  src.beginGroup("mainwindow");
  geometry = src.value("geometry").toByteArray();
  state = src.value("savestate").toByteArray();
  position = src.value("pos").toPoint();
  size = src.value("size").toSize();
  splitterLeft = src.value("splitterLeft").toInt();
  splitterRight = src.value("splitterRight").toInt();
  maximized = src.value("maximized").toBool();
  src.endGroup();
}

Settings::Window::Window(QByteArray geo, QByteArray st, QPoint p, QSize s, QList<int> spl)
  : geometry(geo),
    state(st),
    position(p),
    size(s),
    maximized(false),
    splitterLeft(spl[0]),
    splitterRight(spl[1])
{
}

Settings::Settings(QSettings& src)
  : m_window(src),
    m_version(src.value("version").toString())
{
}
