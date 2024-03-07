#include "Receive.h"

Receive::Receive()
{
	Getting.record.resize(NUMPTS);
}

Receive::~Receive()
{
	WSACleanup();
	closesocket(ServConnection);
	waveOutClose(hWaveOut);
}

bool Receive::wsaLibraryStartUp()
{
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 2);

	if (WSAStartup(DllVersion, &wsaData) != 0)
	{
		std::cout << "ERROR LIB";
		return 0;
	}

	return 1;
}

bool Receive::Connection()
{
	if (!wsaLibraryStartUp()) return 0;

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

bool Receive::ToReceive()
{
	int totalBytesReceived = 0;
	int bufferSize = 0;

	totalBytesReceived = recv(ServConnection, reinterpret_cast<char*>(&bufferSize), sizeof(int), 0);

	if (totalBytesReceived != sizeof(int)) return 0;
	else totalBytesReceived = 0;

	bufferBytes.resize(bufferSize);

	while (totalBytesReceived < bufferSize) {
		int bytesReceived = recv(ServConnection, bufferBytes.data() + totalBytesReceived, bufferSize - totalBytesReceived, 0);

		if (bytesReceived <= 0) {
			// Handling errors or closing connections
			MessageBox(NULL, L"Error to data receive", L"Ok", MB_ICONERROR);
			return false;
		}
		totalBytesReceived += bytesReceived;
	}

	if (totalBytesReceived != bufferSize) return 0;

	Getting = Getting.deserialize(bufferBytes);

	return true;
}

bool Receive::VideoPlaying()
{
	receivedImage = cv::imdecode(Getting.buffer, cv::IMREAD_COLOR);
	if (receivedImage.empty())
	{
		std::cout << "Error: Image is empty.\n";
		return 0;
	}

	flip(receivedImage, receivedImage, 1);
	imshow("Client", receivedImage);

	return 1;
}

bool Receive::SoundPlaying()
{
	std::copy(Getting.record.begin(), Getting.record.end(), waveOut);

	if (waveOut == 0)
	{
		std::cout << "Error: Sound Buffer is empty.\n";
		return 0;
	}

	WaveOutHdr.lpData = (LPSTR)waveOut;

	waveOutPrepareHeader(hWaveOut, &WaveOutHdr, sizeof(WAVEHDR));
	waveOutWrite(hWaveOut, &WaveOutHdr, sizeof(WAVEHDR));
	waveOutUnprepareHeader(hWaveOut, &WaveOutHdr, sizeof(WAVEHDR));

	return 1;
}

void Receive::OpenAudioDevice()
{
	soundFormat.wFormatTag = WAVE_FORMAT_PCM;     // simple, uncompressed format
	soundFormat.nChannels = STEREO;                    //  1=mono, 2=stereo
	soundFormat.wBitsPerSample = 16;              //  16 for high quality, 8 for telephone-grade
	soundFormat.nSamplesPerSec = sampleRateF;
	soundFormat.nAvgBytesPerSec = sampleRateF * soundFormat.nChannels * soundFormat.wBitsPerSample / 8;
	soundFormat.nBlockAlign = soundFormat.nChannels * soundFormat.wBitsPerSample / 8;
	soundFormat.cbSize = 0;

	waveOutOpen(&hWaveOut, WAVE_MAPPER, &soundFormat, 0L, 0L, WAVE_FORMAT_DIRECT);

	WaveOutHdr.dwBufferLength = NUMPTS * 2;
	WaveOutHdr.dwBytesRecorded = 0;
	WaveOutHdr.dwUser = 0L;
	WaveOutHdr.dwFlags = 0L;
	WaveOutHdr.dwLoops = 1L;
}
