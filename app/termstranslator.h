/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TERMSTRANSLATOR_H
#define TERMSTRANSLATOR_H

#include <QPointer>
#include <QRegularExpression>
#include <QTranslator>
#include <QVariantMap>

/**
 * Replaces words of the app interface with an organisation's own terminology, e.g. "Projects"
 * with "Missions". The terms come from the "terms" section of the field profile, see FieldProfile.
 *
 * It is installed as the last translator so it is asked first: it takes the text in the app
 * language (from the base translator, or the English source text) and replaces whole words,
 * case insensitive and keeping the capital first letter. Longer terms win, so "Create project"
 * can be set apart from "project". Terms are written in the language the app is used in.
 */
class TermsTranslator : public QTranslator
{
    Q_OBJECT

  public:
    explicit TermsTranslator( QTranslator *baseTranslator, QObject *parent = nullptr );

    QString translate( const char *context, const char *sourceText, const char *disambiguation = nullptr, int n = -1 ) const override;
    bool isEmpty() const override;

    //! Sets the terms (word in the app -> replacement), an empty map restores the app wording
    void setTerms( const QVariantMap &terms );
    QVariantMap terms() const;

  private:
    QString replaceTerms( const QString &text ) const;

    QPointer<QTranslator> mBaseTranslator;
    QVariantMap mTerms;
    QHash<QString, QString> mLookup; // lower case term -> replacement
    QRegularExpression mPattern;
};

#endif // TERMSTRANSLATOR_H
