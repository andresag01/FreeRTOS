/* Scheduler includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

/* Demo application includes. */
#include "serial.h"
#include "utils.h"

/*-----------------------------------------------------------*/

/* Constants to setup and access the UART. */
#define portUSART0_AIC_CHANNEL	( ( unsigned long ) 2 )

#define serINVALID_QUEUE		( ( QueueHandle_t ) 0 )
#define serHANDLE				( ( xComPortHandle ) 1 )
#define serNO_BLOCK				( ( TickType_t ) 0 )

/*-----------------------------------------------------------*/

/* Queues used to hold received characters, and characters waiting to be
transmitted. */
QueueHandle_t xRxedChars;
QueueHandle_t xCharsForTx;

/*-----------------------------------------------------------*/

static void vSerialISRCreateQueues( unsigned portBASE_TYPE uxQueueLength, QueueHandle_t *pxRxedChars, QueueHandle_t *pxCharsForTx )
{
	/* Create the queues used to hold Rx and Tx characters. */
	xRxedChars = xQueueCreate( uxQueueLength, ( unsigned portBASE_TYPE ) sizeof( signed char ) );
	xCharsForTx = xQueueCreate( uxQueueLength + 1, ( unsigned portBASE_TYPE ) sizeof( signed char ) );

	/* Pass back a reference to the queues so the serial API file can
	post/receive characters. */
	*pxRxedChars = xRxedChars;
	*pxCharsForTx = xCharsForTx;
}

/*-----------------------------------------------------------*/

xComPortHandle xSerialPortInitMinimal( unsigned long ulWantedBaud, unsigned portBASE_TYPE uxQueueLength )
{
	xComPortHandle xReturn = serHANDLE;
	volatile UART_t *uart = (UART_t *)io[INTERRUPT_REASON_UART0];
	volatile InterruptConfig_t *icfg =
		(InterruptConfig_t *)io[INTERRUPT_REASON_ICFG];

	/* The queues are used in the serial ISR routine, so are created from
	serialISR.c (which is always compiled to ARM mode. */
	vSerialISRCreateQueues( uxQueueLength, &xRxedChars, &xCharsForTx );

	if( ( xRxedChars != serINVALID_QUEUE ) &&
		( xCharsForTx != serINVALID_QUEUE ) &&
		( ulWantedBaud != ( unsigned long ) 0 ) )
	{
		/* Enable UART0 interrupts */
		icfg->enbdev = 1 << INTERRUPT_REASON_UART0;

		/* Disable UART0 interrupt sources to begin... */
		uart->status = 0x00000000;

		/* Enable Rx and Tx operations */
		uart->status |= (3 <<	0);

		/* Enable Rx interrupt sources */
		uart->status |= 1 << 2;
	}
	else
	{
		xReturn = ( xComPortHandle ) 0;
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xSerialGetChar( xComPortHandle pxPort, signed char *pcRxedChar, TickType_t xBlockTime )
{
	/* The port handle is not required as this driver only supports UART0. */
	( void ) pxPort;

	/* Get the next character from the buffer.  Return false if no characters
	are available, or arrive before xBlockTime expires. */
	if( xQueueReceive( xRxedChars, pcRxedChar, xBlockTime ) )
	{
		printString("GetChar\n");
		return pdTRUE;
	}
	else
	{
		return pdFALSE;
	}
}
/*-----------------------------------------------------------*/

void vSerialPutString( xComPortHandle pxPort, const signed char * const pcString, unsigned short usStringLength )
{
	signed char *pxNext;

	/* NOTE: This implementation does not handle the queue being full as no
	block time is used! */

	/* The port handle is not required as this driver only supports UART0. */
	( void ) pxPort;
	( void ) usStringLength;

	/* Send each character in the string, one at a time. */
	pxNext = ( signed char * ) pcString;
	while( *pxNext )
	{
		xSerialPutChar( pxPort, *pxNext, serNO_BLOCK );
		pxNext++;
	}
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xSerialPutChar( xComPortHandle pxPort, signed char cOutChar, TickType_t xBlockTime )
{
	UART_t *uart = ( UART_t * )io[INTERRUPT_REASON_UART0];
	( void ) pxPort;

	/* Place the character in the queue of characters to be transmitted. */
	if( xQueueSend( xCharsForTx, &cOutChar, xBlockTime ) != pdPASS )
	{
		return pdFAIL;
	}

	printString("PutChar\n");

	/* Turn on the Tx interrupt so the ISR will remove the character from the
	queue and send it.   This does not need to be in a critical section as
	if the interrupt has already removed the character the next interrupt
	will simply turn off the Tx interrupt again. */
	uart->status |= 1 << 3;

	return pdPASS;
}
/*-----------------------------------------------------------*/

void vSerialClose( xComPortHandle xPort )
{
	/* Not supported as not required by the demo application. */
	( void ) xPort;
}
/*-----------------------------------------------------------*/

