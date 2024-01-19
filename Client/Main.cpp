#include "ClientIncluding.h"

using namespace cv;

const int sampleRateF = 44100;	//for sounds
const int NUMPTS = sampleRateF * 2 * (1);
short int* waveOut = new short int[NUMPTS];

struct data
{
	std::vector<uchar> buffer;
	std::vector<short int> record;

	std::vector<char> serialize() const {
		std::vector<char> result( buffer.size() + NUMPTS);
		std::memcpy(result.data(), buffer.data(), buffer.size());
		std::memcpy(result.data() + buffer.size(), record.data(), NUMPTS);	//looks right
		return result;
	}

	static data deserialize(const std::vector<char>&bytes) {
	data result;
	result.buffer.resize(bytes.size() - NUMPTS);
	std::memcpy(result.buffer.data(), bytes.data(), result.buffer.size());
	result.record.resize(NUMPTS);
	std::memcpy(result.record.data(), bytes.data() + result.buffer.size(), NUMPTS);
	return result;	//idk
	}
};

int main()
{
	short int* waveOut = new short int[NUMPTS];
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 2);

	if (WSAStartup(DllVersion, &wsaData) != 0)
	{
		std::cout << "ERROR LIB";
	}

	SOCKADDR_IN SockAddr;	// инфа\адрес сервера
	in_addr addr;
	if (inet_pton(AF_INET, "127.0.0.1", &addr) <= 0)
	{
		return 0;
	}

	SockAddr.sin_addr = addr; //адрес, который должен подключится
	SockAddr.sin_port = htons(1111);
	SockAddr.sin_family = AF_INET;

	SOCKET Connection = socket(AF_INET, SOCK_STREAM, NULL);

	if (connect(Connection, (SOCKADDR*)&SockAddr, sizeof(SockAddr)) != 0)
	{
		std::cout << "ERROR connect to server" << std::endl;
	}
	else
	{
		WAVEFORMATEX wf;
		wf.wFormatTag = WAVE_FORMAT_PCM;     // simple, uncompressed format
		wf.nChannels = 2;                    //  1=mono, 2=stereo
		wf.wBitsPerSample = 16;              //  16 for high quality, 8 for telephone-grade
		wf.nSamplesPerSec = sampleRateF;
		wf.nAvgBytesPerSec = sampleRateF * wf.nChannels * wf.wBitsPerSample / 8;
		wf.nBlockAlign = wf.nChannels * wf.wBitsPerSample / 8;
		wf.cbSize = 0;

		HWAVEOUT hWaveOut;
		WAVEHDR WaveOutHdr;

		std::cout << "CONNECTED" << std::endl;

		int bufferSize = 0;

		int bytesRecv = 0;

		waveOutOpen(&hWaveOut, WAVE_MAPPER, &wf, 0L, 0L, WAVE_FORMAT_DIRECT);

		WaveOutHdr.dwBufferLength = NUMPTS * 2;
		WaveOutHdr.dwBytesRecorded = 0;
		WaveOutHdr.dwUser = 0L;
		WaveOutHdr.dwFlags = 0L;
		WaveOutHdr.dwLoops = 1L;

		while (true)
		{
			bytesRecv = recv(Connection, reinterpret_cast<char*>(&bufferSize), sizeof(int), 0);

			std::cout << "SIZE: " << bufferSize << " bits: " << bytesRecv << std::endl;

			std::vector<char> bufferBytes(bufferSize);

			int totalBytesReceived = 0;

			while (totalBytesReceived < bufferSize) {
				int bytesReceived = recv(Connection, bufferBytes.data() + totalBytesReceived, bufferSize - totalBytesReceived, 0);

				if (bytesReceived <= 0) {
					// Обработка ошибки или закрытия соединения
					return 0;
					break;
				}

				totalBytesReceived += bytesReceived;
			}

			std::cout << "Data, bits: " << totalBytesReceived << std::endl;

			const data Getting = Getting.deserialize(bufferBytes);

			std::copy(Getting.record.begin(), Getting.record.end(), waveOut);

			WaveOutHdr.lpData = (LPSTR)waveOut;
			waveOutPrepareHeader(hWaveOut, &WaveOutHdr, sizeof(WAVEHDR));
			waveOutWrite(hWaveOut, &WaveOutHdr, sizeof(WAVEHDR));
			waveOutUnprepareHeader(hWaveOut, &WaveOutHdr, sizeof(WAVEHDR));

			Mat receivedImage = cv::imdecode(Getting.buffer, cv::IMREAD_COLOR);
			flip(receivedImage, receivedImage, 1);
			imshow("Client", receivedImage);


			if (waitKey(1) == 27) // Ожидание нажатия клавиши ESC для выхода из цикла
				break;
			bufferSize = 0;
		}
		waveOutClose(hWaveOut);

	}


	WSACleanup();
	closesocket(Connection);
	return 0;
}