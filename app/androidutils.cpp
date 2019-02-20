#include "androidutils.h"

#ifdef ANDROID
#include <QtAndroid>
#include <QAndroidJniObject>
#include <QAndroidJniEnvironment>
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
#ifdef ANDROID

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
#else
  Q_UNUSED(permissionString)
#endif
  return true;
}


void AndroidUtils::callImagePicker()
{
#ifdef ANDROID
    QAndroidJniObject ACTION_PICK = QAndroidJniObject::getStaticObjectField("android/content/Intent", "ACTION_PICK", "Ljava/lang/String;");
    QAndroidJniObject EXTERNAL_CONTENT_URI = QAndroidJniObject::getStaticObjectField("android/provider/MediaStore$Images$Media", "EXTERNAL_CONTENT_URI", "Landroid/net/Uri;");

    QAndroidJniObject intent=QAndroidJniObject("android/content/Intent", "(Ljava/lang/String;Landroid/net/Uri;)V", ACTION_PICK.object<jstring>(), EXTERNAL_CONTENT_URI.object<jobject>());

    if (ACTION_PICK.isValid() && intent.isValid())
    {
        intent.callObjectMethod("setType", "(Ljava/lang/String;)Landroid/content/Intent;", QAndroidJniObject::fromString("image/*").object<jstring>());
        QtAndroid::startActivity(intent.object<jobject>(), 101, this); // this as receiver
        qDebug() << "OK";
    }
    else
    {
        qDebug() << "ERROR";
    }
#endif
}

#ifdef ANDROID
void AndroidUtils::handleActivityResult(int receiverRequestCode, int resultCode, const QAndroidJniObject &data)
{

    jint RESULT_OK = QAndroidJniObject::getStaticField<jint>("android/app/Activity", "RESULT_OK");
    if (receiverRequestCode == 101 && resultCode == RESULT_OK)
    {
        QAndroidJniObject uri = data.callObjectMethod("getData", "()Landroid/net/Uri;");
        QAndroidJniObject dadosAndroid = QAndroidJniObject::getStaticObjectField("android/provider/MediaStore$MediaColumns", "DATA", "Ljava/lang/String;");
        QAndroidJniEnvironment env;
        jobjectArray projecao = (jobjectArray)env->NewObjectArray(1, env->FindClass("java/lang/String"), NULL);
        jobject projacaoDadosAndroid = env->NewStringUTF(dadosAndroid.toString().toStdString().c_str());
        env->SetObjectArrayElement(projecao, 0, projacaoDadosAndroid);
        QAndroidJniObject contentResolver = QtAndroid::androidActivity().callObjectMethod("getContentResolver", "()Landroid/content/ContentResolver;");
        QAndroidJniObject cursor = contentResolver.callObjectMethod("query", "(Landroid/net/Uri;[Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;Ljava/lang/String;)Landroid/database/Cursor;", uri.object<jobject>(), projecao, NULL, NULL, NULL);
        jint columnIndex = cursor.callMethod<jint>("getColumnIndex", "(Ljava/lang/String;)I", dadosAndroid.object<jstring>());
        cursor.callMethod<jboolean>("moveToFirst", "()Z");
        QAndroidJniObject result = cursor.callObjectMethod("getString", "(I)Ljava/lang/String;", columnIndex);
        QString selectedImagePath = "file://" + result.toString();
        emit imageSelected(selectedImagePath);
    }
    else
    {
        qDebug() << "Caminho errado";
    }

}
#endif
