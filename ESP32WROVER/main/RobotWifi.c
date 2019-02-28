/* Robot WIFI Project

   This project code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

/* Can run 'make menuconfig' to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define LED_GPIO        CONFIG_LED_GPIO
#define LED_PIN_SEL     (1ULL << LED_GPIO)
#define BUTTON_GPIO     CONFIG_BTN_GPIO
#define BUTTON_PIN_SEL  (1ULL << BUTTON_GPIO)
#define ESP_INTR_FLAG_DEFAULT 0

static xQueueHandle button_evt_queue = NULL;

void blink_task(void *pvParameter)
{
    /* Configure the IOMUX register for pad BLINK_GPIO (some pads are
       muxed to GPIO on reset already, but some default to other
       functions and need to be switched to GPIO. Consult the
       Technical Reference for a list of pads and their default
       functions.)
    */
//    gpio_pad_select_gpio(LED_GPIO);
    /* Set the GPIO as a push/pull output */
//    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
    while(1) {
        /* Blink off (output low) */
        gpio_set_level(LED_GPIO, 0);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        /* Blink on (output high) */
        gpio_set_level(LED_GPIO, 1);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

static void IRAM_ATTR button_isr_handler(void* arg)
{
	uint32_t gpio_num = (uint32_t) arg;
	xQueueSendFromISR(button_evt_queue, &gpio_num, NULL);
}

static void button_pressed_task(void* arg)
{
    uint32_t io_num;
    for(;;) {
        if(xQueueReceive(button_evt_queue, &io_num, portMAX_DELAY)) {
            printf("GPIO[%d] intr, val: %d\n", io_num, gpio_get_level(io_num));
        }
    }
}

static void bsp_init(void)
{
	gpio_config_t io_conf;
	//disable interrupt
	io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
	//set as output mode
	io_conf.mode = GPIO_MODE_OUTPUT;
	//bit mask of the pins that you want to set,e.g.GPIO26
	io_conf.pin_bit_mask = LED_PIN_SEL;
	//disable pull-down mode
	io_conf.pull_down_en = 0;
	//disable pull-up mode
	io_conf.pull_up_en = 0;
	//configure GPIO with the given settings
	gpio_config(&io_conf);

	//interrupt of falling edge
	io_conf.intr_type = GPIO_PIN_INTR_POSEDGE;
	//bit mask of the pins, use GPIO34 here
	io_conf.pin_bit_mask = BUTTON_PIN_SEL;
	//set as input mode
	io_conf.mode = GPIO_MODE_INPUT;
	//disable pull-up mode
	io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
	gpio_config(&io_conf);

	//create a queue to handle button event from isr
	button_evt_queue = xQueueCreate(5, sizeof(uint32_t));
	//start button task
	xTaskCreate(button_pressed_task, "pressed task", configMINIMAL_STACK_SIZE, NULL, 10, NULL);

	//install gpio isr service
	gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
	//hook isr handler for specific gpio pin
	gpio_isr_handler_add(BUTTON_GPIO, button_isr_handler, (void*) BUTTON_GPIO);
}

void app_main()
{
	bsp_init();
    xTaskCreate(&blink_task, "blink_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
}
