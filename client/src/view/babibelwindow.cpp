#include "include/view/babibelwindow.h"
#include "ui_babibelwindow.h"
#include "ui_conversation.h"
#include "ui_login.h"
#include "include/view/conversation.h"
#include "include/controller/controller.h"
#include "../headers-shared/shared/Exception.h"
#include "include/view/ConcactListItem.h"
#include "include/Enum.h"

Babibel::View::BabibelWindow::BabibelWindow(::Babibel::Controller::Controller *controller, QWidget *parent) :
    QMainWindow(parent),
    _controller(controller),
    ui(new Ui::BabibelWindow)
{
    ui->setupUi(this);

    this->_listItemDelegate = new ContactListItemDelegate(this);
    ui->contactList->setItemDelegate(this->_listItemDelegate);

    ui->contactList->setModel(&this->_listModel);

    bool ok;
    QString serverinfo = QInputDialog::getText(this, "Enter Server Info","Enter \"IP:PORT\" :", QLineEdit::Normal,DEFAULT_IP_FIELD, &ok);


    if (ok)
    {
        QStringList info = serverinfo.split(":");
        if (info.size() == 2)
        {
            this->_isActive = true;

            this->_loginWindow = new Login(this);
            this->_loginWindow->show();

            this->_controller->network()->connectTCP(info[0].toStdString(), info[1].toInt());

            if (this->_controller->network()->initTCP() == false)
            {
                this->cannotConnect("Server Connection Error");
            }
            else
            {
                this->connected();
            }
        }
        else
        {
            this->_isActive = false;
            QMessageBox::critical(this, "Error in format", "We apologize for this problem, closing the cheese right now !");
        }
    }
    else
    {
        this->_isActive = false;
    }
}

Babibel::View::BabibelWindow::~BabibelWindow()
{
    delete ui;
    delete this->_listItemDelegate;
}

void        Babibel::View::BabibelWindow::updateList(const std::map<std::string, std::pair<std::string, eStatus> > &contacts)
{
    int     i = 0;
    int     onlineCount = 0;

    this->_listModel.setRowCount(contacts.size());
    for (std::map<std::string, std::pair<std::string, eStatus> >::const_iterator it = contacts.begin(); it != contacts.end(); ++it)
    {
        QStandardItem *item = new QStandardItem();
        item->setData((*it).first.c_str(), ContactListItemDelegate::NICKNAME);
        item->setData((*it).second.first.c_str(), ContactListItemDelegate::TEXTSTATUS);
        item->setData(QVariant(qRegisterMetaType<eStatus>("ContactListItemDelegate::eStatus"), &(*it).second.second), ContactListItemDelegate::STATUS);

        if ((*it).second.second != DISCONNECTED)
            ++onlineCount;
        this->_listModel.setItem(i++, 0, item);
    }
    ui->contactList->setModel(&this->_listModel);
    ui->onlinePeople->setText(QString::number(onlineCount) + " People Online");
    this->update();
}

int                 Babibel::View::BabibelWindow::openConversation(const QString &login)
{
    std::map<std::string, Conversation *>::iterator it = this->_conversations.find(login.toStdString());

    if (it != this->_conversations.end())
    {
        (*it).second->show();
        (*it).second->raise();
        (*it).second->call();
    }
    else
    {
        Conversation *conv = new Conversation(this);
        this->_conversations[login.toStdString()] = conv;
        conv->setLogin(login);
        conv->refresh();
        conv->show();
        conv->raise();
        conv->call();
    }
    ui->contactList->viewport()->repaint();
    ui->contactList->viewport()->update();
    QApplication::processEvents();
    return (0);
}

int                 Babibel::View::BabibelWindow::openConversationCall(const QString &login)
{
    std::map<std::string, Conversation *>::iterator it = this->_conversations.find(login.toStdString());

    if (it != this->_conversations.end())
    {
        (*it).second->refresh();
        (*it).second->show();
        (*it).second->raise();
        (*it).second->call();
        (*it).second->ui->callButton->click();

    }
    else
    {
        Conversation *conv = new Conversation(this);
        this->_conversations[login.toStdString()] = conv;
        conv->setLogin(login);
        conv->show();
        conv->raise();
        conv->call();
        conv->ui->callButton->click();

    }
    ui->contactList->viewport()->repaint();
    ui->contactList->viewport()->update();
    QApplication::processEvents();
    return (0);
}

int                 Babibel::View::BabibelWindow::closeConversation(const QString &login)
{
    std::map<std::string, Conversation *>::iterator it = this->_conversations.find(login.toStdString());

    if (it != this->_conversations.end())
    {
        delete (*it).second;
        this->_conversations.erase(it);
    }
    return (0);
}

