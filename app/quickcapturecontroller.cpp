/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "quickcapturecontroller.h"

#include <QBuffer>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QPixmap>
#include <QUrl>

#include "coreutils.h"
#include "featurelayerpair.h"
#include "fieldprofile.h"
#include "inpututils.h"
#include "variablesmanager.h"
#include "position/positionkit.h"

#include "qgsproject.h"
#include "qgsvectorlayer.h"
#include "qgsvectorlayerutils.h"
#include "qgseditorwidgetsetup.h"
#include "qgsexpressioncontextutils.h"
#include "qgsmarkersymbol.h"
#include "qgsrendercontext.h"
#include "qgsrenderer.h"
#include "qgssymbollayerutils.h"

QuickCaptureController::QuickCaptureController( QObject *parent )
  : QObject( parent )
{
}

void QuickCaptureController::reload()
{
  clearConfiguration();

  if ( !mProject )
  {
    emit configurationChanged();
    return;
  }

  const QVariantMap config = FieldProfile::read( mProject ).value( QStringLiteral( "quickCapture" ) ).toMap();
  if ( config.isEmpty() )
  {
    emit configurationChanged();
    return;
  }

  // the layer can be referenced by id or by name
  const QString layerRef = config.value( QStringLiteral( "layer" ) ).toString();
  QgsVectorLayer *layer = mProject->mapLayer<QgsVectorLayer *>( layerRef );
  if ( !layer )
  {
    const QList<QgsMapLayer *> layers = mProject->mapLayersByName( layerRef );
    if ( !layers.isEmpty() )
      layer = qobject_cast<QgsVectorLayer *>( layers.first() );
  }

  if ( !layer || !layer->isValid() || layer->geometryType() != Qgis::GeometryType::Point )
  {
    CoreUtils::log( QStringLiteral( "Quick capture" ), QStringLiteral( "Layer %1 not found or not a valid point layer" ).arg( layerRef ) );
    emit configurationChanged();
    return;
  }

  mLayerId = layer->id();
  mTypeField = config.value( QStringLiteral( "typeField" ) ).toString();
  mMaxHorizontalAccuracy = config.value( QStringLiteral( "maxHorizontalAccuracy" ), -1 ).toDouble();

  const int typeFieldIndex = layer->fields().lookupField( mTypeField );

  if ( config.contains( QStringLiteral( "types" ) ) )
  {
    mTypes = config.value( QStringLiteral( "types" ) ).toList();
  }
  else if ( typeFieldIndex >= 0 )
  {
    // reuse the value map of the type field defined in the QGIS form
    const QgsEditorWidgetSetup setup = layer->editorWidgetSetup( typeFieldIndex );
    if ( setup.type() == QLatin1String( "ValueMap" ) )
    {
      const QVariantList valueMap = setup.config().value( QStringLiteral( "map" ) ).toList();
      for ( const QVariant &item : valueMap )
      {
        const QVariantMap entry = item.toMap();
        for ( auto it = entry.constBegin(); it != entry.constEnd(); ++it )
        {
          mTypes << QVariantMap( { { QStringLiteral( "label" ), it.key() }, { QStringLiteral( "value" ), it.value() } } );
        }
      }
    }
  }

  // icon of each type: an image from the project ("icon") or the type's symbol on the map
  const QDir projectDir = QFileInfo( FieldProfile::filePath( mProject ) ).absoluteDir();
  for ( QVariant &item : mTypes )
  {
    QVariantMap type = item.toMap();
    const QString icon = type.value( QStringLiteral( "icon" ) ).toString();
    if ( !icon.isEmpty() && QFileInfo::exists( projectDir.absoluteFilePath( icon ) ) )
      type[QStringLiteral( "iconSource" )] = QUrl::fromLocalFile( projectDir.absoluteFilePath( icon ) ).toString();
    else
      type[QStringLiteral( "iconSource" )] = symbolIcon( layer, typeFieldIndex, type.value( QStringLiteral( "value" ) ) );
    item = type;
  }

  mGridView = config.value( QStringLiteral( "view" ), QStringLiteral( "grid" ) ).toString() != QLatin1String( "list" );

  const QVariantMap media = config.value( QStringLiteral( "media" ) ).toMap();
  mMediaTypeValue = media.value( QStringLiteral( "typeValue" ) );

  const QStringList kinds = { QStringLiteral( "photo" ), QStringLiteral( "audio" ), QStringLiteral( "video" ) };
  for ( const QString &kind : kinds )
  {
    const QString fieldName = media.value( kind + QStringLiteral( "Field" ) ).toString();
    if ( fieldName.isEmpty() )
      continue;

    if ( layer->fields().lookupField( fieldName ) < 0 )
    {
      CoreUtils::log( QStringLiteral( "Quick capture" ), QStringLiteral( "Media field %1 not found in layer %2" ).arg( fieldName, layer->name() ) );
      continue;
    }

    mMediaFields.insert( kind, fieldName );
  }

  emit configurationChanged();
}

