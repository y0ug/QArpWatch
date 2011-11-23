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

#include "dialogcustomroute.h"
#include "ui_dialogcustomroute.h"

#include "networkutils.h"

DialogCustomRoute::DialogCustomRoute(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogCustomRoute)
{
    ui->setupUi(this);
}

DialogCustomRoute::~DialogCustomRoute()
{
    delete ui;
}

NetRoute DialogCustomRoute::getRoute()
{
    NetRoute r;
    r.gate = ui->ipEdit->text();
    r.iface = ui->ifaceEdit->text();
    r.dest = "0.0.0.0";
    return r;
}

void DialogCustomRoute::setRoute(NetRoute r)
{
     ui->ipEdit->setText(r.gate);
     ui->ifaceEdit->setText(r.iface);
}

QString DialogCustomRoute::getMac()
{
    return ui->macEdit->text();
}

void DialogCustomRoute::setMac(QString mac)
{
    ui->macEdit->setText(mac);
}
