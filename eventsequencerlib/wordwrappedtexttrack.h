#ifndef WORDWRAPPEDTEXTTRACK_H
#define WORDWRAPPEDTEXTTRACK_H

#include <QObject>
#include <QFont>
#include <QAbstractListModel>
#include <vector>
#include <QPoint>

class WordWrappedTextTrack : public QAbstractListModel
{
    Q_OBJECT

    enum CustomRoles {
        ModelDataRole = Qt::UserRole + 1,
        TextOffsetRole,
        HasNextLineRole
    };

    int width_ = 1;
    QString text_;
    QFont font_;

    Q_PROPERTY(int width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)

    struct RowData {
        QString text;
        int offset;
    };
    std::vector<RowData> rows_;

    void relayout();

    int cursorFrame_ = 0;
    QPoint cursorPosition_;
    Q_PROPERTY(int cursorFrame READ cursorFrame WRITE setCursorFrame NOTIFY cursorFrameChanged)
    Q_PROPERTY(QPoint cursorPosition READ cursorPosition NOTIFY cursorPositionChanged)

    void reposition();

    std::pair<bool, int> upDownDesiredColumn_;

public:
    explicit WordWrappedTextTrack(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int,QByteArray> roleNames() const override;

    int width() const;
    void setWidth(int width);

    QString text() const;
    void setText(const QString &text);

    QFont font() const;
    void setFont(const QFont &font);

    int cursorFrame() const;
    void setCursorFrame(int cursorFrame);

    QPoint cursorPosition() const;

    Q_INVOKABLE int calculateCursorPositionClosestTo(QPoint target);
    Q_INVOKABLE int calculateMaxCursorPosition();

signals:

    void widthChanged();
    void textChanged();
    void fontChanged();

    void cursorFrameChanged();
    void cursorPositionChanged();

public slots:
};

#endif // WORDWRAPPEDTEXTTRACK_H
