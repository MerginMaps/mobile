#include "androidutils.h"

#ifdef ANDROID
#include <QtAndroid>
#include <QAndroidJniObject>
#include <QDebug>
#endif

AndroidUtils::AndroidUtils(QObject* parent):QObject(parent)
{
}

void AndroidUtils::showToast(QString message)
{
#ifdef ANDROID
    QtAndroid::runOnAndroidThread([message] {
        QAndroidJniObject javaString = QAndroidJniObject::fromString(message);
        QAndroidJniObject toast = QAndroidJniObject::callStaticObjectMethod("android/widget/Toast", "makeText",
                                                                            "(Landroid/content/Context;Ljava/lang/CharSequence;I)Landroid/widget/Toast;",
                                                                            QtAndroid::androidActivity().object(),
                                                                            javaString.object(),
                                                                            jint(1));
        toast.callMethod<void>("show");
    });
#else
Q_UNUSED(message)
#endif
}

bool AndroidUtils::isAndroid() const
{
#ifdef ANDROID
    return true;
#else
    return false;
#endif
}

void AndroidUtils::requirePermissions()
{
    checkAndAcquirePermissions("android.permission.WRITE_EXTERNAL_STORAGE");
}

bool AndroidUtils::checkAndAcquirePermissions( const QString &permissionString )
{
  QtAndroid::PermissionResult r = QtAndroid::checkPermission( permissionString );
  if ( r == QtAndroid::PermissionResult::Denied )
  {
    QtAndroid::requestPermissionsSync( QStringList() << permissionString );
    r = QtAndroid::checkPermission( permissionString );
    if ( r == QtAndroid::PermissionResult::Denied )
    {
      return false;
    }
  }
  return true;
}
