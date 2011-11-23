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

#include "arpwatcher.h"


ArpWatcher::ArpWatcher() :
    QThread(),
    sleepTime(5)
{
    abort = false;
    detected = false;

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
