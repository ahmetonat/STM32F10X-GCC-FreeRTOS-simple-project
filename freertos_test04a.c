
/************************************************************************
* Simple example FreeRTOS project for STM32F103.
* Creates two periodic tasks with
*  different static priorities and other parameters.
* The tasks perform several thousand floating point divisions to burn off
*  CPU time.
* A logic analyzer can be used to trace run times of tasks.
* Only one task function is created. The two tasks share the same function
*  call. Their parameters are passed within a structure from main.
* ************************************************************************/


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
// This header is not to be included directly in this source. 
// RTOS reads it at compile time:
//#include "FreeRTOSConfig.h" 

TaskHandle_t T1_handle;
TaskHandle_t T2_handle;

#define T1_PERIOD 50
#define T1_Priority 2 // Try swapping priorities of T1 and T2.

#define T2_PERIOD 30
#define T2_Priority 3

#define T1_OPERATION_COUNT 2500
#define T2_OPERATION_COUNT 2500

// Parameters to be passed to tasks:
typedef struct {
  TickType_t period;
  int operation_count;
  uint16_t GPIO_Pin;
}task_params;

//Some convenient macros:
#define PA5 GPIO_Pin_5
#define PA6 GPIO_Pin_6
#define GPIOA_ON(pin) {GPIO_WriteBit(GPIOA , pin , Bit_SET);}
#define GPIOA_OFF(pin) {GPIO_WriteBit(GPIOA , pin , Bit_RESET);}

void ledinit(void);
static void Task(void *pvParam); //Single task function is sufficient...

/////////////////////////////////////////////////////////////////////


int main(void)
{	

  //The parameter structure must be created in the memory so that it exists
  // when the task is started (see FreeRTOS documentation): 
  task_params *T1Parameters;
  T1Parameters = pvPortMalloc(sizeof(task_params));
  T1Parameters->period = T1_PERIOD;
  T1Parameters->operation_count = T1_OPERATION_COUNT;
  T1Parameters->GPIO_Pin = PA6;

  task_params *T2Parameters;
  T2Parameters = pvPortMalloc(sizeof(task_params));
  T2Parameters->period = T2_PERIOD;
  T2Parameters->operation_count = T2_OPERATION_COUNT;
  T2Parameters->GPIO_Pin = PA5;


  NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );  //Needed for STM32F10x

  ledinit(); 

  xTaskCreate(Task,"Task1",128,T1Parameters,T1_Priority,&T1_handle); 
  xTaskCreate(Task,"Task2",128,T2Parameters,T2_Priority,&T2_handle);
  //Task parameters are automatically cast as (void *) in function call.

  vTaskStartScheduler();  //Finally, start the scheduler.
  
  return 0;
}


/////////////////////////////////////////////////////////////////////

static void Task(void *Params)
{
  int i;
  float y;
  TickType_t StartTime; // This value is automatically updated.

  //Initialize a pointer to the passed task parameter structure:
  // So that we do not need to typecast it each time we use it:
  task_params* Parameters = (task_params*) Params;
  
  StartTime = xTaskGetTickCount();// The task starts at this time.


  // This is the main part of the task:
  while(1){ 
    GPIOA_ON(Parameters->GPIO_Pin);
    // Burn off some processor cycles as the function of this task.
    for (i=0; i<Parameters->operation_count; ++i){ 
      y=31111.0/((float)i); // the constant is just some random value.
      //It is necessary to do something with the result so that
      // the optimizer does not compile the instructions out:
      if (y==0){ 
	GPIOA_OFF(Parameters->GPIO_Pin);
      }
    }
    GPIOA_OFF(Parameters->GPIO_Pin);
    //Prototype of vTaskDelayUntil(Release time, period)
    //Period is defined in clockticks.
    vTaskDelayUntil(&StartTime, Parameters->period); 
  }
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

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

// See if these may be useful later on: (AO!)
/* vTaskSuspend(xHandle); */
/* vTaskResume(xHandle); */
/* vTaskDelete(xHandle); */
/* vTaskDelay(xDelay); */

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file , uint32_t line)
{
  /* Infinite loop */
  /* Use GDB to find out why we're here */ 
  while (1);
  
}
#endif 
