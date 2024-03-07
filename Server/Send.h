#pragma once

#include "ServerIncluding.h"

#define MONO 1
#define STEREO 2

class Send
{
	data Sending;
	
	SOCKET sockConnect = 0;

	Mat videoPicture;
	WAVEFORMATEX SoundFormat = { 0 };
	HWAVEIN hAudioInput = { 0 };
	WAVEHDR WaveInHdr = { 0 };

public:
	Send();
	~Send();
	bool wsaLibraryStartUp();
	bool socketConnection();
	bool VideoRecording(VideoCapture webcam);
	bool SoundRecording();
	bool ToSend();
	void ErrorEnd(int error, SOCKET& sock);
	void OpenAudioDevice();
};

