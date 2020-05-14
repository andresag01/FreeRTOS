#ifndef __FREERTOS_CONFIG_H__
#define __FREERTOS_CONFIG_H__

#include <assert.h>

#define configUSE_NEWLIB_REENTRANT 0

/*
 * The minimal stack size MUST remain at 15 words so that the following can be
 * accommodated:
 *	- Callee wp
 *	- 14 registers
 */
#define configMINIMAL_STACK_SIZE ( 15 )

#define configMAX_PRIORITIES     5
#define configUSE_PREEMPTION     1
#define configUSE_IDLE_HOOK      0

// This clock speed is here for completeness but is largely meaningless in a
// functional simulation, so we do not use it.
#define configCPU_CLOCK_HZ       ( ( unsigned long ) 58982400 )
#define configTICK_RATE_HZ       ( ( TickType_t ) 1000 )
#define configUSE_TICK_HOOK      0
#define configUSE_16_BIT_TICKS   0

#define configUSE_CO_ROUTINES    0

#define INCLUDE_vTaskPrioritySet             1
#define INCLUDE_uxTaskPriorityGet            1
#define INCLUDE_vTaskDelete                  1
#define INCLUDE_vTaskSuspend                 1
#define INCLUDE_vTaskDelayUntil              1
#define INCLUDE_vTaskDelay                   1
#define INCLUDE_xTaskGetIdleTaskHandle       0
#define INCLUDE_xTaskAbortDelay              0
#define INCLUDE_xQueueGetMutexHolder         0
#define INCLUDE_xSemaphoreGetMutexHolder     0
#define INCLUDE_xTaskGetHandle               0
#define INCLUDE_uxTaskGetStackHighWaterMark  0
#define INCLUDE_uxTaskGetStackHighWaterMark2 0
#define INCLUDE_eTaskGetState                0
#define INCLUDE_xTaskResumeFromISR           0
#define INCLUDE_xTimerPendFunctionCall       0
#define INCLUDE_xTaskGetSchedulerState       0
#define INCLUDE_xTaskGetCurrentTaskHandle    0

#define configUSE_DAEMON_TASK_STARTUP_HOOK      0
#define configUSE_APPLICATION_TASK_TAG          0
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS 0
#define configUSE_RECURSIVE_MUTEXES             0
#define configUSE_MUTEXES                       0
#define configUSE_TIMERS                        0
#define configUSE_COUNTING_SEMAPHORES           0
#define configUSE_ALTERNATIVE_API               0

/*
 * This config can be eliminated by dynamically allocating the pcTaskName field
 * from the tskTaskControlBlock struct.
 */
#define configMAX_TASK_NAME_LEN 16
#define configIDLE_SHOULD_YIELD 1

#define configASSERT( x ) assert( x )

#define configCHECK_FOR_STACK_OVERFLOW 0
#define configRECORD_STACK_HIGH_ADDRESS 0

#define configINCLUDE_FREERTOS_TASK_C_ADDITIONS_H 0

#define configUSE_MALLOC_FAILED_HOOK 0

#define configUSE_TRACE_FACILITY 0

#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0
#define configAPPLICATION_ALLOCATED_HEAP 0

#define configUSE_TASK_NOTIFICATIONS 0

#define configUSE_POSIX_ERRNO 0

#define configSUPPORT_STATIC_ALLOCATION 0
#define configSUPPORT_DYNAMIC_ALLOCATION 1

#define configSTACK_DEPTH_TYPE uint16_t
#define configMESSAGE_BUFFER_LENGTH_TYPE size_t

#define configENABLE_BACKWARD_COMPATIBILITY 0

#define configUSE_TASK_FPU_SUPPORT 0
#define configENABLE_MPU 0
#define configENABLE_FPU 0
#define configENABLE_TRUSTZONE 0
#define configRUN_FREERTOS_SECURE_ONLY 0

#endif /* __FREERTOS_CONFIG_H__ */
