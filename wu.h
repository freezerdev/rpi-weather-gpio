#pragma once

#include "weatherdata.h"
#include <time.h>

bool UpdateWU(const time_t nRaw, const CWeatherData &data);
