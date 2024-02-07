#pragma once

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib,"winmm.lib") 
#include <winsock2.h>
#include <WS2tcpip.h>
#include <iostream>

#include <windows.h>
#include <opencv2/opencv.hpp>
#include <stdio.h>

using namespace cv;

const int sampleRateF = 44100;	//for sounds
const int NUMPTS = sampleRateF * 2 * (1);
//short int* waveOut = new short int[NUMPTS];

struct data
{
	std::vector<uchar> buffer;
	std::vector<short int> record;

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