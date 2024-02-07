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

		if (waitKey(1) == 27) // �������� ������� ������� ESC ��� ������ �� �����
			break;
	}

	Taking.Clear();
}