/* Standard includes. */
#include "utils.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "countsem.h"

/*-----------------------------------------------------------*/

/* Priorities for the demo application tasks. */
#define mainCHECK_TASK_PRIORITY				( configMAX_PRIORITIES - 1 )

/* The period of the check task, in ms, provided no errors have been reported by
any of the standard demo tasks.  ms are converted to the equivalent in ticks
using the pdMS_TO_TICKS() macro constant. */
#define mainNO_ERROR_CHECK_TASK_PERIOD		( 300UL )

/*-----------------------------------------------------------*/

/*
 * Configure the processor for use with the Olimex demo board.  This includes
 * setup for the I/O, system clock, and access timings.
 */
static void prvSetupHardware( void );

/*
 * The check task, as described at the top of this file.
 */
static void prvCheckTask( void *pvParameters );

/*-----------------------------------------------------------*/

/*
 * Starts all the other tasks, then starts the scheduler. The processor is in
 * INTERRUPT mode when executing this function.
 */
int main( void )
{
    /* Set up the hardware for use */
    printString("Setting up hardware...\n");
    prvSetupHardware();

    /* Start the demo/test application tasks */
    printString("Setting up semaphore tasks...\n");
    vStartCountingSemaphoreTasks();

    /* Create the task that performs the 'check' functionality */
    xTaskCreate( prvCheckTask,
                 "Check",
                 configMINIMAL_STACK_SIZE,
                 NULL,
                 mainCHECK_TASK_PRIORITY,
                 NULL );

	/* Now all the tasks have been started - start the scheduler.

	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used here. */
    printString("Starting scheduler...\n");
	vTaskStartScheduler();

	/*
     * Return "out" of INTERRUPT mode. This will transfer control to
     * exitInterruptHandler that contains only a couple of instructions to
     * set the correct interrupt handler address. See startup.asm for more
     * information on how this mechanism works
     */
    printString("Terminating setup...\n");
	return 0;
}

/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
    // Do nothing... This is a functional simulator so there is nothing to set
    // up for now. Also, the compatibility mode and stacks (for INTERRUPT,
    // EXCEPTION and PANIC modes) were already created in startup.asm
}

/*-----------------------------------------------------------*/

static void prvCheckTask( void *pvParameters )
{
    TickType_t xDelayPeriod = mainNO_ERROR_CHECK_TASK_PERIOD;
    TickType_t xLastExecutionTime;
    unsigned long ulErrorFound = pdFALSE;
    unsigned long ulCheckCount = 0;

	/* Just to stop compiler warnings. */
	( void ) pvParameters;

	/* Initialise xLastExecutionTime so the first call to vTaskDelayUntil()
	works correctly. */
	xLastExecutionTime = xTaskGetTickCount();

	/* Cycle for ever, delaying then checking all the other tasks are still
	operating without error.  The on board LED is toggled on each iteration.
	If an error is detected then the delay period is decreased from
	mainNO_ERROR_CHECK_TASK_PERIOD to mainERROR_CHECK_TASK_PERIOD.  This has the
	effect of increasing the rate at which the on board LED toggles, and in so
	doing gives visual feedback of the system status. */
	for( ;; )
	{
		/* Delay until it is time to execute again. */
		vTaskDelayUntil( &xLastExecutionTime, xDelayPeriod );

		/* Check all the demo tasks (other than the flash tasks) to ensure
		that they are all still running, and that none have detected an error. */
		if( xAreCountingSemaphoreTasksStillRunning() != pdPASS )
		{
			ulErrorFound = pdTRUE;
		}

		if( ulErrorFound != pdFALSE )
		{
			/* An error has been detected in one of the tasks - flash the LED
			at a higher frequency to give visible feedback that something has
			gone wrong. */
            fail();
		}
        else
        {
            printString( "Success: " );
            printHex( ulCheckCount );
            putchar( '\n' );

            ulCheckCount++;
		}
	}
}
