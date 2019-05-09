#include "inpututils.h"
#include "qgsvectorlayer.h"
#include <QFile>
#include <QFileInfo>

InputUtils::InputUtils( QObject *parent ): QObject( parent )
{
}

bool InputUtils::removeFile( const QString &filePath )
{
  QFile file( filePath );
  return file.remove( filePath );
}

bool InputUtils::copyFile( const QString &srcPath, const QString &dstPath )
{
  QString modSrcPath = srcPath;
  if ( srcPath.startsWith( "file://" ) )
  {
    modSrcPath = modSrcPath.replace( "file://", "" );
  }

  return QFile::copy( modSrcPath, dstPath );
}

QString InputUtils::getFileName( const QString &filePath )
{
  QFileInfo fileInfo( filePath );
  return fileInfo.fileName();
}

QString InputUtils::formatProjectName( const QString &fullProjectName )
{
  if ( fullProjectName.contains( "/" ) )
  {
    QStringList list = fullProjectName.split( "/" );
    return QString( "%1/<b>%2</b>" ).arg( list.at( 0 ) ).arg( list.at( 1 ) );

  }
  else
  {
    return QString( "<b>%1</b>" ).arg( fullProjectName );
  }
}

void InputUtils::setExtentToFeature( const QgsQuickFeatureLayerPair &pair, QgsQuickMapSettings *mapSettings, double panelOffsetRatio )
{

  if ( !mapSettings )
    return;

  if ( !pair.layer() )
    return;

  if ( !pair.feature().isValid() )
    return;

  QgsGeometry geom = pair.feature().geometry();
  if ( geom.isNull() || !geom.constGet() )
    return;

  QgsRectangle bbox = mapSettings->mapSettings().layerExtentToOutputExtent( pair.layer(), geom.boundingBox() );
  QgsRectangle currentExtent = mapSettings->mapSettings().extent();
  QgsPointXY currentExtentCenter = currentExtent.center();
  QgsPointXY featureCenter = bbox.center();

  double panelOffset = ( currentExtent.yMaximum() - currentExtent.yMinimum() ) * panelOffsetRatio / 2;
  double offsetX = currentExtentCenter.x() - featureCenter.x();
  double offsetY = currentExtentCenter.y() - featureCenter.y();
  currentExtent.setXMinimum( currentExtent.xMinimum() - offsetX );
  currentExtent.setXMaximum( currentExtent.xMaximum() - offsetX );
  currentExtent.setYMinimum( currentExtent.yMinimum() - offsetY - panelOffset );
  currentExtent.setYMaximum( currentExtent.yMaximum() - offsetY - panelOffset );
  mapSettings->setExtent( currentExtent );
}
