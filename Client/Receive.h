#pragma once

#include "ClientIncluding.h"

#define MONO 1
#define STEREO 2

class Receive
{
	data Getting;

	SOCKADDR_IN SockAddr;	// инфа\адрес сервера
	SOCKET ServConnection = 0;

	Mat receivedImage;

	WAVEFORMATEX soundFormat = { 0 };
	WAVEHDR WaveOutHdr;
	HWAVEOUT hWaveOut;

	std::vector<char> bufferBytes;

	short int waveOut[NUMPTS];

public:
	Receive();
	~Receive();
	bool wsaLibraryStartUp();
	bool Connection();
	bool ToReceive();
	bool VideoPlaying();
	bool SoundPlaying();
	void OpenAudioDevice();
};

