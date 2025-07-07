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
    ColorPath( const QColor color, const QVector<QPointF> &list ): mColor( color ), mPoints( list ) {};
    ColorPath();

    bool operator==( const ColorPath &other ) const;

    QColor mColor;
    QVector<QPointF> mPoints;
};

class PhotoSketchingController: public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY( QString photoSource MEMBER mPhotoSource REQUIRED );
    Q_PROPERTY( bool canUndo MEMBER mCanUndo NOTIFY canUndoChanged );
    Q_PROPERTY( QColor activeColor MEMBER mPenColor WRITE setActiveColor NOTIFY activeColorChanged );
    Q_PROPERTY( double photoScaleRatio MEMBER mPhotoScaleRatio WRITE setPhotoScaleRatio NOTIFY photoScaleRatioChanged );

  public:
    explicit PhotoSketchingController( QObject *parent = nullptr ): QObject( parent ) {};
    ~PhotoSketchingController() override = default;

    // starts new line, to store points in
    Q_INVOKABLE void newSketch();

    // adds new point to current line
    Q_INVOKABLE void addPoint( QPointF newPoint );

    // sets color of new line
    Q_INVOKABLE void setActiveColor( QColor newColor );

    // undoes the last line
    Q_INVOKABLE void undo();

    // resets the controller to default state ( removes all lines, sets white color... )
    Q_INVOKABLE void clear();

    // saves the drawings into the image
    Q_INVOKABLE void saveDrawings() const;

    // redraws all paths in mPaths (used after user comes back to drawing screen)
    Q_INVOKABLE void redrawPaths();

    // sets the annotations offset, which is equivalent to padding around photo
    Q_INVOKABLE void setAnnotationsOffset( double newXOffset, double newYOffset );

    // sets the photo scale ratio ( original size / printed size )
    Q_INVOKABLE void setPhotoScaleRatio( double newRatio );

    // Get the qml representation of ColorPath.
    Q_INVOKABLE ColorPath getPath( int row ) const;

  signals:
    void canUndoChanged();
    void activeColorChanged();
    void annotationsChanged();
    void photoScaleRatioChanged();

    void newPathAdded( int pathIndex );
    void pathUpdated( QVector<int> pathIndexes );
    void lastPathRemoved();
    void pathsReset();

  private:
    std::pair<double, double> mAnnotationOffsets = std::make_pair( 0.0, 0.0 );
    double mPhotoScaleRatio = 1.0;
    QString mPhotoSource;
    QColor mPenColor = QColor::fromString( "#FFFFFF" );
    bool mCanUndo = false;
    ColorPath mCurrentLine = ColorPath( mPenColor, {} );
    // it's a vector of polylines by color
    QVector<ColorPath> mPaths;
};



#endif //PHOTODRAWINGCONTROLLER_H
