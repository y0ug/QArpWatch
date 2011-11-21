#include "arpwatcher.h"


ArpWatcher::ArpWatcher() :
    QThread(), sleepTime(5), abort(false)
{
    updateRoute();
}

ArpWatcher::~ArpWatcher()
{

}

void ArpWatcher::run()
 {
    while(!getAbort()){
        mutex.lock();
        QString mac = NetworkUtils::getArpFromHost(route.gate, route.iface);
        qDebug() << mac;
        emit newCheck(route.iface, route.gate, mac);
        mutex.unlock();
        if ( savedMac != mac){
            qDebug() << "HACK!!!!";
            emit changeDetected();
        }
        sleep(sleepTime);
    }

    exit(0);
 }

void ArpWatcher::updateRoute()
{
    mutex.lock();

    route = NetworkUtils::getDefaultRouteIp();
    savedMac = NetworkUtils::getArpFromHost(route.gate, route.iface);
    qDebug() << "updateRoute" << route.gate << savedMac;

    mutex.unlock();
}
