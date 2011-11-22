#ifndef ARPWATCHER_H
#define ARPWATCHER_H

#include <QtCore>

#include "networkutils.h"


class ArpWatcher : public QThread
{
    Q_OBJECT

public:
    ArpWatcher();
    virtual ~ArpWatcher();
    void run();

    NetRoute getRoute(){
        return route;
    }

    void setRoute(NetRoute route){
        mutex.lock();
        this->route = route;
        mutex.unlock();
        emit routeUpdated();
    }

    QString getSavedMac(){
        return savedMac;
    }

    void setSavedMac(QString mac){
        mutex.lock();
        savedMac = mac;
        mutex.unlock();
    }

    bool getAbort(){
        mutex.lock();
        bool b = abort;
        mutex.unlock();
        return b;
    }



signals:
    void routeUpdated();
    void changeDetected(bool detected);
    void newCheck(QString ifnet, QString ip, QString mac);
    void networkChanged(bool changed);

public slots:
    void setAbort(bool b){
        mutex.lock();
        abort = b;
        mutex.unlock();
    }

    void updateRoute();

protected:
    NetRoute route;
    QString savedMac;
    int sleepTime;
    bool detected;

    QMutex mutex;
    bool abort;
};

#endif // ARPWATCHER_H
