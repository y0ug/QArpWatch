#include "networkutils.h"

#include <QtCore>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>

#include <sys/ioctl.h>
#include <net/if_arp.h>

#include <ctype.h>
#include <errno.h>
#include <netdb.h>
#include <resolv.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

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
#define RTF_NOTCACHED   0x0400          /* this route isn't cached        */

static char *ethernet_mactoa(struct sockaddr *addr)
{
    static char buff[256];
    unsigned char *ptr = (unsigned char *) addr->sa_data;

    sprintf(buff, "%02X:%02X:%02X:%02X:%02X:%02X",
            (ptr[0] & 0377), (ptr[1] & 0377), (ptr[2] & 0377),
            (ptr[3] & 0377), (ptr[4] & 0377), (ptr[5] & 0377));

    return (buff);
}

NetworkUtils::NetworkUtils()
{

}

NetRoute NetworkUtils::getDefaultRouteIp()
{
    char buf[BUFF_SIZE];
    //char ipgate[IP_SIZE], ipmask[IP_SIZE];

    char iface[INTERFACE_NAME_SIZE], dest[9], gate[9], mask[9];
    int flags, refcnt, use, metric, mtu, window, rtt;

    FILE *fp = fopen(_PATH_PROCNET_ROUTE, "r");

    if (!fp) {
        //perror(_PATH_PROCNET_ROUTE);
        qDebug() << "INET (IPv4) not configured in this system.\n";
        return NetRoute();
    }


    while (fgets(buf, sizeof(buf), fp) != NULL) {
        int n;
        //struct prefix_ipv4 p;
        struct in_addr tmpmask;
        struct in_addr gateway;
        struct in_addr destination;

        n = sscanf (buf, "%s %s %s %x %d %d %d %s %d %d %d",
                    iface, dest, gate, &flags, &refcnt, &use, &metric,
                    mask, &mtu, &window, &rtt);

        if (n != 11)
        {
            //qDebug() << "can't read all of routing information" << n;
            continue;
        }

        if (! (flags & RTF_UP))
          continue;
        if (! (flags & RTF_GATEWAY))
          continue;


        sscanf (dest, "%lX", (unsigned long *)&destination);
        sscanf (mask, "%lX", (unsigned long *)&tmpmask);
        sscanf (gate, "%lX", (unsigned long *)&gateway);

        /*
        printf("%s :\t", iface);
        printf("%s/", inet_ntoa(gateway));
        printf("%s\n", inet_ntoa(tmpmask));
        */
        NetRoute route;
        route.iface = QString(iface);
        route.dest = QString(inet_ntoa(destination));
        route.mask = QString(inet_ntoa(tmpmask));
        route.gate = QString(inet_ntoa(gateway));

        (void) fclose(fp);
        return route;
    }

    (void) fclose(fp);

    return NetRoute();
}

QString NetworkUtils::getArpFromHost(QString ip, QString iface)
{
    int                 s;

    struct arpreq       areq;
    struct sockaddr_in *sin;
    struct in_addr      ipaddr;

    /* Get an internet domain socket. */
    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
            perror("socket");
            exit(1);
    }

    /* Make the ARP request. */
    memset(&areq, 0, sizeof(areq));
    sin = (struct sockaddr_in *) &areq.arp_pa;
    sin->sin_family = AF_INET;

    if (inet_aton(ip.toStdString().c_str(), &ipaddr) == 0) {
        qDebug() << QObject::tr("invalid numbers-and-dots IP address %s").arg(ip);
        return QString();
    }

    sin->sin_addr = ipaddr;
    sin = (struct sockaddr_in *) &areq.arp_ha;
    sin->sin_family = ARPHRD_ETHER;

    strncpy(areq.arp_dev, iface.toStdString().c_str(), 15);

    if (ioctl(s, SIOCGARP, (caddr_t) &areq) == -1) {
        qDebug() << "unable to make ARP request, error";
        return QString();
    }

    /*printf("%s  -> %s\n", ip.toStdString().c_str(),
                    inet_ntoa(&((struct sockaddr_in *) &areq.arp_pa)->sin_addr),
                    ethernet_mactoa(&areq.arp_ha));*/

    return QString(ethernet_mactoa(&areq.arp_ha));
}
