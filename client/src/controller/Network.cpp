#include "include/controller/controller.h"
#include "../client/include/controller/Network.hpp"
#include    "include/controller/AClientTCP.h"
#include    "include/controller/AClientUDP.h"
#include <stdexcept>
#include "../headers-shared/shared/LibC.hh"
#include "include/model/contact.h"
#include "include/Enum.h"
#include <string>
#include <algorithm>

void	Babibel::Controller::Network::setInitTCP(bool b)
{
    _initTCP = b;
}

bool	Babibel::Controller::Network::initTCP() const
{
    return (_initTCP);
}

bool Babibel::Controller::Network::initUDP() const
{
    return _initUDP;
}

void Babibel::Controller::Network::setInitUDP(bool initUDP)
{
    _initUDP = initUDP;
}

Babibel::Controller::AClientTCP *Babibel::Controller::Network::sockTCP() const
{
    return _sockTCP;
}


Babibel::Controller::AClientUDP *Babibel::Controller::Network::sockUDP() const
{
    return _sockUDP;
}

void	Babibel::Controller::Network::connectTCP(const std::string &ip, int port)
{
    if (_initTCP == false)
    {
        this->_sockTCP = new Babibel::Controller::QClientTCP();
        this->_sockTCP->setIp(ip);
        this->_sockTCP->setPort(port);
        this->_sockTCP->connect();
        while (this->_sockTCP->event() == false)
            LibC::mSleep(16);
        if (this->_sockTCP->isConnected() == false)
        {
            delete this->_sockTCP;
            _initTCP = false;
        }
        else
        {
            _initTCP = true;
        }
        this->_pingTimer = LibC::time(NULL);
    }
}

void	Babibel::Controller::Network::connectUDP(const std::string &ip, int port)
{
    if (this->_sockUDP && this->_initUDP == false)
    {
        this->_sockUDP->setIp(ip);
        this->_sockUDP->setPort(port);
        this->_sockUDP->bind();

        if (this->_sockUDP->isConnected() == false)
        {
            delete this->_sockUDP;
            _initUDP = false;
        }
        else
        {
            _initUDP = true;
        }
    }
}

void	Babibel::Controller::Network::handleNetworkUDP()
{
    if (_sockUDP->isReadable())
    {
        //        Packet *p;

        std::list<std::pair<char *, int> > lst = this->_sockUDP->getQueue();

        for (std::list<std::pair<char *, int> >::iterator it = lst.begin(); it != lst.end(); ++it)
        {
            this->_controller->getAudio()->setPlayData((unsigned char *)(*it).first, (*it).second);
            //this->_controller->getAudio()->setDataPlay((unsigned char *)(*it).first);
            //this->_controller->getAudio()->setRetencPlay((*it).second);
        }
    }
}

void	Babibel::Controller::Network::handleNetworkTCP()
{
    if (LibC::time(NULL) - this->_pingTimer > 45)
    {
        Packet *p = new Packet();
        p->setInstruction(PING);
        this->_sendQueueTCP.push(p);
        this->_pingTimer = LibC::time(NULL);
    }
    if (_sendQueueTCP.empty() == false)
    {
        Packet	*packet = this->_sendQueueTCP.front();
        this->_sendQueueTCP.pop();
        try
        {
            //	  std::cout << "SEND";
            //	  packet->show();
            //	  std::cout << "ENDSEND" << std::endl;
            _sockTCP->writeSocket(packet->serialize(), packet->size());
        }
        catch (std::runtime_error &e)
        {
            std::cout << "erreur :" << e.what() << std::endl;
            delete _sockTCP;
            _sockTCP = NULL;
            _initTCP = false;
        }
        delete packet;
    }
    if (_sockTCP->isReadable())
    {
        Packet *p;

        std::list<std::pair<char *, int> > lst = this->_sockTCP->getQueue();
        for (std::list<std::pair<char *, int> >::iterator it = lst.begin(); it != lst.end(); ++it)
        {
            if (_factoryTCP.feed((*it).first, (*it).second) == false)
            {
                delete[] (*it).first;
                std::cerr << "FAKE PACKET !!!!" << std::endl;
                return;
            }
            delete[] (*it).first;
            while ((p = _factoryTCP.getPacket()))
            {
                //PROCESS PACKET

                if (p && p->getInstruction() < ENUM_COUNT)
                {
                    //		  std::cout << "GOT";
                    //		  p->show();
                    //		  std::cout << "ENDGOT" << std::endl;

                    if (!(this->*_getPtrs[p->getInstruction()])(p))
                    {
                        //ERROR
                    }
                }
                if (p)
                    delete p;
            }
        }
    }
}


