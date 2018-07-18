#ifndef SHOWMULTIPLELINESONONELINE_H
#define SHOWMULTIPLELINESONONELINE_H

#include <QObject>
#include <QVariant>

class ShowMultipleLinesOnOneLine : public QObject
{
    Q_OBJECT

    QString input_;

    QString output_;
    QVariantList outputNewLinePositions_;

    Q_PROPERTY(QString input READ input WRITE setInput NOTIFY inputChanged)
    Q_PROPERTY(QString output READ output NOTIFY outputChanged)
    Q_PROPERTY(QVariantList outputNewLinePositions READ outputNewLinePositions NOTIFY outputNewLinePositionsChanged)

public:
    explicit ShowMultipleLinesOnOneLine(QObject *parent = nullptr);

    QString input() const;
    void setInput(const QString &input);

    QString output() const;

    QVariantList outputNewLinePositions() const;

signals:

    void inputChanged();
    void outputChanged();
    void outputNewLinePositionsChanged();

public slots:
};

#endif // SHOWMULTIPLELINESONONELINE_H
