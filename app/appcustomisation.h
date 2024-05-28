/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef APPCUSTOMISATION_H
#define APPCUSTOMISATION_H

#include <QObject>
#include <QHash>
#include <QVariant>
#include <QString>
#include <QColor>

// Like this or pure string in code? If we use AC_* (like App Customization, ..)
// then at least we have here list of all entries that could be whitelabeled?
const QString AC_ORG_NAME = QStringLiteral("organizationName");

class AppCustomisation: public QObject
{
  public:
    explicit AppCustomisation( QObject *parent = nullptr );
    Q_INVOKABLE QString value( const QString &key, const QString &defaultValue ) {return getValue<QString>(key, defaultValue);}
    Q_INVOKABLE QColor color( const QString &key, const QColor &defaultValue ){return getValue<QColor>(key, defaultValue);}

    // maybe for some on -off things like allow switching default server (so in the QML we can read this and do not need to patch the file)
    Q_INVOKABLE bool boolean( const QString &key, bool defaultValue ){return getValue<bool>(key, defaultValue);}

  private:
    // Template, ou yeah!
    template <typename T> T getValue(const QString &key, const T &defaultValue )
    {
        const auto it = mValues.find(key);
        if (it == mValues.constEnd()) {
            return defaultValue;
        } else {
            return it->value<T>();
        }
    }

    QHash<QString, QVariant> mValues;
};

#endif // APPCUSTOMISATION_H
