#include "utils.h"
#include "serial.h"
#include "packet.h"

#define serINVALID_QUEUE        ( ( QueueHandle_t ) 0 )

/* Recv structs */
typedef enum RecvStatus
{
    RECVING_SIZE,
    RECVING_BODY,
}
RecvStatus_e;

typedef struct RecvContext
{
    RecvStatus_e status;
    Packet_t *packet;
    uint32_t recvedSizeBytes;
    uint32_t recvedBodyBytes;
}
RecvContext_t;

static RecvContext_t xRecvCtx;

/* Send structs */
typedef enum SendStatus
{
    SENDING_SIZE,
    SENDING_BODY,
}
SendStatus_e;

typedef struct SendContext
{
    SendStatus_e status;
    Packet_t *packet;
    uint32_t sentSizeBytes;
    uint32_t sentBodyBytes;
}
SendContext_t;

static SendContext_t xSendCtx;

void vInitializeSerialISR( void )
{
    UART_t *uart = ( UART_t * ) io[INTERRUPT_REASON_UART1];
    InterruptConfig_t *icfg =
        ( InterruptConfig_t * )io[INTERRUPT_REASON_ICFG];
    unsigned portBASE_TYPE uxQueueLength = 10;

    /* Create the queues used to hold sent and received packets */
    xRxedPackets = xQueueCreate( uxQueueLength,
                                 ( unsigned portBASE_TYPE ) sizeof( Packet_t * ) );
    xTxedPackets = xQueueCreate( uxQueueLength,
                                 ( unsigned portBASE_TYPE ) sizeof( Packet_t * ) );

    /* Initialize the serial device */
    if( xRxedPackets != serINVALID_QUEUE || xTxedPackets != serINVALID_QUEUE )
    {
        /* Enable UART1 and disable UART0 */
        icfg->enbdev |= 1 << INTERRUPT_REASON_UART1;
        icfg->enbdev &= ~( 1 << INTERRUPT_REASON_UART0 );

        /* Disable UART1 interrupt sources to begin */
        uart->status = 0x00000000;

        /* Enable Rx and Tx operations */
        uart->status |= ( 3 << 0 );

        /* Enable Rx interrupt sources */
        uart->status |= ( 1 << 2 );
    }
    else
    {
        printString( "Could not create queues\n" );
        fail();
    }

    /* Initialize UART1 interrupt buffer */
    xRecvCtx.status = RECVING_SIZE;
    xRecvCtx.packet = pvPortMalloc( sizeof( Packet_t ) );
    xRecvCtx.recvedSizeBytes = 0;
    xRecvCtx.recvedBodyBytes = 0;

    xSendCtx.status = SENDING_SIZE;
    xSendCtx.packet = NULL;
    xSendCtx.sentSizeBytes = 0;
    xSendCtx.sentBodyBytes = 0;
}