QString QuickCaptureController::symbolIcon( QgsVectorLayer *layer, int typeFieldIndex, const QVariant &typeValue ) const
{
  if ( !layer->renderer() )
    return QString();

  QgsFeature feature( layer->fields() );
  feature.setGeometry( QgsGeometry::fromPointXY( QgsPointXY( 0, 0 ) ) );
  if ( typeFieldIndex >= 0 )
    feature.setAttribute( typeFieldIndex, typeValue );

  QgsRenderContext context;
  context.setExpressionContext( QgsExpressionContext( QgsExpressionContextUtils::globalProjectLayerScopes( layer ) ) );
  context.expressionContext().setFeature( feature );

  std::unique_ptr<QgsFeatureRenderer> renderer( layer->renderer()->clone() );
  renderer->startRender( context, layer->fields() );
  QgsSymbol *symbol = renderer->symbolForFeature( feature, context );
  std::unique_ptr<QgsSymbol> iconSymbol( symbol ? symbol->clone() : nullptr );
  renderer->stopRender( context );

  if ( !iconSymbol )
    return QString();

  // map symbols are a few millimetres, draw them to fill the icon
  const int iconSize = 128;
  if ( QgsMarkerSymbol *marker = dynamic_cast<QgsMarkerSymbol *>( iconSymbol.get() ) )
  {
    marker->setSizeUnit( Qgis::RenderUnit::Pixels );
    marker->setSize( iconSize * 0.8 );
  }

  const QPixmap pixmap = QgsSymbolLayerUtils::symbolPreviewPixmap( iconSymbol.get(), QSize( iconSize, iconSize ), 0, nullptr, false, &context.expressionContext() );

  QByteArray png;
  QBuffer buffer( &png );
  buffer.open( QIODevice::WriteOnly );
  pixmap.save( &buffer, "PNG" );
  return QStringLiteral( "data:image/png;base64," ) + QString::fromLatin1( png.toBase64() );
}

bool QuickCaptureController::capturePoint( const QVariant &typeValue )
{
  QVariantMap attributes;
  if ( !mTypeField.isEmpty() && typeValue.isValid() )
    attributes.insert( mTypeField, typeValue );

  const QString label = typeLabel( typeValue );
  return saveFeature( attributes, label.isEmpty() ? tr( "Point saved" ) : tr( "%1 saved" ).arg( label ) );
}

QString QuickCaptureController::mediaTargetDir( const QString &kind ) const
{
  if ( !mProject || !mMediaFields.contains( kind ) )
    return QString();

  return InputUtils::resolveTargetDir( mProject->homePath(), fieldConfig( mMediaFields.value( kind ) ), FeatureLayerPair(), FeatureLayerPair(), mProject );
}

bool QuickCaptureController::captureMedia( const QString &kind, const QString &mediaPath )
{
  if ( !mProject || !mMediaFields.contains( kind ) || mediaPath.isEmpty() )
  {
    emit captureFailed( tr( "Could not attach the file" ) );
    return false;
  }

  const QString fieldName = mMediaFields.value( kind );
  const QVariantMap config = fieldConfig( fieldName );
  const QString targetDir = mediaTargetDir( kind );

  // media paths can come as file urls from pickers
  const QString sourcePath = InputUtils::getRelativePath( mediaPath, QString() );
  QString absolutePath = QDir( targetDir ).absoluteFilePath( QFileInfo( sourcePath ).fileName() );

  // the Android camera already copies the captured file to the target folder and returns its temporary path
  const bool copyNeeded = !QFileInfo::exists( absolutePath );
  if ( copyNeeded )
  {
    InputUtils::createDirectory( targetDir );
    if ( !InputUtils::copyFile( sourcePath, absolutePath ) )
    {
      CoreUtils::log( QStringLiteral( "Quick capture" ), QStringLiteral( "Failed to copy %1 to %2" ).arg( sourcePath, absolutePath ) );
      emit captureFailed( tr( "Could not attach the file" ) );
      return false;
    }
  }

  if ( kind == QLatin1String( "photo" ) )
    InputUtils::rescaleImage( absolutePath, mProject );

  const int relativeStorage = config.value( QStringLiteral( "RelativeStorage" ) ).toInt();
  const QString prefix = InputUtils::resolvePrefixForRelativePath( relativeStorage, mProject->homePath(), targetDir );

  QVariantMap attributes;
  attributes.insert( fieldName, InputUtils::getRelativePath( absolutePath, prefix ) );
  if ( !mTypeField.isEmpty() && mMediaTypeValue.isValid() )
    attributes.insert( mTypeField, mMediaTypeValue );

  QString message;
  if ( kind == QLatin1String( "photo" ) )
    message = tr( "Photo saved" );
  else if ( kind == QLatin1String( "audio" ) )
    message = tr( "Audio saved" );
  else
    message = tr( "Video saved" );

  if ( !saveFeature( attributes, message ) )
  {
    // do not leave orphaned copies in the project, files that were already there are kept
    if ( copyNeeded )
      QFile::remove( absolutePath );
    return false;
  }
  return true;
}

