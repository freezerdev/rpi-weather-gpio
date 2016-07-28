#include "windsensor.h"
#include "utils.h"
#include <limits.h>
#include <wiringPi.h>

#define ANEMOMETER_GPIO	4

#define N_GPIO		5
#define NE_GPIO		6
#define E_GPIO		13
#define SE_GPIO		19
#define S_GPIO		26
#define SW_GPIO		16
#define W_GPIO		20
#define NW_GPIO		21

pthread_mutex_t CWindSensor::s_mutex;
unsigned int CWindSensor::s_nPrevTimeSpeed = 0;
unsigned int CWindSensor::s_nCountSpeed = 0;
unsigned int CWindSensor::s_nShortestGust = UINT_MAX;

//*****************************************************************************
CWindSensor::CWindSensor(void)
{
	pthread_mutex_init(&s_mutex, NULL);

	s_nPrevTimeSpeed = millis();

	pinMode(N_GPIO, INPUT);
	pullUpDnControl(N_GPIO, PUD_DOWN);
	pinMode(NE_GPIO, INPUT);
	pullUpDnControl(NE_GPIO, PUD_DOWN);
	pinMode(E_GPIO, INPUT);
	pullUpDnControl(E_GPIO, PUD_DOWN);
	pinMode(SE_GPIO, INPUT);
	pullUpDnControl(SE_GPIO, PUD_DOWN);
	pinMode(S_GPIO, INPUT);
	pullUpDnControl(S_GPIO, PUD_DOWN);
	pinMode(SW_GPIO, INPUT);
	pullUpDnControl(SW_GPIO, PUD_DOWN);
	pinMode(W_GPIO, INPUT);
	pullUpDnControl(W_GPIO, PUD_DOWN);
	pinMode(NW_GPIO, INPUT);
	pullUpDnControl(NW_GPIO, PUD_DOWN);

	pinMode(ANEMOMETER_GPIO, INPUT);
	pullUpDnControl(ANEMOMETER_GPIO, PUD_DOWN);
	wiringPiISR(ANEMOMETER_GPIO, INT_EDGE_FALLING, AnemometerCallback);
}

//*****************************************************************************
CWindSensor::~CWindSensor(void)
{
	pthread_mutex_destroy(&s_mutex);
}

//*****************************************************************************
float CWindSensor::GetWindSpeed(void)
{
	float fWindSpeed = 0.0;

	unsigned int nTimeNow = millis();
	unsigned int nDiff = TimeDiff(nTimeNow, s_nPrevTimeSpeed);

	pthread_mutex_lock(&s_mutex);
	if(nDiff != 0)
		fWindSpeed = 1492.0 * (float)s_nCountSpeed / (float)nDiff;

	s_nCountSpeed = 0;
	pthread_mutex_unlock(&s_mutex);

	s_nPrevTimeSpeed = nTimeNow;

	return fWindSpeed;
}

//*****************************************************************************
float CWindSensor::GetWindGust(void)
{
	float fWindGust = 0.0;

	pthread_mutex_lock(&s_mutex);
	if(s_nShortestGust != UINT_MAX && s_nShortestGust != 0)
	{
		fWindGust = 1492.0 / (float)s_nShortestGust;
		s_nShortestGust = UINT_MAX;
	}
	pthread_mutex_unlock(&s_mutex);

	return fWindGust;
}

//*****************************************************************************
float CWindSensor::GetWindDirection(void) const
{
	float fWindDir = 0.0;

	int nPinN = digitalRead(N_GPIO);
	int nPinNE = digitalRead(NE_GPIO);
	int nPinE = digitalRead(E_GPIO);
	int nPinSE = digitalRead(SE_GPIO);
	int nPinS = digitalRead(S_GPIO);
	int nPinSW = digitalRead(SW_GPIO);
	int nPinW = digitalRead(W_GPIO);
	int nPinNW = digitalRead(NW_GPIO);

	if(nPinN && nPinNE)
		fWindDir = 22.5;
	else if(nPinNE && nPinE)
		fWindDir = 67.5;
	else if(nPinE && nPinSE)
		fWindDir = 112.5;
	else if(nPinSE && nPinS)
		fWindDir = 157.5;
	else if(nPinS && nPinSW)
		fWindDir = 202.5;
	else if(nPinSW && nPinW)
		fWindDir = 247.5;
	else if(nPinW && nPinNW)
		fWindDir = 292.5;
	else if(nPinNW && nPinN)
		fWindDir = 337.5;
	else if(nPinN)
		fWindDir = 0.0;
	else if(nPinNE)
		fWindDir = 45.0;
	else if(nPinE)
		fWindDir = 90.0;
	else if(nPinSE)
		fWindDir = 135.0;
	else if(nPinS)
		fWindDir = 180.0;
	else if(nPinSW)
		fWindDir = 225.0;
	else if(nPinW)
		fWindDir = 270.0;
	else if(nPinNW)
		fWindDir = 315.0;

	return fWindDir;
}

//*****************************************************************************
const char *CWindSensor::GetWindDirectionString(const float fWindDir)
{
	if(fWindDir == 22.5)
		return "NNE";
	else if(fWindDir == 45.0)
		return "NE";
	else if(fWindDir == 67.5)
		return "ENE";
	else if(fWindDir == 90.0)
		return "E";
	else if(fWindDir == 112.5)
		return "ESE";
	else if(fWindDir == 135.0)
		return "SE";
	else if(fWindDir == 157.5)
		return "SSE";
	else if(fWindDir == 180.0)
		return "S";
	else if(fWindDir == 202.5)
		return "SSW";
	else if(fWindDir == 225.0)
		return "SW";
	else if(fWindDir == 247.5)
		return "WSW";
	else if(fWindDir == 270.0)
		return "W";
	else if(fWindDir == 292.5)
		return "WNW";
	else if(fWindDir == 315.0)
		return "NW";
	else if(fWindDir == 337.5)
		return "NNW";
	else //if(fWindDir == 0.0 || fWindDir == 360.0)
		return "N";
}

//*****************************************************************************
void CWindSensor::AnemometerCallback(void)
{
	static unsigned int nPrevCallbackTime = 0;
	static unsigned int nPrevTimeGust = UINT_MAX;
	static bool bPrevGust = false;
	static bool bTick = false;

	unsigned int nTimeNow = millis();

	// The read sensor can "bounce" causing multiple events when only one
	// event should be generated, so ignore events really close together
	if(nTimeNow - nPrevCallbackTime > 10)
	{	// The sensor is triggered twice per rotation, so use a tick/tock
		// to count off 2 events to get the time for one full rotation
		if(bTick == false)
		{
			unsigned int nDiff = UINT_MAX;
			if(bPrevGust == true)
				nDiff = TimeDiff(nTimeNow, nPrevTimeGust);

			pthread_mutex_lock(&s_mutex);

			++s_nCountSpeed;

			if(bPrevGust && nDiff < s_nShortestGust)
				s_nShortestGust = nDiff;

			pthread_mutex_unlock(&s_mutex);

			nPrevTimeGust = nTimeNow;
			bPrevGust = true;
			bTick = true;
		}
		else
			bTick = false;

		nPrevCallbackTime = nTimeNow;
	}
}
