#pragma once

#include <pthread.h>
#include <list>

//*****************************************************************************
class CRainSensor
{
public:
	CRainSensor(void);
	virtual ~CRainSensor(void);

	float GetRainHour(void);
	float GetRainDay(void);

protected:
	static pthread_mutex_t s_mutex;
	static float s_fRainDay;
	static std::list<unsigned int> s_lstTipTimes;

	static void BucketCallback(void);
};
