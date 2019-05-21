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
      PENDING, // sent version check, but waiting for the response
      NOT_FOUND, // cannot be checked due to network/wrong url
      OK, // server version satisfied requirements
      INCOMPATIBLE // server version below requirements
    };
    Q_ENUMS( VersionStatus )
};

#endif // MERGINAPISTATUS_H
