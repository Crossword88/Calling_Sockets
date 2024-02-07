#include "Send.h"

Send::Send()
{
	std::cout << "YIP\n";
	Sending.record.resize(NUMPTS);
}

bool Send::Connection()
{
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 2);

	if (WSAStartup(DllVersion, &wsaData) != 0)
	{
		std::cout << "ERROR LIB\n";
		return 0;
	}

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

	newConnection = 0;
	std::cout << "Waiting Connection...\n";
	newConnection = accept(sListen, (SOCKADDR*)&clientInfo, &sizeofAddr);

	if (newConnection == 0)
	{
		std::cout << "ERROR Connection\n";
		return false;
	}
	else
	{
		std::cout << "Connected\n";
		return true;
	}
}

void Send::VideoRecording(VideoCapture capture)
{
	capture.read(frame); // Reading of the picture from webcam

	if (frame.empty()) {
		std::cout << "\n\n\n\n\nError read pictue" << std::endl;
		return;
	}

}

void Send::SoundRecording()
{
	waveInPrepareHeader(hwi, &WaveInHdr, sizeof(WAVEHDR));
	waveInAddBuffer(hwi, &WaveInHdr, sizeof(WAVEHDR));
	waveInStart(hwi);
	waveInUnprepareHeader(hwi, &WaveInHdr, sizeof(WAVEHDR));
}

void Send::ToSend()
{
	cv::imencode(".jpg", frame, Sending.buffer);

	std::vector<char> serializeBuffer = Sending.serialize();
	bufferSize = serializeBuffer.size();

	std::cout << bufferSize << std::endl;

	send(newConnection, reinterpret_cast<const char*>(&bufferSize), sizeof(int), 0);	//send size

	std::cout << "HERE IS ALL: \n" << "\nbufferSize: " << Sending.buffer.size() << "\nrecordSize: " << Sending.record.size() << "\nSerializeSize: " << serializeBuffer.size() << "\n";

	bytes = send(newConnection, serializeBuffer.data(), bufferSize, 0);		//send buffer

	std::cout << "All bytes sended: " << bytes << "\n";

	if (bytes != bufferSize) std::cout << "Error to send buffer\n";
}

void Send::ErrorEnd(int error, SOCKET& sock)
{
	std::cout << "Error socket: " << error << std::endl;
	closesocket(sock);
	WSACleanup();
	exit(0);
}

void Send::Clear()
{
	WSACleanup();
	closesocket(newConnection);
	waveInClose(hwi);
}

void Send::OpenAudioDevice()
{
	wf.wFormatTag = WAVE_FORMAT_PCM;     // simple, uncompressed format
	wf.nChannels = 2;                    //  1=mono, 2=stereo
	wf.wBitsPerSample = 16;              //  16 for high quality, 8 for telephone-grade
	wf.nSamplesPerSec = sampleRateF;
	wf.nAvgBytesPerSec = sampleRateF * wf.nChannels * wf.wBitsPerSample / 8;
	wf.nBlockAlign = wf.nChannels * wf.wBitsPerSample / 8;
	wf.cbSize = 0;

	waveInOpen(&hwi, WAVE_MAPPER, &wf, 0, 0, WAVE_FORMAT_DIRECT);

	WaveInHdr.lpData = (LPSTR)Sending.record.data();
	WaveInHdr.dwBufferLength = NUMPTS * 2;
	WaveInHdr.dwBytesRecorded = 0;
	WaveInHdr.dwUser = 0L;
	WaveInHdr.dwFlags = 0L;
	WaveInHdr.dwLoops = 0L;
}
