#include "ServerIncluding.h"

using namespace cv;

const int sampleRateF = 44100;	//for sounds
const int NUMPTS = sampleRateF * 2 * (1);	//last one - seconds
short int* waveIn = new short int[NUMPTS];

void ErrorEnd(int error, SOCKET& sock)
{
	std::cout << "Error socket: " << error << std::endl;
	closesocket(sock);
	WSACleanup();
	exit(0);
}

struct data
{
	std::vector<uchar> buffer;
	std::vector<short int> record;	//all sounds

	std::vector<char> serialize() const {
		std::vector<char> result(buffer.size() + NUMPTS);
		std::memcpy(result.data(), buffer.data(), buffer.size());
		std::memcpy(result.data() + buffer.size(), record.data(), NUMPTS);	//looks right
		return result;
	}

	static data deserialize(const std::vector<char>& bytes) {
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
	if (InternetCheckConnectionW(L"http://www.microsoft.com", FLAG_ICC_FORCE_CONNECTION, 0) == FALSE)
	{
		std::cout << "No connect\n";
		return 0;
	}

	data Sending;
	Sending.record.resize(NUMPTS);


	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 2);

	if (WSAStartup(DllVersion, &wsaData) != 0)
	{
		std::cout << "ERROR LIB";
	}

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL); //creating a listening socket
	if (sListen == INVALID_SOCKET)
	{
		ErrorEnd(WSAGetLastError(), sListen);
	}

	SOCKADDR_IN SockAddr;	// info/address of server
	SOCKADDR_IN clientInfo;	 // info\address of the connected client
	in_addr addr;
	if (inet_pton(AF_INET, "127.0.0.1", &addr) <= 0)
	{
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
	}

	if (listen(sListen, SOMAXCONN) != 0)	//listening
	{
		ErrorEnd(WSAGetLastError(), sListen);
	}

	SOCKET newConnection = 0;
	std::cout << "Waiting Connection...\n";
	newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeofAddr);

	if (newConnection == 0)
	{
		std::cout << "ERROR Connection";
	}
	else
	{
		system("cls");
		std::cout << "Connected successfully\n";
		VideoCapture capture(0); //Opening webcam with index 0 (first available webcam)
		if (!capture.isOpened()) {
			std::cout << "\n\n\n\nError open webcam" << std::endl;
			return -1;
		}

		Mat frame;
		int bufferSize;

		WAVEFORMATEX wf;
		wf.wFormatTag = WAVE_FORMAT_PCM;     // simple, uncompressed format
		wf.nChannels = 2;                    //  1=mono, 2=stereo
		wf.wBitsPerSample = 16;              //  16 for high quality, 8 for telephone-grade
		wf.nSamplesPerSec = sampleRateF;
		wf.nAvgBytesPerSec = sampleRateF * wf.nChannels * wf.wBitsPerSample / 8;
		wf.nBlockAlign = wf.nChannels * wf.wBitsPerSample / 8;
		wf.cbSize = 0;

		HWAVEIN hwi = { 0 };

		WAVEHDR WaveInHdr = { 0 };

		waveInOpen(&hwi, WAVE_MAPPER, &wf, 0, 0, WAVE_FORMAT_DIRECT);

		WaveInHdr.lpData = (LPSTR)Sending.record.data();
		WaveInHdr.dwBufferLength = NUMPTS * 2;
		WaveInHdr.dwBytesRecorded = 0;
		WaveInHdr.dwUser = 0L;
		WaveInHdr.dwFlags = 0L;
		WaveInHdr.dwLoops = 0L;

		while (true)
		{
			int bytes = 0;
			waveInPrepareHeader(hwi, &WaveInHdr, sizeof(WAVEHDR));
			waveInAddBuffer(hwi, &WaveInHdr, sizeof(WAVEHDR));
			waveInStart(hwi);

			capture.read(frame); // Reading of the picture from webcam

			if (frame.empty()) {
				std::cout << "\n\n\n\n\nError read pictue" << std::endl;
				break;
			}

			if (waitKey(1) == 27) // Waiting esc key
				break;

			cv::imencode(".png", frame, Sending.buffer);

			std::vector<char> serializeBuffer = Sending.serialize();
			bufferSize = serializeBuffer.size();

			std::cout << bufferSize << std::endl;

			send(newConnection, reinterpret_cast<const char*>(&bufferSize), sizeof(int), 0);	//send size

			std::cout << "HERE IS ALL: \n" << "\nbufferSize: " << Sending.buffer.size() << "\nrecordSize: " << Sending.record.size() << "\nSerializeSize: " << serializeBuffer.size() << "\n";

			bytes = send(newConnection, serializeBuffer.data(), bufferSize, 0);		//send buffer

			std::cout << "All bytes sended: " << bytes << "\n";

		}
		waveInClose(hwi);
		capture.release(); // Closing of Webcam
		closesocket(newConnection);
		closesocket(sListen);
		WSACleanup();
	}
	return 0;
}