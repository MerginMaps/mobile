#ifndef QIDEVICE_H
#define QIDEVICE_H

#include <QObject>
#include <QVariantMap>

/// Device information provider

class QIDevice : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool screenFillStatusBar READ screenFillStatusBar WRITE setScreenFillStatusBar NOTIFY screenFillStatusBarChanged)
    Q_PROPERTY(int screenWidth READ screenWidth WRITE setScreenWidth NOTIFY screenWidthChanged)
    Q_PROPERTY(int screenHeight READ screenHeight WRITE setScreenHeight NOTIFY screenHeightChanged)
    Q_PROPERTY(QString identifierForVendor READ identifierForVendor WRITE setIdentifierForVendor NOTIFY identifierForVendorChanged)

public:
    ~QIDevice();

    /// It is truth if the status bar area should be filled by the application
    bool screenFillStatusBar() const;
    void setScreenFillStatusBar(bool screenFillStatusBar);

    int screenWidth() const;
    void setScreenWidth(int screenWidth);

    int screenHeight() const;
    void setScreenHeight(int screenHeight);

    static QIDevice* instance();

    QString identifierForVendor() const;
    void setIdentifierForVendor(const QString &identifierForVendor);

signals:
    void screenFillStatusBarChanged();
    void screenHeightChanged();
    void screenWidthChanged();

    void identifierForVendorChanged();

private:
    // Fetch device specific information.
    QVariantMap fetch() const;

    QIDevice(QObject* parent = 0);

    bool m_screenFillStatusBar;
    int m_screenWidth;
    int m_screenHeight;

    QString m_identifierForVendor;
};

#endif // QIDEVICE_H
