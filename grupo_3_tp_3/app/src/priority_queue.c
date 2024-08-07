
/********************** inclusions *******************************************/
#include "priority_queue.h"
#include <string.h>

/********************** macros and definitions *******************************/

/********************** internal functions declaration ***********************/

/**
 * @brief Swap two events.
 *
 * @param a Pointer to the first event.
 * @param b Pointer to the second event.
 */
static void _swap(pq_event_t *a, pq_event_t *b);

/**
 * @brief Heapify the queue upwards.
 *
 * @param pq Pointer to the priority queue.
 * @param index Index to start heapifying from.
 */
static void _heapifyUp(pq_handle_t *pq, int index);

/**
 * @brief Heapify the queue downwards.
 *
 * @param pq Pointer to the priority queue.
 * @param index Index to start heapifying from.
 */
static void _heapifyDown(pq_handle_t *pq, int index);

/********************** internal data definition *****************************/

/********************** external data definition *****************************/

/********************** internal functions definition ************************/

static void _swap(pq_event_t *a, pq_event_t *b) 
{
    pq_event_t temp = *a;
    *a = *b;
    *b = temp;
}

static void _heapifyUp(pq_handle_t *pq, int index) 
{
    int parentIndex = (index - 1) / 2;
    if (parentIndex >= 0 && pq->events[index].priority < pq->events[parentIndex].priority) 
	{
        _swap(&pq->events[index], &pq->events[parentIndex]);
        _heapifyUp(pq, parentIndex);
    }
}

static void _heapifyDown(pq_handle_t *pq, int index)
{
    int smallest = index;
    int leftChild = 2 * index + 1;
    int rightChild = 2 * index + 2;

    if (leftChild < pq->size && pq->events[leftChild].priority < pq->events[smallest].priority) 
	{
        smallest = leftChild;
    }

    if (rightChild < pq->size && pq->events[rightChild].priority < pq->events[smallest].priority) 
	{
        smallest = rightChild;
    }

    if (smallest != index) 
	{
        _swap(&pq->events[index], &pq->events[smallest]);
        _heapifyDown(pq, smallest);
    }
}

/********************** external functions definition ************************/

pq_handle_t *xPriorityQueueCreate(void) 
{
    pq_handle_t *pq = pvPortMalloc(sizeof(pq_handle_t));
	
    if (NULL == pq) {
        return NULL; // Memory allocation failed
    }
	
    pq->size = 0;
	
    pq->mutex = xSemaphoreCreateMutex();
    if (NULL == pq->mutex) 
	{
		vPortFree(pq);
        return NULL; // Mutex creation failed
    }
	
    pq->eventSemaphore = xSemaphoreCreateCounting(PQ_MAX_EVENT_SIZE, PQ_MAX_EVENT_SIZE);
    if (NULL == pq->eventSemaphore) 
	{
		vPortFree(pq);
        return NULL; // Semaphore creation failed
    }
	
    return pq;
}

BaseType_t xPriorityQueueSend(pq_handle_t *pq, pq_event_t *event, TickType_t ticksToWait) 
{
    if (pdTRUE == xSemaphoreTake(pq->mutex, ticksToWait)) 
	{
        if (PQ_MAX_EVENT_SIZE <= pq->size) 
		{
            // Error: Priority queue is full
            xSemaphoreGive(pq->mutex);
            return pdFAIL;
        }
		
        pq->events[pq->size] = *event;
        pq->size++;
        _heapifyUp(pq, pq->size - 1);
		
        xSemaphoreGive(pq->eventSemaphore); // Signal that a new event has been added
        xSemaphoreGive(pq->mutex);
		
        return pdPASS;
    }
    return pdFAIL;
}

BaseType_t xPriorityQueueReceive(pq_handle_t *pq, pq_event_t *event, TickType_t ticksToWait) 
{
    if (pdTRUE == xSemaphoreTake(pq->eventSemaphore, ticksToWait)) 
	{
        xSemaphoreTake(pq->mutex, portMAX_DELAY);
        if (0 < pq->size) 
		{
            *event = pq->events[0];
            pq->events[0] = pq->events[pq->size - 1];
            pq->size--;
            _heapifyDown(pq, 0);
        }		
        xSemaphoreGive(pq->mutex);
		
        return pdPASS;
    }
    return pdFAIL;
}

/********************** end of file ******************************************/
