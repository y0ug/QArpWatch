#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "networkutils.h"

#include "arpwatcher.h"

#include <QtGui>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    createActions();
    createTrayIcon();

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));


    connect(&arpWatcher, SIGNAL(changeDetected()), this,
            SLOT(changeDetected()));
    connect(&arpWatcher, SIGNAL(newCheck(QString,QString,QString)),
            this, SLOT(newCheck(QString,QString,QString)));

    setIcon("trash", QApplication::applicationName());

    trayIcon->show();

    arpWatcher.start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeDetected()
{
    showMessageSystray(QApplication::applicationName(),
                       "Arp poisoning in progress ...", 3);

    setIcon("bad", QApplication::applicationName());
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
    QIcon icon = QIcon(":/images/"+name+".svg");
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
    minimizeAction = new QAction(tr("Mi&nimize"), this);
    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));


    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), this, SLOT(quit()));
}

void MainWindow::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}

void MainWindow::quit()
{
    if(!arpWatcher.getAbort()){
        arpWatcher.setAbort(true);
        arpWatcher.wait();
    }

    qApp->quit();
}