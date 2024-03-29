#ifndef __UTIL_H__
#define __UTIL_H__

#include "pcb.h"
#include "queue.h"
#include <stdio.h>

enum outmode_3_settings {
    OUTMODE_3_SETTINGS_NONE = 0,
    OUTMODE_3_SETTINGS_CPU_EXITING = 1,
    OUTMODE_3_SETTINGS_PCB_ADDED_TO_READY_QUEUE = 2,
    OUTMODE_3_SETTINGS_PCB_PICKED_FROM_READY_QUEUE = 4,
    OUTMODE_3_SETTINGS_PCB_TIME_SLICE_EXPIRED = 8,
    OUTMODE_3_SETTINGS_PCB_FINISHED = 16,
    OUTMODE_3_SETTINGS_PCB_ADDED_TO_READY_QUEUE_MULTI = 32,
};

long long gettimeofday_ms();
void print_pcb(pcb_t* pcb);
void print_for_outmode(pcb_t* pcb, long long time, char outmode, enum outmode_3_settings settings,
                       int where, FILE* outfile);
void print_history_queue(queue_t* queue, FILE* outfile);

#endif // __UTIL_H__