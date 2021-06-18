#include "rangewidgethelper.h"

#include <QtMath>

RangeWidgetHelper::RangeWidgetHelper( QObject *parent ) :
  QObject( parent )
{
}

void RangeWidgetHelper::setWidgetConfig( QVariantMap config )
{
  if ( mWidgetConfig != config )
  {
    mWidgetConfig = config;
    emit widgetConfigChanged( mWidgetConfig );
  }

  setup();
}

QVariantMap RangeWidgetHelper::widgetConfig() const
{
  return mWidgetConfig;
}

double RangeWidgetHelper::step() const
{
  return mStep;
}

QString RangeWidgetHelper::suffix() const
{
  return mSuffix;
}

int RangeWidgetHelper::precision() const
{
  return mPrecision;
}

void RangeWidgetHelper::setup()
{
  // read the config and set bounds, suffix, step and validator
  if ( mWidgetConfig.empty() || mWidgetConfig["Style"] != "SpinBox" ) return;

  if ( mWidgetConfig.contains( "Precision" ) )
  {
    mPrecision = mWidgetConfig["Precision"].toInt();
    emit precisionChanged( mPrecision );
  }

  if ( mWidgetConfig.contains( "Suffix" ) )
  {
    mSuffix = mWidgetConfig["Suffix"].toString();
    emit suffixChanged( mSuffix );
  }

  // don't ever use a step smaller than would be visible in the widget
  // i.e. if showing 2 decimals, smallest increment will be 0.01
  // https://github.com/qgis/QGIS/blob/master/src/gui/editorwidgets/qgsdoublespinbox.cpp
  if ( mWidgetConfig.contains( "Step" ) )
  {
    mStep = qMax( mWidgetConfig["Step"].toDouble(), qPow( 10.0, 0.0 - mPrecision ) );
    emit stepChanged( mStep );
  }
}
