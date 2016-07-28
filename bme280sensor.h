#pragma once

//*****************************************************************************
class CBme280Sensor
{
public:
	typedef enum _ESampleMode
	{
		ESM_1 = 1,
		ESM_2 = 2,
		ESM_4 = 3,
		ESM_8 = 4,
		ESM_16 = 5
	} ESampleMode, *PESampleMode;

	CBme280Sensor(const ESampleMode eMode = ESM_1);
	virtual ~CBme280Sensor(void);

	float GetTemperature(void);			// Return temperature in Fahrenheit
	float GetPressure(void) const;		// Return pressure in inches of Mercury
	float GetHumidity(void) const;		// Return humidity in percentage

protected:
	ESampleMode m_eMode;
	int m_nDev;

	unsigned short m_nRegTemp1;
	short m_nRegTemp2;
	short m_nRegTemp3;

	unsigned short m_nRegPressure1;
	short m_nRegPressure2;
	short m_nRegPressure3;
	short m_nRegPressure4;
	short m_nRegPressure5;
	short m_nRegPressure6;
	short m_nRegPressure7;
	short m_nRegPressure8;
	short m_nRegPressure9;

	unsigned short m_nRegHumidity1;
	short m_nRegHumidity2;
	unsigned short m_nRegHumidity3;
	short m_nRegHumidity4;
	short m_nRegHumidity5;
	short m_nRegHumidity6;

	int m_nTempFine;
};
