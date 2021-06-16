#ifndef INPUTNUMBERVALIDATOR_H
#define INPUTNUMBERVALIDATOR_H

#include <QObject>
#include <QDoubleValidator>

// Custom double validator https://stackoverflow.com/a/35223994/7875594
class InputNumberValidator : public QDoubleValidator
{
    Q_OBJECT

  public:

    explicit InputNumberValidator( QObject *parent = 0 );

    InputNumberValidator( double bottom, double top, int decimals, QObject *parent = nullptr );

    ~InputNumberValidator() override;

    QValidator::State validate( QString &s, int &pos ) const override;
};

#endif // INPUTNUMBERVALIDATOR_H
