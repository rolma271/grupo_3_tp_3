/**
 * @file priority_queue.h
 * @brief Priority Queue Library for FreeRTOS applications
 *
 * This library provides a priority queue implementation suitable for use in FreeRTOS.
 * The priority queue is implemented as a binary heap and supports operations to
 * send and receive events with varying priorities. The heap is structured as a max-heap,
 * where the event with the highest priority is always at the root.
 *
 * The library includes functions to create a priority queue, send events to the queue,
 * and receive events from the queue. The library is thread-safe, utilizing FreeRTOS
 * mutexes and semaphores to synchronize access to the queue.
 *
 * @authors 
 * - Marco Rolón Radcenco
 * - Pablo Eduardo Gimenez
 * - Iván Podoroska
 */

#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "cmsis_os.h"

/**
 * @brief Priority levels for events.
 */
typedef enum 
{
    LOW_PRIORITY,    /**< Low priority event */
    MEDIUM_PRIORITY, /**< Medium priority event */
    HIGH_PRIORITY    /**< High priority event */
} 
pq_priority_t;

/**
 * @brief Structure representing an event.
 */
typedef struct 
{
    pq_priority_t priority; /**< Priority of the event */
} 
pq_event_t;

/**
 * @brief Type representing the priority queue current size in events.
 */
typedef int pq_size_t;

#define PQ_MAX_EVENT_SIZE 10 /**< Maximum size of the priority queue */

/**
 * @brief Structure representing the priority queue.
 *
 * The priority queue is implemented as a binary heap stored in an array. The queue
 * maintains a size indicating the number of events currently in the queue, and
 * uses a mutex and semaphore for thread-safe operations.
 */
typedef struct 
{
    pq_event_t 	events[PQ_MAX_EVENT_SIZE]; /**< Array of events in the queue */
    pq_size_t 	size;                      /**< Current size of the queue */
	
    SemaphoreHandle_t mutex;       /**< Mutex for synchronizing access to the queue */
    SemaphoreHandle_t eventSemaphore; /**< Semaphore for signaling event presence */
} 
pq_handle_t;

/**
 * @brief Creates a new priority queue.
 *
 * Allocates memory for a new priority queue and initializes its components.
 * The queue is implemented as a max-heap, where the highest priority event
 * is always at the root.
 *
 * @return Pointer to the newly created priority queue, or NULL if creation failed.
 */
pq_handle_t *xPriorityQueueCreate(void);

/**
 * @brief Sends an event to the priority queue.
 *
 * Inserts an event into the priority queue, maintaining the max-heap property.
 * The function is thread-safe and will block if the queue is full, until space
 * becomes available or the specified timeout period expires.
 *
 * @param pq Pointer to the priority queue.
 * @param event Pointer to an event to send to the queue.
 * @param ticksToWait Maximum time (in ticks) to wait for space to become available.
 * @return pdPASS if the event was successfully sent, pdFAIL otherwise.
 */
BaseType_t xPriorityQueueSend(pq_handle_t *pq, pq_event_t *event, TickType_t ticksToWait);

/**
 * @brief Receives an event from the priority queue.
 *
 * Retrieves and removes the highest priority event from the priority queue,
 * maintaining the max-heap property. The thread-safe function will block
 * if the queue is empty, until an event becomes available or the specified timeout
 * period expires.
 *
 * @param pq Pointer to the priority queue.
 * @param event Pointer to store the received event.
 * @param ticksToWait Maximum time (in ticks) to wait for an event to become available.
 * @return pdPASS if an event was successfully received, pdFAIL otherwise.
 */
BaseType_t xPriorityQueueReceive(pq_handle_t *pq, pq_event_t *event, TickType_t ticksToWait);

#endif // PRIORITY_QUEUE_H
