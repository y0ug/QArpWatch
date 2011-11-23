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
