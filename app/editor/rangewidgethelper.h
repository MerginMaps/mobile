#ifndef NUMBERWIDGETHANDLER_H
#define NUMBERWIDGETHANDLER_H

#include <QObject>
#include <QVariant>
#include <QDoubleValidator>
#include <memory>

#include "qgsfield.h"

#include "inputnumbervalidator.h"

/**
 * \brief The NumberWidgetHandler class suits as a helper class for range widgets.
 */
class RangeWidgetHelper : public QObject
{
    Q_OBJECT

    Q_PROPERTY( QVariantMap widgetConfig READ widgetConfig WRITE setWidgetConfig NOTIFY widgetConfigChanged )
    Q_PROPERTY( InputNumberValidator *validator READ validator NOTIFY validatorChanged )
    Q_PROPERTY( int precision READ precision NOTIFY precisionChanged )
    Q_PROPERTY( QString suffix READ suffix NOTIFY suffixChanged )
    Q_PROPERTY( double step READ step NOTIFY stepChanged )

  public:
    explicit RangeWidgetHelper( QObject *parent = nullptr );
    virtual ~RangeWidgetHelper() {};

    void setWidgetConfig( QVariantMap config );

    QVariantMap widgetConfig() const;

    int precision() const;

    double step() const;

    QString suffix() const;

    InputNumberValidator *validator() const;

  signals:
    void validatorChanged( InputNumberValidator *validator );
    void widgetConfigChanged( QVariantMap config );
    void precisionChanged( int precision );
    void suffixChanged( QString suffix );
    void stepChanged( double step );

  private:
    void setup();

    QVariantMap mWidgetConfig;
    double mStep = .0;
    QString mSuffix;
    int mPrecision;

    std::unique_ptr<InputNumberValidator> mValidator; //owned
};

#endif // NUMBERWIDGETHANDLER_H


