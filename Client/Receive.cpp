#include "Receive.h"

Receive::Receive()
{
	Getting.record.resize(NUMPTS);
}

bool Receive::Connection()
{
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 2);

	if (WSAStartup(DllVersion, &wsaData) != 0)
	{
		std::cout << "ERROR LIB";
	}

	in_addr addr;
	if (inet_pton(AF_INET, "127.0.0.1", &addr) <= 0)
	{
		return 0;
	}

	SockAddr.sin_addr = addr; //адрес, который должен подключится
	SockAddr.sin_port = htons(1111);
	SockAddr.sin_family = AF_INET;

	ServConnection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(ServConnection, (SOCKADDR*)&SockAddr, sizeof(SockAddr)) != 0)
	{
		MessageBox(NULL, L"ERROR connect to server", L"Ok", MB_ICONERROR);
		return false;
	}
	else
	{
		return true;
	}
}

void Receive::ToReceive()
{
	recv(ServConnection, reinterpret_cast<char*>(&bufferSize), sizeof(int), 0);

	bufferBytes.resize(bufferSize);

	int totalBytesReceived = 0;

	while (totalBytesReceived < bufferSize) {
		int bytesReceived = recv(ServConnection, bufferBytes.data() + totalBytesReceived, bufferSize - totalBytesReceived, 0);

		if (bytesReceived <= 0) {
			// Обработка ошибки или закрытия соединения
			MessageBox(NULL, L"Error to data receive", L"Ok", MB_ICONERROR);
			return;
		}
		totalBytesReceived += bytesReceived;
	}

	Getting = Getting.deserialize(bufferBytes);

	bufferSize = 0;
}

void Receive::VideoPlaying()
{
	receivedImage = cv::imdecode(Getting.buffer, cv::IMREAD_COLOR);
	flip(receivedImage, receivedImage, 1);
	imshow("Client", receivedImage);
}

void Receive::SoundPlaying()
{
	std::copy(Getting.record.begin(), Getting.record.end(), waveOut);

	WaveOutHdr.lpData = (LPSTR)waveOut;

	waveOutPrepareHeader(hWaveOut, &WaveOutHdr, sizeof(WAVEHDR));
	waveOutWrite(hWaveOut, &WaveOutHdr, sizeof(WAVEHDR));
	waveOutUnprepareHeader(hWaveOut, &WaveOutHdr, sizeof(WAVEHDR));

}

void Receive::Clear()
{
	WSACleanup();
	closesocket(ServConnection);
	waveOutClose(hWaveOut);
	exit(0);
}

void Receive::OpenAudioDevice()
{
	wf.wFormatTag = WAVE_FORMAT_PCM;     // simple, uncompressed format
	wf.nChannels = 2;                    //  1=mono, 2=stereo
	wf.wBitsPerSample = 16;              //  16 for high quality, 8 for telephone-grade
	wf.nSamplesPerSec = sampleRateF;
	wf.nAvgBytesPerSec = sampleRateF * wf.nChannels * wf.wBitsPerSample / 8;
	wf.nBlockAlign = wf.nChannels * wf.wBitsPerSample / 8;
	wf.cbSize = 0;

	waveOutOpen(&hWaveOut, WAVE_MAPPER, &wf, 0L, 0L, WAVE_FORMAT_DIRECT);

	WaveOutHdr.dwBufferLength = NUMPTS * 2;
	WaveOutHdr.dwBytesRecorded = 0;
	WaveOutHdr.dwUser = 0L;
	WaveOutHdr.dwFlags = 0L;
	WaveOutHdr.dwLoops = 1L;

	waveOutOpen(&hWaveOut, WAVE_MAPPER, &wf, 0L, 0L, WAVE_FORMAT_DIRECT);
}
