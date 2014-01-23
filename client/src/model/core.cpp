#include "include/model/core.h"
#include "include/controller/controller.h"

Babibel::Model::Core::Core(::Babibel::Controller::Controller *controller)
    : _controller(controller)
{
    _statusCall = NONE;
    _callingLogin = "";
}

Babibel::Model::Core::~Core()
{
}

std::string Babibel::Model::Core::getLogin() const
{
    return _login;
}

void Babibel::Model::Core::setLogin(const std::string &login)
{
    _login = login;
}

std::string Babibel::Model::Core::getStatusText() const
{
    return _statusText;
}

void Babibel::Model::Core::setStatusText(const std::string &statusText)
{
    _statusText = statusText;
}

eStatus Babibel::Model::Core::getStatus() const
{
    return _status;
}

void Babibel::Model::Core::setStatus(const eStatus &status)
{
    _status = status;
}

std::list<Babibel::Model::Contact *> &Babibel::Model::Core::getContacts()
{
    return _contacts;
}

void Babibel::Model::Core::setContacts(const std::list<Contact *> &contacts)
{
    _contacts = contacts;
}

eStatusCall Babibel::Model::Core::statusCall() const
{
    return _statusCall;
}

void Babibel::Model::Core::setStatusCall(const eStatusCall &statusCall)
{
    _statusCall = statusCall;
}

std::string Babibel::Model::Core::callingLogin() const
{
    return _callingLogin;
}

void Babibel::Model::Core::setCallingLogin(const std::string &callingLogin)
{
    _callingLogin = callingLogin;
}
