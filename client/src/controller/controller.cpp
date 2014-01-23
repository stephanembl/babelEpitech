#include "include/controller/controller.h"
#include "include/view/MyApplication.h"
#include "../headers-shared/shared/Exception.h"
#include "../headers-shared/shared/LibC.hh"
#include "include/audio/AudioManager.hpp"

#include <iostream>

Babibel::Controller::Controller::Controller()
{
    int i = 0;
    MyApplication app(i, NULL);

    this->_model = new Babibel::Model::Core(this);
    this->_network = new Babibel::Controller::Network(this);
    this->_audio = new AudioManager();
    this->_audio->initAudio();
    this->_view = new Babibel::View::BabibelWindow(this);

    while (this->_view->isActive())
    {
        if (this->_audio->isStarted())
        {
            std::pair<const unsigned char *, int> d = this->_audio->getRecData();
            this->_network->sockUDP()->writeSocket((char *)d.first, d.second);
            //std::cout << "SEND" << d.second << std::endl;
//            Packet   *p = new Packet(0);
//            p->setInstruction(::Babibel::Controller::Network::AUDIO);
//            p->updateData(d.second + 3 + 4);
//            p->setFormat("a");
//            p->appendToData(0, (char *)d.first, d.second);
//            this->_network->pushUDP(p);
        }
        if (this->_network->sockTCP()->hasBeenDisconnected() == true)
            this->_view->disconnected();
        this->_network->handleNetwork();
        app.processEvents();
        LibC::mSleep(16);
    }
}

Babibel::Controller::Controller::~Controller()
{
    delete this->_model;
}

::Babibel::Model::Core *Babibel::Controller::Controller::model() const
{
    return _model;
}

void Babibel::Controller::Controller::setModel(::Babibel::Model::Core *model)
{
    _model = model;
}

::Babibel::View::IMainWindow *Babibel::Controller::Controller::view() const
{
    return _view;
}

void Babibel::Controller::Controller::setView(::Babibel::View::IMainWindow *view)
{
    _view = view;
}

::Babibel::Controller::Network *Babibel::Controller::Controller::network() const
{
    return _network;
}

void Babibel::Controller::Controller::setNetwork(::Babibel::Controller::Network *network)
{
    _network = network;
}


AbsAudio *Babibel::Controller::Controller::getAudio() const
{
    return _audio;
}

void Babibel::Controller::Controller::setAudio(AbsAudio *audio)
{
    _audio = audio;
}
