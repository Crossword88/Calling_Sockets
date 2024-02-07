#pragma once

#include "ClientIncluding.h"

class Receive
{
	data Getting;

	SOCKADDR_IN SockAddr;	// инфа\адрес сервера
	SOCKET ServConnection = 0;

	Mat receivedImage;	//picture
	int bufferSize = 0;
	WAVEFORMATEX wf = { 0 };	//sound
	WAVEHDR WaveOutHdr;
	HWAVEOUT hWaveOut;
	std::vector<char> bufferBytes;

	short int waveOut[NUMPTS];

public:
	Receive();
	bool Connection();
	void ToReceive();
	void VideoPlaying();
	void SoundPlaying();
	void Clear();
	void OpenAudioDevice();
};

