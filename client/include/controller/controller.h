#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "include/view/babibelwindow.h"
#include "include/model/core.h"
#include "include/controller/Network.hpp"
#include "include/audio/AbsAudio.hpp"
#include "include/audio/AbsAudio.hpp"

namespace Babibel
{
namespace Controller
{
class Controller
{
    ::Babibel::Model::Core                  *_model;
    ::Babibel::View::IMainWindow            *_view;
    ::Babibel::Controller::Network          *_network;
    AbsAudio                                *_audio;

public:
    Controller();
    ~Controller();
    ::Babibel::Model::Core *model() const;
    void setModel(::Babibel::Model::Core *model);
    ::Babibel::View::IMainWindow *view() const;
    void setView(::Babibel::View::IMainWindow *view);
    ::Babibel::Controller::Network *network() const;
    void setNetwork(::Babibel::Controller::Network *network);
    AbsAudio *getAudio() const;
    void setAudio(AbsAudio *audio);
};
}
}

#endif // Controller_H
