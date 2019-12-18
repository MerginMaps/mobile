#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QObject>
#include <QHash>

class AppSettings: public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString defaultProject READ defaultProject WRITE setDefaultProject NOTIFY defaultProjectChanged )
    Q_PROPERTY( QString activeProject READ activeProject WRITE setActiveProject NOTIFY activeProjectChanged )
    Q_PROPERTY( QString defaultProjectName READ defaultProjectName NOTIFY defaultProjectChanged )
    Q_PROPERTY( QString defaultLayer READ defaultLayer WRITE setDefaultLayer NOTIFY defaultLayerChanged )
    Q_PROPERTY( QString defaultMapTheme READ defaultMapTheme WRITE setDefaultMapTheme NOTIFY defaultMapThemeChanged )
    Q_PROPERTY( bool autoCenterMapChecked READ autoCenterMapChecked WRITE setAutoCenterMapChecked NOTIFY autoCenterMapCheckedChanged )
    Q_PROPERTY( int lineRecordingInterval READ lineRecordingInterval WRITE setLineRecordingInterval NOTIFY lineRecordingIntervalChanged )
    Q_PROPERTY( int gpsAccuracyTolerance READ gpsAccuracyTolerance WRITE setGpsAccuracyTolerance NOTIFY gpsAccuracyToleranceChanged )

  public:
    explicit AppSettings( QObject *parent = nullptr );

    QString defaultProject() const;
    void setDefaultProject( const QString &value );

    QString activeProject() const;
    void setActiveProject( const QString &value );

    QString defaultLayer() const;

    bool autoCenterMapChecked();
    void setAutoCenterMapChecked( const bool value );

    QString defaultProjectName() const;

    int gpsAccuracyTolerance() const;
    void setGpsAccuracyTolerance( int gpsAccuracyTolerance );

    int lineRecordingInterval() const;
    void setLineRecordingInterval( int lineRecordingInterval );

    QString defaultMapTheme() const;


  signals:
    void defaultProjectChanged();
    void activeProjectChanged();
    void defaultLayerChanged();
    void defaultMapThemeChanged();
    void autoCenterMapCheckedChanged();
    void gpsAccuracyToleranceChanged();
    void lineRecordingIntervalChanged();
    void reloadDefaultLayerSignal( const QString &name );
    void reloadDefaultMapThemeSignal( const QString &name );
  public slots:
    void setDefaultMapTheme( const QString &value );
    void setDefaultLayer( const QString &value );
    void reloadDefaultLayer();
    void reloadDefaultMapTheme();

  private:
    // Projects path
    QString mDefaultProject;
    // Path to active project
    QString mActiveProject;
    // flag for following GPS position
    bool mAutoCenterMapChecked = false;
    // used in GPS signal indicator
    int mGpsAccuracyTolerance = -1;
    // Digitizing period in seconds
    int mLineRecordingInterval = 3;

    // Projects path -> defaultLayer name
    QHash<QString, QString> mDefaultLayers;

    // Projects path -> defaultMapTheme name
    QHash<QString, QString> mDefaultMapTheme;

    const QString mGroupName = QString( "inputApp" );

    void reloadDefaultLayers();
};

#endif // APPSETTINGS_H
