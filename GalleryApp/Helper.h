#ifndef HELPER_H
#define HELPER_H

#include <QObject>

class Helper : public QObject
{
  Q_OBJECT
public:
  explicit Helper(QObject *parent = nullptr);

  // Install custom fonts
  static QString installFonts();

  // Calculates real screen DPR based on DPI
  static qreal calculateScreenDpr();

  // Calculates ratio between real DPR calculated by us with DPR calculated by QT that is later used in qml sizing
  static qreal calculateDpRatio();
};

#endif // HELPER_H
