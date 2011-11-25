#include "dialogabout.h"
#include "ui_dialogabout.h"

DialogAbout::DialogAbout(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAbout)
{
    ui->setupUi(this);

    ui->labelNom->setText(tr("<b>%1</b> (v%2)").arg(QApplication::applicationName(), QApplication::applicationVersion()));
}

DialogAbout::~DialogAbout()
{
    delete ui;
}
