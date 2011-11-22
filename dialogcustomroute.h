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
