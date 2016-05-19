
//First experiments with freertos.
//The tasks are initialized with static parameters. 
//They are not periodic because they simply wait a constant amount of time
// after the end of computation.


#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stdio.h> //extra
#include <string.h>
#include "stddef.h"
#include "FreeRTOS.h"
#include "task.h"
#include "list.h" 
#include "misc.h"
/* #include "queue.h" */
// This file is not to be included directly in this source. 
// RTOS reads it at compile time:
//#include "FreeRTOSConfig.h" 

TaskHandle_t T1_handle;
TaskHandle_t T2_handle;

const TickType_t xDelay = 200 / portTICK_PERIOD_MS;


// AO! void Delay(uint32_t nTime);
void ledinit(void);
static void Led1(void *pvParam);
static void Led2(void *pvParam);

#define PA5_ON() {GPIO_WriteBit(GPIOA , GPIO_Pin_5 , Bit_SET);}
#define PA5_OFF() {GPIO_WriteBit(GPIOA , GPIO_Pin_5 , Bit_RESET);}
#define PA6_ON() {GPIO_WriteBit(GPIOA , GPIO_Pin_6 , Bit_SET);}
#define PA6_OFF() {GPIO_WriteBit(GPIOA , GPIO_Pin_6 , Bit_RESET);}

int main(void)
{	
  
  NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );  //Needed for STM32F10x
  
  ledinit(); 
  
  xTaskCreate( Led1, "Task1", 128, NULL, 3, &T1_handle); 
  xTaskCreate( Led2, "Task2", 128, NULL, 2, &T2_handle); 
  vTaskStartScheduler();
  
  return 0;
}

// See if these may be useful later on: (AO!)
/* vTaskSuspend(xHandle); */
/* vTaskResume(xHandle); */
/* vTaskDelete(xHandle); */


void ledinit(){
  GPIO_InitTypeDef GPIO_InitStructure;
  // Enable Peripheral Clocks
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE); // (1)
  // Configure Pins
  GPIO_StructInit (& GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOA , &GPIO_InitStructure);
  // Configure SysTick Timer
  if (SysTick_Config(SystemCoreClock / 1000))
    while (1);  
}

static void Led1(void *pvParam) // This is Task1
{
  int i;
  float y;
  while(1){
    PA5_ON();
    for (i=0; i<20000; ++i){ // Burn off some processor cycles.
      y=30000.0/((float)i);
    }
    PA5_OFF();
    vTaskDelay(xDelay);
  }
}


static void Led2(void *pvParam)
{
  int i;
  float y;
  while(1){
    // toggle led
    PA6_ON();
    for (i=0; i<10000; ++i){ // Burn off some processor cycles.
      y=30000.0/((float)i);
    }
    PA6_OFF();
    vTaskDelay(xDelay);
  }
}


#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file , uint32_t line)
{
  /* Infinite loop */
  /* Use GDB to find out why we're here */ 
  while (1);
  
}
#endif 
