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

#include "colorpathmodel.h"

class PhotoDrawingController: public QObject {
    Q_OBJECT

    Q_PROPERTY( QString photoSource MEMBER mPhotoSource REQUIRED );
    Q_PROPERTY( bool canUndo MEMBER mCanUndo NOTIFY canUndoChanged );
    Q_PROPERTY( QColor activeColor MEMBER mPenColor WRITE setActiveColor NOTIFY activeColorChanged );
    Q_PROPERTY( ColorPathModel *annotations READ annotations NOTIFY annotationsChanged );

    public:
        explicit PhotoDrawingController( QObject *parent = nullptr );
        ~PhotoDrawingController() override = default;

        // starts new line, to store points in
        Q_INVOKABLE void newDrawing();

        // adds new point to current line
        Q_INVOKABLE void addPoint(QPointF newPoint, double xOffset, double yOffset);

        // sets color of new line
        Q_INVOKABLE void setActiveColor( QColor newColor );

        // undoes the last line
        Q_INVOKABLE void undo();

        // resets the controller to default state ( removes all lines, sets white color... )
        Q_INVOKABLE void clear();

        // saves the drawings into the image
        Q_INVOKABLE void saveDrawings();

        // saves the ratio by which the actual image was scaled in app
        Q_INVOKABLE void setPhotoScaleRatio( double ratio );

    signals:
        void canUndoChanged();
        void activeColorChanged();
        void annotationsChanged();

    private:
        ColorPathModel *annotations() const;

        std::pair<double, double> mAnnotationOffsets = std::make_pair( 0.0, 0.0 );
        double mPhotoScaleRatio = 1.0;
        QString mPhotoSource;
        QColor mPenColor = QColor::fromString("#FFFFFF");
        bool mCanUndo = false;
        ColorPath mCurrentLine = ColorPath( mPenColor, {} );
        // it's a vector of polylines by color
        ColorPathModel *mColorPathModel;
};



#endif //PHOTODRAWINGCONTROLLER_H
