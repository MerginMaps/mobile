#include <QCoreApplication>
#include <QPointer>
#include <QtCore>

#include "ioshandler.h"


// NOTE: WILL BE PROBABLY EMPTY -- no implemenation --

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
