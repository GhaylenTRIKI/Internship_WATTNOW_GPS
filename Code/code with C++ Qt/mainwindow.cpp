#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include "sunrise.h"
#include <iostream>
#include <QPixmap>
#include <sstream>
#include <string>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QPixmap pix("D:/ACADEMIQUE/stage/stage WATTNOW/code/sunrise&sunset calculator.png");
    ui->label_Pic->setPixmap(pix.scaled(311,181,Qt::KeepAspectRatio));
    ui->dateEdit->setDate(QDate(2021,8,1));
    ui->lineEdit_longitude->setText("9.5735");
    ui->lineEdit_latitude->setText("33.8869");
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    QDate d;
     using namespace std;
     d = ui->dateEdit->date();

     cout << d.toString().toStdString() << endl;

     Sunrise sunrise(ui->lineEdit_latitude->text().toDouble(), ui->lineEdit_longitude->text().toDouble(), ui->lineEdit_elevation->text().toDouble());

     double longit = ui->lineEdit_longitude->text().toDouble();
     cout << "longit= "<<longit<<endl;

    ostringstream  ss1;
     double timez= sunrise.timezone(longit)+1;
     cout << "timezone= "<<timez<<endl;

     ss1<<timez;
     string d_str1 = ss1.str();

    //ui->label_sunrise->setText("SUNRISE: "+sunrise.sunrise(d).toString()+ " UTC "+QString::fromUtf8(d_str1.c_str()));
    ui->label_sunrise->setText("<b> <font color='#550000'>SUNRISE: </font></b>"+sunrise.sunrise(d).toString());
    ui->label_noon->setText("<b> <font color='#550000'>NOON:    </font></b>"+sunrise.noon(d).toString());
    ui->label_sunset->setText("<b> <font color='#550000'>SUNSET:   </font></b>"+sunrise.sunset(d).toString());

    ui->label_DayLength->setText("<b> <font color='#f23aff'>Day Length: </font></b>"+sunrise.DayLength(d).toString()+ " ");

    //civil twilight: elevation angle of the sun= -6°
    Sunrise CivilTwilight(ui->lineEdit_latitude->text().toDouble(), ui->lineEdit_longitude->text().toDouble(), -6);

    ui->label_CTS->setText(" "+CivilTwilight.sunrise(d).toString());
    ui->label_CTE->setText(" "+CivilTwilight.sunset(d).toString());

    //Nautical twilight: elevation angle of the sun= -12°
    Sunrise NauticalTwilight(ui->lineEdit_latitude->text().toDouble(), ui->lineEdit_longitude->text().toDouble(), -12);

    ui->label_NTS->setText(" "+NauticalTwilight.sunrise(d).toString());
    ui->label_NTE->setText(" "+NauticalTwilight.sunset(d).toString());

    //Astronomical twilight: elevation angle of the sun= -18°
    Sunrise AstronomicalTwilight(ui->lineEdit_latitude->text().toDouble(), ui->lineEdit_longitude->text().toDouble(), -18);

    ui->label_ATS->setText(" "+AstronomicalTwilight.sunrise(d).toString());
    ui->label_ATE->setText(" "+AstronomicalTwilight.sunset(d).toString());

    if(timez>0)
    ui->label_timezone->setText("<b> <font color='#73C6B6'>Time Zone: </font></b>   GMT+"+QString::fromUtf8(d_str1.c_str()));

    else ui->label_timezone->setText("<b> <font color='#73C6B6'>Time Zone:  </font></b>  GMT"+QString::fromUtf8(d_str1.c_str()));


}

