#ifndef SIMAPI_H
#define SIMAPI_H

#include <thread>
#include <chrono>
#include <iostream>
#include <fstream>
#include <ctime>
#include <string.h>
#include <condition_variable>
#include <mutex>

class simapi
{
public:
	simapi(const char* mv_id);
	~simapi();

private:
	std::string mainVehicleId;
	static std::ofstream mLog;
};

#endif
