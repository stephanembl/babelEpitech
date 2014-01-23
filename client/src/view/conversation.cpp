#include "include/view/conversation.h"
#include "include/model/contact.h"
#include "include/view/babibelwindow.h"
#include "include/controller/controller.h"

#include "ui_conversation.h"
#include <QCloseEvent>
#include <QScrollBar>
#include <QDate>


Babibel::View::Conversation::Conversation(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui(new Ui::Conversation)
{
    ui->setupUi(this);
    connect(this, SIGNAL(closeConversation(QString)), parent, SLOT(closeConversation(QString)));
}

Babibel::View::Conversation::~Conversation()
{
    delete ui;
}

void    Babibel::View::Conversation::setLogin(const QString &login)
{
    this->_login = login;
    this->setWindowTitle("Babibel - " + login);
    this->refresh();
}

void    Babibel::View::Conversation::refresh()
{
    std::list<Babibel::Model::Contact *> & contacts =  ((Babibel::View::BabibelWindow *)this->parent())->_controller->model()->getContacts();
    std::list<Babibel::Model::Contact *>::iterator it = std::find_if(contacts.begin(), contacts.end(), Babibel::Model::Contact::contact_has_login(this->_login.toStdString()));
    if (it != contacts.end())
    {
        Babibel::Model::Contact *c = (*it);
        ui->login->setText(c->getLogin().c_str());
        ui->textStatus->setText(c->getStatusText().c_str());
        std::list<std::pair<eIdentity, std::pair<time_t, std::string> > > & list = c->getMessages();
        ui->textBrowser->clear();
        for (std::list<std::pair<eIdentity, std::pair<time_t, std::string> > >::iterator it2 = list.begin(); it2 != list.end(); ++it2)
        {
            if ((*it2).first == ME)
                ui->textBrowser->append(QDateTime::fromTime_t((*it2).second.first).toString("HH:mm:ss") + QString(" - Me : ") + QString((*it2).second.second.c_str()));
            else
                ui->textBrowser->append(QDateTime::fromTime_t((*it2).second.first).toString("HH:mm:ss") + QString(" - ") + this->_login + QString(": ") + QString((*it2).second.second.c_str()));
        }

        QScrollBar *sb = ui->textBrowser->verticalScrollBar();
        sb->setValue(sb->maximum());
    }
}

void    Babibel::View::Conversation::call()
{



    std::list<Babibel::Model::Contact *> & contacts =  ((Babibel::View::BabibelWindow *)this->parent())->_controller->model()->getContacts();
    std::list<Babibel::Model::Contact *>::iterator it = std::find_if(contacts.begin(), contacts.end(), Babibel::Model::Contact::contact_has_login(this->_login.toStdString()));

    if (it != contacts.end())
    {
        Babibel::Model::Contact *c = (*it);
        switch (c->getStatusCall())
        {
        case NONE:
            if (((Babibel::View::BabibelWindow *)this->parent())->_controller->model()->callingLogin() == "" && ((Babibel::View::BabibelWindow *)this->parent())->_controller->model()->statusCall() == NONE)
                ui->callButton->setEnabled(true);
            if (((Babibel::View::BabibelWindow *)this->parent())->_controller->model()->callingLogin() != "" && ((Babibel::View::BabibelWindow *)this->parent())->_controller->model()->statusCall() != NONE)
                ui->callButton->setEnabled(false);
            ui->callButton->setText("~ CALL ~");
            break;
        case ISWAITING:
        {
            QMessageBox *msgBox = new QMessageBox(this);
            msgBox->setWindowTitle("Incoming Call");
            msgBox->setText("Would you like to accept it ?");
            msgBox->setStandardButtons(QMessageBox::Yes|QMessageBox::No);
            msgBox->setDefaultButton(QMessageBox::No);
            msgBox->setModal(false);
            msgBox->show();
            connect(msgBox, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(acceptorreject(QAbstractButton*)));


//            QMessageBox::StandardButton error;
//            error = QMessageBox::question(this, "Incoming Call", "Would you like to accept it ?",
//                                          QMessageBox::Yes|QMessageBox::No);
//            if (error == QMessageBox::Yes)
//            {
//                ((Babibel::View::BabibelWindow *)this->parent())->_controller->network()->setAcceptCall(this->_login.toStdString());
//            }
//            else
//            {
//                ((Babibel::View::BabibelWindow *)this->parent())->_controller->network()->setRejectCall(this->_login.toStdString());
//            }
        }
            break;
        case ISCALLING:
            ui->callButton->setText("~ HANGUP ~");
            break;
        case ISRINGING:
            ui->callButton->setText("~ CALLING ~");
            break;
        }
    }
}

void                Babibel::View::Conversation::acceptorreject(QAbstractButton *btn)
{
    if (btn->text() == "&Yes")
    {
        if (((Babibel::View::BabibelWindow *)this->parent())->_controller->model()->callingLogin() != "" && ((Babibel::View::BabibelWindow *)this->parent())->_controller->model()->statusCall() != NONE)
         ((Babibel::View::BabibelWindow *)this->parent())->_controller->network()->setAcceptCall(this->_login.toStdString());
    }
    else if (btn->text() == "&No")
    {

        if (((Babibel::View::BabibelWindow *)this->parent())->_controller->model()->callingLogin() != "" && ((Babibel::View::BabibelWindow *)this->parent())->_controller->model()->statusCall() != NONE)
         ((Babibel::View::BabibelWindow *)this->parent())->_controller->network()->setRejectCall(this->_login.toStdString());
    }
}


void                Babibel::View::Conversation::closeEvent(QCloseEvent *event)
{
    emit closeConversation(this->_login);
    event->accept();
}

void Babibel::View::Conversation::on_sendButton_clicked()
{
    if (ui->lineEdit->text().size())
    {
        ((Babibel::View::BabibelWindow *)this->parent())->_controller->network()->setChat(this->_login.toStdString(), ui->lineEdit->text().toStdString());
        ui->lineEdit->setText("");
    }
}

void Babibel::View::Conversation::on_lineEdit_returnPressed()
{
    if (ui->lineEdit->text().size())
    {
        ((Babibel::View::BabibelWindow *)this->parent())->_controller->network()->setChat(this->_login.toStdString(), ui->lineEdit->text().toStdString());
        ui->lineEdit->setText("");
    }
}

void Babibel::View::Conversation::on_callButton_clicked()
{
    if (ui->callButton->text() == "~ CALL ~")
    {
        if (((Babibel::View::BabibelWindow *)this->parent())->_controller->model()->callingLogin() == "" && ((Babibel::View::BabibelWindow *)this->parent())->_controller->model()->statusCall() == NONE)
            ((Babibel::View::BabibelWindow *)this->parent())->_controller->network()->setCall(this->_login.toStdString());
    }
    else if (ui->callButton->text() == "~ HANGUP ~")
    {
        if (((Babibel::View::BabibelWindow *)this->parent())->_controller->model()->callingLogin() != "" && ((Babibel::View::BabibelWindow *)this->parent())->_controller->model()->statusCall() != NONE)
                ((Babibel::View::BabibelWindow *)this->parent())->_controller->network()->setHangup(this->_login.toStdString());
    }
}
