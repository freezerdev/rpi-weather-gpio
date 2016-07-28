#pragma once

#include <pthread.h>

//*****************************************************************************
class CWindSensor
{
public:
	CWindSensor(void);
	virtual ~CWindSensor(void);

	float GetWindSpeed(void);		// Return average wind speed (MPH) since last query
	float GetWindGust(void);		// Return highest wind gust (MPH) since last query

	float GetWindDirection(void) const;
	static const char *GetWindDirectionString(const float fWindDir);

protected:
	static pthread_mutex_t s_mutex;

	static unsigned int s_nPrevTimeSpeed;
	static unsigned int s_nCountSpeed;

	static unsigned int s_nShortestGust;

	static void AnemometerCallback(void);
};
