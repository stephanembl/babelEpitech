#pragma once

#include "AbsAudio.hpp"
#include "EncodeManager.hpp"
#include "portaudio.h"
#include "audiosettings.h"
#include "../headers-shared/shared/CircularBuffer.hpp"
#include "../headers-shared/shared/MutexPool.h"

#define PLAY_MUTEX  "play"
#define REC_MUTEX   "rec"


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
    bool                                _started;

    MutexPool                           _pool;

    unsigned char                       _staticBufferRec[255 * 255];
    unsigned char                       _staticBufferRecRet[255 * 255];

    CircularBuffer                      _bufferRec;
    int                                 _sizeRec;

    unsigned char                       _staticBufferPlay[255 * 255];

    CircularBuffer                      _bufferPlay;
    int                                 _sizePlay;

    unsigned char						*_datarec;
    int									_retencrec;

    unsigned char						*_dataplay;
    int									_retencplay;
public:
	AudioManager();
	virtual ~AudioManager();

	virtual int						initAudio();
	virtual void					errorAudio();
	virtual void					initInput();
	virtual void					initOutput();
	virtual int						setupStream();
	virtual int						start();
    virtual bool					isStarted() const;
	virtual int						stop();
	virtual AbsEncode *				getEnc();
	virtual const std::pair<const unsigned char *, const int>	getData() const;
    virtual unsigned char *         getDataRec() const;
    virtual void					setDataRec(unsigned char *);
    virtual int                     getRetencRec() const;
    virtual void					setRetencRec(int);

    virtual unsigned char *         getDataPlay() const;
    virtual void					setDataPlay(unsigned char *);
    virtual int                     getRetencPlay() const;
    virtual void					setRetencPlay(int);

    virtual const std::pair<const unsigned char *, const int>   getRecData();
    virtual void                                                setPlayData(unsigned char *data, int size);

    CircularBuffer &                getBufferRec();
    void                            setBufferRec(const CircularBuffer &bufferRec);
    int                             getSizeRec() const;
    void                            setSizeRec(int sizeRec);
    CircularBuffer &                getBufferPlay();
    void                            setBufferPlay(const CircularBuffer &bufferPlay);
    int                             getSizePlay() const;
    void                            setSizePlay(int sizePlay);
    unsigned char                   *getStaticBufferRec() const;
    unsigned char                   *getStaticBufferPlay() const;
    MutexPool &                     getPool();
};
