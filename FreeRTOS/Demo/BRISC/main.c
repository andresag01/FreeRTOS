/* Standard includes. */
#include "utils.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Demo application includes. */
#include "integer.h"

/*-----------------------------------------------------------*/

/* Priorities for the demo application tasks. */
#define mainCHECK_TASK_PRIORITY		( tskIDLE_PRIORITY + 4 )

/* The rate at which the errors are checked. */
#define mainNO_ERROR_DELAY_PERIOD	( ( TickType_t ) 3000 / portTICK_PERIOD_MS  )

/*-----------------------------------------------------------*/

/*
 * Checks that all the demo application tasks are still executing without error
 * - as described at the top of the file.
 */
static long prvCheckOtherTasksAreStillRunning( void );

/*
 * The task that executes at the highest priority and calls
 * prvCheckOtherTasksAreStillRunning().  See the description at the top
 * of the file.
 */
static void vErrorChecks( void *pvParameters );

/*
 * Configure the processor for use with the Olimex demo board.  This includes
 * setup for the I/O, system clock, and access timings.
 */
static void prvSetupHardware( void );

/*-----------------------------------------------------------*/

/*
 * Starts all the other tasks, then starts the scheduler.
 */
int main( void )
{
    /* Set up the hardware for use */
    prvSetupHardware();

    /* Start the demo/test application tasks */
    vStartIntegerMathTasks( tskIDLE_PRIORITY );

    /* Start the check task - which is defined in this file. */
    xTaskCreate( vErrorChecks,
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
	vTaskStartScheduler();

	/* Should never reach here! */
	return 0;
}

/*-----------------------------------------------------------*/

static void vErrorChecks( void *pvParameters )
{
	/* The parameters are not used in this function. */
	( void ) pvParameters;

	/* Cycle for ever, delaying then checking all the other tasks are still
	operating without error.  If an error is detected then raise an exception.
    */

	for( ;; )
	{
		/* Delay until it is time to execute again. */
		vTaskDelay( mainNO_ERROR_DELAY_PERIOD );

		/* Check all the standard demo application tasks are executing without
		error.  ulMemCheckTaskRunningCount is checked to ensure it was
		modified by the task just deleted. */
		if( prvCheckOtherTasksAreStillRunning() != pdPASS )
		{
			/* An error has been detected in one of the tasks - flash faster. */
            /* TODO: Work out how to raise an exception */
            fail( 1 );
		}
	}
}

/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
}

/*-----------------------------------------------------------*/

static long prvCheckOtherTasksAreStillRunning( void )
{
    long lReturn = ( long ) pdPASS;

	/* Check all the demo tasks (other than the flash tasks) to ensure
	that they are all still running, and that none of them have detected
	an error. */

	if( xAreIntegerMathsTaskStillRunning() != pdTRUE )
	{
		lReturn = ( long ) pdFAIL;
	}

	return lReturn;
}
