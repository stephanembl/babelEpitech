#include "include/controller/QClientTCP.h"
#include "../headers-shared/shared/Exception.h"
#include <iostream>

Babibel::Controller::QClientTCP::QClientTCP() : AClientTCP()
{
    QObject::connect(&this->_QSocket, SIGNAL(connected()), this, SLOT(connectOk()));
    QObject::connect(&this->_QSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    QObject::connect(&this->_QSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error(QAbstractSocket::SocketError)));
    QObject::connect(&this->_QSocket, SIGNAL(readyRead()), this, SLOT(okToRead()));
}

Babibel::Controller::QClientTCP::~QClientTCP()
{
}

void Babibel::Controller::QClientTCP::connect()
{
    QString ip(this->_ip.c_str());
    this->_event = false;
    this->_QSocket.connectToHost(ip, this->_port);
    this->_QSocket.waitForConnected(30000);
}

void Babibel::Controller::QClientTCP::connectOk()
{
    this->_connected = true;
    this->_event = true;
}

void Babibel::Controller::QClientTCP::disconnected()
{
    this->_connected = false;
    this->_disconnected = true;
    this->_event = true;
}

void Babibel::Controller::QClientTCP::error(QAbstractSocket::SocketError)
{
    this->_connected = false;
    this->_error = this->_QSocket.errorString().toStdString();
    this->_event = true;
}

void Babibel::Controller::QClientTCP::okToRead()
{
    this->_readable = true;
    this->readSocket();
}

void Babibel::Controller::QClientTCP::setIp(const std::string &ip)
{
    this->_ip = ip;
}

void Babibel::Controller::QClientTCP::setPort(int port)
{
    this->_port = port;
}

void Babibel::Controller::QClientTCP::readSocket()
{
    qint64 maxLen = 4096;
    std::pair<char *, int> line;

    while(42)
    {
        line.first = new char[maxLen];
        if (this->_disconnected == false && (line.second = this->_QSocket.read(line.first, maxLen)) <= 0)
        {
            delete[] line.first;
            break;
        }
        this->_queue.push_back(line);
    }
}

void Babibel::Controller::QClientTCP::writeSocket(const char *data, int size)
{
    if (this->_disconnected == false && this->_QSocket.write(data, size) == -1)
    {
        this->_connected = false;
        this->_disconnected = true;
        this->_event = true;
    }
}
