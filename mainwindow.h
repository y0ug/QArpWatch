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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSystemTrayIcon>
#include <QLabel>
#include <QMainWindow>

#include "arpwatcher.h"

namespace Ui {
    class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setVisible(bool visible);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void setIcon(QString name, QString tooltip);
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void showMessageSystray(QString title, QString msg, int duration);
    void changeDetected(bool detected);
    void networkChanged(bool changed);
    void routeUpdated();
    void newCheck(QString ifnet, QString ip, QString mac);
    void quit();
    //void messageClicked();

    void on_action_customRoute_triggered();

private:
    void createIconGroupBox();
    void createActions();
    void createTrayIcon();

    Ui::MainWindow *ui;

    QAction *minimizeAction;
    QAction *restoreAction;
    QAction *quitAction;
    QAction *refreshRouteAction;

    QLabel *statusLabel;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    ArpWatcher arpWatcher;

    bool lastChanged;
    bool lastNetworkChanged;
};

#endif // MAINWINDOW_H
