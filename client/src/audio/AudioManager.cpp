#include "include/audio/AudioManager.hpp"
#ifdef _WIN32
#include <Windows.h>
#endif
#include    "../headers-shared/shared/LibC.hh"


MutexPool & AudioManager::getPool()
{
    return _pool;
}

AudioManager::AudioManager()
{
    this->_enc = new EncodeManager;
    this->_streamin = NULL;
    this->_streamout = NULL;
    this->_started = false;
    this->_pool.init(PLAY_MUTEX);
    this->_pool.init(REC_MUTEX);

    for (int i = 0; i < FRAMES_PER_BUFFER; i++)
	  this->_buff[i] = 0;
    this->_run = true;
    this->_sizeRec = 0;
    this->_sizePlay = 0;
}

AudioManager::~AudioManager()
{
	if (this->_enc)
		delete this->_enc;
    if (this->_dataplay)
        delete this->_dataplay;
    if (this->_datarec)
        delete this->_datarec;
	if (this->_streamin)
		Pa_CloseStream(this->_streamin);
	if (this->_streamout)
		Pa_CloseStream(this->_streamout);
	Pa_Terminate();
}

int			AudioManager::initAudio()
{
	this->_err = Pa_Initialize();
	if (this->_err != paNoError)
	{
		this->errorAudio();
		return (0);
	}
	this->initInput();
	this->initOutput();
	this->setupStream();
	return (1);
}

void		AudioManager::errorAudio()
{
	if (this->_err != paNoError)
	{
		this->_run = false;
		if (this->_streamin)
		{
			Pa_AbortStream(this->_streamin);
			Pa_CloseStream(this->_streamin);
		}
		if (this->_streamout)
		{
			Pa_AbortStream(this->_streamout);
			Pa_CloseStream(this->_streamout);
		}
		Pa_Terminate();
		std::cerr << "Error !" << std::endl;
		std::cerr << "Error number : " << this->_err << std::endl;
		std::cerr << "Error message : " << Pa_GetErrorText(this->_err) << std::endl;
	}
}

void		AudioManager::initInput()
{
    if ((this->_inputParam.device = Pa_GetDefaultInputDevice()) == paNoDevice)
    {
//        std::cout << "ZIZI" << std::endl;
        this->errorAudio();
        return ;
    }
	this->_inputParam.channelCount = 1;
	this->_inputParam.sampleFormat = PA_SAMPLE_TYPE;
	this->_inputParam.suggestedLatency = Pa_GetDeviceInfo(this->_inputParam.device)->defaultHighInputLatency;
	this->_inputParam.hostApiSpecificStreamInfo = NULL;
//	std::cout << "Input device # " << this->_inputParam.device << std::endl;
//	std::cout << "Input LowLatency : " << Pa_GetDeviceInfo(this->_inputParam.device)->defaultLowInputLatency << std::endl;
//	std::cout << "Input HighLatency : " << Pa_GetDeviceInfo(this->_inputParam.device)->defaultHighInputLatency << std::endl;
}

void		AudioManager::initOutput()
{
    if ((this->_outputParam.device = Pa_GetDefaultOutputDevice()) == paNoDevice)
    {
        this->errorAudio();
        return ;
    }
	this->_outputParam.channelCount = 1;
	this->_outputParam.sampleFormat = PA_SAMPLE_TYPE;
	this->_outputParam.suggestedLatency = Pa_GetDeviceInfo(this->_outputParam.device)->defaultHighOutputLatency;
	this->_outputParam.hostApiSpecificStreamInfo = NULL;
//	std::cout << "Output device # " << this->_outputParam.device << std::endl;
//	std::cout << "Output LowLatency : " << Pa_GetDeviceInfo(this->_outputParam.device)->defaultLowOutputLatency << std::endl;
//	std::cout << "Output HighLatency : " << Pa_GetDeviceInfo(this->_outputParam.device)->defaultHighOutputLatency << std::endl;
}

int			recordCallback(	const void *input, void *output,
                            unsigned long,
							const PaStreamCallbackTimeInfo* timeInfo,
							PaStreamCallbackFlags statusFlags,
							void *userData)
{
	AudioManager *dis = (AudioManager*)userData;
	const SAMPLE *in = (const SAMPLE *)input;
	int retenc(0);
	(void) timeInfo;
	(void) statusFlags;
	(void) output;

    //OLD
//        dis->setDataRec(dis->getEnc()->encodeAudio(in, &retenc));
//    dis->setRetencRec(retenc);

    dis->getPool().tryLock(REC_MUTEX);
    //dis->getBufferRec().write(dis->getEnc()->encodeAudio(in, &retenc), retenc);
    unsigned char *tmp = dis->getEnc()->encodeAudio(in, &retenc);

    if (dis->getSizeRec() + retenc < 255*255)
    {
      LibC::memcpy(dis->getStaticBufferRec() + dis->getSizeRec(), tmp, retenc);
         dis->setSizeRec(dis->getSizeRec() + retenc);
    }
    dis->getPool().unLock(REC_MUTEX);

	return paContinue;
}

