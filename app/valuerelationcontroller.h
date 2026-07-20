/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef VALUERELATIONCONTROLLER_H
#define VALUERELATIONCONTROLLER_H

#include "qgsvectorlayer.h"

#include <QAtomicInt>
#include <QFutureWatcher>
#include <QObject>
#include <qqmlintegration.h>
#include <QVariantMap>

class QgsVectorLayerFeatureSource;
class QgsFeature;

class ValueRelationController : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    // in
    Q_PROPERTY( QVariantMap config READ config WRITE setConfig NOTIFY configChanged )
    Q_PROPERTY( bool isEditable READ isEditable WRITE setIsEditable NOTIFY isEditableChanged )

    // out
    Q_PROPERTY( bool isMultiSelection READ isMultiSelection NOTIFY isMultiSelectionChanged )
    Q_PROPERTY( QString displayText READ displayText NOTIFY displayTextChanged )

  public:

    enum LookupReason
    {
      ValueChanged = 0,
      HotReload
    };
    Q_ENUM( LookupReason );

    explicit ValueRelationController( QObject *parent = nullptr );
    ~ValueRelationController() override;

    /**
     * Parses a QGIS value-relation wire value into a list of key strings.
     *   allowMulti=true  → "{1,2,3}"  becomes ["1","2","3"]
     *   allowMulti=false → "1"        becomes ["1"]
     * An empty or null input always returns an empty list.
     */
    Q_INVOKABLE QStringList qgisFormatToArray( const QVariant &qgsValue ) const;

    /**
     * Formats a list of key strings into the QGIS wire value "{k1,k2,...}".
     *   allowMulti=true  → "["1","2","3"]"  becomes ["1","2","3"]
     *   allowMulti=true  → "["1"]"          becomes "1"
     *   allowMulti=false → "1"              becomes "1"
     * An empty list produces "".
     */
    Q_INVOKABLE QString arrayToQgisFormat( const QStringList &keys ) const;

    /**
     * Starts an async fetch to resolve display label(s).
     * Returns immediately; results are delivered via displayValuesReady().
     *
     * Emits invalidate() when fieldValue is non-null/non-empty but
     * no matching features are found AND a FilterExpression is configured.
     */
    Q_INVOKABLE void lookupDisplayTextOnValueChanged( const QString &currentValue );


    Q_INVOKABLE void lookupDisplayTextOnHotreload( const QString &currentValue, const QgsFeature &feature );

    QVariantMap config() const;
    void setConfig( const QVariantMap &newConfig );

    bool isEditable() const;
    void setIsEditable( bool newIsEditable );

    bool isMultiSelection() const;
    const QString &displayText() const;

  signals:
    void invalidateSelection(); // the value should reset as it is longer available (due to drill-down forms)
    void presentRawValue(); // the value should reset as it is longer available (due to drill-down forms)

    void configChanged();
    void isEditableChanged();

    void isMultiSelectionChanged();
    void displayTextChanged();

  private slots:
    void onLookupFinished();

  private:
    void lookupDisplayTextAsync( const QString &currentValue, bool useFilterExpression = false, const QgsFeature &feature = QgsFeature() );
    void setDisplayText( const QString &newDisplayText );

    void setup();
    void clearLayer();

    // Runs on background thread. Takes ownership of the feature source.
    static QgsFeatureList _performLookup( QgsVectorLayerFeatureSource *s, QgsFeatureRequest r, std::shared_ptr<QgsFeedback> f );

    QVariantMap mConfig;
    QgsVectorLayer *mTargetLayer = nullptr;

    QString mTargetLayerKeyField;
    int mTargetLayerKeyFieldIndex = -1;
    int mTargetLayerValueFieldIndex = -1;

    bool mIsEditable = true;

    bool mIsInitialized = false;
    bool mIsMultiSelection = false;

    QString mDisplayText;
    QString mFilterExpression;

    QFutureWatcher<QgsFeatureList> mLookupWatcher;
    std::weak_ptr<QgsFeedback> mLastLookupFeedback;

    LookupReason mLastLookupReason = LookupReason::ValueChanged;
};

#endif // VALUERELATIONCONTROLLER_H
