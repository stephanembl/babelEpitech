#include "include/controller/AClientUDP.h"
#include <iostream>

Babibel::Controller::AClientUDP::AClientUDP()
{
    this->_connected = false;
    this->_readable = false;
    this->_disconnected = false;
}

Babibel::Controller::AClientUDP::~AClientUDP()
{
}

void Babibel::Controller::AClientUDP::bind()
{
}

void Babibel::Controller::AClientUDP::setIp(const std::string &ip)
{
    this->_ip = ip;
}

void Babibel::Controller::AClientUDP::setSender(const std::string & ip)
{
    this->_sip = ip;
}

void Babibel::Controller::AClientUDP::setPort(int)
{
}

void Babibel::Controller::AClientUDP::readSocket()
{
}

void Babibel::Controller::AClientUDP::writeSocket(const char *data, int size)
{
    (void)data;
    (void)size;
}

std::list<std::pair<char *, int> > Babibel::Controller::AClientUDP::getQueue()
{
    std::list<std::pair<char *, int> > retQueue;

    this->_readable = false;
    retQueue = this->_queue;
    this->_queue.clear();
    return (retQueue);
}

bool Babibel::Controller::AClientUDP::isConnected() const
{
    return (this->_connected);
}

bool Babibel::Controller::AClientUDP::hasBeenDisconnected()
{
    if (this->_disconnected == true)
    {
        this->_disconnected = false;
        return (true);
    }
    return (false);
}


bool Babibel::Controller::AClientUDP::isReadable() const
{
    return (this->_readable);
}

std::string Babibel::Controller::AClientUDP::error() const
{
    return _error;
}
bool Babibel::Controller::AClientUDP::event() const
{
    return _event;
}

void Babibel::Controller::AClientUDP::setEvent(bool event)
{
    _event = event;
}
