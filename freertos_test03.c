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

#define T1_PERIOD 500
#define T1_Priority 2 // Try swapping priorities of T1 and T2.

#define T2_PERIOD 300
#define T2_Priority 3


// Some sample task parameters:
typedef struct {
  TickType_t period;
  int var1;
  char char1;
  uint16_t GPIO_Pin;
}task_params;


void ledinit(void);
static void Task1(void *pvParam);
static void Task2(void *pvParam);

//Some convenient macros:
#define PA5 GPIO_Pin_5
#define PA6 GPIO_Pin_6
#define PA5_ON() {GPIO_WriteBit(GPIOA , GPIO_Pin_5 , Bit_SET);}
#define PA5_OFF() {GPIO_WriteBit(GPIOA , GPIO_Pin_5 , Bit_RESET);}
//#define PA6_ON() {GPIO_WriteBit(GPIOA , GPIO_Pin_6 , Bit_SET);}
//#define PA6_OFF() {GPIO_WriteBit(GPIOA , GPIO_Pin_6 , Bit_RESET);}

#define GPIOA_ON(pin) {GPIO_WriteBit(GPIOA , pin , Bit_SET);}
#define GPIOA_OFF(pin) {GPIO_WriteBit(GPIOA , pin , Bit_RESET);}

int main(void)
{	

  //The parameter structure must be created in the memory so that it exists
  // when the task is started (see FreeRTOS documentation): 
  task_params *T1Parameters;
  T1Parameters = pvPortMalloc(sizeof(task_params));
  T1Parameters->period = T1_PERIOD;
  T1Parameters->var1 = 3;    //Just some random variables to check.
  T1Parameters->char1 = 'c'; // same.
  T1Parameters->GPIO_Pin = PA6;

  NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );  //Needed for STM32F10x

  ledinit(); 

  //The following parameter passing does not work
  // (declaring T1Parameters as a local struct task_params). 
  // It compiles but does not work.
  // Apparently the values are not in the memory when task is initialized.
  // (void*) cast is automatically done.
  //xTaskCreate(Task1,"Task1",128,&T1Parameters,T1_Priority,&T1_handle); 
  //This works (declaration of a pointer which is then connected to a
  // structure created in the memory.:
  xTaskCreate(Task1,"Task1",128,T1Parameters,T1_Priority,&T1_handle); 

  // The following parameter passing is also possible (period):
  //xTaskCreate( Task2, "Task2", 128, (void*)300, T2_Priority, &T2_handle); 
  xTaskCreate(Task2,"Task2",128,(void*)T2_PERIOD,T2_Priority,&T2_handle);

 vTaskStartScheduler();
  
  return 0;
}


/////////////////////////////////////////////////////////////////////

static void Task1(void *Params)
{
  int i;
  float y;
  TickType_t StartTime; // This value is automatically updated.

  //Initialize a pointer to the passed task parameters:
  // So that we do not need to typecast it each time we use it:
  task_params* Parameters = (task_params*) Params;
  
  StartTime = xTaskGetTickCount();// The task starts at this time.

  i= Parameters->var1; // We can assign local variables as so.

  // This is the main part of the task:
  while(1){ 
    GPIOA_ON(Parameters->GPIO_Pin);
    // Burn off some processor cycles as the function of this task.
    for (i=0; i<30000; ++i){ 
      y=31111.0/((float)i);
      //It is necessary to do something with the result so that
      // the optimizer does not compile the instructions out:
      if (y==0){ 
	//PA5_OFF();
	//PA6_OFF();
	GPIOA_OFF(Parameters->GPIO_Pin);
      }
    }
    //PA5_OFF();
    //PA6_OFF();
    GPIOA_OFF(Parameters->GPIO_Pin);
    //vTaskDelayUntil(Release time, period)
    //Period is defined in clockticks.
    vTaskDelayUntil(&StartTime, Parameters->period); 
  }
}

/////////////////////////////////////////////////////////////////////

static void Task2(void *PERIOD)
{
  int i;
  float y;
  TickType_t pxPreviousWakeTime; // This value is automatically updated.

  while(1){
    //PA6_ON();
    PA5_ON();
    for (i=0; i<30000; ++i){ // Burn off some processor cycles.
      y=30000.0/((float)i);
      if (y==0){ 
	//PA6_OFF();
	PA5_OFF();
      }
    }
    //PA6_OFF();
    PA5_OFF();

    //Period in clockticks.
    vTaskDelayUntil(&pxPreviousWakeTime,(TickType_t)PERIOD); 
  }
}

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
