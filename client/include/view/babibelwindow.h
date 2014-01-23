#ifndef BABIBELWINDOW_H
#define BABIBELWINDOW_H

#include <QMainWindow>
#include <map>
#include <QStandardItemModel>
#include <QString>
#include <QMessageBox>
#include <QInputDialog>
#include "IMainWindow.h"
#include "include/Enum.h"
#include "include/view/conversation.h"
#include "include/view/login.h"

#define DEFAULT_IP_FIELD "10.20.86.82:11235"

namespace Ui {
class BabibelWindow;
}

namespace Babibel
{
namespace Controller
{
class Controller;
}
namespace View
{
class ContactListItemDelegate;
}
}

namespace Babibel
{
namespace View
{
class BabibelWindow : public QMainWindow, public Babibel::View::IMainWindow
{
    Q_OBJECT
public:
    ::Babibel::Controller::Controller                    *_controller;
private:
    QStandardItemModel      _listModel;
    ::Babibel::View::ContactListItemDelegate *_listItemDelegate;

    Login                    *_loginWindow;

    std::map<std::string, Conversation *>   _conversations;
    bool                    _isActive;


public:
    explicit BabibelWindow(::Babibel::Controller::Controller *controller, QWidget *parent = 0);
    ~BabibelWindow();

    virtual void        updateList(const std::map<std::string, std::pair<std::string, eStatus> > &);

    virtual bool        isActive() const;
    virtual bool        cannotConnect(const std::string &);
    virtual bool        connected();
    virtual bool        disconnected();
    virtual bool        serverError(const std::string &);
    virtual bool        serverLoginError(const std::string &);
    virtual bool        loggedIn();
    virtual bool        updateUserInfo();
    virtual bool        updateList();
    virtual bool        updateConversation(const std::string &);
    virtual bool        call(const std::string &);


    void                setActive(bool);

public slots:
    int                 openConversation(const QString &login);
    int                 openConversationCall(const QString &login);
    int                 closeConversation(const QString &login);
    void                debugLogin(const QString &login);
private slots:
    void on_statusEdit_returnPressed();
    void on_statusComboBox_activated(int index);

    void on_addContactButton_clicked();

    void on_removeContactButton_clicked();

    void on_blockContactButton_clicked();

    void on_contactList_doubleClicked(const QModelIndex &index);

private:
    void closeEvent(QCloseEvent *);
private:
    Ui::BabibelWindow *ui;
};
}
}


#endif // BABIBELWINDOW_H
