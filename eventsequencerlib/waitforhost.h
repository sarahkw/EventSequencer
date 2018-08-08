#ifndef WAITFORHOST_H
#define WAITFORHOST_H

#include <waitfor.h>

#include <map>
#include <vector>

class QObject;

template <typename KeyType>
class WaitForHost {
    std::map<KeyType, std::vector<WaitFor*>> waiters_;

    Q_DISABLE_COPY(WaitForHost)

public:
    WaitForHost() {}
    ~WaitForHost()
    {
        // Tell all the WaitFors to no longer alert us when they're destroyed.
        // Because we're going to be destroyed right now.
        for (auto keysToVec : waiters_) {
            for (WaitFor* wf : keysToVec.second) {
                wf->eraseUnregisterFn();
            }
        }
    }

    void deleteAfter(KeyType afterExclusive)
    {
        for (auto iter = waiters_.upper_bound(afterExclusive);
             iter != waiters_.end();
             ++iter) {
            beforeDelete(iter->first);
        }
    }

    void rekeyAfter(KeyType afterExclusive, KeyType delta)
    {
        std::map<KeyType, QObject*> collectDataTypes;
        for (auto iter = waiters_.upper_bound(afterExclusive);
             iter != waiters_.end();
             ++iter) {
            if (!iter->second.empty()) {
                collectDataTypes[iter->first] = iter->second[0]->result();
            }
        }
        for (auto iter = waiters_.upper_bound(afterExclusive);
             iter != waiters_.end();
             ++iter) {
            beforeDelete(iter->first);
        }
        for (auto iter = collectDataTypes.begin();
             iter != collectDataTypes.end();
             ++iter) {
            KeyType newLocation = iter->first + delta;
            if (newLocation > afterExclusive) {
                afterAdd(newLocation, iter->second);
            }
        }
    }

    void afterAdd(KeyType key, QObject* val)
    {
        auto waitersIter = waiters_.find(key);
        if (waitersIter != waiters_.end()) {
            for (WaitFor* wf : waitersIter->second) {
                wf->setResult(val);
            }
        }
    }
    void beforeDelete(KeyType key)
    {
        auto waitersIter = waiters_.find(key);
        if (waitersIter != waiters_.end()) {
            for (WaitFor* wf : waitersIter->second) {
                wf->setResult(nullptr);
            }
        }
    }

    WaitFor* waitFor(KeyType key, QObject* initialValue = nullptr)
    {
        WaitFor* ret = new WaitFor([key, this](WaitFor* ths) {
            unregisterWaitFor(key, ths);
        });

        if (initialValue != nullptr) {
            ret->setResult(initialValue);
        }

        std::vector<WaitFor*>& v = waiters_[key];
        if (!v.empty()) {
            if (v[0]->result() != initialValue) {
                qWarning("WaitFor had a mismatch between new value and old values");
            }
        }
        v.push_back(ret);
        return ret;
    }

    void unregisterWaitFor(KeyType key, WaitFor* wf)
    {
        auto& vec = waiters_[key];
        auto it = std::find(vec.begin(), vec.end(), wf);
        if (it != vec.end()) {
            vec.erase(it);
        } else {
            qWarning("Failed to erase WaitFor*");
        }
    }
};

#endif // WAITFORHOST_H
