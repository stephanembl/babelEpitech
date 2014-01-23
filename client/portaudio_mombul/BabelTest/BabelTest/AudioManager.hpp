#pragma once

#include "AbsAudio.hpp"
#include "EncodeManager.hpp"
#include "portaudio.h"
#include "audiosettings.h"

class	AudioManager : public AbsAudio
{
private:
	PaStream							*_streamin;
	PaStream							*_streamout;
	PaError								_err;
	PaStreamParameters					_inputParam;
	PaStreamParameters					_outputParam;
	AbsEncode							*_enc;
	SAMPLE								_buff[FRAMES_PER_BUFFER];
	bool								_run;
	unsigned char						*_data;
	int									_retenc;

public:
	AudioManager();
	virtual ~AudioManager();

	virtual int						initAudio();
	virtual void					errorAudio();
	virtual void					initInput();
	virtual void					initOutput();
	virtual int						setupStream();
	virtual int						start();
	virtual int						stop();
	virtual AbsEncode *				getEnc();
	virtual const std::pair<const unsigned char *, const int>	getData() const;
	virtual void					setData(unsigned char *);
	virtual const int				getRetenc() const;
	virtual void					setRetenc(int);
};
