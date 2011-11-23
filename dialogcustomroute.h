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

#ifndef DIALOGCUSTOMROUTE_H
#define DIALOGCUSTOMROUTE_H

#include <QDialog>

namespace Ui {
    class DialogCustomRoute;
}

class NetRoute;

class DialogCustomRoute : public QDialog
{
    Q_OBJECT

public:
    explicit DialogCustomRoute(QWidget *parent = 0);
    ~DialogCustomRoute();

    NetRoute getRoute();
    void setRoute(NetRoute route);

    QString getMac();
    void setMac(QString mac);

private:
    Ui::DialogCustomRoute *ui;
};

#endif // DIALOGCUSTOMROUTE_H
