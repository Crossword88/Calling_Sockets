#include "Receive.h"

Receive Taking;

int main()
{
	if (!Taking.Connection()) exit(0);

	std::cout << "Connected to server\n";

	Taking.OpenAudioDevice();

	while(true)
	{
		if (!Taking.ToReceive()) break;

		if(!Taking.VideoPlaying()) break;
		if(!Taking.SoundPlaying()) break;

		if (waitKey(1) == 27) // ќжидание нажати€ клавиши ESC дл€ выхода из цикла
			break;
	}
}