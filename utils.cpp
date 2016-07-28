#include "utils.h"
#include <stdio.h>
#include <limits.h>

//*****************************************************************************
unsigned int TimeDiff(const unsigned int nNow, const unsigned int nPrev)
{
	unsigned nDiff;

	if(nNow >= nPrev)
		nDiff = nNow - nPrev;
	else
		nDiff = (UINT_MAX - nPrev) + nNow;

	return nDiff;
}

//*****************************************************************************
float GetPiCpuTemperature(void)
{
	float fTemp = -459.666;

	FILE *pFile = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
	if(pFile)
	{
		unsigned long nTempC;
		fscanf(pFile, "%li", &nTempC);
		fTemp = C2F((float)nTempC / 1000.0);

		fclose(pFile);
	}

	return fTemp;
}

//*****************************************************************************
const char *GetRowColor(const unsigned int nRow)
{
	return ((nRow % 2) == 0) ? "#404060" : "#406040";
}

//*****************************************************************************
int Get12Hour(const int n24Hour)
{
	int n12Hour = n24Hour % 12;

	if(n12Hour == 0)
		n12Hour = 12;

	return n12Hour;
}

//*****************************************************************************
const char *AmPm(const int nHour)
{
	return (nHour < 12) ? "AM" : "PM";
}
