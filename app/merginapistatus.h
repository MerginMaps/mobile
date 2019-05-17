#ifndef MERGINAPISTATUS_H
#define MERGINAPISTATUS_H
#pragma once

#include <QObject>

class MerginApiStatus
{
    Q_GADGET
  public:
    explicit MerginApiStatus();

    enum VersionStatus
    {
      UNKNOWN, // unchecked
      NOT_FOUND, // cannot be checked due to network/wrong url
      PASSED,
      FAILED
    };
    Q_ENUMS( VersionStatus )
};

#endif // MERGINAPISTATUS_H