void UART1InterruptHandler( uint32_t reason )
{
	volatile InterruptConfig_t *icfg =
		( InterruptConfig_t * )io[INTERRUPT_REASON_ICFG];
	volatile UART_t *uart = ( UART_t * )io[INTERRUPT_REASON_UART1];

	portBASE_TYPE xTxHigherPriorityTaskWoken = pdFALSE;
    portBASE_TYPE xRxHigherPriorityTaskWoken = pdFALSE;
    BaseType_t xInsertStatus = errQUEUE_FULL;
    BaseType_t xDequeueStatus = errQUEUE_FULL;

	/* Acknoledge the interrupt first to ensure that future interrupts do not
     * get ignored
     */
	icfg->pendev &= ~( 1 << INTERRUPT_REASON_UART1 );

	/* What caused the interrupt */
	uint32_t ulStatus = ( uart->status >> 4 ) & 0x3;

    //printString("UART1: ");

	/* Was a character transferred? */
	if( ulStatus & 2 )
	{
        //printString("mode:tx\n");
        switch( xSendCtx.status )
        {
            case SENDING_SIZE:
                //printString("Sending size\n");
                if( xSendCtx.packet == NULL )
                {
                    xDequeueStatus = xQueueReceiveFromISR( xTxedPackets,
                                                           &xSendCtx.packet,
                                                           &xTxHigherPriorityTaskWoken );
                    if( xDequeueStatus != pdTRUE )
                    {
                        printString("Packet not found!\n");
			            /* Queue empty, nothing to send so turn off the Tx interrupt */
			            uart->status &= ~( 1 << 3 );
                        break;
                    }
                }
                if( xSendCtx.sentSizeBytes == 0 )
                {
                    printString("Sending size:");
                    printHex( xSendCtx.packet->size );
                    putchar('\n');

                    int i;
                    for (i = 0; i < xSendCtx.packet->size; i++)
                    {
                        if (i != 0 && (i % 8) == 0)
                            putchar('\n');
                        else if (i != 0)
                            putchar(' ');
                        printHex(xSendCtx.packet->body[i]);
                    }
                    putchar('\n');
                }
                uart->buftx = ( xSendCtx.packet->size >> ( 8 * xSendCtx.sentSizeBytes ) ) & 0xff;
                xSendCtx.sentSizeBytes++;
                if( xSendCtx.sentSizeBytes == sizeof( uint32_t ) )
                {
                    xSendCtx.status = SENDING_BODY;
                }
                break;

            case SENDING_BODY:
                uart->buftx = xSendCtx.packet->body[xSendCtx.sentBodyBytes];
                xSendCtx.sentBodyBytes++;
                if( xSendCtx.sentBodyBytes == xSendCtx.packet->size )
                {
                    /* Finished sending packet */
                    xSendCtx.status = SENDING_SIZE;
                    xSendCtx.packet = NULL;
                    xSendCtx.sentSizeBytes = 0;
                    xSendCtx.sentBodyBytes = 0;

                    /* Check if there are more packets to send */
                    xDequeueStatus = xQueueReceiveFromISR( xTxedPackets,
                                                           &xSendCtx.packet,
                                                           &xTxHigherPriorityTaskWoken );
                    if( xDequeueStatus != pdTRUE )
                    {
                        /* Queue empty, nothing to send so turn off the Tx interrupt */
                        uart->status &= ~( 1 << 3 );
                        break;
                    }
                }
                break;

            default:
                fail();
        }
	}

	/* Was a character received? */
	if( ulStatus & 1 )
	{
        //printString("mode:rx\n");
        switch( xRecvCtx.status )
        {
            case RECVING_SIZE:
                printString("Starting receive\n");
                xRecvCtx.packet->size |= uart->bufrx << ( 8 * xRecvCtx.recvedSizeBytes );
                xRecvCtx.recvedSizeBytes++;
                if( xRecvCtx.recvedSizeBytes == sizeof( uint32_t ) )
                {
                    xRecvCtx.status = RECVING_BODY;
                    xRecvCtx.packet->body = pvPortMalloc( sizeof( uint8_t ) * xRecvCtx.packet->size );
                }
                break;

            case RECVING_BODY:
                xRecvCtx.packet->body[xRecvCtx.recvedBodyBytes] = ( uint8_t ) uart->bufrx;
                xRecvCtx.recvedBodyBytes++;
                if( xRecvCtx.recvedBodyBytes == xRecvCtx.packet->size )
                {
                    printString("Received size:");
                    printHex( xRecvCtx.packet->size );
                    putchar('\n');

                    int i;
                    for (i = 0; i < xRecvCtx.packet->size; i++)
                    {
                        if (i != 0 && (i % 8) == 0)
                            putchar('\n');
                        else if (i != 0)
                            putchar(' ');
                        printHex(xRecvCtx.packet->body[i]);
                    }
                    putchar('\n');

                    xInsertStatus = xQueueSendFromISR( xRxedPackets,
                                                       &xRecvCtx.packet,
                                                       &xRxHigherPriorityTaskWoken );
                    if( xInsertStatus == pdTRUE )
                    {
                        xRecvCtx.status = RECVING_SIZE;
                        xRecvCtx.packet = pvPortMalloc( sizeof( Packet_t ) );
                        xRecvCtx.recvedSizeBytes = 0;
                        xRecvCtx.recvedBodyBytes = 0;
                    }
                    else
                    {
                        /* The queue is full! */
                        fail();
                    }
                }
                break;

            default:
                fail();
        }
	}

	/* If an event caused a task to unblock then we call "Yield from ISR" to
	ensure that the unblocked task is the task that executes when the interrupt
	completes if the unblocked task has a priority higher than the interrupted
	task. */
	if( xRxHigherPriorityTaskWoken || xTxHigherPriorityTaskWoken )
	{
		portYIELD_FROM_ISR();
	}
}
