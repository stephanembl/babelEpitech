#include <iostream>
#include "SerializeAccountBoost.hpp"
#include "ManagerAccount.hh"

ManagerAccount::ManagerAccount()
{
  this->_serialize = new SerializeAccountBoost();
}

ManagerAccount::ManagerAccount(const ManagerAccount& copy)
{
  for (std::map<std::string, Account *>::const_iterator it = copy.getListAccount().begin();
       it != copy.getListAccount().end(); ++it)
    this->_accounts[it->first] = new Account(*it->second);
}

ManagerAccount::~ManagerAccount()
{
  this->_serialize->save(this->_accounts);
  for (std::map<std::string, Account *>::iterator it = this->_accounts.begin();
       it != this->_accounts.end(); ++it)
    delete it->second;
  this->_accounts.clear();
  delete this->_serialize;
}

ManagerAccount&					ManagerAccount::operator=(const ManagerAccount& other)
{
  if (&other != this)
    {
      for (std::map<std::string, Account *>::const_iterator it = other.getListAccount().begin();
	   it != other.getListAccount().end(); ++it)
	this->_accounts[it->first] = new Account(*it->second);
    }
  return (*this);
}

void						ManagerAccount::init()
{
  this->_serialize->restore(this->_accounts);
}

bool						ManagerAccount::createAccount(const std::string& login, const std::string& hash)
{
  Account					*newAccount;

  if (this->_accounts.find(login) != this->_accounts.end())
    return (false);
  newAccount = new Account(login, hash);
  this->_accounts[newAccount->getLogin()] = newAccount;
  return (true);
}

bool						ManagerAccount::deleteAccount(const std::string& login)
{
  if (this->_accounts.find(login) == this->_accounts.end())
    return (false);
  for (std::map<std::string, Account *>::iterator it = this->_accounts.begin();
       it != this->_accounts.end(); ++it)
    if (it->first != login)
      it->second->deleteFriends(login);
  this->_accounts.erase(login);
  return (true);
}

Account						*ManagerAccount::findAccount(const std::string& login)
{
  std::map<std::string, Account *>::iterator	found;

  if ((found = this->_accounts.find(login)) == this->_accounts.end())
    return (NULL);
  return (found->second);
}

Account						*ManagerAccount::login(const std::string& login, const std::string& hash)
{
  std::map<std::string, Account *>::iterator	found;

  if ((found = this->_accounts.find(login)) == this->_accounts.end())
    return (NULL);
  if ((found->second->getLogin() == login) && (found->second->getHash() == hash))
    return (found->second);
  return (NULL);
}

const std::map<std::string, Account *>		&ManagerAccount::getListAccount() const
{
  return (this->_accounts);
}

bool						ManagerAccount::call(const std::string& from, const std::string& to)
{
  std::map<std::string, Account *>::iterator    fromAct;
  std::map<std::string, Account *>::iterator    toAct;

  if (((fromAct = this->_accounts.find(from)) == this->_accounts.end()) ||
      ((toAct = this->_accounts.find(to)) == this->_accounts.end()))
    throw Exception("Account not found.");
  if (toAct->second->isBlocked(from))
    return (false);
  if ((fromAct->second->getStatusCall() != Account::NONE) || (toAct->second->getStatusCall() != Account::NONE))
    throw Exception((fromAct->second->getStatusCall() != Account::NONE) ? ("You are already in call.") : ("This account is already in call."));
  fromAct->second->setStatusCall(Account::ISWAITING);
  toAct->second->setStatusCall(Account::ISWAITING);
  fromAct->second->setTimeOutCall(LibC::time(NULL));
  toAct->second->setTimeOutCall(LibC::time(NULL));
  return (true);
}

bool						ManagerAccount::hangUp(const std::string& from)
{
  std::map<std::string, Account *>::iterator    fromAct;
  std::list<Account *>::iterator		found;

  if ((fromAct = this->_accounts.find(from)) == this->_accounts.end())
    return (false);
  else if ((fromAct->second->getStatusCall() == Account::ISWAITING) && ((LibC::time(NULL) - fromAct->second->getTimeOutCall()) > 30))
    {
      fromAct->second->getConversation().clear();
      fromAct->second->setStatusCall(Account::NONE);
      return (false);
    }
  else if (fromAct->second->getConversation().empty())
    return (false);
  for (std::list<Account *>::iterator it = fromAct->second->getConversation().begin();
       it != fromAct->second->getConversation().end(); ++it)
    {
      if ((found = (*it)->findAccountConversation(from)) != (*it)->getConversation().end())
	(*it)->getConversation().erase(found);
      if ((*it)->getConversation().empty())
	(*it)->setStatusCall(Account::NONE);
    }
  fromAct->second->getConversation().clear();
  fromAct->second->setStatusCall(Account::NONE);
  return (true);
}

bool						ManagerAccount::answerCall(const std::string& from, const std::string& to, bool accept)
{
  std::map<std::string, Account *>::iterator    fromAct;
  std::map<std::string, Account *>::iterator    toAct;

  if (((fromAct = this->_accounts.find(from)) == this->_accounts.end()) ||
      ((toAct = this->_accounts.find(to)) == this->_accounts.end()))
    return (false);
  fromAct->second->setStatusCall(((accept == false) ? (Account::NONE) : (Account::ISCALLING)));
  toAct->second->setStatusCall(((accept == false) ? (Account::NONE) : (Account::ISCALLING)));
  if (accept)
    {
      fromAct->second->getConversation().push_back(toAct->second);
      toAct->second->getConversation().push_back(fromAct->second);
    }
  return (true);
}

bool						ManagerAccount::chat(const std::string& from, const std::string& to)
{
  std::map<std::string, Account *>::iterator    fromAct;
  std::map<std::string, Account *>::iterator    toAct;

  if (((fromAct = this->_accounts.find(from)) == this->_accounts.end()) ||
      ((toAct = this->_accounts.find(to)) == this->_accounts.end()))
    throw Exception("Account not found.");
  if (toAct->second->isBlocked(from))
    return (false);
  return (true);
}

bool						ManagerAccount::renameAccount(const std::string& login, const std::string& newLogin)
{
  std::map<std::string, Account *>::iterator	found;

  if ((this->_accounts.find(newLogin) != this->_accounts.end()) ||
      (found = this->_accounts.find(login)) == this->_accounts.end())
    return (false);
  if (!found->second->rename(newLogin))
    return (false);
  this->_accounts[newLogin] = found->second;
  this->_accounts.erase(found);
  for (std::map<std::string, Account *>::iterator it = this->_accounts.begin();
       it != this->_accounts.end(); ++it)
    it->second->renameFriends(login, newLogin);
  return (true);
}
