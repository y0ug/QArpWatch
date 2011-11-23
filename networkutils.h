/**
 * QArpWatch is a GUI that monitor and alert if the
 * default gateway mac address change
 *
 * Copyright (C) 2011 Hugo Caron <h.caron@codsec.com>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation; either version 2.1 of
 *  the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 **/

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

protected:
#ifndef Q_WS_WIN
    static QString ethernet_mactoa(struct sockaddr *addr);
#endif
};

#endif // ROUTENET_H
