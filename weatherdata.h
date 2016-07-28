#pragma once

//*****************************************************************************
class CWeatherData
{
public:
	CWeatherData(void);
	virtual ~CWeatherData(void);

	float GetTemperature(void) const {return m_fTemp;}
	void SetTemperature(const float fTemp);

	float GetPressure(void) const {return m_fPressure;}
	void SetPressure(const float fPressure) {m_fPressure = fPressure;}

	float GetHumidity(void) const {return m_fHumidity;}
	void SetHumidity(const float fHumidity);

	float GetWindSpeed(void) const {return m_fWindSpeed;}
	void SetWindSpeed(const float fWindSpeed) {m_fWindSpeed = fWindSpeed;}

	float GetWindGust(void) const {return m_fWindGust;}
	void SetWindGust(const float fWindGust);

	float GetWindDirection(void) const {return m_fWindDir;}
	void SetWindDirection(const float fWindDir) {m_fWindDir = fWindDir;}

	float GetRainHour(void) const {return m_fRainHour;}
	void SetRainHour(const float fRainHour) {m_fRainHour = fRainHour;}

	float GetRainDay(void) const {return m_fRainDay;}
	void SetRainDay(const float fRainDay) {m_fRainDay = fRainDay;}

	float GetDewPoint(void) const;

	bool IsValidDailyHighTemperature(void) const;
	float GetDailyHighTemperature(void) const {return m_fTempDailyHigh;}

	bool IsValidDailyLowTemperature(void) const;
	float GetDailyLowTemperature(void) const {return m_fTempDailyLow;}

	bool IsValidDailyHighHumidity(void) const;
	float GetDailyHighHumidity(void) const {return m_fHumidityDailyHigh;}

	bool IsValidDailyLowHumidity(void) const;
	float GetDailyLowHumidity(void) const {return m_fHumidityDailyLow;}

	bool IsValidDailyHighWindGust(void) const;
	float GetDailyHighWindGust(void) const {return m_fWindGustDailyHigh;}

	void ResetDailyValues(void);

protected:
	float m_fTemp;
	float m_fPressure;
	float m_fHumidity;
	float m_fWindSpeed;
	float m_fWindGust;
	float m_fWindDir;
	float m_fRainHour;
	float m_fRainDay;

	float m_fTempDailyHigh;
	float m_fTempDailyLow;
	float m_fHumidityDailyHigh;
	float m_fHumidityDailyLow;
	float m_fWindGustDailyHigh;
};
