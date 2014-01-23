#include "AudioManager.hpp"

int	main(int ac, char **av)
{
	AbsAudio	*a = new AudioManager;

	a->initAudio();
	a->start();
	a->stop();
	delete a;
	return (0);
}