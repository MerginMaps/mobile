#ifndef VARIABLESMANAGER_H
#define VARIABLESMANAGER_H

#include <QObject>
#include "merginapi.h"
#include "qgsproject.h"

class MerginApi;

class VariablesManager : public QObject
{
  public:
    VariablesManager( MerginApi *merginApi, QObject *parent = nullptr );

  public slots:
    void apiRootChanged();
    void authChanged();
    void merginProjectChanged( QgsProject *project );

  private:
    MerginApi *mMerginApi = nullptr;
};

#endif // VARIABLESMANAGER_H
