#include <QCoreApplication>
#include <QPointer>
#include <QtCore>
#include "iossystemdispatcher.h"

typedef bool (*handler)(QVariantMap& data);
static QMap<QString,handler> handlers;
static QPointer<QISystemDispatcher> m_instance;

QISystemDispatcher *QISystemDispatcher::instance()
{
    if (!m_instance) {
        QCoreApplication* app = QCoreApplication::instance();
        m_instance = new QISystemDispatcher(app);
    }
    return m_instance;
}

QISystemDispatcher::QISystemDispatcher(QObject *parent) : QObject(parent) {
}

bool QISystemDispatcher::dispatch(QString type , QVariantMap message) {

    QMetaObject::invokeMethod(this,"dispatched",Qt::QueuedConnection,
                              Q_ARG(QString , type),
                              Q_ARG(QVariantMap,message));

    bool res = false;
    if (handlers.contains(type)) {
        res = handlers[type](message);
    }

    return res;
}

bool QISystemDispatcher::addListener(QString name, bool (*func)(QVariantMap&))
{
    if (handlers.contains(name)) {
        qWarning() << QString("%s is already registered").arg(name);
        return false;
    }

    handlers[name] = func;
    return true;
}
