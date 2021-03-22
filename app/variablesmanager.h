/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef VARIABLESMANAGER_H
#define VARIABLESMANAGER_H

#include <QObject>
#include "merginapi.h"
#include "qgsproject.h"
#include "qvariant.h"
#include "qgsexpressionfunction.h"

#include "androidutils.h"
#include "inpututils.h"

class MerginApi;

/*
 * The class sets global and project variables related to Mergin.
 */
class VariablesManager : public QObject
{
  public:
    VariablesManager( MerginApi *merginApi, QObject *parent = nullptr );

    void removeMerginProjectVariables( QgsProject *project );

    class ExifFunction : public QgsScopedExpressionFunction
    {
      public:
        ExifFunction()
          : QgsScopedExpressionFunction( QStringLiteral( "read_exif" ), QgsExpressionFunction::ParameterList() << QgsExpressionFunction::Parameter( QStringLiteral( "absolute_file_path" ) ) << QgsExpressionFunction::Parameter( QStringLiteral( "exif_tag" ) ), QStringLiteral( "Custom" ) ) {}

        /**
         * Custom expression function to read EXIF metadata. It is platform dependent and currently supported only on Android.
         * Example field definition: read_exif('<ABSOLUTE_PATH_TO_IMAGE>', '<EXIF_TAG_STRING>')
         * @param values - suppose to contain 2 parameters:
         *  - absolute_file_path: Absolute path of an image that exif attribute will be read from,
         *  - exif_tag: The officially supported EXIF TAG name.
         * @return Formatted attribute value for given parameters, if parameters are not valid, returns empty string
         */
        QVariant func( const QVariantList &values, const QgsExpressionContext *, QgsExpression *, const QgsExpressionNodeFunction * ) override
        {
#ifdef ANDROID
          if ( values.size() != 2 ) return QVariant();

          QString filepath( values.at( 0 ).toString() );
          QString exifTag( values.at( 1 ).toString() );
          QString result = AndroidUtils::getExifInfo( filepath, exifTag );
          return InputUtils::formatExifString( result, exifTag );
# else
          return QString();
#endif
        }

        QgsScopedExpressionFunction *clone() const override
        {
          return new ExifFunction();
        }

    };

  public slots:
    void merginProjectChanged( QgsProject *project );

  private slots:
    void apiRootChanged();
    void authChanged();
    void setVersionVariable( const QString &projectFullName );

  private:
    MerginApi *mMerginApi = nullptr;
    QgsProject *mCurrentProject = nullptr;

    void setProjectVariables();
};

#endif // VARIABLESMANAGER_H
