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

#define T1PERIOD 200
#define T2PERIOD 300

const TickType_t xDelay = 200/portTICK_PERIOD_MS;


void ledinit(void);
static void Led1(void *pvParam);
static void Led2(void *pvParam);

// Some convenient macros:
#define PA5_ON() {GPIO_WriteBit(GPIOA , GPIO_Pin_5 , Bit_SET);}
#define PA5_OFF() {GPIO_WriteBit(GPIOA , GPIO_Pin_5 , Bit_RESET);}
#define PA6_ON() {GPIO_WriteBit(GPIOA , GPIO_Pin_6 , Bit_SET);}
#define PA6_OFF() {GPIO_WriteBit(GPIOA , GPIO_Pin_6 , Bit_RESET);}

int main(void)
{	
  
//Define the period of the task.
  static const TickType_t T1_PERIOD= 1000; 
  static const TickType_t T2_PERIOD= 300;

  #define T1_Priority 3
  #define T2_Priority 2

  NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );  //Needed for STM32F10x
  
  ledinit(); 

  xTaskCreate( Led1, "Task1", 128, (void*)T1_PERIOD, T1_Priority, &T1_handle); 
  xTaskCreate( Led2, "Task2", 128, (void*)T2_PERIOD, T2_Priority, &T2_handle);
  // The following period assignment is also possible:
  //xTaskCreate( Led2, "Task2", 128, (void*)300, T2_Priority, &T2_handle); 

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

static void Led1(void *PERIOD) // This is Task1
{
  int i;
  float y;
  TickType_t pxPreviousWakeTime; // This value is automatically updated.

  pxPreviousWakeTime = xTaskGetTickCount();

  while(1){
    PA5_ON();
    for (i=0; i<20000; ++i){ // Burn off some processor cycles.
      y=30000.0/((float)i);  // AO: later realized this is not compiled in because of optimization...
    }
    PA5_OFF();
    //vTaskDelay(xDelay);
    //Period in clockticks.
    vTaskDelayUntil(&pxPreviousWakeTime, (TickType_t)PERIOD); 
  }

  //Should never come here. If it does, we delete it explicitly.
  vTaskDelete(NULL); 
}

// Try waitUntil function for periodic task implementation.

static void Led2(void *pvParam)
{
  int i;
  float y;
  TickType_t pxPreviousWakeTime; // This value is automatically updated.
  const TickType_t PERIOD=300;
  while(1){
    // toggle led
    PA6_ON();
    for (i=0; i<10000; ++i){ // Burn off some processor cycles.
      y=30000.0/((float)i);
    }
    PA6_OFF();
    //vTaskDelay(xDelay);
    vTaskDelayUntil(&pxPreviousWakeTime, PERIOD); //Period in clockticks.
  }
  //Should never come here. If it does, we delete it explicitly.
  vTaskDelete(NULL); 
}


#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file , uint32_t line)
{
  /* Infinite loop */
  /* Use GDB to find out why we're here */ 
  while (1);
  
}
#endif 
