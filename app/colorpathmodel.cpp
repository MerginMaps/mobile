/***************************************************************************
*                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "colorpathmodel.h"

#include <QPointF>

#include "coreutils.h"

ColorPathModel::ColorPathModel(QObject *parent) : QAbstractListModel(parent) {}

QVariant ColorPathModel::getPath( const int row) const
{
 if (row < 0 || row >= rowCount()) return {};
 const ColorPath colorPath = mPaths.at( row );
 QVariantMap data;
 data["color"] = colorPath.mColor;
 data["points"] = QVariant::fromValue( colorPath.mPoints );
 CoreUtils::log("ColorPathModel", "ColorPathModel: [" + colorPath.mColor.name() + ", " + QString::number(colorPath.mPoints.size()) + "]");
 return data;
}

void ColorPathModel::addPath(const ColorPath &path) {
 beginInsertRows(QModelIndex(), mPaths.size(), mPaths.size());
 mPaths.append(path);
 endInsertRows();
}

void ColorPathModel::updatePath(const int row, const ColorPath &path)
{
 mPaths[row] = path;
 emit pathUpdated( row );
}

void ColorPathModel::removeLastPath()
{
 if (mPaths.isEmpty() || mPaths.size() == 1 )
  return;

 // we remove the last finished path instead of last path as that is the current active path
 beginRemoveRows(QModelIndex(), mPaths.size() - 2, mPaths.size() - 2 );
 mPaths.remove( mPaths.size() - 2 );
 endRemoveRows();
}

void ColorPathModel::clear()
{
 beginResetModel();
 mPaths.clear();
 endResetModel();
}

bool ColorPathModel::isEmpty() const
{
 return mPaths.isEmpty();
}

int ColorPathModel::rowCount(const QModelIndex &parent) const {
 Q_UNUSED(parent);
 return mPaths.size();
}

QVariant ColorPathModel::data(const QModelIndex &index, const int role) const {
 if (!index.isValid() || index.row() >= mPaths.size())
  return QVariant();

 const ColorPath &path = mPaths.at(index.row());

 switch (role) {
  case ColorRole:
   return path.mColor;
  case PointsRole: {
   QVariantList pointList;
   for (const QPointF &pt : path.mPoints)
    pointList.append(QVariant::fromValue(pt));
   return pointList;
  }
  default:
   return QVariant();
 }
}

QHash<int, QByteArray> ColorPathModel::roleNames() const {
 return {
              { ColorRole, "color" },
              { PointsRole, "points" }
 };
}