//GETTERZ

bool      Babibel::Controller::Network::getList(Packet *packet)
{
    int		id(1);
    std::string login;
    std::string statusText;
    eStatus     status;
    std::list<Babibel::Model::Contact *> & contacts =  this->_controller->model()->getContacts();

    char err = packet->getIntTypeInData<char>(0);

    //ERROR
    if (err == 0)
    {
        this->_controller->view()->serverError(packet->getStringInData(1));
        return (false);
    }
    else
    {
        for(std::list<Babibel::Model::Contact *>::iterator it = contacts.begin(); it != contacts.end(); ++it)
            (*it)->setUpdated(false);

        for (unsigned short i = 0; i < packet->getIntTypeInData<unsigned short>(1);++i)
        {
            try
            {
                login = packet->getStringInData(++id);
                statusText = packet->getStringInData(++id);
                status = (eStatus)packet->getIntTypeInData<char>(++id);
            }
            catch (std::out_of_range &)
            {
                break;
            }
            //PROCESS SUCCESS
            if (login.size())
            {
                std::list<Babibel::Model::Contact *>::iterator it = std::find_if(contacts.begin(), contacts.end(), Babibel::Model::Contact::contact_has_login(login));
                if (it != contacts.end())
                {
                    (*it)->setLogin(login);
                    (*it)->setStatusText(statusText);
                    (*it)->setStatus(status);
                    (*it)->setContact(true);
                    (*it)->setUpdated(true);
                }
                else
                {
                    Babibel::Model::Contact *c = new Babibel::Model::Contact;
                    c->setLogin(login);
                    c->setStatusText(statusText);
                    c->setStatus(status);
                    c->setContact(true);
                    c->setUpdated(true);
                    contacts.push_back(c);
                }
            }
        }
        for(std::list<Babibel::Model::Contact *>::iterator it = contacts.begin(); it != contacts.end();)
        {
            if ((*it)->getUpdated() == false)
            {
                delete (*it);
                it = contacts.erase(it);
            }
            else
                ++it;
        }
        this->_controller->view()->updateList();
        return (true);
    }
    return (true);
}

bool      Babibel::Controller::Network::getCall(Packet *packet)
{
    std::list<Babibel::Model::Contact *> & contacts =  this->_controller->model()->getContacts();
    char err = packet->getIntTypeInData<char>(0);

    //ERROR
    if (err == 0)
    {
        std::string login = packet->getStringInData(2);
        if (login == "")
            login = this->_controller->model()->callingLogin();
        std::list<Babibel::Model::Contact *>::iterator it = std::find_if(contacts.begin(), contacts.end(), Babibel::Model::Contact::contact_has_login(login));
        if (it != contacts.end())
        {
            (*it)->setStatusCall(NONE);
            this->_controller->model()->setStatusCall(NONE);
            this->_controller->model()->setCallingLogin("");
        }
        this->_controller->view()->serverError(packet->getStringInData(1));
        this->_controller->view()->call(login);
        return (false);
    }
    else
    {
        std::string login = packet->getStringInData(1);
        std::string ip = packet->getStringInData(2);
        if (login.size())
        {
            std::list<Babibel::Model::Contact *>::iterator it = std::find_if(contacts.begin(), contacts.end(), Babibel::Model::Contact::contact_has_login(login));
            if (it != contacts.end())
            {
                (*it)->setStatusCall(ISWAITING);
                this->_controller->model()->setStatusCall(ISWAITING);
                this->_controller->model()->setCallingLogin(login);
            }
            else
            {
                Babibel::Model::Contact *c = new Babibel::Model::Contact;
                c->setLogin(login);
                c->setStatusCall(ISWAITING);
                this->_controller->model()->setStatusCall(ISWAITING);
                this->_controller->model()->setCallingLogin(login);
                contacts.push_back(c);
            }
        }

        this->_sockUDP->setSender(ip);

        this->_controller->view()->call(login);
        return (true);
    }
    return (true);
}

