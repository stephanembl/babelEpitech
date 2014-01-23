#pragma once

#include <map>
#include <string>
#include "Account.hh"

class ISerialize
{
public:
	virtual ~ISerialize() {}
	virtual bool save(std::map<std::string, Account *>&) = 0;
	virtual bool restore(std::map<std::string, Account *>&) = 0;
};
