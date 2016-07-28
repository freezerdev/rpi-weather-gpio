#include "weatherdata.h"
#include "utils.h"
#include <math.h>

#define INVALID_HIGH_TEMP	-1000.0
#define INVALID_LOW_TEMP	1000.0
#define INVALID_HIGH_HUMIDITY	-1.0
#define INVALID_LOW_HUMIDITY	101.0
#define INVALID_HIGH_GUST	-1.0

//*****************************************************************************
CWeatherData::CWeatherData(void)
	: m_fTemp(-459.666),	// Absolute zero in Fahrenheit
	m_fPressure(0.0),
	m_fHumidity(0.0),
	m_fWindSpeed(0.0),
	m_fWindGust(0.0),
	m_fWindDir(0.0),
	m_fRainHour(0.0),
	m_fRainDay(0.0),
	m_fTempDailyHigh(INVALID_HIGH_TEMP),
	m_fTempDailyLow(INVALID_LOW_TEMP),
	m_fHumidityDailyHigh(INVALID_HIGH_HUMIDITY),
	m_fHumidityDailyLow(INVALID_LOW_HUMIDITY),
	m_fWindGustDailyHigh(INVALID_HIGH_GUST)
{
}

//*****************************************************************************
CWeatherData::~CWeatherData(void)
{
}

//*****************************************************************************
void CWeatherData::SetTemperature(const float fTemp)
{
	m_fTemp = fTemp;

	if(m_fTemp > m_fTempDailyHigh)
		m_fTempDailyHigh = m_fTemp;

	if(m_fTemp < m_fTempDailyLow)
		m_fTempDailyLow = m_fTemp;
}

//*****************************************************************************
void CWeatherData::SetHumidity(const float fHumidity)
{
	m_fHumidity = fHumidity;

	if(m_fHumidity > m_fHumidityDailyHigh)
		m_fHumidityDailyHigh = m_fHumidity;

	if(m_fHumidity < m_fHumidityDailyLow)
		m_fHumidityDailyLow = m_fHumidity;
}

//*****************************************************************************
void CWeatherData::SetWindGust(const float fWindGust)
{
	m_fWindGust = fWindGust;

	if(m_fWindGust > m_fWindGustDailyHigh)
		m_fWindGustDailyHigh = m_fWindGust;
}

//*****************************************************************************
float CWeatherData::GetDewPoint(void) const
{
	float fDewPoint;

	float fTempC = F2C(m_fTemp);
	float fDewPointC = 243.04 * (logf(m_fHumidity / 100.0) + ((17.625 * fTempC) / (243.04 + fTempC))) / (17.625 - logf(m_fHumidity / 100.0) - ((17.625 * fTempC) / (243.04 + fTempC)));
	fDewPoint = C2F(fDewPointC);

	return fDewPoint;
}

//*****************************************************************************
bool CWeatherData::IsValidDailyHighTemperature(void) const
{
	return (m_fTempDailyHigh != INVALID_HIGH_TEMP);
}

//*****************************************************************************
bool CWeatherData::IsValidDailyLowTemperature(void) const
{
	return (m_fTempDailyLow != INVALID_LOW_TEMP);
}

//*****************************************************************************
bool CWeatherData::IsValidDailyHighHumidity(void) const
{
	return (m_fHumidityDailyHigh != INVALID_HIGH_HUMIDITY);
}

//*****************************************************************************
bool CWeatherData::IsValidDailyLowHumidity(void) const
{
	return (m_fHumidityDailyLow != INVALID_LOW_HUMIDITY);
}

//*****************************************************************************
bool CWeatherData::IsValidDailyHighWindGust(void) const
{
	return (m_fWindGustDailyHigh != INVALID_HIGH_GUST);
}

//*****************************************************************************
void CWeatherData::ResetDailyValues(void)
{
	m_fTempDailyHigh = INVALID_HIGH_TEMP;
	m_fTempDailyLow = INVALID_LOW_TEMP;
	m_fHumidityDailyHigh = INVALID_HIGH_HUMIDITY;
	m_fHumidityDailyLow = INVALID_LOW_HUMIDITY;
	m_fWindGustDailyHigh = INVALID_HIGH_GUST;
}