bool      Babibel::Controller::Network::getHangup(Packet *packet)
{
    std::list<Babibel::Model::Contact *> & contacts =  this->_controller->model()->getContacts();
    char err = packet->getIntTypeInData<char>(0);

    //ERROR
    if (err == 0)
    {
        std::string login = packet->getStringInData(2);
        if (login == "")
            login = this->_controller->model()->callingLogin();
        std::list<Babibel::Model::Contact *>::iterator it = std::find_if(contacts.begin(), contacts.end(), Babibel::Model::Contact::contact_has_login(login));
        if (it != contacts.end())
        {
            (*it)->setStatusCall(NONE);
            this->_controller->model()->setStatusCall(NONE);
            this->_controller->model()->setCallingLogin("");
        }
        this->_controller->view()->serverError(packet->getStringInData(1));
        this->_controller->view()->call(login);
        return (false);
    }
    else
    {
        std::string login = packet->getStringInData(1);
        if (login.size())
        {
            std::list<Babibel::Model::Contact *>::iterator it = std::find_if(contacts.begin(), contacts.end(), Babibel::Model::Contact::contact_has_login(login));
            if (it != contacts.end())
            {
                (*it)->setStatusCall(NONE);
                this->_controller->model()->setStatusCall(NONE);
                this->_controller->model()->setCallingLogin("");
            }
            else
            {
                Babibel::Model::Contact *c = new Babibel::Model::Contact;
                c->setLogin(login);
                c->setStatusCall(NONE);
                this->_controller->model()->setStatusCall(NONE);
                this->_controller->model()->setCallingLogin("");
                contacts.push_back(c);
            }
        }
        this->_controller->view()->call(login);

        this->_sockUDP->setSender("");
        if (this->_controller->getAudio()->isStarted() == true)
            this->_controller->getAudio()->stop();

        return (true);
    }
    return (true);
}

bool      Babibel::Controller::Network::getStatusText(Packet *packet)
{
    char err = packet->getIntTypeInData<char>(0);

    //ERROR
    if (err == 0)
    {
        this->_controller->view()->serverError(packet->getStringInData(1));
        return (false);
    }
    else
    {
        this->_controller->model()->setStatusText(packet->getStringInData(1));
        this->_controller->view()->updateUserInfo();
        return (true);
    }
    return (true);
}

bool      Babibel::Controller::Network::getStatus(Packet *packet)
{
    char err = packet->getIntTypeInData<char>(0);

    //ERROR
    if (err == 0)
    {
        this->_controller->view()->serverError(packet->getStringInData(1));
        return (false);
    }
    else
    {
        this->_controller->model()->setStatus((eStatus)packet->getIntTypeInData<char>(1));
        this->_controller->view()->updateUserInfo();
        return (true);
    }
    return (true);
}

bool      Babibel::Controller::Network::getAcceptCall(Packet *packet)
{
    std::list<Babibel::Model::Contact *> & contacts =  this->_controller->model()->getContacts();
    char err = packet->getIntTypeInData<char>(0);

    //ERROR
    if (err == 0)
    {
        std::string login = packet->getStringInData(2);
        if (login == "")
            login = this->_controller->model()->callingLogin();
        std::list<Babibel::Model::Contact *>::iterator it = std::find_if(contacts.begin(), contacts.end(), Babibel::Model::Contact::contact_has_login(login));
        if (it != contacts.end())
        {
            (*it)->setStatusCall(NONE);
            this->_controller->model()->setStatusCall(NONE);
            this->_controller->model()->setCallingLogin("");
        }
        this->_controller->view()->serverError(packet->getStringInData(1));
        this->_controller->view()->call(login);
        return (false);        return (false);
    }
    else
    {
        std::string login = packet->getStringInData(1);
        std::string ip = packet->getStringInData(2);
        if (login.size())
        {
            std::list<Babibel::Model::Contact *>::iterator it = std::find_if(contacts.begin(), contacts.end(), Babibel::Model::Contact::contact_has_login(login));
            if (it != contacts.end())
            {
                (*it)->setStatusCall(ISCALLING);
                this->_controller->model()->setStatusCall(ISCALLING);
                this->_controller->model()->setCallingLogin(login);
            }
            else
            {
                Babibel::Model::Contact *c = new Babibel::Model::Contact;
                c->setLogin(login);
                c->setStatusCall(ISCALLING);
                this->_controller->model()->setStatusCall(ISCALLING);
                this->_controller->model()->setCallingLogin(login);

                contacts.push_back(c);
            }
        }
        this->_sockUDP->setSender(ip);
        if (this->_controller->getAudio()->isStarted() == false)
            this->_controller->getAudio()->start();
        this->_controller->view()->call(login);
        return (true);
    }
    return (true);
}

