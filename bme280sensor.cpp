#include <unistd.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include "bme280sensor.h"

#define I2CADDRESS	0x77

// Timing registers
#define TIMING_REGISTER_TEMP1		0x88
#define TIMING_REGISTER_TEMP2		0x8A
#define TIMING_REGISTER_TEMP3		0x8C

#define TIMING_REGISTER_PRESSURE1	0x8E
#define TIMING_REGISTER_PRESSURE2	0x90
#define TIMING_REGISTER_PRESSURE3	0x92
#define TIMING_REGISTER_PRESSURE4	0x94
#define TIMING_REGISTER_PRESSURE5	0x96
#define TIMING_REGISTER_PRESSURE6	0x98
#define TIMING_REGISTER_PRESSURE7	0x9A
#define TIMING_REGISTER_PRESSURE8	0x9C
#define TIMING_REGISTER_PRESSURE9	0x9E

#define TIMING_REGISTER_HUMIDITY1	0xA1
#define TIMING_REGISTER_HUMIDITY2	0xE1
#define TIMING_REGISTER_HUMIDITY3	0xE3
#define TIMING_REGISTER_HUMIDITY4	0xE4
#define TIMING_REGISTER_HUMIDITY5	0xE5
#define TIMING_REGISTER_HUMIDITY6	0xE6
#define TIMING_REGISTER_HUMIDITY7	0xE7

// Control registers
#define CONTROL_REGISTER_HUMIDITY	0xF2
#define CONTROL_REGISTER			0xF4

// Data registers
#define DATA_REGISTER_TEMP1			0xFA
#define DATA_REGISTER_TEMP2			0xFB
#define DATA_REGISTER_TEMP3			0xFC

#define DATA_REGISTER_PRESSURE1		0xF7
#define DATA_REGISTER_PRESSURE2		0xF8
#define DATA_REGISTER_PRESSURE3		0xF9

#define DATA_REGISTER_HUMIDITY1		0xFD
#define DATA_REGISTER_HUMIDITY2		0xFE

//*****************************************************************************
CBme280Sensor::CBme280Sensor(const ESampleMode eMode)
	: m_eMode(eMode),
	m_nDev(0),
	m_nRegTemp1(0),
	m_nRegTemp2(0),
	m_nRegTemp3(0),
	m_nRegPressure1(0),
	m_nRegPressure2(0),
	m_nRegPressure3(0),
	m_nRegPressure4(0),
	m_nRegPressure5(0),
	m_nRegPressure6(0),
	m_nRegPressure7(0),
	m_nRegPressure8(0),
	m_nRegPressure9(0),
	m_nRegHumidity1(0),
	m_nRegHumidity2(0),
	m_nRegHumidity3(0),
	m_nRegHumidity4(0),
	m_nRegHumidity5(0),
	m_nRegHumidity6(0),
	m_nTempFine(0)
{
	m_nDev = wiringPiI2CSetup(I2CADDRESS);
	if(m_nDev == -1)
		m_nDev = 0;
	else
	{
		m_nRegTemp1 = wiringPiI2CReadReg16(m_nDev, TIMING_REGISTER_TEMP1);
		m_nRegTemp2 = wiringPiI2CReadReg16(m_nDev, TIMING_REGISTER_TEMP2);
		m_nRegTemp3 = wiringPiI2CReadReg16(m_nDev, TIMING_REGISTER_TEMP3);

		m_nRegPressure1 = wiringPiI2CReadReg16(m_nDev, TIMING_REGISTER_PRESSURE1);
		m_nRegPressure2 = wiringPiI2CReadReg16(m_nDev, TIMING_REGISTER_PRESSURE2);
		m_nRegPressure3 = wiringPiI2CReadReg16(m_nDev, TIMING_REGISTER_PRESSURE3);
		m_nRegPressure4 = wiringPiI2CReadReg16(m_nDev, TIMING_REGISTER_PRESSURE4);
		m_nRegPressure5 = wiringPiI2CReadReg16(m_nDev, TIMING_REGISTER_PRESSURE5);
		m_nRegPressure6 = wiringPiI2CReadReg16(m_nDev, TIMING_REGISTER_PRESSURE6);
		m_nRegPressure7 = wiringPiI2CReadReg16(m_nDev, TIMING_REGISTER_PRESSURE7);
		m_nRegPressure8 = wiringPiI2CReadReg16(m_nDev, TIMING_REGISTER_PRESSURE8);
		m_nRegPressure9 = wiringPiI2CReadReg16(m_nDev, TIMING_REGISTER_PRESSURE9);

		m_nRegHumidity1 = wiringPiI2CReadReg8(m_nDev, TIMING_REGISTER_HUMIDITY1);
		m_nRegHumidity2 = wiringPiI2CReadReg16(m_nDev, TIMING_REGISTER_HUMIDITY2);
		m_nRegHumidity3 = wiringPiI2CReadReg8(m_nDev, TIMING_REGISTER_HUMIDITY3);
		m_nRegHumidity4 = ((wiringPiI2CReadReg8(m_nDev, TIMING_REGISTER_HUMIDITY4) << 24) >> 20) | (wiringPiI2CReadReg8(m_nDev, TIMING_REGISTER_HUMIDITY5) & 0x0F);
		m_nRegHumidity5 = ((wiringPiI2CReadReg8(m_nDev, TIMING_REGISTER_HUMIDITY6) << 24) >> 20) | (wiringPiI2CReadReg8(m_nDev, TIMING_REGISTER_HUMIDITY5) >> 4 & 0x0F);
		m_nRegHumidity6 = wiringPiI2CReadReg8(m_nDev, TIMING_REGISTER_HUMIDITY7);

		wiringPiI2CWriteReg8(m_nDev, CONTROL_REGISTER, 0x3F);
	}
}

