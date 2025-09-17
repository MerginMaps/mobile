/***************************************************************************
*                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PHOTODRAWINGCONTROLLER_H
#define PHOTODRAWINGCONTROLLER_H

#include <QPointF>
#include <QObject>
#include <QColor>
#include <QtQml/qqmlregistration.h>


/**
 * Utility class, which is used in place of std::pair, because of QML limitations.
 */
class ColorPath
{
    Q_GADGET
    Q_PROPERTY( QColor color MEMBER mColor )
    Q_PROPERTY( QVector<QPointF> points MEMBER mPoints )
  public:
    ColorPath( const QColor &color, const QVector<QPointF> &list ): mColor( color ), mPoints( list ) {};
    ColorPath();

    bool operator==( const ColorPath &other ) const;

    QColor mColor;
    QVector<QPointF> mPoints;
};

static constexpr int SKETCH_WIDTH = 4;

class PhotoSketchingController: public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY( QString photoSource MEMBER mPhotoSource REQUIRED );
    Q_PROPERTY( QString projectName MEMBER mProjectName REQUIRED );
    Q_PROPERTY( bool canUndo MEMBER mCanUndo NOTIFY canUndoChanged );
    Q_PROPERTY( QColor activeColor MEMBER mPenColor WRITE setActiveColor NOTIFY activeColorChanged );
    Q_PROPERTY( double photoScale MEMBER mPhotoScale WRITE setPhotoScale NOTIFY photoScaleChanged );
    Q_PROPERTY( int sketchWidth READ sketchWidth NOTIFY sketchWidthChanged );

  public:
    explicit PhotoSketchingController( QObject *parent = nullptr ): QObject( parent ) {};
    ~PhotoSketchingController() override = default;

    // starts new line, to store points in
    Q_INVOKABLE void newSketch();

    // adds new point to current line
    Q_INVOKABLE void addPoint( const QPointF &newPoint );

    // sets color of new line
    Q_INVOKABLE void setActiveColor( const QColor &newColor );

    // undoes the last line
    Q_INVOKABLE void undo();

    // resets the controller to default state ( removes all lines, sets white color... )
    Q_INVOKABLE void clear();

    // saves the drawings into temporary image
    Q_INVOKABLE void backupSketches();

    // removes sketched file from temporary folder
    Q_INVOKABLE void removeBackupSketches();

    // redraws all paths in mPaths (used after user comes back to drawing screen)
    Q_INVOKABLE void redrawPaths();

    // sets the photo scale ratio ( original size / printed size )
    Q_INVOKABLE void setPhotoScale( double newRatio );

    // Get the qml representation of ColorPath.
    Q_INVOKABLE ColorPath getPath( int row ) const;

    // returns the temporary photo path if it exists and if not original path
    Q_INVOKABLE QUrl getCurrentPhotoPath() const;

    /**
     * gets triggered on creation and checks if file with same name as mPhotoSource exists in temp folder, if yes set
     * it as current photoSource. Also saves the original mPhotoSource to mOriginalPhotoSource
     */
    Q_INVOKABLE void prepareController();

    static int sketchWidth();

  signals:
    void canUndoChanged();
    void activeColorChanged();
    void photoScaleChanged();
    void sketchWidthChanged();
    void tempPhotoSourceChanged( const QString &newPath );

    void newPathAdded( int pathIndex );
    void pathUpdated( QVector<int> pathIndexes );
    void lastPathRemoved();
    void pathsReset();

  private:
    double mPhotoScale = 1.0;
    QString mPhotoSource;
    QString mOriginalPhotoSource;
    QString mProjectName;
    QColor mPenColor = QColor( Qt::white );
    bool mCanUndo = false;
    ColorPath mCurrentLine = ColorPath( mPenColor, {} );
    // it's a vector of polylines by color
    QVector<ColorPath> mPaths;
    // the difference is that active paths are those that can be undone in current session
    QVector<ColorPath> mActivePaths;

    friend class TestSketching;
};



#endif //PHOTODRAWINGCONTROLLER_H
