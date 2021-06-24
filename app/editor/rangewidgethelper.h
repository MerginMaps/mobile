/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef NUMBERWIDGETHANDLER_H
#define NUMBERWIDGETHANDLER_H

#include <QObject>
#include <QVariant>
#include <QDoubleValidator>
#include <memory>

#include "qgsfield.h"

/**
 * \brief The NumberWidgetHandler class suits as a helper class for range widgets.
 */
class RangeWidgetHelper : public QObject
{
    Q_OBJECT

    Q_PROPERTY( QVariantMap widgetConfig READ widgetConfig WRITE setWidgetConfig NOTIFY widgetConfigChanged )
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

  signals:
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
};

#endif // NUMBERWIDGETHANDLER_H


