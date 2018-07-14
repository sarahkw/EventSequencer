#ifndef LABELSTRIPEXT_H
#define LABELSTRIPEXT_H

#include <QObject>

namespace pb {
class Strip_Label;
}

namespace stripext {

class LabelStripExt : public QObject
{
    Q_OBJECT

    QString content_;

    Q_PROPERTY(QString content READ content WRITE setContent NOTIFY contentChanged)

public:
    explicit LabelStripExt(QObject *parent = nullptr);

    void toPb(pb::Strip_Label& pb) const;
    void fromPb(const pb::Strip_Label& pb);

    QString content() const;
    void setContent(const QString &content);

signals:

    void contentChanged();

public slots:
};

} // namespace stripext

#endif // LABELSTRIPEXT_H
