#ifndef BADJSSTRIPEXT_H
#define BADJSSTRIPEXT_H

#include <QObject>

namespace pb {
class Strip_BadJs;
}

namespace stripext {

class BadJsStripExt : public QObject
{
    Q_OBJECT

    QString script_;

    Q_PROPERTY(QString script READ script WRITE setScript NOTIFY scriptChanged)

public:
    explicit BadJsStripExt(QObject *parent = nullptr);

    void toPb(pb::Strip_BadJs& pb) const;
    void fromPb(const pb::Strip_BadJs& pb);

    QString script() const;
    void setScript(const QString &script);

signals:

    void scriptChanged();

public slots:
};

} // namespace stripext

#endif // BADJSSTRIPEXT_H
