/***************************************************************************
*                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef COLORPATHMODEL_H
#define COLORPATHMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QColor>

/**
 * Utility class, which is used in place of std::pair, because of QML limitations.
 */
class ColorPath
{
 Q_GADGET
 Q_PROPERTY( QColor color MEMBER mColor )
 Q_PROPERTY( QVector<QPointF> points MEMBER mPoints )
 public:
 ColorPath( const QColor color, const QVector<QPointF> &list): mColor(color), mPoints(list) {};

 QColor mColor;
 QVector<QPointF> mPoints;
};

/**
 * Wrapper class for QVector<ColorPath> for QML to be able to iterate over it. We use this when the user draws
 * annotations over images and we need QML to draw paths on top.
 */
class ColorPathModel : public QAbstractListModel {
 Q_OBJECT

public:
 enum ColorPathRoles {
  ColorRole = Qt::UserRole + 1,
  PointsRole
};

 explicit ColorPathModel(QObject *parent = nullptr) : QAbstractListModel(parent) {}

 /**
  * Get the qml representation of ColorPath.
  */
 Q_INVOKABLE QVariant getPath( int row ) const;

 /**
  * Adds a new path to the model in the end of list.
  * \param path the finished ColorPath to add
  */
 void addPath(const ColorPath &path);

 /**
  * Removes the last added path from the model.
  */
 void removeLastPath();

 /**
  * Removes all paths from the model.
  */
 void clear();

 /**
  * Returns whether there are any paths stored in the model.
  */
 bool isEmpty() const;

 int rowCount(const QModelIndex &parent = QModelIndex()) const override;

 QVariant data(const QModelIndex &index, int role) const override;

 QHash<int, QByteArray> roleNames() const override;

private:
 QVector<ColorPath> mPaths;
};



#endif //COLORPATHMODEL_H
