#pragma once

#include "weatherdata.h"
#include <time.h>

void UpdateWU(const time_t nRaw, const CWeatherData &data);
