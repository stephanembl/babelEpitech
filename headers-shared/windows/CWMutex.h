#pragma once

#include <Windows.h>
#include "../headers-shared/shared/IMutex.h"

class					CWMutex : public IMutex
{
protected:
	CRITICAL_SECTION	_mutex;

public:
	CWMutex();
	virtual ~CWMutex();

	void				lock();
	void				unLock();
	bool				tryLock();
	CRITICAL_SECTION		*getMutex();
};

