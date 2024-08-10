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
#include "board.h"
#include "logger.h"
#include "dwt.h"
#include "ao_ui.h"

/********************** macros and definitions *******************************/
#define QUEUE_AO_UI_LENGTH_            (5)
#define QUEUE_AO_UI_ITEM_SIZE_         (sizeof(ao_ui_message_t))

/********************** internal data declaration ****************************/
ao_ui_handle_t ao_ui;

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

/********************** external data definition *****************************/

/********************** internal functions definition ************************/

/********************** external functions definition ************************/

static void ao_task_(void *argument)
{
	ao_ui_handle_t *hao_ui = (ao_ui_handle_t *)argument;

	LOGGER_INFO("AO UI \t- Started");

	while (true)
	{
		ao_ui_message_t rcvEvt;
		pq_event_t sendEvt;

		LOGGER_INFO("AO UI\t- Waiting event");

		if(pdPASS == xQueueReceive(hao_ui->hqueue, &rcvEvt, portMAX_DELAY))
		{
			switch(rcvEvt)
			{
				case AO_UI_MESSAGE_PULSE:
					sendEvt.priority = HIGH_PRIORITY;					
					ao_led_send(&ao_led, sendEvt); // @todo: check return
					
					LOGGER_INFO("AO UI\t- Send a HIGH_PRIORITY event to the priority queue");
					break;

				case AO_UI_MESSAGE_SHORT:					
					sendEvt.priority = MEDIUM_PRIORITY;					
					ao_led_send(&ao_led, sendEvt); // @todo: check return
					
					LOGGER_INFO("AO UI\t- Send a MEDIUM_PRIORITY event to the priority queue");
					break;

				case AO_UI_MESSAGE_LONG:			
					sendEvt.priority = LOW_PRIORITY;					
					ao_led_send(&ao_led, sendEvt); // @todo: check return
					
					LOGGER_INFO("AO UI\t- Send a LOW_PRIORITY event to the priority queue");
					break;

				default:
					LOGGER_LOG("AO UI\t- ERROR - Bad message");
					break;
			}
		}
	}
}

/********************** internal functions definition ************************/
void ao_ui_init(ao_ui_handle_t *hao_ui)
{
	  hao_ui->hqueue  = xQueueCreate(QUEUE_AO_UI_LENGTH_, QUEUE_AO_UI_ITEM_SIZE_);
	  configASSERT(NULL != hao_ui->hqueue);

	  BaseType_t status;
	  status = xTaskCreate
			  (
				  ao_task_,
				  "task_ao_ui",
				  128,
				  (void* const)hao_ui,
				  (tskIDLE_PRIORITY + 1),
				  &hao_ui->htask
			  );
	  configASSERT(pdPASS == status);
}

bool ao_ui_send(ao_ui_handle_t *hao_ui, ao_ui_message_t msg)
{
	return (pdPASS == xQueueSend(hao_ui->hqueue, (void*)&msg, (TickType_t)0U));
}

/********************** end of file ******************************************/
