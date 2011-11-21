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
    void changeDetected();
    void newCheck(QString ifnet, QString ip, QString mac);
    void quit();
    //void messageClicked();

private:
    void createIconGroupBox();
    void createActions();
    void createTrayIcon();

    Ui::MainWindow *ui;

    QAction *minimizeAction;
    QAction *restoreAction;
    QAction *quitAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    ArpWatcher arpWatcher;
};

#endif // MAINWINDOW_H
