#include "include/model/contact.h"

Babibel::Model::Contact::Contact()
{
    this->_contact = false;
    this->_statusCall = NONE;
}

Babibel::Model::Contact::~Contact()
{
}

eStatus Babibel::Model::Contact::getStatus() const
{
    return _status;
}

void Babibel::Model::Contact::setStatus(const eStatus &status)
{
    _status = status;
}

std::string Babibel::Model::Contact::getIp() const
{
    return _ip;
}

void Babibel::Model::Contact::setIp(const std::string &ip)
{
    _ip = ip;
}

std::string Babibel::Model::Contact::getStatusText() const
{
    return _statusText;
}

void Babibel::Model::Contact::setStatusText(const std::string &statusText)
{
    _statusText = statusText;
}

std::string Babibel::Model::Contact::getLogin() const
{
    return _login;
}

void Babibel::Model::Contact::setLogin(const std::string &login)
{
    _login = login;
}

std::list<std::pair<eIdentity, std::pair<time_t, std::string> > >  & Babibel::Model::Contact::getMessages()
{
    return _messages;
}

void Babibel::Model::Contact::setMessages(const std::list<std::pair<eIdentity, std::pair<time_t, std::string> > > &messages)
{
    _messages = messages;
}

bool Babibel::Model::Contact::getUpdated() const
{
    return _updated;
}

void Babibel::Model::Contact::setUpdated(bool updated)
{
    _updated = updated;
}

bool Babibel::Model::Contact::getContact() const
{
    return _contact;
}

void Babibel::Model::Contact::setContact(bool contact)
{
    _contact = contact;
}

void Babibel::Model::Contact::addMessage(eIdentity id, time_t time, const std::string & msg)
{
    this->_messages.push_back(std::pair<eIdentity, std::pair<int, std::string> >(id, std::pair<int, std::string>(time, msg)));
}

eStatusCall Babibel::Model::Contact::getStatusCall() const
{
    return _statusCall;
}

void Babibel::Model::Contact::setStatusCall(const eStatusCall &statusCall)
{
    _statusCall = statusCall;
}
