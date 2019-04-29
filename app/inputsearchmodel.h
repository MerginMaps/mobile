#ifndef INPUTSEARCHMODEL_H
#define INPUTSEARCHMODEL_H

#include <QObject>
#include <QAbstractListModel>

class InputSearchModel: public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY( QString searchExpression READ searchExpression WRITE setSearchExpression )

public:
    explicit InputSearchModel( QObject *parent = nullptr );
    QString searchExpression() const;
    void setSearchExpression(const QString &searchExpression);

protected:
    QString mSearchExpression;
};

#endif // INPUTSEARCHMODEL_H
