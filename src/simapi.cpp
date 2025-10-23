#include "simapi.h"

std::ofstream simapi::mLog;

simapi::simapi(const char* mv_id) :mainVehicleId(mv_id)
{
	mLog.open("log.txt", std::ios::out | std::ios::trunc);
}

simapi::~simapi()
{
	mLog.close();
}