FeatureLayerPair QuickCaptureController::prepareFeature( const QVariant &typeValue )
{
  QVariantMap attributes;
  if ( !mTypeField.isEmpty() && typeValue.isValid() )
    attributes.insert( mTypeField, typeValue );

  double accuracy = -1;
  return createFeature( attributes, accuracy );
}

QString QuickCaptureController::mediaNotAllowedReason( const QString &kind )
{
  if ( !mMediaFields.contains( kind ) )
    return tr( "Could not attach the file" );

  // a placeholder file name stands for the media the user would record here
  QVariantMap attributes;
  attributes.insert( mMediaFields.value( kind ), QStringLiteral( "pending" ) );
  if ( !mTypeField.isEmpty() && mMediaTypeValue.isValid() )
    attributes.insert( mTypeField, mMediaTypeValue );

  double accuracy = -1;
  QString error;
  const FeatureLayerPair pair = createFeature( attributes, accuracy, &error );
  if ( !pair.isValid() )
    return error;

  return failedHardConstraints( pair );
}

QString QuickCaptureController::failedHardConstraints( const FeatureLayerPair &pair ) const
{
  QgsVectorLayer *layer = pair.layer();
  if ( !layer )
    return QString();

  QStringList messages;
  const QgsFields fields = layer->fields();
  for ( int i = 0; i < fields.count(); ++i )
  {
    QStringList errors;
    if ( !QgsVectorLayerUtils::validateAttribute( layer, pair.feature(), i, errors, QgsFieldConstraints::ConstraintStrengthHard ) )
    {
      const QString description = fields.at( i ).constraints().constraintDescription();
      messages << ( description.isEmpty() ? errors.join( QStringLiteral( "; " ) ) : description );
    }
  }
  messages.removeDuplicates();
  return messages.join( QLatin1Char( '\n' ) );
}

FeatureLayerPair QuickCaptureController::createFeature( const QVariantMap &attributes, double &accuracy, QString *error )
{
  // reports the error to the caller when asked, otherwise to the user
  auto fail = [this, error]( const QString & message )
  {
    if ( error )
      *error = message;
    else
      emit captureFailed( message );
    return FeatureLayerPair();
  };

  QgsVectorLayer *layer = captureLayer();
  if ( !layer || !mVariablesManager || !mVariablesManager->positionKit() )
  {
    return fail( tr( "Quick capture is not available in this project" ) );
  }

  PositionKit *positionKit = mVariablesManager->positionKit();
  if ( !positionKit->hasPosition() || positionKit->positionCoordinate().isEmpty() )
  {
    return fail( tr( "GPS position is not available" ) );
  }

  accuracy = positionKit->horizontalAccuracy();
  if ( mMaxHorizontalAccuracy > 0 && ( accuracy < 0 || accuracy > mMaxHorizontalAccuracy ) )
  {
    return fail( tr( "GPS accuracy is too low (required %1 m)" ).arg( mMaxHorizontalAccuracy ) );
  }

  QgsPoint position = positionKit->positionCoordinate();
  if ( positionKit->requireAntennaHeightTransform() )
    position.setZ( position.z() - positionKit->antennaHeight() );

  const QgsGeometry geometryInLayerCrs = InputUtils::transformGeometry( QgsGeometry( position.clone() ), positionKit->positionCrs3D(), layer );

  // match the layer geometry type (Z/M, single/multi point)
  const QVector<QgsGeometry> geometries = geometryInLayerCrs.coerceToType( layer->wkbType() );
  if ( geometries.isEmpty() )
  {
    return fail( tr( "Could not create the point" ) );
  }
  const QgsGeometry geometry = geometries.first();

  // the point comes from GPS, so @position_from_gps is true while default values are evaluated
  const bool useGpsPoint = mVariablesManager->useGpsPoint();
  mVariablesManager->setUseGpsPoint( true );
  FeatureLayerPair pair = InputUtils::createFeatureLayerPair( layer, geometry, mVariablesManager );
  mVariablesManager->setUseGpsPoint( useGpsPoint );

  QgsFeature &feature = pair.featureRef();
  for ( auto it = attributes.constBegin(); it != attributes.constEnd(); ++it )
  {
    feature.setAttribute( it.key(), it.value() );
  }

  return pair;
}

