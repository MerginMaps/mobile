#include <QCoreApplication>
#include <QPointer>
#include <QtCore>

#include "ioshandler.h"

IOSHandler::IOSHandler(QObject *parent) : QObject(parent)
{

}

//IOSHandler *IOSHandler::instance()
//{
//    if (!mInstance) {
//        QCoreApplication* app = QCoreApplication::instance();
//        mInstance = new IOSHandler(app);
//    }
//    return mInstance;
//}
