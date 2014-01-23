#ifndef CONTACT_H
#define CONTACT_H

#include <list>
#include <string>
#include "include/Enum.h"

namespace Babibel
{
namespace Model
{
class Contact
{
    std::string     _login;
    std::string     _statusText;
    std::string     _ip;
    eStatus         _status;
    bool            _updated;
    bool            _contact;
    eStatusCall     _statusCall;

    std::list<std::pair<eIdentity, std::pair<time_t, std::string> > >   _messages;
public:
    Contact();
    ~Contact();

    eStatus getStatus() const;
    void setStatus(const eStatus &status);

    std::string getIp() const;
    void setIp(const std::string &ip);

    std::string getStatusText() const;
    void setStatusText(const std::string &statusText);

    std::string getLogin() const;
    void setLogin(const std::string &login);

    std::list<std::pair<eIdentity, std::pair<time_t, std::string> > >  & getMessages();
    void setMessages(const std::list<std::pair<eIdentity, std::pair<time_t, std::string> > > &messages);

    void addMessage(eIdentity, time_t, const std::string & msg);

    bool getUpdated() const;
    void setUpdated(bool updated);
    bool getContact() const;
    void setContact(bool contact);
    eStatusCall getStatusCall() const;
    void setStatusCall(const eStatusCall &statusCall);

public:
    struct contact_has_login
    {
        contact_has_login(std::string const& n) : login(n) { }
        bool operator () (Babibel::Model::Contact const* p) { return p->getLogin() == login; }
    private:
        std::string login;
    };

};
}
}



#endif // Contact_H
