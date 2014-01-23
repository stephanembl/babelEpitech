#include "EncodeManager.hpp"

EncodeManager::EncodeManager()
{
	this->_enc = opus_encoder_create(SAMPLE_RATE, 1, OPUS_APPLICATION_VOIP, &this->_err);
	this->_dec = opus_decoder_create(SAMPLE_RATE, 1, &this->_err);
	opus_encoder_ctl(this->_enc, OPUS_GET_BANDWIDTH(&this->_len));
}

EncodeManager::~EncodeManager()
{
	opus_encoder_destroy(this->_enc);
	opus_decoder_destroy(this->_dec);
}

unsigned char		*EncodeManager::encodeAudio(const SAMPLE *frame, int *retenc)
{
	unsigned char	*compressed = new (unsigned char[this->_len]);

	*retenc = opus_encode_float(this->_enc, frame, FRAMES_PER_BUFFER, compressed, this->_len);
	return (compressed);
}

void		EncodeManager::decodeAudio(const unsigned char *data, SAMPLE *out, int retenc)
{
	int		ret;

	ret = opus_decode_float(this->_dec, data, retenc, out, FRAMES_PER_BUFFER, 0);
}