bool      Babibel::Controller::Network::getRejectCall(Packet *packet)
{
    std::list<Babibel::Model::Contact *> & contacts =  this->_controller->model()->getContacts();
    char err = packet->getIntTypeInData<char>(0);

    //ERROR
    if (err == 0)
    {
        std::string login = packet->getStringInData(2);
        if (login == "")
            login = this->_controller->model()->callingLogin();
        std::list<Babibel::Model::Contact *>::iterator it = std::find_if(contacts.begin(), contacts.end(), Babibel::Model::Contact::contact_has_login(login));
        if (it != contacts.end())
        {
            (*it)->setStatusCall(NONE);
            this->_controller->model()->setStatusCall(NONE);
            this->_controller->model()->setCallingLogin("");
        }
        this->_controller->view()->serverError(packet->getStringInData(1));
        this->_controller->view()->call(login);
        return (false);
    }
    else
    {
        std::string login = packet->getStringInData(1);
        std::string ip = packet->getStringInData(2);
        if (login.size())
        {
            std::list<Babibel::Model::Contact *>::iterator it = std::find_if(contacts.begin(), contacts.end(), Babibel::Model::Contact::contact_has_login(login));
            if (it != contacts.end())
            {
                (*it)->setStatusCall(NONE);
                this->_controller->model()->setStatusCall(NONE);
                this->_controller->model()->setCallingLogin("");
            }
            else
            {
                Babibel::Model::Contact *c = new Babibel::Model::Contact;
                c->setLogin(login);
                c->setStatusCall(NONE);
                this->_controller->model()->setStatusCall(NONE);
                this->_controller->model()->setCallingLogin("");
                contacts.push_back(c);
            }
        }
        this->_controller->view()->call(login);

        this->_sockUDP->setSender("");
        if (this->_controller->getAudio()->isStarted() == true)
            this->_controller->getAudio()->stop();

        return (true);
    }
    return (true);
}

bool      Babibel::Controller::Network::getLogin(Packet *packet)
{
    char err = packet->getIntTypeInData<char>(0);

    //ERROR
    if (err == 0)
    {
        this->_controller->view()->serverLoginError(packet->getStringInData(1));
        return (false);
    }
    else
    {
        this->_controller->model()->setLogin(packet->getStringInData(1));
        this->_controller->view()->updateUserInfo();
        this->_controller->view()->loggedIn();
        return (true);
    }
    return (true);
}

bool      Babibel::Controller::Network::getCreateAccount(Packet *packet)
{
    char err = packet->getIntTypeInData<char>(0);

    //ERROR
    if (err == 0)
    {
        this->_controller->view()->serverError(packet->getStringInData(1));
        return (false);
    }
    else
    {
        this->_controller->model()->setLogin(packet->getStringInData(1));
        this->_controller->view()->updateUserInfo();
        this->_controller->view()->loggedIn();
        return (true);
    }
    return (true);
}

bool      Babibel::Controller::Network::getAddContact(Packet *packet)
{
    char err = packet->getIntTypeInData<char>(0);

    //ERROR
    if (err == 0)
    {
        this->_controller->view()->serverError(packet->getStringInData(1));
        return (false);
    }
    else
    {
        return (true);
    }
    return (true);
}

bool      Babibel::Controller::Network::getRemoveContact(Packet *packet)
{
    char err = packet->getIntTypeInData<char>(0);

    //ERROR
    if (err == 0)
    {
        this->_controller->view()->serverError(packet->getStringInData(1));
        return (false);
    }
    else
    {
        return (true);
    }
    return (true);
}

