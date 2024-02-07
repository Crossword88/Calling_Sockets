#include "Receive.h"

Receive Taking;

int main()
{
	if (!Taking.Connection()) exit(0);

	std::cout << "Connected\n";

	Taking.OpenAudioDevice();

	for (; true;)
	{
		Taking.ToReceive();

		Taking.VideoPlaying();
		Taking.SoundPlaying();

		if (waitKey(1) == 27) // ќжидание нажати€ клавиши ESC дл€ выхода из цикла
			break;
	}

	Taking.Clear();
}