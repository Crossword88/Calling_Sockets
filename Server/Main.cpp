#include "Send.h"

bool checkInternetConnection()
{
	if (InternetCheckConnectionW(L"http://www.microsoft.com", FLAG_ICC_FORCE_CONNECTION, 0) == FALSE)
	{
		std::cout << "No internet connection\n";
		return 0;
	}
	else
	{
		return 1;
	}
}

bool openWebcam(VideoCapture webcam)
{
	if (!webcam.isOpened()) {
		std::cout << "\n\n\n\nError open webcam" << std::endl;
		return 0;
	}
	else
	{
		return 1;
	}
}

int main()
{
	if (!checkInternetConnection())	return -1;

	Send sender;
	if (!sender.socketConnection()) return -1;

	VideoCapture webcam(0);	//Opening webcam with index 0 (first available webcam)
	if (!openWebcam(webcam)) return -1;

	sender.OpenAudioDevice();

	system("cls");

	std::cout << "client Connected\n";

	while (true)
	{
		if (!sender.VideoRecording(webcam)) break;
		
		if(!sender.SoundRecording()) break;

		if (!sender.ToSend()) break;
	}

	webcam.release();
	return 0;
}
