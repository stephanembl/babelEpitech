#include "include/view/login.h"
#include "ui_login.h"
#include "include/view/babibelwindow.h"
#include "include/controller/controller.h"
#include <QCryptographicHash>
#include <QByteArray>
#include <iostream>

Babibel::View::Login::Login(QWidget *parent) :
    QDialog(parent, Qt::Window),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    ui->createButton->setEnabled(false);
    ui->okButton->setEnabled(false);
    ui->nickEdit->setEnabled(false);
    QRegExpValidator *v = new QRegExpValidator;
    v->setRegExp(QRegExp("([A-Za-z0-9\\-\\_]+)"));
    ui->nickEdit->setMaxLength(15);
    ui->nickEdit->setValidator(v);
    ui->passwordEdit->setEnabled(false);
    ui->passwordEdit->setMaxLength(30);
    connect(this, SIGNAL(debugLogin(QString)), parent, SLOT(debugLogin(QString)));
    this->_shouldNotExit = false;
}

Babibel::View::Login::~Login()
{
    delete ui;
}

void Babibel::View::Login::on_okButton_clicked()
{
    this->_shouldNotExit = true;
    QString passwordHash = "";

    if (ui->passwordEdit->text().size() > 0)
        passwordHash = QString(QCryptographicHash::hash(QString(ui->passwordEdit->text()+ QString("a6xx7869B7oL5R1")).toUtf8(), QCryptographicHash::Md5).toHex());

    if (ui->nickEdit->text().size() == 0)
    {
        QMessageBox::warning(this, "Format error", "Login should not be empty");
        return;
    }
    ((Babibel::View::BabibelWindow *)this->parent())->_controller->network()->setLogin(ui->nickEdit->text().toStdString(),
                                                                                               passwordHash.toStdString());}

void Babibel::View::Login::on_createButton_clicked()
{
    this->_shouldNotExit = true;
    QString passwordHash = "";

    if (ui->passwordEdit->text().size() > 0)
        passwordHash = QString(QCryptographicHash::hash(QString(ui->passwordEdit->text()+ QString("a6xx7869B7oL5R1")).toUtf8(), QCryptographicHash::Md5).toHex());

    if (ui->nickEdit->text().size() == 0)
    {
        QMessageBox::warning(this, "Format error", "Login should not be empty");
        return;
    }
    ((Babibel::View::BabibelWindow *)this->parent())->_controller->network()->setCreateAccount(ui->nickEdit->text().toStdString(),
                                                                                               passwordHash.toStdString());
}

void Babibel::View::Login::on_cancelButton_clicked()
{
    ((Babibel::View::BabibelWindow *)this->parent())->setActive(false);
}

void Babibel::View::Login::closeEvent(QCloseEvent *)
{
    if (this->_shouldNotExit == false)
        ((Babibel::View::BabibelWindow *)this->parent())->setActive(false);
}

bool Babibel::View::Login::shouldNotExit() const
{
    return _shouldNotExit;
}

void Babibel::View::Login::setShouldNotExit(bool shouldNotExit)
{
    _shouldNotExit = shouldNotExit;
}

