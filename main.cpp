#include "weatherdata.h"
#include "bme280sensor.h"
#include "windsensor.h"
#include "rainsensor.h"
#include "wu.h"
#include "sqlitedb.h"
#include "utils.h"
#include <stdio.h>
#include <time.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

void WriteToDB(const struct tm *pTime, const CWeatherData &data);
void WriteToHTML(const struct tm *pTime, const CWeatherData &data);

//*****************************************************************************
int main(void)
{
	unsigned int nPrevDay = 0;
	int nPrevMin = -1;

	wiringPiSetupGpio();

	CWeatherData data;
	CBme280Sensor bme280(CBme280Sensor::ESM_8);
	CWindSensor wind;
	CRainSensor rain;

	while(1)
	{
		delay(10000);
		time_t nRaw = time(NULL);
		struct tm *pTime = localtime(&nRaw);

		// The Raspberry Pi does not have an RTC, so if the time is before
		// the year 2000 then it has not contacted an NTP server
		if(pTime->tm_year > 100)
		{
			if(nPrevMin == -1 || nPrevMin != pTime->tm_min)
			{	// Take sensor readings once per minute
				nPrevMin = pTime->tm_min;

				if(nPrevDay == 0 || nPrevDay != pTime->tm_mday)
				{
					nPrevDay = pTime->tm_mday;
					data.ResetDailyValues();
				}

				// Read the sensors
				data.SetTemperature(bme280.GetTemperature());
				data.SetPressure(bme280.GetPressure());
				data.SetHumidity(bme280.GetHumidity());
				data.SetWindSpeed(wind.GetWindSpeed());
				data.SetWindGust(wind.GetWindGust());
				data.SetWindDirection(wind.GetWindDirection());
				data.SetRainHour(rain.GetRainHour());
				data.SetRainDay(rain.GetRainDay());

				// Write the values to SQLite
				WriteToDB(pTime, data);

				// Update the html file
				WriteToHTML(pTime, data);

				// Update weather underground
				UpdateWU(nRaw, data);
			}
		}
	}

	return 0;
}

//*****************************************************************************
void WriteToDB(const struct tm *pTime, const CWeatherData &data)
{
	char szFilename[64];
	sprintf(szFilename, "/home/pi/%d.sqlite", pTime->tm_year + 1900);

	CSQLiteDB db;
	if(db.Open(szFilename))
	{
		db.ExecuteQuery("CREATE TABLE IF NOT EXISTS weather_data ("\
			"time TEXT NOT NULL, "\
			"temperature REAL, "\
			"pressure REAL, "\
			"humidity REAL, "\
			"windspeed REAL, "\
			"windgust REAL, "\
			"winddir REAL, "\
			"rainhour REAL, "\
			"rainday REAL, "\
			"dewpoint REAL, "\
			"PRIMARY KEY(time))");

		char szQuery[256];
		sprintf(szQuery, "insert into weather_data (time, temperature, pressure, humidity, windspeed, windgust, winddir, rainhour, rainday, dewpoint) "\
			"values(datetime(), '%f', '%f', '%f', '%f', '%f' , '%f', '%f', '%f', '%f')",
			data.GetTemperature(),
			data.GetPressure(),
			data.GetHumidity(),
			data.GetWindSpeed(),
			data.GetWindGust(),
			data.GetWindDirection(),
			data.GetRainHour(),
			data.GetRainDay(),
			data.GetDewPoint());
		db.ExecuteQuery(szQuery);

		db.Close();
	}
}

