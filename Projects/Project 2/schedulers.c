#include "schedulers.h"
#include "pcb.h"
#include "queue.h"
#include "util.h"
#include <pthread.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern long long start_time;

void* fcfs(void* args) {
    scheduler_args_t* scheduler_args = (scheduler_args_t*)args;

    // Get the queue
    queue_t* queue = scheduler_args->source_queue;
    // Get the history queue
    queue_t* history_queue = scheduler_args->history_queue;
    pcb_t item;

    while (1) {
        

        while(1) {
            // Lock the queue
            pthread_mutex_lock(scheduler_args->queue_generator_lock);

            printf("\nProcessor %d has the lock\n", scheduler_args->id_of_processor);

            if(!(queue->size > 0)){
                
                pthread_mutex_unlock(scheduler_args->queue_generator_lock);
                printf("Time to sleep");
                usleep(1000);
                printf("\nProcessor %d releases the lock\n", scheduler_args->id_of_processor);
            }
            else {

                item = queue_dequeue(queue);
                printf("Size of the queue is %d for process %d\n" ,queue->size, scheduler_args->id_of_processor);
                break;
            }
        }

        printf("\nProcessor %d has exited\n", scheduler_args->id_of_processor);

        // Get the next process

        

        printf("\nProcessor %d has taken item\n", scheduler_args->id_of_processor);
        printf("Size of the queue is %d\n" ,queue->size);

        if (item.is_dummy != 0) {
            long long current_time = gettimeofday_ms() - start_time;
            // print_for_outmode(&item, current_time, scheduler_args->outmode,
            //                   OUTMODE_3_SETTINGS_PCB_ADDED_TO_READY_QUEUE,
            //                   scheduler_args->id_of_processor);

            print_for_outmode(NULL, current_time, scheduler_args->outmode,
                              OUTMODE_3_SETTINGS_CPU_EXITING, scheduler_args->id_of_processor,
                              scheduler_args->outfile);
            queue_enqueue(queue, item);
            pthread_mutex_unlock(scheduler_args->queue_generator_lock);
            break;
        }

        item.id_of_processor = scheduler_args->id_of_processor;

        pthread_mutex_unlock(scheduler_args->queue_generator_lock);

        print_for_outmode(&item, gettimeofday_ms() - start_time, scheduler_args->outmode,
                          OUTMODE_3_SETTINGS_PCB_PICKED_FROM_READY_QUEUE,
                          scheduler_args->id_of_processor, scheduler_args->outfile);

        usleep(item.burst_length * 1000);

        long long current_time = gettimeofday_ms() - start_time;
        item.remaining_time = 0;
        item.finish_time = current_time;
        item.turnaround_time = item.finish_time - item.arrival_time;
        item.waiting_time = item.turnaround_time - item.burst_length;

        print_for_outmode(&item, item.finish_time, scheduler_args->outmode,
                          OUTMODE_3_SETTINGS_PCB_FINISHED, scheduler_args->id_of_processor,
                          scheduler_args->outfile);

        pthread_mutex_lock(scheduler_args->history_queue_lock);
        queue_enqueue(history_queue, item);
        pthread_mutex_unlock(scheduler_args->history_queue_lock);
    }

    return NULL;
}

void* sjf(void* args) {
    scheduler_args_t* scheduler_args = (scheduler_args_t*)args;

    // Get the queue
    queue_t* queue = scheduler_args->source_queue;
    // Get the history queue
    queue_t* history_queue = scheduler_args->history_queue;

    while (1) {

        while(1) {
            // Lock the queue
            pthread_mutex_lock(scheduler_args->queue_generator_lock);
            if(!(queue->size > 0)){
                pthread_mutex_unlock(scheduler_args->queue_generator_lock);
                usleep(1000);
            }
            else {
                break;
            }
        }

        // Get the next process
        pcb_t item = queue_dequeue(queue);
        if (item.is_dummy != 0) {
            long long current_time = gettimeofday_ms() - start_time;

            // print_for_outmode(&item, current_time, scheduler_args->outmode,
            //                   OUTMODE_3_SETTINGS_PCB_ADDED_TO_READY_QUEUE,
            //                   scheduler_args->id_of_processor);

            print_for_outmode(NULL, current_time, scheduler_args->outmode,
                              OUTMODE_3_SETTINGS_CPU_EXITING, scheduler_args->id_of_processor,
                              scheduler_args->outfile);

            queue_enqueue(queue, item);
            pthread_mutex_unlock(scheduler_args->queue_generator_lock);

            break;
        }

        item.id_of_processor = scheduler_args->id_of_processor;
        // printf("PICKING UP PCB\n");
        // print_pcb(&item);

        pthread_mutex_unlock(scheduler_args->queue_generator_lock);

        print_for_outmode(&item, gettimeofday_ms() - start_time, scheduler_args->outmode,
                          OUTMODE_3_SETTINGS_PCB_PICKED_FROM_READY_QUEUE,
                          scheduler_args->id_of_processor, scheduler_args->outfile);

        usleep(item.burst_length * 1000);

        long long current_time = gettimeofday_ms() - start_time;
        item.remaining_time = 0;
        item.finish_time = current_time;
        item.turnaround_time = item.finish_time - item.arrival_time;
        item.waiting_time = item.turnaround_time - item.burst_length;

        print_for_outmode(&item, item.finish_time, scheduler_args->outmode,
                          OUTMODE_3_SETTINGS_PCB_FINISHED, scheduler_args->id_of_processor,
                          scheduler_args->outfile);

        pthread_mutex_lock(scheduler_args->history_queue_lock);
        queue_enqueue(history_queue, item);
        pthread_mutex_unlock(scheduler_args->history_queue_lock);
    }

    return NULL;
}

