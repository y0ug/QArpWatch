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

#include "networkutils.h"

#include <QtCore>

NetworkUtils::NetworkUtils()
{

}

#ifdef Q_WS_WIN

    QString NetworkUtils::getArpFromHost(QString ip, QString iface)
    {
        Q_UNUSED(iface);

        QString mac = "";

        /* Bad way, tired don't won't to look the Win32 API */
        QString program = "C:\\WINDOWS\\system32\\arp";
        QStringList arguments;
        arguments << "-a";

        QProcess *myProcess = new QProcess();
        myProcess->start(program, arguments);
        myProcess->waitForReadyRead();

        while(myProcess->canReadLine()){
            QString line = myProcess->readLine();
            line = line.trimmed();
            //qDebug() << line;
            if(line.startsWith(ip)){
                QStringList list = line.split(" ", QString::SkipEmptyParts);
                if ( list.count() != 3 )
                    continue;

                mac = list[1].trimmed();
                //qDebug() << mac;
                break;
            }
        }
        /*if (myProcess->error()){
            qDebug() << myProcess->errorString();
        }*/
        myProcess->close();

        return mac;
    }

    NetRoute NetworkUtils::getDefaultRouteIp()
    {
        NetRoute route;

        route.gate = "0.0.0.0";
        route.dest = "0.0.0.0";
        route.iface = "unavailable";

        /* Bad way, tired don't won't to look the Win32 API */
        QString program = "C:\\WINDOWS\\system32\\route";
        QStringList arguments;
        arguments << "PRINT";

        QProcess *myProcess = new QProcess();
        myProcess->start(program, arguments);
        myProcess->waitForReadyRead();
        //if (myProcess->error()){
            //qDebug() << myProcess->errorString();
        //}
        while(myProcess->canReadLine()){
            QString line = myProcess->readLine();
            line = line.trimmed();
            if(line.startsWith("0.0.0.0")){
                QStringList list = line.split(" ", QString::SkipEmptyParts);
                if ( list.count() != 5)
                    continue;

                route.gate = list[2].trimmed();
                //qDebug() << route.gate;
                route.dest = "0.0.0.0";
                route.iface = list[3].trimmed();

                break;
            }
        }

        myProcess->close();

        return route;
    }

