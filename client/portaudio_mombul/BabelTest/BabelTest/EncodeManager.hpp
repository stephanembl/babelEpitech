#pragma once

#include <iostream>
#include "AbsEncode.hpp"
#include "opus.h"
#include "audiosettings.h"

class	EncodeManager : public AbsEncode
{
private:
	OpusEncoder	*_enc;
	OpusDecoder	*_dec;
	opus_int32	_len;
	int			_err;

public:
	EncodeManager();
	virtual ~EncodeManager();
	virtual unsigned char	*encodeAudio(const SAMPLE *frame, int *retenc);
	virtual void			decodeAudio(const unsigned char *data, SAMPLE *out, int retenc);
};