void* rr(void* args) {
    scheduler_args_t* scheduler_args = (scheduler_args_t*)args;

    // Get the queue
    queue_t* queue = scheduler_args->source_queue;
    // Get the history queue
    queue_t* history_queue = scheduler_args->history_queue;

    while (1) {

        while(1) {
            // Lock the queue
            pthread_mutex_lock(scheduler_args->queue_generator_lock);
            if(!(queue->size > 0)){
                pthread_mutex_unlock(scheduler_args->queue_generator_lock);
                usleep(1000);
            }
            else {
                break;
            }
        }

        // Get the next process
        pcb_t item = queue_dequeue(queue);
        if (queue->size < 1 && item.is_dummy != 0) {
            long long current_time = gettimeofday_ms() - start_time;
            // print_for_outmode(&item, current_time, scheduler_args->outmode,
            //                   OUTMODE_3_SETTINGS_PCB_ADDED_TO_READY_QUEUE,
            //                   scheduler_args->id_of_processor);

            print_for_outmode(NULL, current_time, scheduler_args->outmode,
                              OUTMODE_3_SETTINGS_CPU_EXITING, scheduler_args->id_of_processor,
                              scheduler_args->outfile);

            queue_enqueue(queue, item);
            pthread_mutex_unlock(scheduler_args->queue_generator_lock);
            break;
        } else if (item.is_dummy != 0) {
            pcb_t temp_item = item;
            item = queue_dequeue(queue);

            // print_for_outmode(&temp_item, gettimeofday_ms() - start_time,
            // scheduler_args->outmode,
            //                   OUTMODE_3_SETTINGS_PCB_ADDED_TO_READY_QUEUE,
            //                   scheduler_args->id_of_processor);

            queue_enqueue(queue, temp_item);
        }

        item.id_of_processor = scheduler_args->id_of_processor;

        pthread_mutex_unlock(scheduler_args->queue_generator_lock);

        print_for_outmode(&item, gettimeofday_ms() - start_time, scheduler_args->outmode,
                          OUTMODE_3_SETTINGS_PCB_PICKED_FROM_READY_QUEUE,
                          scheduler_args->id_of_processor, scheduler_args->outfile);

        if (item.remaining_time > scheduler_args->time_quantum) {
            usleep(scheduler_args->time_quantum * 1000);
            item.remaining_time -= scheduler_args->time_quantum;

            long long current_time = gettimeofday_ms() - start_time;

            print_for_outmode(&item, current_time, scheduler_args->outmode,
                              OUTMODE_3_SETTINGS_PCB_TIME_SLICE_EXPIRED,
                              scheduler_args->id_of_processor, scheduler_args->outfile);
            print_for_outmode(&item, current_time, scheduler_args->outmode,
                              OUTMODE_3_SETTINGS_PCB_ADDED_TO_READY_QUEUE,
                              scheduler_args->id_of_processor, scheduler_args->outfile);

            pthread_mutex_lock(scheduler_args->queue_generator_lock);
            queue_enqueue(queue, item);
            pthread_mutex_unlock(scheduler_args->queue_generator_lock);
        } else {
            usleep(item.remaining_time * 1000);
            long long current_time = gettimeofday_ms() - start_time;
            item.remaining_time = 0;
            item.finish_time = current_time;
            item.turnaround_time = item.finish_time - item.arrival_time;
            item.waiting_time = item.turnaround_time - item.burst_length;

            print_for_outmode(&item, item.finish_time, scheduler_args->outmode,
                              OUTMODE_3_SETTINGS_PCB_FINISHED, scheduler_args->id_of_processor,
                              scheduler_args->outfile);

            pthread_mutex_lock(scheduler_args->history_queue_lock);
            queue_enqueue(history_queue, item);
            pthread_mutex_unlock(scheduler_args->history_queue_lock);
        }
    }

    return NULL;
}