#pragma once
#include <QObject>
#include <QVariantMap>

/// QISystemMessenger provides an simple async messaging interface between C/C++/QML and Objective-C source code.
/**
 */

class QISystemDispatcher : public QObject
{
    Q_OBJECT

  public:
    static QISystemDispatcher *instance();

    /// Deliver a message
    /** If there has a registered listener function , it will return TRUE. Otherwise, it will return FALSE.
     *
     * After processed by the registered helper, the "received" signal will be emitted
     * in next tick of event loop.
     */
    Q_INVOKABLE bool dispatch( QString type, QVariantMap message );

    /// Register a message listener.
    /**
     * @brief registerMessageHandler
     * @param name
     * @return TRUE if it is successfully. If it is already registered, it will return false.
     */
    bool addListener( QString name, bool ( *func )( QVariantMap & ) );

    /// Return TRUE if it there has a message helper registered already.
    bool hasListener( QString name );

  signals:
    /// The signal is emitted when a message is received.
    void dispatched( QString name, QVariantMap data );

  private:
    explicit QISystemDispatcher( QObject *parent = nullptr );

};
