#ifndef IMAINWINDOW_H
#define IMAINWINDOW_H

#include <map>
#include "include/Enum.h"

namespace Babibel
{
namespace View
{
class IMainWindow
{
public:
    virtual void        updateList(const std::map<std::string, std::pair<std::string, eStatus> > &) = 0;
    virtual bool        isActive() const = 0;
    virtual bool        cannotConnect(const std::string &) = 0;
    virtual bool        connected() = 0;
    virtual bool        disconnected() = 0;
    virtual bool        serverError(const std::string &) = 0;
    virtual bool        serverLoginError(const std::string &) = 0;
    virtual bool        loggedIn() = 0;
    virtual bool        updateUserInfo() = 0;
    virtual bool        updateList() = 0;
    virtual bool        updateConversation(const std::string &) = 0;
    virtual bool        call(const std::string &) = 0;

};
}
}

#endif // IMAINWINDOW_H
