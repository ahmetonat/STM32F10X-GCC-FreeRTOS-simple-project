# STM32F10X-GCC-FreeRTOS-simple-project
A sample project implementing periodic functions with passed parameters running on STM32F103 and GCC ARM Embedded.

See the blog entry at:

http://aviatorahmet.blogspot.com/2016/05/freertos-simple-project-for-stm32f103.html

This is an example of how two periodic tasks can be created in FreeRTOS. It is intended as a starting point where the basic setup has been completed and working. It is easy to modify to your own needs.

GCC ARM EMBEDDED is used, running on a STM32F103RB processor.
The tests were done on a STM32 Nucleo FRB103 board.
FreeRTOS version is V8.2.1, which is recent at the time of writing.

There are some pitfalls in the initial setup of the processor:

First: FreeRTOSConfig.h. It is necessary to set pointers to interrupt handlers to those provided by the FreeRTOS. This is done in the lines: 

//Needed for the STM32F10x processors:

/#define vPortSVCHandler SVC_Handler

/#define xPortPendSVHandler PendSV_Handler

/#define xPortSysTickHandler SysTick_Handler

The remaining series of lines in FreeRTOSConfig.h are to set the properties of the processor etc. They are fairly easy to understand.

Second: The main body of the program; freertos_testXX.c. This sets up the processor to run the tasks. It is fairly easy to understand and get running,  using the FreeRTOS supplied functions. It is necessary to add the line to initialize the nested vectored interrupt controller on the STM32F103:

NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );  //Needed for STM32F10x

Sometimes it is necessary to initiate tasks with parameters from a structure. An example of that is shown in the creation of task1. Task2 is simpler, in which a literal (number) is passed as a parameter and used as its period. Task priorities etc. can be set as desired, as documented in the FreeRTOS homepage.

The project is intended to run on a STM32 Nucleo F103RB board. But any similar isfine.

Several versions of freertos_testxx.c have been included. The project can be built using any of them by changing the Makefile. I wrote them when working with the project. Their function is shown below:

freertos_test01.c

First tests of periodic task creation using vTaskDelayUntil. Tasks are created with no passed data structures. Their operation as intended was confirmed.

As set up, T1 and T2 are "periodic" tasks; T2 has lower priority than T1. T2 will only run when T1 is done. T1 flashes the on board LED when it is running. Since T1 has a long execution time, the start of T2 is delayed, and you can see this in the flashing pattern of T2; it is irregular. However, when we exchange the priorities and T2 becomes the high priority task, its flashing becomes regular; whenever it is released, the RTOS kernel starts to run it. 


freertos_test02.c

Same as previous.

Added a single value data passed to the task; its period. The passed value is declared in the function call as a void pointer. In the target function body, it must be either assigned to the correct pointer type (by declaring a pointer of the correct type, and equating it to the function argument void pointer through a suitable type-cast), or each time it is used, type-cast it to the correct pointer type. This may even be done implicitly since the function argument where this is used, or the left hand side of the equality will dictate a correct type-cast. However, better not to leave it to the compiler as implicit; some ambiguity might break the program.

Tasks are truly periodic (in the sense of FreeRTOS) in which vTaskDelayUntil() function call is used.

It works fine.

freertos_test03.c

Same as previous.

Added a data structure to pass the data to the task. However, the data structure must be in the memory when the task runs. So that precludes the possibility of creating a data structure on the fly for the task in main() and remove it after task creation. It must be there all the time.

The data structure must be created by using a malloc call (apparently this costs time, but it is done only during initialization, so it is possible). 

TODO: Check if malloc is allowed in the heap-1 model.

When created correctly, the data is passed to the task correctly. If a normal structure declaration is used within the main function, then apparently, the values are on the stack of main, and are not available to the calling function. The latter method does not work. Program compiles but the tasks do not get the intended values.


freertos_test04.c

Tasks receive all the critical values through the passed variable structure, i.e. period, number of instructions, and GPIO port to toggle for execution. This simplifies the structure for demos, because only 1 task function can be written, and shared among all created tasks.

freertos_test04a.c

Only 1 task function is written and all tasks are created as different instances of this. Their parameters are set by the passed data structure during the creation function call. This works fine. The body of the same task is called with different arguments to create several tasks.
