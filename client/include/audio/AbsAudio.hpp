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
    virtual bool					isStarted() const = 0;
	virtual int						stop() = 0;
	virtual AbsEncode *				getEnc() = 0;
    virtual unsigned char *         getDataRec() const = 0;
    virtual void					setDataRec(unsigned char *) = 0;
    virtual int                     getRetencRec() const = 0;
    virtual void					setRetencRec(int) = 0;

    virtual unsigned char *         getDataPlay() const = 0;
    virtual void					setDataPlay(unsigned char *) = 0;
    virtual int                     getRetencPlay() const = 0;
    virtual void					setRetencPlay(int) = 0;
	virtual const std::pair<const unsigned char *, const int>	getData() const = 0;

    virtual const std::pair<const unsigned char *, const int>   getRecData() = 0;
    virtual void                                                        setPlayData(unsigned char *data, int size) = 0;

};