bool      Babibel::Controller::Network::getBlockContact(Packet *packet)
{
    char err = packet->getIntTypeInData<char>(0);

    //ERROR
    if (err == 0)
    {
        this->_controller->view()->serverError(packet->getStringInData(1));
        return (false);
    }
    else
    {
        return (true);
    }
    return (true);
}

bool      Babibel::Controller::Network::getChat(Packet *packet)
{
    std::list<Babibel::Model::Contact *> & contacts =  this->_controller->model()->getContacts();
    char err = packet->getIntTypeInData<char>(0);

    //ERROR
    if (err == 0)
    {
        this->_controller->view()->serverError(packet->getStringInData(1));
        return (false);
    }
    else
    {
        std::string login = packet->getStringInData(1);
        std::string message = packet->getStringInData(2);
        if (login.size())
        {
            std::list<Babibel::Model::Contact *>::iterator it = std::find_if(contacts.begin(), contacts.end(), Babibel::Model::Contact::contact_has_login(login));
            if (it != contacts.end())
            {
                (*it)->addMessage((packet->getRequestUID() != 0) ? (ME) : (NOT_ME),
                                  LibC::time(NULL), message);
            }
            else
            {
                Babibel::Model::Contact *c = new Babibel::Model::Contact;
                c->setLogin(login);
                c->addMessage((packet->getRequestUID() != 0)  ? (ME) : (NOT_ME),
                              LibC::time(NULL), message);
                contacts.push_back(c);
            }
        }
        this->_controller->view()->updateConversation(login);
        return (true);
    }
    return (true);
}

bool      Babibel::Controller::Network::getError(Packet *packet)
{
    (void)packet;
    return (true);
}

bool      Babibel::Controller::Network::getHandshake(Packet *packet)
{
    (void)packet;
    Packet *p = new Packet();
    p->setInstruction(HANDSHAKE);
    this->_sendQueueTCP.push(p);
    return (true);
}

bool      Babibel::Controller::Network::getPing(Packet *packet)
{
    (void)packet;
    return (true);
}





//SETTERS (senders)

bool      Babibel::Controller::Network::setCreateAccount(const std::string &login, const std::string &password)
{
    Packet *p = new Packet();
    p->setInstruction(CREATE_ACCOUNT);
    p->updateData(4+login.size() + 4 + password.size() + 4);
    p->setFormat("ss");
    p->appendToData(0, login.c_str());
    p->appendToData(1, password.c_str());
    this->_sendQueueTCP.push(p);
    return (true);
}

bool      Babibel::Controller::Network::setLogin(const std::string &login, const std::string &password)
{
    Packet *p = new Packet();
    p->setInstruction(LOGIN);
    p->updateData(4+login.size() + 4 + password.size() + 4);
    p->setFormat("ss");
    p->appendToData(0, login.c_str());
    p->appendToData(1, password.c_str());
    this->_sendQueueTCP.push(p);
    return (true);
}

bool      Babibel::Controller::Network::setStatusText(const std::string &status)
{
    Packet *p = new Packet();
    p->setInstruction(STATUSTEXT);
    p->updateData(3+status.size() + 4);
    p->setFormat("s");
    p->appendToData(0, status.c_str());
    this->_sendQueueTCP.push(p);
    return (true);
}

bool      Babibel::Controller::Network::setStatus(eStatus s)
{
    Packet *p = new Packet();
    p->setInstruction(STATUS);
    p->updateData(6);
    p->setFormat("c");
    p->appendToData<char>(0, s);
    this->_sendQueueTCP.push(p);
    return (true);
}

bool      Babibel::Controller::Network::setAddContact(const std::string &login)
{
    Packet *p = new Packet();
    p->setInstruction(ADD_CONTACT);
    p->updateData(3 + login.size() + 4);
    p->setFormat("s");
    p->appendToData(0, login.c_str());
    this->_sendQueueTCP.push(p);
    return (true);
}

bool      Babibel::Controller::Network::setRemoveContact(const std::string &login)
{
    Packet *p = new Packet();
    p->setInstruction(REMOVE_CONTACT);
    p->updateData(3 + login.size() + 4);
    p->setFormat("s");
    p->appendToData(0, login.c_str());
    this->_sendQueueTCP.push(p);
    return (true);
}

