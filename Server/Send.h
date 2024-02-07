#pragma once

#include "ServerIncluding.h"

class Send
{
	data Sending;
	
	SOCKET newConnection = 0;

	Mat frame;	//picture
	int bufferSize = 0;
	WAVEFORMATEX wf = { 0 };	//sound
	HWAVEIN hwi = 0;
	WAVEHDR WaveInHdr = { 0 };
	int bytes = 0;	//bytes sended

public:
	Send();
	bool Connection();
	void VideoRecording(VideoCapture capture);
	void SoundRecording();
	void ToSend();
	void ErrorEnd(int error, SOCKET& sock);
	void Clear();
	void OpenAudioDevice();

};

