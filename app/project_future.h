#ifndef PROJECT_FUTURE_H
#define PROJECT_FUTURE_H

#include <QObject>
#include <QDateTime>

enum ProjectStatus_future
{
  _NoVersion,  //!< the project is not available locally
  _UpToDate,   //!< both server and local copy are in sync with no extra modifications
  _OutOfDate,  //!< server has newer version than what is available locally (but the project is not modified locally)
  _Modified,    //!< there are some local modifications in the project that need to be pushed (note: also server may have newer version)
  _NonProjectItem      //!< only for mock projects, acts like a hook to enable extra functionality for models working with projects .
  // TODO: replace _NonProjectItem with footer property in ListView
  // TODO2: add orphaned state?
};
Q_ENUMS( ProjectStatus_future )

struct RemoteProject
{
  QString projectName;
  QString projectNamespace;

  QString projectIdentifier() { return QString(); };

  QDateTime serverUpdated; // available latest version of project files on server // TODO: maybe we do not need this at all

  bool pending = false;
  ProjectStatus_future status = ProjectStatus_future::_NoVersion;
  qreal progress = 0;
};


struct MerginProject_deprecated
{
  QString projectName;
  QString projectNamespace;
  QString projectDir;  // full path to the project directory
  QDateTime clientUpdated; // client's version of project files
  QDateTime serverUpdated; // available latest version of project files on server
  bool pending = false; // if there is a pending request for downlaod/update a project
  ProjectStatus_future status = ProjectStatus_future::_NoVersion;
  qreal progress = 0;  // progress in case of pending download/upload (values [0..1])
};


class Project_future : public QObject
{
    Q_OBJECT
  public:
  explicit Project_future( QObject *parent = nullptr );
  ~Project_future() override {}

};

#endif // PROJECT_FUTURE_H
