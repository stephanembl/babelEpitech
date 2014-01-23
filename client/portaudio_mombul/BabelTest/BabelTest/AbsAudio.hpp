#pragma once

#include <iostream>
#include <fstream>
#include <iterator>
#include <utility>
#include "AbsEncode.hpp"

class	AbsAudio
{
public:
	virtual ~AbsAudio() {}

	virtual int						initAudio() = 0;
	virtual void					errorAudio() = 0;
	virtual void					initInput() = 0;
	virtual void					initOutput() = 0;
	virtual int						setupStream() = 0;
	virtual int						start() = 0;
	virtual int						stop() = 0;
	virtual AbsEncode *				getEnc() = 0;
	virtual void					setData(unsigned char *) = 0;
	virtual const int				getRetenc() const = 0;
	virtual void					setRetenc(int) = 0;
	virtual const std::pair<const unsigned char *, const int>	getData() const = 0;
};