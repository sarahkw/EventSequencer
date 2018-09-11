#ifndef QMLDIALOGBUTTONORDERHACK_H
#define QMLDIALOGBUTTONORDERHACK_H

#include <QObject>
#include <QVariantList>

#include <set>

// As of Qt 5.11, DialogButtonBox orders buttons by their memory address if you
// don't give them buttonRoles. This class sorts buttons by memory address so you
// can have your buttons in the right order.
/*
    Dialog {
        width: parent.width * .9

        T.QmlDialogButtonOrderHack {
            id: qdboh
            buttonCount: 3
            onButtonInit: {
                buttonList[0].text = "A"
                buttonList[1].text = "B"
                buttonList[2].text = "C"
            }
            onButtonClicked: {
                switch (index) {
                case 0: print("A"); break
                case 1: print("B"); break
                case 2: print("C"); break
                }
            }
        }

        footer: DialogButtonBox {
            Repeater {
                model: qdboh.buttonCount
                Button {
                    property bool reg: qdboh.registerButton(this);
                    onClicked: qdboh.registerButtonClick(this)
                }
            }
        }
    }
*/
class QmlDialogButtonOrderHack : public QObject
{
    Q_OBJECT

    std::set<QObject*> orderedButtons_;
    bool complete_ = false;

    int buttonCount_ = -1;
    Q_PROPERTY(int buttonCount READ buttonCount WRITE setButtonCount NOTIFY buttonCountChanged)

    void finishInit();

public:
    explicit QmlDialogButtonOrderHack(QObject *parent = nullptr);


    Q_INVOKABLE bool registerButton(QObject* button);

    int buttonCount() const;
    void setButtonCount(int buttonCount);

    Q_INVOKABLE void registerButtonClick(QObject* button);

signals:

    void buttonInit(QVariantList buttonList);
    void buttonClicked(int index);

    void buttonCountChanged();

public slots:
};

#endif // QMLDIALOGBUTTONORDERHACK_H
