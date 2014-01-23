#include "include/audio/EncodeManager.hpp"

EncodeManager::EncodeManager()
{
    this->_enc = opus_encoder_create(ENCODESAMPLE_RATE, 1, OPUS_APPLICATION_VOIP, &this->_err);
    this->_dec = opus_decoder_create(ENCODESAMPLE_RATE, 1, &this->_err);
	opus_encoder_ctl(this->_enc, OPUS_GET_BANDWIDTH(&this->_len));
    this->_compressed = new char[this->_len];
}

EncodeManager::~EncodeManager()
{
    delete this->_compressed;
	opus_encoder_destroy(this->_enc);
	opus_decoder_destroy(this->_dec);
}

unsigned char		*EncodeManager::encodeAudio(const SAMPLE *frame, int *retenc)
{
	if (this->_len)
	{
       *retenc = opus_encode_float(this->_enc, frame, FRAMES_PER_BUFFER, (unsigned char*)this->_compressed, this->_len);
        return ((unsigned char*)this->_compressed);
	}
    return (NULL);
}

void		EncodeManager::decodeAudio(const unsigned char *data, SAMPLE *out, int retenc)
{
	int		ret;

	ret = opus_decode_float(this->_dec, data, retenc, out, FRAMES_PER_BUFFER, 0);
    (void)ret;
}
