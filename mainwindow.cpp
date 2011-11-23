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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "networkutils.h"

#include "dialogcustomroute.h"
#include "arpwatcher.h"

#include <QtGui>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    lastChanged(false), lastNetworkChanged(true)
{
    ui->setupUi(this);

    createActions();
    createTrayIcon();

    statusLabel = new QLabel(this);
    ui->statusBar->addWidget(statusLabel, 1);

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    connect(&arpWatcher, SIGNAL(routeUpdated()), this,
            SLOT(routeUpdated()));
    connect(&arpWatcher, SIGNAL(networkChanged(bool)), this,
            SLOT(networkChanged(bool)));
    connect(&arpWatcher, SIGNAL(changeDetected(bool)), this,
            SLOT(changeDetected(bool)));
    connect(&arpWatcher, SIGNAL(newCheck(QString,QString,QString)),
            this, SLOT(newCheck(QString,QString,QString)));

    setIcon("trash", QApplication::applicationName());

    trayIcon->show();

    /* To show the set-up on start */
    routeUpdated();

    arpWatcher.start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::networkChanged(bool changed)
{
    if(!changed && lastNetworkChanged){
        QString msg = "Network down...";

        showMessageSystray(QApplication::applicationName(),
                           msg, 10);

        ui->logText->append(tr("<em>%1</em> %2").
                            arg(QTime::currentTime().toString(), msg));

        setIcon("heart", QApplication::applicationName());

        statusLabel->setText(msg);
    }else if(changed && !lastNetworkChanged){
        QString msg = "Network up...";

        showMessageSystray(QApplication::applicationName(),
                           msg, 10);

        ui->logText->append(tr("<em>%1</em> %2").
                            arg(QTime::currentTime().toString(), msg));

        setIcon("trash", QApplication::applicationName());

        NetRoute route = arpWatcher.getRoute();
        QString mac = arpWatcher.getSavedMac();

        msg = tr("watch gateway %1 with %2 on %3").
            arg(route.gate, mac, route.iface);

        statusLabel->setText(msg);

        //arpWatcher.updateRoute();
    }

    lastNetworkChanged = changed;
}

void MainWindow::changeDetected(bool detected)
{
    if ( detected && !lastChanged){
        QString msg = "Arp poisoning in progress ...";
        showMessageSystray(QApplication::applicationName(),
                           msg, 30);

        ui->logText->append(tr("<em>%1</em> %2").
                            arg(QTime::currentTime().toString(), msg));

        setIcon("bad", msg);

    }else if (!detected && lastChanged){
        QString msg = "Arp poisoning stop ...";

        showMessageSystray(QApplication::applicationName(),
                           msg, 10);

        ui->logText->append(tr("<em>%1</em> %2").
                            arg(QTime::currentTime().toString(), msg));

        setIcon("trash", QApplication::applicationName());
    }


    lastChanged = detected;
}

void MainWindow::routeUpdated()
{
    NetRoute route = arpWatcher.getRoute();
    QString mac = arpWatcher.getSavedMac();

    QString msg = tr("watch gateway %1 with %2 on %3").
        arg(route.gate, mac, route.iface);

    ui->logText->append(tr("<em>%1</em> %2").
                        arg(QTime::currentTime().toString(), msg));

    showMessageSystray(QApplication::applicationName(),
                       msg, 10);

    statusLabel->setText(msg);
}

void MainWindow::newCheck(QString ifnet, QString ip, QString mac)
{
    ui->logText->append(tr("<em>%1</em> %2 &lt;-&gt; %3 with %4").
                        arg(QTime::currentTime().toString(), ip, mac, ifnet));
}

void MainWindow::setVisible(bool visible)
{
    minimizeAction->setEnabled(visible);
    restoreAction->setEnabled(isMaximized() || !visible);
    QMainWindow::setVisible(visible);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (trayIcon->isVisible()) {
        /*QMessageBox::information(this, tr("Systray"),
                                 tr("The program will keep running in the "
                                    "system tray. To terminate the program, "
                                    "choose <b>Quit</b> in the context menu "
                                    "of the system tray entry."));*/
        hide();
        event->ignore();
    }
}

void MainWindow::setIcon(QString name, QString tooltip)
{
#ifdef Q_WS_X11
    QIcon icon = QIcon(":/images/"+name+".svg");
#else
    QIcon icon = QIcon(":/images/"+name+".png");
#endif
    trayIcon->setIcon(icon);
    setWindowIcon(icon);

    trayIcon->setToolTip(tooltip);
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        if (isHidden() ){
            showNormal();
        }else{
            hide();
        }
        /*iconComboBox->setCurrentIndex((iconComboBox->currentIndex() + 1)
                                      % iconComboBox->count());*/
        break;
    case QSystemTrayIcon::MiddleClick:
        /*this->showMessageSystray("Boom", "WTF! man WARN you are getting hacked!", 5);*/
        break;
    default:
        ;
    }
}

void MainWindow::showMessageSystray(QString title, QString msg, int duration)
{
    trayIcon->showMessage(title, msg,  QSystemTrayIcon::Information,
                          duration * 1000);
}

void MainWindow::createActions()
{
    connect(ui->action_refreshRoute, SIGNAL(triggered()), &arpWatcher, SLOT(updateRoute()));

    minimizeAction = new QAction(tr("Mi&nimize"), this);
    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    connect(ui->action_Quitter, SIGNAL(triggered()), this, SLOT(quit()));
}

void MainWindow::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(ui->action_refreshRoute);
    trayIconMenu->addAction(ui->action_customRoute);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(ui->action_Quitter);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}

void MainWindow::quit()
{
    /*ui->logText->append(tr("<em>%1</em> Please wait exit in progress...").
                        arg(QTime::currentTime().toString()));*/


    if(!arpWatcher.getAbort()){
        arpWatcher.setAbort(true);
        arpWatcher.wait();
    }

    qApp->quit();
}

void MainWindow::on_action_customRoute_triggered()
{
    DialogCustomRoute diag(this);
    diag.setRoute(arpWatcher.getRoute());
    diag.setMac(arpWatcher.getSavedMac());
    if (diag.exec() != 1 ){
        return;
    }

    arpWatcher.setSavedMac(diag.getMac());
    arpWatcher.setRoute(diag.getRoute());
}
