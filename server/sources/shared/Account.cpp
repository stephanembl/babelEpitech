#include "Account.hh"

Account::Account(const std::string& login, const std::string& hash)
  : _login(login), _hash(hash)
{
  this->_status = Account::AVAILABLE;
  this->_statusCall = Account::NONE;
  this->_timeoutCall = 0;
  this->_isConnected = false;
}

Account::Account(const Account& copy)
  : _login(copy._login), _hash(copy._hash), _state(copy._state), _status(copy._status), _friends(copy._friends), _statusCall(copy._statusCall), _timeoutCall(copy._timeoutCall), _conversation(copy._conversation), _isConnected(copy._isConnected)
{

}

Account::~Account()
{
  this->_friends.clear();
}

Account				&Account::operator=(const Account& other)
{
  if (&other != this)
    {
      this->_login = other._login;
      this->_hash = other._hash;
      this->_state = other._state;
      this->_status = other._status;
      this->_friends = other._friends;
      this->_statusCall = other._statusCall;
      this->_timeoutCall = other._timeoutCall;
      this->_conversation = other._conversation;
      this->_isConnected = other._isConnected;
    }
  return (*this);
}

bool				Account::changePasswd(const std::string& hash)
{
  this->_hash = hash;
  return (true);
}

const std::string		&Account::getHash() const
{
  return (this->_hash);
}

const std::string		&Account::getLogin() const
{
  return (this->_login);
}

bool				Account::setStatus(Account::eStatus status)
{
  if ((status < Account::AVAILABLE) || (status >= Account::ENUM_COUNT))
    return (false);
  this->_status = status;
  return (true);
}

Account::eStatus		Account::getStatus() const
{
  return (this->_status);
}

const std::string		&Account::getState() const
{
  return (this->_state);
}

bool				Account::setState(const std::string& state)
{
  this->_state = state;
  return (true);
}

Account::eStatusCall		Account::getStatusCall() const
{
  return (this->_statusCall);
}

bool				Account::setStatusCall(Account::eStatusCall statusCall)
{
  this->_statusCall = statusCall;
  return (true);
}

bool				Account::addFriend(const std::string& friends)
{
  if ((this->_friends.find(friends) != this->_friends.end()) || (this->_login == friends))
    return (false);
  this->_friends[friends] = false;
  return (true);
}

bool				Account::deleteFriends(const std::string& friends)
{
  if (this->_friends.find(friends) == this->_friends.end())
    return (false);
  this->_friends.erase(friends);
  return (true);
}

bool				Account::rename(const std::string& newLogin)
{
  if (this->_login == newLogin)
    return (false);
  this->_login = newLogin;
  return (true);
}

bool				Account::friendBlock(const std::string& friends, bool block)
{
  if (this->_friends.find(friends) == this->_friends.end())
    return (false);
  this->_friends[friends] = block;
  return (true);
}

std::list<std::string>		Account::getListFriends() const
{
  std::list<std::string>	tmpList;

  for (std::map<std::string, bool>::const_iterator it = this->_friends.begin();
       it != this->_friends.end(); ++it)
    tmpList.push_back(it->first);
  return (tmpList);
}

bool					Account::renameFriends(const std::string& oldLogin, const std::string& newLogin)
{
  std::map<std::string, bool>::iterator found;

  if ((found = this->_friends.find(oldLogin)) == this->_friends.end())
    return (false);
  this->_friends[newLogin] = found->second;
  this->_friends.erase(found);
  return (true);
}

bool				Account::isBlocked(const std::string& friends)
{
  if (this->_friends.find(friends) == this->_friends.end())
    return (false);
  return (this->_friends[friends]);
}

std::list<Account *>		&Account::getConversation()
{
  return (this->_conversation);
}

std::list<Account *>::iterator		Account::findAccountConversation(const std::string& login)
{
  for (std::list<Account *>::iterator it = this->_conversation.begin(); it != this->_conversation.end(); ++it)
    if ((*it)->getLogin() == login)
      return (it);
  return (this->_conversation.end());
}

const std::map<std::string, bool>	&Account::getFriends() const
{
  return (this->_friends);
}

void					Account::setConnected(bool value)
{
  this->_isConnected = value;
}

bool					Account::isConnected() const
{
  return (this->_isConnected);
}

time_t					Account::getTimeOutCall() const
{
  return (this->_timeoutCall);
}

void					Account::setTimeOutCall(time_t timer)
{
  this->_timeoutCall = timer;
}
