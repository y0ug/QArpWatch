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
