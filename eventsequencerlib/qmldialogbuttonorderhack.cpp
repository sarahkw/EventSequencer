#include "qmldialogbuttonorderhack.h"

int QmlDialogButtonOrderHack::buttonCount() const
{
    return buttonCount_;
}

void QmlDialogButtonOrderHack::setButtonCount(int buttonCount)
{
    if (buttonCount_ != buttonCount) {

        if (!complete_ && orderedButtons_.size() > 0 &&
                buttonCount == orderedButtons_.size()) {
            complete_ = true;
            finishInit();
        }

        buttonCount_ = buttonCount;
        emit buttonCountChanged();
    }
}

void QmlDialogButtonOrderHack::registerButtonClick(QObject *button)
{
    auto iter = orderedButtons_.find(button);
    if (iter == orderedButtons_.end()) {
        qWarning("Unknown button!");
        return;
    }
    auto idx = std::distance(orderedButtons_.begin(), iter);
    emit buttonClicked(idx);
}

void QmlDialogButtonOrderHack::finishInit()
{
    QVariantList buttonList;
    for (QObject* b : orderedButtons_) {
        buttonList.push_back(QVariant::fromValue(b));
    }
    emit buttonInit(buttonList);
}

QmlDialogButtonOrderHack::QmlDialogButtonOrderHack(QObject *parent) : QObject(parent)
{

}

bool QmlDialogButtonOrderHack::registerButton(QObject *button)
{
    if (!complete_) {
        orderedButtons_.insert(button);

        if (orderedButtons_.size() > 0 &&
                buttonCount_ == orderedButtons_.size()) {
            complete_ = true;

            finishInit();
        }
    }
    return false;
}
