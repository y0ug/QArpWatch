#ifndef ROUTENET_H
#define ROUTENET_H

#include <QtCore>

class NetRoute : public QObject
{
public:
    QString iface;
    QString dest;
    QString gate;
    QString mask;

    NetRoute() : QObject(){
        this->iface = "";
        this->dest = "";
        this->gate = "";
        this->mask = "";
    }

    NetRoute(const NetRoute& r) : QObject() {
        this->iface = r.iface;
        this->dest = r.dest;
        this->gate = r.gate;
        this->mask = r.mask;
    }
    NetRoute& operator=(const NetRoute& r){
        this->iface = r.iface;
        this->dest = r.dest;
        this->gate = r.gate;
        this->mask = r.mask;
        return *this;
    }
};

class NetworkUtils
{
public:
    NetworkUtils();
    static NetRoute getDefaultRouteIp();
    static QString getArpFromHost(QString ip, QString iface);
};

#endif // ROUTENET_H