bool QuickCaptureController::saveFeature( const QVariantMap &attributes, const QString &successMessage )
{
  double accuracy = -1;
  FeatureLayerPair pair = createFeature( attributes, accuracy );
  if ( !pair.isValid() )
    return false;

  // the layer's hard constraints apply as in the form, e.g. geofencing
  const QString constraintsMessage = failedHardConstraints( pair );
  if ( !constraintsMessage.isEmpty() )
  {
    emit captureFailed( constraintsMessage );
    return false;
  }

  QgsVectorLayer *layer = pair.layer();
  QgsFeature &feature = pair.featureRef();
  layer->startEditing();
  layer->addFeature( feature );

  if ( !layer->commitChanges() )
  {
    CoreUtils::log( QStringLiteral( "Quick capture" ),
                    QStringLiteral( "Failed to commit changes:\n%1" ).arg( layer->commitErrors().join( QLatin1Char( '\n' ) ) ) );
    layer->rollBack();
    emit captureFailed( tr( "Could not save the point" ) );
    return false;
  }

  layer->triggerRepaint();

  QString message = successMessage;
  if ( accuracy >= 0 )
    message += QStringLiteral( " (±%1 m)" ).arg( accuracy, 0, 'f', 1 );

  emit captureSucceeded( message );
  return true;
}

QgsVectorLayer *QuickCaptureController::captureLayer() const
{
  if ( !mProject || mLayerId.isEmpty() )
    return nullptr;

  QgsVectorLayer *layer = mProject->mapLayer<QgsVectorLayer *>( mLayerId );
  if ( !layer || !layer->isValid() )
    return nullptr;

  return layer;
}

QVariantMap QuickCaptureController::fieldConfig( const QString &fieldName ) const
{
  QgsVectorLayer *layer = captureLayer();
  if ( !layer )
    return QVariantMap();

  const int index = layer->fields().lookupField( fieldName );
  if ( index < 0 )
    return QVariantMap();

  return layer->editorWidgetSetup( index ).config();
}

QString QuickCaptureController::typeLabel( const QVariant &typeValue ) const
{
  for ( const QVariant &item : mTypes )
  {
    const QVariantMap type = item.toMap();
    if ( type.value( QStringLiteral( "value" ) ) == typeValue )
      return type.value( QStringLiteral( "label" ) ).toString();
  }
  return typeValue.toString();
}

void QuickCaptureController::clearConfiguration()
{
  mLayerId.clear();
  mTypeField.clear();
  mTypes.clear();
  mMaxHorizontalAccuracy = -1;
  mMediaFields.clear();
  mMediaTypeValue = QVariant();
}

QgsProject *QuickCaptureController::qgsProject() const
{
  return mProject;
}

void QuickCaptureController::setQgsProject( QgsProject *project )
{
  if ( mProject == project )
    return;

  mProject = project;
  emit qgsProjectChanged();
  reload();
}

VariablesManager *QuickCaptureController::variablesManager() const
{
  return mVariablesManager;
}

void QuickCaptureController::setVariablesManager( VariablesManager *variablesManager )
{
  if ( mVariablesManager == variablesManager )
    return;

  mVariablesManager = variablesManager;
  emit variablesManagerChanged();
}

bool QuickCaptureController::enabled() const
{
  return !mLayerId.isEmpty();
}

QVariantList QuickCaptureController::types() const
{
  return mTypes;
}

QStringList QuickCaptureController::mediaKinds() const
{
  if ( !enabled() )
    return QStringList();

  QStringList kinds;
  for ( const QString &kind : { QStringLiteral( "photo" ), QStringLiteral( "audio" ), QStringLiteral( "video" ) } )
  {
    if ( mMediaFields.contains( kind ) )
      kinds << kind;
  }
  return kinds;
}