//*****************************************************************************
void WriteToHTML(const struct tm *pTime, const CWeatherData &data)
{
	FILE *pHtml = fopen("/var/www/html/index.html", "w");
	if(pHtml)
	{
		unsigned int nRow = 0;

		fputs("<!DOCTYPE html>\n", pHtml);
		fputs("<html>\n", pHtml);
		fputs("<head>\n", pHtml);
		fputs("<title>Raspberry Pi Weather Station</title>\n", pHtml);
		fputs("<meta http-equiv=\"refresh\" content=\"60\">", pHtml);
		fputs("</head>\n", pHtml);
		fputs("<body bgcolor=\"#000060\">\n", pHtml);
		fputs("<font face=\"Arial\" color=\"white\" size=5>\n", pHtml);
		fputs("<table border=0 cellspacing=5 witdh=100%>\n", pHtml);
		fprintf(pHtml, "<tr bgcolor=\"%s\"><td>Date</td><td>%d-%d-%d</td></tr>\n", GetRowColor(nRow++), pTime->tm_year + 1900, pTime->tm_mon + 1, pTime->tm_mday);
		fprintf(pHtml, "<tr bgcolor=\"%s\"><td>Time</td><td>%d:%02d:%02d %s</td></tr>\n", GetRowColor(nRow++), Get12Hour(pTime->tm_hour), pTime->tm_min, pTime->tm_sec, AmPm(pTime->tm_hour));
		fprintf(pHtml, "<tr bgcolor=\"%s\"><td>Temperature</td><td>%.2f&deg; F</td></tr>\n", GetRowColor(nRow++), data.GetTemperature());

		if(data.IsValidDailyHighTemperature())
			fprintf(pHtml, "<tr bgcolor=\"%s\"><td>&nbsp;&nbsp;&nbsp;&nbsp;Daily high temperature</td><td>%.2f&deg; F</td></tr>\n", GetRowColor(nRow++), data.GetDailyHighTemperature());

		if(data.IsValidDailyLowTemperature())
			fprintf(pHtml, "<tr bgcolor=\"%s\"><td>&nbsp;&nbsp;&nbsp;&nbsp;Daily low temperature</td><td>%.2f&deg; F</td></tr>\n", GetRowColor(nRow++), data.GetDailyLowTemperature());

		fprintf(pHtml, "<tr bgcolor=\"%s\"><td>Humidity</td><td>%.2f%%</td></tr>\n", GetRowColor(nRow++), data.GetHumidity());

		if(data.IsValidDailyHighHumidity())
			fprintf(pHtml, "<tr bgcolor=\"%s\"><td>&nbsp;&nbsp;&nbsp;&nbsp;Daily high humidity</td><td>%.2f%</td></tr>\n", GetRowColor(nRow++), data.GetDailyHighHumidity());

		if(data.IsValidDailyLowHumidity())
			fprintf(pHtml, "<tr bgcolor=\"%s\"><td>&nbsp;&nbsp;&nbsp;&nbsp;Daily low humidity</td><td>%.2f%</td></tr>\n", GetRowColor(nRow++), data.GetDailyLowHumidity());

		fprintf(pHtml, "<tr bgcolor=\"%s\"><td>Pressure</td><td>%.2f in/Hg</td></tr>\n", GetRowColor(nRow++), data.GetPressure());
		fprintf(pHtml, "<tr bgcolor=\"%s\"><td>Dew point</td><td>%.2f&deg; F</td></tr>\n", GetRowColor(nRow++), data.GetDewPoint());
		fprintf(pHtml, "<tr bgcolor=\"%s\"><td>Wind speed</td><td>%.2f MPH</td></tr>\n", GetRowColor(nRow++), data.GetWindSpeed());
		fprintf(pHtml, "<tr bgcolor=\"%s\"><td>Wind gust</td><td>%.2f MPH</td></tr>\n", GetRowColor(nRow++), data.GetWindGust());

		if(data.IsValidDailyHighWindGust())
			fprintf(pHtml, "<tr bgcolor=\"%s\"><td>&nbsp;&nbsp;&nbsp;&nbsp;Daily high wind gust</td><td>%.2f MPH</td></tr>\n", GetRowColor(nRow++), data.GetDailyHighWindGust());

		fprintf(pHtml, "<tr bgcolor=\"%s\"><td>Wind direction</td><td>%.2f&deg; (%s)</td></tr>\n", GetRowColor(nRow++), data.GetWindDirection(), CWindSensor::GetWindDirectionString(data.GetWindDirection()));
		fprintf(pHtml, "<tr bgcolor=\"%s\"><td>Rain hourly</td><td>%.2f in</td></tr>\n", GetRowColor(nRow++), data.GetRainHour());
		fprintf(pHtml, "<tr bgcolor=\"%s\"><td>Rain daily</td><td>%.2f in</td></tr>\n", GetRowColor(nRow++), data.GetRainDay());
		fputs("<tr><td>&nbsp;</td><td></td></tr>", pHtml);
		fprintf(pHtml, "<tr bgcolor=\"%s\"><td>Raspberry Pi CPU</td><td>%.2f in</td></tr>\n", GetRowColor(nRow++), GetPiCpuTemperature());
		fputs("</table>\n", pHtml);
		fputs("</font>\n", pHtml);
		fputs("</body>\n", pHtml);
		fputs("</html>\n", pHtml);

		fclose(pHtml);
	}
}
