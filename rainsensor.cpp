#include "rainsensor.h"
#include "utils.h"
#include <wiringPi.h>

#define BUCKET_GPIO	17

pthread_mutex_t CRainSensor::s_mutex;
float CRainSensor::s_fRainDay = 0.0;
std::list<unsigned int> CRainSensor::s_lstTipTimes;

//*****************************************************************************
CRainSensor::CRainSensor(void)
{
	pthread_mutex_init(&s_mutex, NULL);

	pinMode(BUCKET_GPIO, INPUT);
	pullUpDnControl(BUCKET_GPIO, PUD_DOWN);
	wiringPiISR(BUCKET_GPIO, INT_EDGE_FALLING, BucketCallback);
}

//*****************************************************************************
CRainSensor::~CRainSensor(void)
{
	pthread_mutex_destroy(&s_mutex);
}

//*****************************************************************************
float CRainSensor::GetRainHour(void)
{
	float fRainHour = 0.0;

	unsigned int nTimeNow = millis();

	pthread_mutex_lock(&s_mutex);

	std::list<unsigned int>::iterator itr = s_lstTipTimes.begin();
	while(itr != s_lstTipTimes.end())
	{
		if(TimeDiff(nTimeNow, *itr) > 3600000)
		{
			itr = s_lstTipTimes.erase(itr);
			continue;
		}

		fRainHour += 0.011;
		++itr;
	}

	pthread_mutex_unlock(&s_mutex);

	return fRainHour;
}

//*****************************************************************************
float CRainSensor::GetRainDay(void)
{
	float fRainDay;

	pthread_mutex_lock(&s_mutex);

	fRainDay = s_fRainDay;

	pthread_mutex_unlock(&s_mutex);

	return fRainDay;
}

//*****************************************************************************
void CRainSensor::BucketCallback(void)
{
	static unsigned int nPrevCallbackTime = 0;
	static unsigned int nPrevDay = 0;

	unsigned int nTimeNow = millis();

	// The read sensor can "bounce" causing multiple events when only one
	// event should be generated, so ignore events close together
	if(nTimeNow - nPrevCallbackTime > 500)
	{
		time_t nRaw = time(NULL);
		struct tm *pTime = localtime(&nRaw);

		pthread_mutex_lock(&s_mutex);
		s_lstTipTimes.push_back(nTimeNow);

		if(nPrevDay != pTime->tm_mday)
			s_fRainDay = 0.0;
		s_fRainDay += 0.011;

		pthread_mutex_unlock(&s_mutex);

		nPrevDay = pTime->tm_mday;

		nPrevCallbackTime = nTimeNow;
	}
}
