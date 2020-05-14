/* Standard includes. */
#include "utils.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/*-----------------------------------------------------------*/

/* Priorities for the demo application tasks. */
#define mainQUEUE_RECEIVE_TASK_PRIORITY ( tskIDLE_PRIORITY + 1 )
#define mainQUEUE_SEND_TASK_PRIORITY    ( tskIDLE_PRIORITY + 2 )

/* The rate at which data is sent to the queue. */
#define mainQUEUE_SEND_FREQUENCY_MS			( 1000 )

/*
 * The number of items the queue can hold. This is 1 as the receive task
 * will remove items as they are added, meaning the send task should always
 * find the queue empty. */
#define mainQUEUE_LENGTH					( 1 )

/*-----------------------------------------------------------*/

/*
 * Configure the processor for use with the Olimex demo board.  This includes
 * setup for the I/O, system clock, and access timings.
 */
static void prvSetupHardware( void );

/*
 * The tasks as described in the comments at the top of this file.
 */
static void prvQueueReceiveTask( void *pvParameters );
static void prvQueueSendTask( void *pvParameters );

/*-----------------------------------------------------------*/

/* The queue used by both tasks. */
static QueueHandle_t xQueue = NULL;

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

    /* Check whether the queue was correctly allocated */
    xQueue = xQueueCreate( mainQUEUE_LENGTH, sizeof( uint32_t ) );
    if( xQueue == NULL )
    {
        printString("Failed to allocate xQueue\n");
        fail();
    }

    /* Start the demo/test application tasks */
    printString("Setting up ReceiverTask...\n");
    xTaskCreate( prvQueueReceiveTask,
                 "Rx",
                 configMINIMAL_STACK_SIZE,
                 NULL,
                 mainQUEUE_RECEIVE_TASK_PRIORITY,
                 NULL );

    /* Start the check task - which is defined in this file. */
    printString("Setting up SendTask...\n");
    xTaskCreate( prvQueueSendTask,
                 "Tx",
                 configMINIMAL_STACK_SIZE,
                 NULL,
                 mainQUEUE_SEND_TASK_PRIORITY,
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

static void prvQueueSendTask( void *pvParameters )
{
    TickType_t xNextWakeTime;
    const uint32_t ulValueToSend = 100UL;

	/* Remove compiler warning about unused parameter. */
	( void ) pvParameters;

	/* Initialise xNextWakeTime - this only needs to be done once. */
	xNextWakeTime = xTaskGetTickCount();

	for( ;; )
	{
		/* Place this task in the blocked state until it is time to run again.
         */
		vTaskDelayUntil( &xNextWakeTime, mainQUEUE_SEND_FREQUENCY_MS );

		/* Send to the queue - causing the queue receive task to unblock and
		toggle the LED.  0 is used as the block time so the sending operation
		will not block - it shouldn't need to block as the queue should always
		be empty at this point in the code. */
		xQueueSend( xQueue, &ulValueToSend, 0U );
	}
}

/*-----------------------------------------------------------*/

static void prvQueueReceiveTask( void *pvParameters )
{
    uint32_t ulReceivedValue;
    uint32_t ulReceivedCount = 0;
    const uint32_t ulExpectedValue = 100UL;
    const TickType_t xShortDelay = pdMS_TO_TICKS( 10 );

	/* Remove compiler warning about unused parameter. */
	( void ) pvParameters;

	for( ;; )
	{
        /* Overwrite the receive buffer with a wrong value */
        ulReceivedValue = 0xffffffff;

		/* Wait until something arrives in the queue - this task will block
		indefinitely provided INCLUDE_vTaskSuspend is set to 1 in
		FreeRTOSConfig.h. */
		xQueueReceive( xQueue, &ulReceivedValue, portMAX_DELAY );

		/*  To get here something must have been received from the queue, but
		is it the expected value?  If it is, toggle the LED. */
		if( ulReceivedValue == ulExpectedValue )
		{
            printString( "Success: " );
            printHex( ulReceivedCount );
            putchar('\n');
            ulReceivedCount++;

			ulReceivedValue = 0U;
		}
        else
        {
            fail();
        }
	}
}