bool        Babibel::View::BabibelWindow::isActive() const
{
    return (this->_isActive);
}

void        Babibel::View::BabibelWindow::setActive(bool val)
{
    this->_isActive = val;
}

void Babibel::View::BabibelWindow::closeEvent(QCloseEvent *)
{
    this->_isActive = false;
}

bool        Babibel::View::BabibelWindow::cannotConnect(const std::string &str)
{
    QMessageBox::StandardButton error;
    error = QMessageBox::question(this, str.c_str(), "Check your configuration file ! Retry ?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (error == QMessageBox::Yes)
    {
        bool ok;
        QString serverinfo = QInputDialog::getText(this, "Enter Server Info","Enter \"IP:PORT\" :", QLineEdit::Normal,DEFAULT_IP_FIELD, &ok);

        if (ok)
        {
            QStringList info = serverinfo.split(":");
            if (info.size() == 2)
            {
                this->_controller->network()->connectTCP(info[0].toStdString(), info[1].toInt());

                if (this->_controller->network()->initTCP() == false)
                {
                    this->cannotConnect("Server Connection Error");
                }
                else
                {
                    this->connected();
                }
            }
            else
            {
                this->_isActive = false;
                QMessageBox::critical(this, "Error in format", "We apologize for this problem, closing the cheese right now !");
            }
        }
        else
            this->_isActive =false;
    }
    else
    {
        this->_isActive =false;
    }
    return (0);
}

bool        Babibel::View::BabibelWindow::connected()
{
    this->_loginWindow->ui->createButton->setEnabled(true);
    this->_loginWindow->ui->okButton->setEnabled(true);
    this->_loginWindow->ui->nickEdit->setEnabled(true);
    this->_loginWindow->ui->passwordEdit->setEnabled(true);
    return (true);
}

bool        Babibel::View::BabibelWindow::disconnected()
{
    QMessageBox::critical(this, "Critical Server Disconnect", "We apologize for this problem, closing the cheese right now !");
    this->_isActive = false;
    return (true);
}

bool        Babibel::View::BabibelWindow::serverError(const std::string &msg)
{
    QMessageBox* msgBox = new QMessageBox( this );
    msgBox->setAttribute( Qt::WA_DeleteOnClose );
    msgBox->setStandardButtons( QMessageBox::Ok );
    msgBox->setWindowTitle( tr("Server Error") );
    msgBox->setText( tr(msg.c_str()) );
    msgBox->setModal( false );
    msgBox->show();
    return (true);
}

bool        Babibel::View::BabibelWindow::serverLoginError(const std::string &msg)
{
    QMessageBox* msgBox = new QMessageBox( this->_loginWindow );
    msgBox->setAttribute( Qt::WA_DeleteOnClose );
    msgBox->setStandardButtons( QMessageBox::Ok );
    msgBox->setWindowTitle( tr("Server Error") );
    msgBox->setText( tr(msg.c_str()) );
    msgBox->setModal( false );
    msgBox->show();
    return (true);
}

bool        Babibel::View::BabibelWindow::loggedIn()
{
    this->_loginWindow->close();
    this->show();
    return (true);
}

bool        Babibel::View::BabibelWindow::updateUserInfo()
{
    ui->login->setText(this->_controller->model()->getLogin().c_str());
    ui->statusEdit->setText(this->_controller->model()->getStatusText().c_str());
    switch (this->_controller->model()->getStatus())
    {
    case AVAILABLE :
        ui->statusComboBox->setCurrentIndex(0);
        break;
    case BUSY :
        ui->statusComboBox->setCurrentIndex(1);
        break;
    case AWAY :
        ui->statusComboBox->setCurrentIndex(2);
        break;
    case DISCONNECTED :
        ui->statusComboBox->setCurrentIndex(3);
        break;
    default:
        ui->statusComboBox->setCurrentIndex(3);
        break;
    }
    this->update();
    return (true);
}

bool        Babibel::View::BabibelWindow::updateList()
{
    std::list<Babibel::Model::Contact *> & contacts =  this->_controller->model()->getContacts();
    int     i = 0;
    int     onlineCount = 0;
    int count = 0;

    for (std::list<Babibel::Model::Contact *>::const_iterator it = contacts.begin(); it != contacts.end(); ++it)
        if ((*it)->getContact())
            ++count;

    this->_listModel.setRowCount(contacts.size());
    for (std::list<Babibel::Model::Contact *>::const_iterator it = contacts.begin(); it != contacts.end(); ++it)
    {
        if ((*it)->getContact())
        {
            std::map<std::string, Conversation *>::iterator it2 = this->_conversations.find((*it)->getLogin());
            if (it2 != this->_conversations.end())
            {
                (*it2).second->refresh();
            }

            QStandardItem *item = new QStandardItem();
            item->setData((*it)->getLogin().c_str(), ContactListItemDelegate::NICKNAME);
            item->setData((*it)->getStatusText().c_str(), ContactListItemDelegate::TEXTSTATUS);
            eStatus status = (*it)->getStatus();
            item->setData(QVariant(qRegisterMetaType<eStatus>("ContactListItemDelegate::eStatus"), &status), ContactListItemDelegate::STATUS);
            if ((*it)->getStatus() != DISCONNECTED)
                ++onlineCount;
            this->_listModel.setItem(i++, 0, item);
        }
    }
    ui->contactList->setModel(&this->_listModel);
    ui->onlinePeople->setText(QString::number(onlineCount) + " People Online");
    this->update();
    return (true);
}

bool        Babibel::View::BabibelWindow::updateConversation(const std::string &login)
{
    std::map<std::string, Conversation *>::iterator it = this->_conversations.find(login);

    if (it != this->_conversations.end())
    {
        (*it).second->refresh();
        (*it).second->show();
        (*it).second->raise();
        (*it).second->call();
    }
    else
    {
        Conversation *conv = new Conversation(this);
        this->_conversations[login] = conv;
        conv->setLogin(login.c_str());
        conv->show();
        conv->raise();
        conv->call();
    }
    ui->contactList->viewport()->repaint();
    ui->contactList->viewport()->update();
    QApplication::processEvents();
    return (true);
}

bool        Babibel::View::BabibelWindow::call(const std::string &login)
{
    std::map<std::string, Conversation *>::iterator it = this->_conversations.find(login);

    if (it != this->_conversations.end())
    {
        (*it).second->refresh();
        (*it).second->show();
        (*it).second->raise();
        (*it).second->call();
    }
    else
    {
        Conversation *conv = new Conversation(this);
        this->_conversations[login] = conv;
        conv->setLogin(login.c_str());
        conv->show();
        conv->raise();
        conv->call();
    }
    ui->contactList->viewport()->repaint();
    ui->contactList->viewport()->update();
    QApplication::processEvents();
    return (true);
}

//DEBUG
void                Babibel::View::BabibelWindow::debugLogin(const QString &)
{
    std::map<std::string, std::pair<std::string, eStatus> > map;
    map["iMart1n"] = std::pair<std::string, eStatus>("Occupé", BUSY);
    map["MOMBUL"] = std::pair<std::string, eStatus>("Babibel", AVAILABLE);
    map["David"] = std::pair<std::string, eStatus>("RCT", AVAILABLE);
    map["Miramo"] = std::pair<std::string, eStatus>("Ecalyptus", AWAY);
    map["Matt"] = std::pair<std::string, eStatus>("Pigliss'", DISCONNECTED);

    this->updateList(map);
}

void        Babibel::View::BabibelWindow::on_statusEdit_returnPressed()
{
    this->_controller->network()->setStatusText(ui->statusEdit->text().toStdString());
}

void Babibel::View::BabibelWindow::on_statusComboBox_activated(int index)
{
    this->_controller->network()->setStatus((eStatus)index);
}

void Babibel::View::BabibelWindow::on_addContactButton_clicked()
{
    bool ok;

    QString login = QInputDialog::getText(this, "Add Contact","Login :", QLineEdit::Normal,"", &ok);
    if (ok)
        this->_controller->network()->setAddContact(login.toStdString());
}

void Babibel::View::BabibelWindow::on_removeContactButton_clicked()
{
    QString login;

    if (ui->contactList->currentIndex().data(ContactListItemDelegate::NICKNAME).canConvert<QString>())
        login = ui->contactList->currentIndex().data(ContactListItemDelegate::NICKNAME).value<QString>();
    if (login.size())
        this->_controller->network()->setRemoveContact(login.toStdString());
}

void Babibel::View::BabibelWindow::on_blockContactButton_clicked()
{
    QString login;

    if (ui->contactList->currentIndex().data(ContactListItemDelegate::NICKNAME).canConvert<QString>())
        login = ui->contactList->currentIndex().data(ContactListItemDelegate::NICKNAME).value<QString>();
    if (login.size())
        this->_controller->network()->setBlockContact(login.toStdString());
}

void Babibel::View::BabibelWindow::on_contactList_doubleClicked(const QModelIndex &index)
{
    this->openConversation(index.data(ContactListItemDelegate::NICKNAME).value<QString>());
}
