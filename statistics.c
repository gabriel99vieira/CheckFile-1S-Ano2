#include "statistics.h"

void init_statistics(statistics_t *statistics)
{
    statistics->counter_analized = 0;
    statistics->counter_error = 0;
    statistics->counter_mismatch = 0;
    statistics->counter_ok = 0;
}
