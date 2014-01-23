#pragma once

#ifdef _WIN32
# include "../headers-shared/windows/CWMutex.h"
typedef	CWMutex	CMutex;
#else
# include "../headers-shared/linux/CUMutex.h"
typedef	CUMutex	CMutex;
#endif // _WIN32

class Mutex : public CMutex
{
public:
	Mutex();
	virtual ~Mutex();
};

