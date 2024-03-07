#include "Send.h"

Send::Send()
{
	Sending.soundRecord.resize(NUMPTS);
}

Send::~Send()
{
	WSACleanup();
	closesocket(sockConnect);
	waveInClose(hAudioInput);
}

bool Send::wsaLibraryStartUp()
{
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 2);

	if (WSAStartup(DllVersion, &wsaData) != 0)
	{
		std::cout << "ERROR LIB\n";
		return 0;
	}

	return 1;
}

bool Send::socketConnection()
{
	if (!wsaLibraryStartUp()) return 0;

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL); //creating a listening socket
	if (sListen == INVALID_SOCKET)
	{
		ErrorEnd(WSAGetLastError(), sListen);
		return 0;
	}

	SOCKADDR_IN SockAddr;	// info/address of server
	SOCKADDR_IN clientInfo;	 // info\address of the connected client
	in_addr addr;
	if (inet_pton(AF_INET, "127.0.0.1", &addr) <= 0)
	{
		std::cout << "Error Addr\n";
		return 0;
	}

	SockAddr.sin_addr = addr;
	SockAddr.sin_port = htons(1111);
	SockAddr.sin_family = AF_INET;
	int sizeofAddr = sizeof(SockAddr);
	int clientInfo_size = sizeof(clientInfo);

	if (bind(sListen, (SOCKADDR*)&SockAddr, sizeof(SockAddr)) != 0)	//Connecting address with a socket 
	{
		ErrorEnd(WSAGetLastError(), sListen);
		return 0;
	}

	if (listen(sListen, SOMAXCONN) != 0)	//listening
	{
		ErrorEnd(WSAGetLastError(), sListen);
		return 0;
	}

	std::cout << "Waiting Connection...\n";
	sockConnect = accept(sListen, (SOCKADDR*)&clientInfo, &sizeofAddr);

	if (sockConnect == 0)
	{
		std::cout << "ERROR Connection to Socket\n";
		return false;
	}
	else
	{
		std::cout << "Connected to Socket\n";
		return true;
	}
}

bool Send::VideoRecording(VideoCapture webcam)
{
	webcam.read(videoPicture); // Reading of the picture from webcam

	if (videoPicture.empty()) {
		std::cout << "\n\n\n\n\nError read pictue" << std::endl;
		return 0;
	}

	return 1;
}

bool Send::SoundRecording()
{
	waveInPrepareHeader(hAudioInput, &WaveInHdr, sizeof(WAVEHDR));
	waveInAddBuffer(hAudioInput, &WaveInHdr, sizeof(WAVEHDR));
	waveInStart(hAudioInput);
	waveInUnprepareHeader(hAudioInput, &WaveInHdr, sizeof(WAVEHDR));

	if (WaveInHdr.lpData == 0)
	{
		std::cout << "Error to get a sound\n";
		return 0;
	}
	else
		return 1;
}

bool Send::ToSend()
{
	int bufferSize = 0;
	int sendBytes = 0;

	cv::imencode(".jpg", videoPicture, Sending.videoBuffer);

	std::vector<char> serializeBuffer = Sending.serialize();
	bufferSize = serializeBuffer.size();

	sendBytes = send(sockConnect, reinterpret_cast<const char*>(&bufferSize), sizeof(int), 0);	//send size

	if (sendBytes != sizeof(int))
	{
		std::cout << "Error to send buffer size\n";
		return 0;
	}

	sendBytes = send(sockConnect, serializeBuffer.data(), bufferSize, 0);		//send buffer

	if (sendBytes != bufferSize)
	{
		std::cout << "Error to send buffer\n";
		return 0;
	}
	else
	{
		return 1;
	}
}

void Send::ErrorEnd(int error, SOCKET& sock)
{
	std::cout << "Error socket: " << error << std::endl;
	closesocket(sock);
	closesocket(sockConnect);
	WSACleanup();
	exit(0);
}

void Send::OpenAudioDevice()
{
	SoundFormat.wFormatTag = WAVE_FORMAT_PCM;     // simple, uncompressed format
	SoundFormat.nChannels = STEREO;               //  1=mono, 2=stereo
	SoundFormat.wBitsPerSample = 16;              //  16 for high quality, 8 for telephone-grade
	SoundFormat.nSamplesPerSec = sampleRateF;
	SoundFormat.nAvgBytesPerSec = sampleRateF * SoundFormat.nChannels * SoundFormat.wBitsPerSample / 8;
	SoundFormat.nBlockAlign = SoundFormat.nChannels * SoundFormat.wBitsPerSample / 8;
	SoundFormat.cbSize = 0;

	waveInOpen(&hAudioInput, WAVE_MAPPER, &SoundFormat, 0, 0, WAVE_FORMAT_DIRECT);

	WaveInHdr.lpData = (LPSTR)Sending.soundRecord.data();
	WaveInHdr.dwBufferLength = NUMPTS * 2;
	WaveInHdr.dwBytesRecorded = 0;
	WaveInHdr.dwUser = 0L;
	WaveInHdr.dwFlags = 0L;
	WaveInHdr.dwLoops = 0L;
}
