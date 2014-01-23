#pragma once

#include <iostream>
#include <fstream>
#include "audiosettings.h"

class	AbsEncode
{
public:
	virtual ~AbsEncode() {}

	virtual unsigned char	*encodeAudio(const SAMPLE *frame, int *retenc) = 0;
	virtual void			decodeAudio(const unsigned char *data, SAMPLE *out, int retenc) = 0;
};