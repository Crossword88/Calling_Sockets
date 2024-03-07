#pragma once

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib,"winmm.lib") 
#include <winsock2.h>
#include <WS2tcpip.h>

#include <windows.h>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <wininet.h>
#pragma comment(lib, "wininet.lib")

using namespace cv;

const int sampleRateF = 44100;	//for sounds
const int NUMPTS = sampleRateF * 2 * (1);	//last one - seconds

struct data
{
	std::vector<uchar> videoBuffer;
	std::vector<short int> soundRecord;	//all sounds

	std::vector<char> serialize() const {
		std::vector<char> result(videoBuffer.size() + NUMPTS);
		std::memcpy(result.data(), videoBuffer.data(), videoBuffer.size());
		std::memcpy(result.data() + videoBuffer.size(), soundRecord.data(), NUMPTS);	//looks right
		return result;
	}

	static data deserialize(const std::vector<char>& bytes) {
		data result;
		result.videoBuffer.resize(bytes.size() - NUMPTS);
		std::memcpy(result.videoBuffer.data(), bytes.data(), result.videoBuffer.size());
		result.soundRecord.resize(NUMPTS);
		std::memcpy(result.soundRecord.data(), bytes.data() + result.videoBuffer.size(), NUMPTS);
		return result;	//idk
	}
};