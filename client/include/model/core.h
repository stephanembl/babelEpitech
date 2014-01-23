#ifndef CORE_H
#define CORE_H

#include <map>
#include <list>
#include <string>
#include "include/Enum.h"
#include "include/model/contact.h"

namespace Babibel
{
namespace Controller
{
class Controller;
}
}

namespace Babibel
{
namespace Model
{
class Core
{
    ::Babibel::Controller::Controller                    *_controller;

    std::string     _login;
    std::string     _statusText;
    eStatus         _status;
    eStatusCall     _statusCall;
    std::string     _callingLogin;

    std::list<Contact *>    _contacts;

public:
    Core(::Babibel::Controller::Controller *controller);
    ~Core();

    std::string getLogin() const;
    void setLogin(const std::string &login);

    std::string getStatusText() const;
    void setStatusText(const std::string &statusText);

    eStatus getStatus() const;
    void setStatus(const eStatus &status);

    std::list<Contact *> &getContacts();
    void setContacts(const std::list<Contact *> &contacts);
    eStatusCall statusCall() const;
    void setStatusCall(const eStatusCall &statusCall);
    std::string callingLogin() const;
    void setCallingLogin(const std::string &callingLogin);
};
}
}

#endif // CORE_H
