#ifndef QTPASS_SETTINGS_H_
#define QTPASS_SETTINGS_H_

#include <QString>
#include <QByteArray>
#include <QPoint>
#include <QSize>

class QSettings;

namespace QtPass
{

class Settings
{
 public:
  struct Window
  {
    Window(QSettings& src);
    Window(QByteArray geo, QByteArray st, QPoint p, QSize s, QList<int> spl);

    QByteArray geometry;
    QByteArray state;
    QPoint position;
    QSize size;
    bool maximized;
    int splitterLeft;
    int splitterRight;
  };

  Settings(QSettings& src);

  void store(QSettings& dst);

  const Window& window() const { return m_window; }

  QString version() const { return m_version; }

 private:
  Window m_window;
  QString m_version;

};

}

#endif
