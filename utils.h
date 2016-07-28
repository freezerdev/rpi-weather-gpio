#pragma once

#define C2F(c)	((c) * 1.8 + 32.0)
#define F2C(f)	(((f) - 32.0) / 1.8)

unsigned int TimeDiff(const unsigned int nNow, const unsigned int nPrev);

float GetPiCpuTemperature(void);

const char *GetRowColor(const unsigned int nRow);

int Get12Hour(const int n24Hour);
const char *AmPm(const int nHour);