int			playCallback(	const void *input, void *output,
                            unsigned long,
							const PaStreamCallbackTimeInfo* timeInfo,
							PaStreamCallbackFlags statusFlags,
							void *userData)
{
	AudioManager *dis = (AudioManager*)userData;
	SAMPLE	*out = (SAMPLE *)output;
	(void) timeInfo;
	(void) statusFlags;
	(void) input;
    int tmp;

//NEW WAY
    dis->getPool().tryLock(PLAY_MUTEX);
    tmp = dis->getSizePlay();
    dis->setSizePlay(0);
    dis->getPool().unLock(PLAY_MUTEX);

    if (tmp)
        dis->getEnc()->decodeAudio(dis->getStaticBufferPlay(), out, tmp);


    //OLD
    //dis->getEnc()->decodeAudio(dis->getDataPlay(), out, dis->getRetencPlay());
	return paContinue;
}

int			AudioManager::setupStream()
{
	this->_err = Pa_OpenStream(
					&this->_streamin,
					&this->_inputParam,
					NULL,
                    AUDIOSAMPLE_RATE,
					FRAMES_PER_BUFFER,
					paClipOff,
					recordCallback,
					this);
	if (this->_err != paNoError)
	{
		this->errorAudio();
		return (0);
	}
	this->_err = Pa_OpenStream(
					&this->_streamout,
					NULL,
					&this->_outputParam,
                    AUDIOSAMPLE_RATE,
					FRAMES_PER_BUFFER,
					paClipOff,
					playCallback,
					this);
	if (this->_err != paNoError)
	{
		this->errorAudio();
		return (0);
	}
	return (1);
}

int	AudioManager::start()
{
	this->_err = Pa_StartStream(this->_streamin);
	if (this->_err != paNoError)
	{
		this->errorAudio();
        this->_started = false;
		return (0);
	}
	this->_err = Pa_StartStream(this->_streamout);
	if (this->_err != paNoError)
	{
		this->errorAudio();
        this->_started = false;
		return (0);
	}
    this->_started = true;
	return (1);
}

bool        AudioManager::isStarted() const
{
    return (this->_started);
}

int			AudioManager::stop()
{
    this->_sizeRec = 0;
    this->_sizePlay = 0;
	if (this->_streamin)
	{
		this->_err = Pa_StopStream(this->_streamin);
		if (this->_err != paNoError)
		{
			this->errorAudio();
            this->_started = false;
			return (0);
		}
	}
	if (this->_streamout)
	{
		this->_err = Pa_StopStream(this->_streamout);
		if (this->_err != paNoError)
		{
			this->errorAudio();
            this->_started = false;
			return (0);
		}
	}
    this->_started = false;
    return (1);
}

AbsEncode	*AudioManager::getEnc()
{
	return (this->_enc);
}

const std::pair<const unsigned char *, const int>	AudioManager::getData() const
{
    return (std::make_pair(this->_datarec, this->_retencrec));
}

unsigned char * AudioManager::getDataRec() const
{
    return (this->_datarec);
}

void		AudioManager::setDataRec(unsigned char *data)
{
    this->_datarec = data;
}

int			AudioManager::getRetencRec() const
{
    return (this->_retencrec);
}

void		AudioManager::setRetencRec(int retenc)
{
    this->_retencrec = retenc;
}

unsigned char * AudioManager::getDataPlay() const
{
    return (this->_dataplay);
}
void		AudioManager::setDataPlay(unsigned char *data)
{
    this->_dataplay = data;
}

int			AudioManager::getRetencPlay() const
{    
    return (this->_retencplay);
}

void		AudioManager::setRetencPlay(int retenc)
{
    std::cout << "GOT" << retenc << std::endl;

    this->_retencplay = retenc;
}

CircularBuffer & AudioManager::getBufferRec()
{
    return _bufferRec;
}

void AudioManager::setBufferRec(const CircularBuffer &bufferRec)
{
    _bufferRec = bufferRec;
}

int AudioManager::getSizeRec() const
{
    return _sizeRec;
}

void AudioManager::setSizeRec(int sizeRec)
{
    _sizeRec = sizeRec;
}

CircularBuffer & AudioManager::getBufferPlay()
{
    return _bufferPlay;
}

void AudioManager::setBufferPlay(const CircularBuffer &bufferPlay)
{
    _bufferPlay = bufferPlay;
}

int AudioManager::getSizePlay() const
{
    return _sizePlay;
}

void AudioManager::setSizePlay(int sizePlay)
{
    _sizePlay = sizePlay;
}

unsigned char *AudioManager::getStaticBufferRec() const
{
    return (unsigned char *)_staticBufferRec;
}

unsigned char *AudioManager::getStaticBufferPlay() const
{
    return (unsigned char *)_staticBufferPlay;
}


const std::pair<const unsigned char *, const int>   AudioManager::getRecData()
{
    int tmp;

    this->_pool.tryLock(REC_MUTEX);
    tmp = this->_sizeRec;
    LibC::memcpy(this->_staticBufferRecRet, this->_staticBufferRec, this->_sizeRec);
    this->_sizeRec = 0;
    this->_pool.unLock(REC_MUTEX);

    return (std::pair<const unsigned char *, const int>(this->_staticBufferRecRet, tmp));
}

void AudioManager::setPlayData(unsigned char *data, int size)
{
    this->_pool.tryLock(PLAY_MUTEX);
    if (this->_sizePlay + size < 255*255)
    {
      LibC::memcpy(this->_staticBufferPlay + this->_sizePlay, data, size);
         this->_sizePlay += size;
    }
    this->_pool.unLock(PLAY_MUTEX);
}
