#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSystemTrayIcon>
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

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    ArpWatcher arpWatcher;

    bool lastChanged;
    bool lastNetworkChanged;
};

#endif // MAINWINDOW_H
