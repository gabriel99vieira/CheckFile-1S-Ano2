#ifndef _STATISTICS_H_
#define _STATISTICS_H_

typedef struct
{
    int counter_analized;
    int counter_ok;
    int counter_mismatch;
    int counter_error;
} statistics_t;

void init_statistics(statistics_t *statistics);

#endif