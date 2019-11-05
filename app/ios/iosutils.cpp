#include "iosutils.h"

IosUtils::IosUtils( QObject *parent ): QObject( parent )
{
}

bool IosUtils::isIos() const
{
#ifdef Q_OS_IOS
  return true;
#else
  return false;
#endif
}