#else

    #include <arpa/inet.h> /* inet_ntoa, inet_naton */
    #include <sys/ioctl.h> /* ioctl */
    #include <net/if_arp.h> /* ARPHRD_ETHER */


    #define BUFF_SIZE 1024
    #define INTERFACE_NAME_SIZE 8
    #define IP_SIZE 32

    #define _PATH_PROCNET_ROUTE		"/proc/net/route"

    /* Keep this in sync with /usr/src/linux/include/linux/ipv6_route.h */
    #ifndef RTF_DEFAULT
    #define RTF_DEFAULT     0x00010000      /* default - learned via ND     */
    #endif
    #define RTF_ALLONLINK   0x00020000      /* fallback, no routers on link */
    #ifndef RTF_ADDRCONF
    #define RTF_ADDRCONF    0x00040000      /* addrconf route - RA          */
    #endif
    #define RTF_NONEXTHOP   0x00200000      /* route with no nexthop        */
    #define RTF_EXPIRES     0x00400000
    #define RTF_CACHE       0x01000000      /* cache entry                  */
    #define RTF_FLOW        0x02000000      /* flow significant route       */
    #define RTF_POLICY      0x04000000      /* policy route                 */
    #define RTF_LOCAL       0x80000000

    /* Keep this in sync with /usr/src/linux/include/linux/route.h */
    #define RTF_UP          0x0001          /* route usable                 */
    #define RTF_GATEWAY     0x0002          /* destination is a gateway     */
    #define RTF_HOST        0x0004          /* host entry (net otherwise)   */
    #define RTF_REINSTATE   0x0008          /* reinstate route after tmout  */
    #define RTF_DYNAMIC     0x0010          /* created dyn. (by redirect)   */
    #define RTF_MODIFIED    0x0020          /* modified dyn. (by redirect)  */
    #define RTF_MTU         0x0040          /* specific MTU for this route  */
    #ifndef RTF_MSS
    #define RTF_MSS         RTF_MTU         /* Compatibility :-(            */
    #endif
    #define RTF_WINDOW      0x0080          /* per route window clamping    */
    #define RTF_IRTT        0x0100          /* Initial round trip time      */
    #define RTF_REJECT      0x0200          /* Reject route                 */

    /* this is a 2.0.36 flag from /usr/src/linux/include/linux/route.h */
    #define RTF_NOTCACHED   0x0400          /* this route isn't cached       */

    QString NetworkUtils::ethernet_mactoa(struct sockaddr *addr)
    {
        char buff[256];
        unsigned char *ptr = (unsigned char *) addr->sa_data;

        sprintf(buff, "%02X:%02X:%02X:%02X:%02X:%02X",
                (ptr[0] & 0377), (ptr[1] & 0377), (ptr[2] & 0377),
                (ptr[3] & 0377), (ptr[4] & 0377), (ptr[5] & 0377));

        return QString(buff);
    }



    NetRoute NetworkUtils::getDefaultRouteIp()
    {
        //char buf[BUFF_SIZE];
        //char ipgate[IP_SIZE], ipmask[IP_SIZE];

        NetRoute route;

        QFile f(_PATH_PROCNET_ROUTE);
        if (! f.open(QIODevice::ReadOnly) ){
            qDebug() << "can't open " << _PATH_PROCNET_ROUTE;
            return route;
        }

        QTextStream str(&f);
        do{
            struct in_addr tmpmask;
            struct in_addr gateway;
            struct in_addr destination;

            //QString iface, dest, gate, mask;
            char iface[INTERFACE_NAME_SIZE], dest[9], gate[9], mask[9];
            int flags, refcnt, use, metric, mtu, window, rtt;
            int n;

            QString line = str.readLine();

            /* Need to find a alternative to this sscanf */
            n = sscanf (line.toLocal8Bit().data(), "%s %s %s %x %d %d %d %s %d %d %d",
                            iface, dest, gate, &flags, &refcnt, &use, &metric,
                            mask, &mtu, &window, &rtt);
            /*line >> iface >> dest >> gate >> flags >> refcnt >> use
                 >> metric >> mask >> mtu >> window >> rtt;*/

            if (n != 11) {
                //qDebug() << "can't read all of routing information" << n;
                continue;
            }

            /* We need a route up and a gateway */
            if (! (flags & RTF_UP))
              continue;
            if (! (flags & RTF_GATEWAY))
              continue;

            /* Need to find a alternative to this sscanf */
            sscanf (dest, "%lX", (unsigned long *)&destination);
            sscanf (mask, "%lX", (unsigned long *)&tmpmask);
            sscanf (gate, "%lX", (unsigned long *)&gateway);


            route.iface = QString(iface);
            route.dest = QString(inet_ntoa(destination));
            route.mask = QString(inet_ntoa(tmpmask));
            route.gate = QString(inet_ntoa(gateway));

            break;
        }while(!str.atEnd());

        f.close();

        return route;
    }


    QString NetworkUtils::getArpFromHost(QString ip, QString iface)
    {
        int                 s;

        struct arpreq       areq;
        struct sockaddr_in *sin;
        struct in_addr      ipaddr;

        const char          *pip = ip.toLocal8Bit().data();
        const char          *piface = iface.toLocal8Bit().data();

        /* Get an internet domain socket. */
        if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
                perror("socket");
                exit(1);
        }

        /* Make the ARP request. */
        memset(&areq, 0, sizeof(areq));
        sin = (struct sockaddr_in *) &areq.arp_pa;
        sin->sin_family = AF_INET;

        if (inet_aton(pip, &ipaddr) == 0) {
            qDebug() << "invalid numbers-and-dots IP address " << ip;
            return QString();
        }

        sin->sin_addr = ipaddr;
        sin = (struct sockaddr_in *) &areq.arp_ha;
        sin->sin_family = ARPHRD_ETHER;

        strncpy(areq.arp_dev, piface, 15);

        if (ioctl(s, SIOCGARP, (caddr_t) &areq) == -1) {
            qDebug() << "unable to make ARP request, error";
            return QString();
        }

        return ethernet_mactoa(&areq.arp_ha);
    }
#endif