//*****************************************************************************
CBme280Sensor::~CBme280Sensor(void)
{
	if(m_nDev)
		close(m_nDev);
}

//*****************************************************************************
float CBme280Sensor::GetTemperature(void)
{	// Return temperature in Fahrenheit
	float fTemp;

	wiringPiI2CWriteReg8(m_nDev, CONTROL_REGISTER_HUMIDITY, m_eMode);
	wiringPiI2CWriteReg8(m_nDev, CONTROL_REGISTER, (m_eMode << 5 | m_eMode << 2 | 1));

	// Sleep while the sensor does its thing
	delay((int)(6.9 * (1 << m_eMode) + 2.4));

	int nMsb = wiringPiI2CReadReg8(m_nDev, DATA_REGISTER_TEMP1);
	int nLsb = wiringPiI2CReadReg8(m_nDev, DATA_REGISTER_TEMP2);
	int nXlsb = wiringPiI2CReadReg8(m_nDev, DATA_REGISTER_TEMP3);
	int nRaw = ((nMsb << 16) | (nLsb << 8) | nXlsb) >> 4;

	float fV1 = ((float)nRaw / 16384.0 - (float)m_nRegTemp1 / 1024.0) * (float)m_nRegTemp2;
	float fV2 = ((float)nRaw / 131072.0 - (float)m_nRegTemp1 / 8192.0) * ((float)nRaw / 131072.0 - (float)m_nRegTemp1 / 8192.0) * (float)m_nRegTemp3;
	m_nTempFine = fV1 + fV2;
	fTemp = ((fV1 + fV2) / 5120.0 * 1.8) + 32.0;

	return fTemp;
}

//*****************************************************************************
float CBme280Sensor::GetPressure(void) const
{	// Return pressure in inches of Mercury
	float fPressure = 0.0;

	if(m_nRegPressure1)
	{
		int nMsb = wiringPiI2CReadReg8(m_nDev, DATA_REGISTER_PRESSURE1);
		int nLsb = wiringPiI2CReadReg8(m_nDev, DATA_REGISTER_PRESSURE2);
		int nXlsb = wiringPiI2CReadReg8(m_nDev, DATA_REGISTER_PRESSURE3);
		int nRaw = ((nMsb << 16) | (nLsb << 8) | nXlsb) >> 4;

		float fV1 = (float)m_nTempFine / 2.0 - 64000.0;
		float fV2 = fV1 * fV1 * (float)m_nRegPressure6 / 32768.0;
		fV2 += fV1 * (float)m_nRegPressure5 * 2;
		fV2 = fV2 / 4.0 + (float)m_nRegPressure4 * 65536.0;
		fV1 = ((float)m_nRegPressure3 * fV1 * fV1 / 524288.0 + (float)m_nRegPressure2 * fV1) / 524288.0;
		fV1 = (1 + fV1 / 32768.0) * (float)m_nRegPressure1;

		fPressure = 1048576.0 - (float)nRaw;
		fPressure = ((fPressure - fV2 / 4096.0) * 6250.0) / fV1;
		fV1 = (float)m_nRegPressure9 * fPressure * fPressure / 2147483648.0;
		fV2 = fPressure * (float)m_nRegPressure8 / 32768.0;
		fPressure += (fV1 + fV2 + (float)m_nRegPressure7) / 16.0;
		fPressure /= 3386.38816;
	}

	return fPressure;
}

//*****************************************************************************
float CBme280Sensor::GetHumidity(void) const
{	// Return humidity in percentage
	float fHumidity;

	int nMsb = wiringPiI2CReadReg8(m_nDev, DATA_REGISTER_HUMIDITY1);
	int nLsb = wiringPiI2CReadReg8(m_nDev, DATA_REGISTER_HUMIDITY2);
	int nRaw = (nMsb << 8) | nLsb;

	fHumidity = (float)m_nTempFine - 76800.0;
	fHumidity = ((float)nRaw - ((float)m_nRegHumidity4 * 64.0 + (float)m_nRegHumidity5 / 16384.0 * fHumidity)) * ((float)m_nRegHumidity2 / 65536.0 * (1.0 + (float)m_nRegHumidity6 / 67108864.0 * fHumidity * (1.0 + (float)m_nRegHumidity3 / 67108864.0 * fHumidity)));
	fHumidity = fHumidity * (1.0 - (float)m_nRegHumidity1 * fHumidity / 524288.0);

	if(fHumidity > 100.0)
		fHumidity = 100.0;
	else if(fHumidity < 0.0)
		fHumidity = 0.0;

	return fHumidity;
}
