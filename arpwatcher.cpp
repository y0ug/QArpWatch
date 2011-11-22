#include "arpwatcher.h"


ArpWatcher::ArpWatcher() :
    QThread(),
    sleepTime(5), abort(false), detected(false)
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
        bool up = true;

        if (mac.length() == 0){
            up = false;
            emit networkChanged(up);
        }else{
            emit networkChanged(up);
            if ( savedMac != mac){
                detected = true;
            }else{
                detected = false;
            }

            emit changeDetected(detected);
            emit newCheck(route.iface, route.gate, mac);
        }


        mutex.unlock();

        sleep(sleepTime);


    }

    exit(0);
 }

void ArpWatcher::updateRoute()
{
    mutex.lock();

    route = NetworkUtils::getDefaultRouteIp();
    savedMac = NetworkUtils::getArpFromHost(route.gate, route.iface);

    mutex.unlock();

    emit routeUpdated();
}
