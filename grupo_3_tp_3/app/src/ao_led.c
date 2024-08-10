/*
 * Copyright (c) 2023 Sebastian Bedin <sebabedin@gmail.com>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @author : Sebastian Bedin <sebabedin@gmail.com>
 */

/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "cmsis_os.h"
#include "timers.h"
#include "board.h"
#include "logger.h"
#include "dwt.h"

#include "ao_led.h"

/********************** macros and definitions *******************************/
#define QUEUE_AO_LED_LENGTH_            (10)
#define QUEUE_AO_LED_ITEM_SIZE_         (sizeof(ao_led_message_t))
#define LED_ON_PERIOD_TICKS_			(TickType_t)(5000U / portTICK_PERIOD_MS)

#define WAIT_TIME   0U

/********************** internal data declaration ****************************/

ao_led_handle_t ao_led =
				{
					.info[RED].port 	= LD3_GPIO_Port,
					.info[RED].pin 	= LD3_Pin,
					.info[RED].state = GPIO_PIN_RESET,
					.info[RED].colour = "RED",

					.info[GREEN].port 	= LD1_GPIO_Port,
					.info[GREEN].pin 	= LD1_Pin,
					.info[GREEN].state = GPIO_PIN_RESET,
					.info[GREEN].colour = "GREEN",
					
					.info[BLUE].port 	= LD2_GPIO_Port,
					.info[BLUE].pin 	= LD2_Pin,
					.info[BLUE].state = GPIO_PIN_RESET,
					.info[BLUE].colour = "BLUE",
				};

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

/********************** external data definition *****************************/

/********************** internal functions definition ************************/

static void ao_task_(void *argument)
{
  ao_led_handle_t *hao = (ao_led_handle_t *)argument;

  LOGGER_INFO("AO LED \t- Task created");

  while (true)
  {
	pq_event_t evt;

    LOGGER_INFO("AO LED \t- Waiting event");

    while (pdPASS == xPriorityQueueReceive(hao->hpq, &evt, portMAX_DELAY))
    {
		LOGGER_INFO("AO LED \t- Receive AO_LED_MESSAGE_ON message");

		HAL_GPIO_WritePin(hao->info[evt.priority].port, hao->info[evt.priority].pin, GPIO_PIN_SET);
		LOGGER_INFO("AO LED \t- LED %s ON", hao->info[evt.priority].colour);
		vTaskDelay(LED_ON_PERIOD_TICKS_);

		HAL_GPIO_WritePin(hao->info[evt.priority].port, hao->info[evt.priority].pin, GPIO_PIN_RESET);
		LOGGER_INFO("AO LED \t- LED %s OFF", hao->info[evt.priority].colour);
    }
  }
}

/********************** external functions definition ************************/

void ao_led_init(ao_led_handle_t* hao_led)
{
  // Queues
  hao_led->hpq = xPriorityQueueCreate();
  configASSERT(NULL != hao_led->hpq);

  // Tasks
  BaseType_t status;
  status = xTaskCreate
		  (
			  ao_task_,
			  "task_ao_led",
			  128,
			  (void* const)hao_led,
			  (tskIDLE_PRIORITY + 1),
			  &hao_led->htask
		  );

  configASSERT(pdPASS == status);
}

bool ao_led_send(ao_led_handle_t* hao_led, pq_event_t evt)
{
	return (pdPASS == xPriorityQueueSend(hao_led->hpq, (void*)&evt, (TickType_t)0U));
}

/********************** end of file ******************************************/
