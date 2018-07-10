#ifndef TEXTSTRIPEXT_H
#define TEXTSTRIPEXT_H

#include <QObject>

namespace pb {
class Strip_Text;
}

namespace stripext {

class TextStripExt : public QObject
{
    Q_OBJECT

    QString content_;

    Q_PROPERTY(QString content READ content WRITE setContent NOTIFY contentChanged)

public:
    explicit TextStripExt(QObject *parent = nullptr);

    void toPb(pb::Strip_Text& pb) const;
    void fromPb(const pb::Strip_Text& pb);

    QString content() const;
    void setContent(const QString &content);

signals:

    void contentChanged();

public slots:
};

} // namespace stripext

#endif // TEXTSTRIPEXT_H