bool      Babibel::Controller::Network::setBlockContact(const std::string &login)
{
    Packet *p = new Packet();
    p->setInstruction(BLOCK_CONTACT);
    p->updateData(3 + login.size() + 4);
    p->setFormat("s");
    p->appendToData(0, login.c_str());
    this->_sendQueueTCP.push(p);
    return (true);
}

bool      Babibel::Controller::Network::setChat(const std::string &login, const std::string &message)
{
    Packet *p = new Packet();
    p->setInstruction(CHAT);
    p->updateData(4+login.size() + 4 + message.size() + 4);
    p->setFormat("ss");
    p->appendToData(0, login.c_str());
    p->appendToData(1, message.c_str());
    this->_sendQueueTCP.push(p);
    return (true);
}

bool      Babibel::Controller::Network::setAcceptCall(const std::string &login)
{
    std::list<Babibel::Model::Contact *> & contacts =  this->_controller->model()->getContacts();
    std::list<Babibel::Model::Contact *>::iterator it = std::find_if(contacts.begin(), contacts.end(), Babibel::Model::Contact::contact_has_login(login));
    if (it != contacts.end())
    {
        (*it)->setStatusCall(ISCALLING);
        this->_controller->model()->setStatusCall(ISCALLING);
        this->_controller->model()->setCallingLogin(login);
    }

    this->_controller->view()->call(login);
    if (this->_controller->getAudio()->isStarted() == false)
        this->_controller->getAudio()->start();

    Packet *p = new Packet();
    p->setInstruction(ACCEPT_CALL);
    p->updateData(3 + login.size() + 4);
    p->setFormat("s");
    p->appendToData(0, login.c_str());
    this->_sendQueueTCP.push(p);
    return (true);
}

bool      Babibel::Controller::Network::setRejectCall(const std::string &login)
{
    std::list<Babibel::Model::Contact *> & contacts =  this->_controller->model()->getContacts();
    std::list<Babibel::Model::Contact *>::iterator it = std::find_if(contacts.begin(), contacts.end(), Babibel::Model::Contact::contact_has_login(login));
    if (it != contacts.end())
    {
        (*it)->setStatusCall(NONE);
        this->_controller->model()->setStatusCall(NONE);
        this->_controller->model()->setCallingLogin("");
    }
    this->_controller->view()->call(login);
    this->_sockUDP->setSender("");
    if (this->_controller->getAudio()->isStarted() == true)
        this->_controller->getAudio()->stop();

    Packet *p = new Packet();
    p->setInstruction(REJECT_CALL);
    p->updateData(3 + login.size() + 4);
    p->setFormat("s");
    p->appendToData(0, login.c_str());
    this->_sendQueueTCP.push(p);
    return (true);
}

bool      Babibel::Controller::Network::setCall(const std::string &login)
{
    std::list<Babibel::Model::Contact *> & contacts =  this->_controller->model()->getContacts();
    std::list<Babibel::Model::Contact *>::iterator it = std::find_if(contacts.begin(), contacts.end(), Babibel::Model::Contact::contact_has_login(login));

    if (it == contacts.end())
    {
        this->_controller->model()->setStatusCall(NONE);
        this->_controller->model()->setCallingLogin("");
    }
    else if ((*it)->getStatusCall() == ISCALLING)
    {
        this->_controller->view()->serverError("Already in call");
        return (false);
    }
    if (it != contacts.end())
    {
        (*it)->setStatusCall(ISRINGING);
        this->_controller->model()->setStatusCall(ISRINGING);
        this->_controller->model()->setCallingLogin(login);
    }

    this->_controller->view()->call(login);

    Packet *p = new Packet();
    p->setInstruction(CALL);
    p->updateData(3 + login.size() + 4);
    p->setFormat("s");
    p->appendToData(0, login.c_str());
    this->_sendQueueTCP.push(p);

    return (true);
}


bool      Babibel::Controller::Network::setHangup(const std::string &)
{
    this->_sockUDP->setSender("");
    if (this->_controller->getAudio()->isStarted() == true)
        this->_controller->getAudio()->stop();

    Packet *p = new Packet();
    p->setInstruction(HANGUP);
    this->_sendQueueTCP.push(p);
    return (true);
}
