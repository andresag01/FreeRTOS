/* Standard includes. */
#include "utils.h"
#include "packet.h"
#include <string.h>
#include <stdint.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "serial.h"

/* Mbed TLS */
#if !defined(MBEDTLS_CONFIG_FILE)
#error "Cannot include default config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif
#include "mbedtls/platform.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/debug.h"
#include "mbedtls/pk.h"
#include "mbedtls/pk_internal.h"

static const char *HTTPS_SERVER_NAME = "localhost";

/* personalization string for the drbg */
static const char *DRBG_PERS = "MBED TLS helloworld client";

/*
 * List of trusted root CA certificates
 */
static const char SSL_CA_PEM[] = "-----BEGIN CERTIFICATE-----\n"
"MIIDhzCCAm+gAwIBAgIBADANBgkqhkiG9w0BAQsFADA7MQswCQYDVQQGEwJOTDER\n"
"MA8GA1UECgwIUG9sYXJTU0wxGTAXBgNVBAMMEFBvbGFyU1NMIFRlc3QgQ0EwHhcN\n"
"MTcwNTA0MTY1NzAxWhcNMjcwNTA1MTY1NzAxWjA7MQswCQYDVQQGEwJOTDERMA8G\n"
"A1UECgwIUG9sYXJTU0wxGTAXBgNVBAMMEFBvbGFyU1NMIFRlc3QgQ0EwggEiMA0G\n"
"CSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDA3zf8F7vglp0/ht6WMn1EpRagzSHx\n"
"mdTs6st8GFgIlKXsm8WL3xoemTiZhx57wI053zhdcHgH057Zk+i5clHFzqMwUqny\n"
"50BwFMtEonILwuVA+T7lpg6z+exKY8C4KQB0nFc7qKUEkHHxvYPZP9al4jwqj+8n\n"
"YMPGn8u67GB9t+aEMr5P+1gmIgNb1LTV+/Xjli5wwOQuvfwu7uJBVcA0Ln0kcmnL\n"
"R7EUQIN9Z/SG9jGr8XmksrUuEvmEF/Bibyc+E1ixVA0hmnM3oTDPb5Lc9un8rNsu\n"
"KNF+AksjoBXyOGVkCeoMbo4bF6BxyLObyavpw/LPh5aPgAIynplYb6LVAgMBAAGj\n"
"gZUwgZIwHQYDVR0OBBYEFLRa5KWz3tJS9rnVppUP6z68x/3/MGMGA1UdIwRcMFqA\n"
"FLRa5KWz3tJS9rnVppUP6z68x/3/oT+kPTA7MQswCQYDVQQGEwJOTDERMA8GA1UE\n"
"CgwIUG9sYXJTU0wxGTAXBgNVBAMMEFBvbGFyU1NMIFRlc3QgQ0GCAQAwDAYDVR0T\n"
"BAUwAwEB/zANBgkqhkiG9w0BAQsFAAOCAQEAHK/HHrTZMnnVMpde1io+voAtql7j\n"
"4sRhLrjD7o3THtwRbDa2diCvpq0Sq23Ng2LMYoXsOxoL/RQK3iN7UKxV3MKPEr0w\n"
"XQS+kKQqiT2bsfrjnWMVHZtUOMpm6FNqcdGm/Rss3vKda2lcKl8kUnq/ylc1+QbB\n"
"G6A6tUvQcr2ZyWfVg+mM5XkhTrOOXus2OLikb4WwEtJTJRNE0f+yPODSUz0/vT57\n"
"ApH0CnB80bYJshYHPHHymOtleAB8KSYtqm75g/YNobjnjB6cm4HkW3OZRVIl6fYY\n"
"n20NRVA1Vjs6GAROr4NqW4k/+LofY9y0LLDE+p0oIEKXIsIvhPr39swxSA==\n"
"-----END CERTIFICATE-----\n";

/*-----------------------------------------------------------*/

/* Priorities for the demo application tasks. */
#define mainRECV_TASK_PRIORITY            ( tskIDLE_PRIORITY + 1     )

/*-----------------------------------------------------------*/

/*
 * Configure the processor for use with the Olimex demo board.  This includes
 * setup for the I/O, system clock, and access timings.
 */
static void prvSetupHardware( void );

/*
 * Receive packet task
 */
static void prvTlsConnection( void *pvParameters );

/*-----------------------------------------------------------*/

/* Send and receive packet functions */
static int prvBlockingSend( void *arg, const unsigned char *buf, size_t len );
static int prvBlockingRecv( void *arg, unsigned char *buf, size_t len );

/*-----------------------------------------------------------*/

/* Queue used to hold received characters */
QueueHandle_t xRxedPackets;
QueueHandle_t xTxedPackets;

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

    /* Initialize serial device */
    vInitializeSerialISR();

    /* Start the demo/test application tasks */
    printString("Setting up receiver task...\n");
    xTaskCreate( prvTlsConnection,
                 "Recv",
                 configMINIMAL_STACK_SIZE,
                 NULL,
                 mainRECV_TASK_PRIORITY,
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

static int prvBlockingSend( void *arg, const unsigned char *buf, size_t len )
{
	UART_t *uart = ( UART_t * )io[INTERRUPT_REASON_UART1];
    Packet_t *packet = malloc( sizeof( Packet_t ) );

    packet->size = len;
    packet->body = (unsigned char *)buf;

    while( xQueueSend( xTxedPackets, &packet, portMAX_DELAY ) != pdTRUE )
    {
        /* Block until the packet is sent */
    }

	/* Turn on the Tx interrupt so the ISR will remove the packet from the
	queue and send it. This does not need to be in a critical section as
	if the interrupt has already removed the packet the next interrupt
	will simply turn off the Tx interrupt again. */
	uart->status |= 1 << 3;

    return (int)len;
}

static int prvBlockingRecv( void *arg, unsigned char *buf, size_t len )
{
    static Packet_t *packet = NULL;

    if( packet == NULL )
    {
        while( xQueueReceive( xRxedPackets, &packet, portMAX_DELAY ) != pdTRUE )
        {
            /* Block until a packet is received */
        }
    }
    else
    {
        // There was a previous packet that was not fully evacuated
    }

    if( len >= packet->size )
    {
        memcpy( buf, packet->body, packet->size );
        packet = NULL;

        return packet->size;
    }
    else
    {
        memcpy( buf, packet->body, len );
        packet->size -= len;
        packet->body += len;

        return len;
    }
}

/*-----------------------------------------------------------*/

static void debugPrintCallback(void *ctx,
                               int level,
                               const char *file,
                               int line,
                               const char *str)
{
    printString("Debug print callback\n");
    fail();
}

static void prvTlsConnection( void *pvParameters )
{
    mbedtls_entropy_context *entropy;
    mbedtls_ctr_drbg_context *ctr_drbg;
    mbedtls_x509_crt *cacert;
    mbedtls_ssl_context *ssl;
    mbedtls_ssl_config *ssl_conf;

    int ret;

    printString("Allocating structs...\n");

    entropy = malloc(sizeof(mbedtls_entropy_context));
    ctr_drbg = malloc(sizeof(mbedtls_ctr_drbg_context));
    cacert = malloc(sizeof(mbedtls_x509_crt));
    ssl = malloc(sizeof(mbedtls_ssl_context));
    ssl_conf = malloc(sizeof(mbedtls_ssl_config));

    printString("Intializing structs...\n");

    mbedtls_entropy_init(entropy);
    mbedtls_ctr_drbg_init(ctr_drbg);
    mbedtls_x509_crt_init(cacert);
    mbedtls_ssl_init(ssl);
    mbedtls_ssl_config_init(ssl_conf);

    printString("mbedtls_ctr_drbg_init...\n");

    /* Configure the client */
    if ((ret = mbedtls_ctr_drbg_seed(ctr_drbg, mbedtls_entropy_func, entropy,
        (const unsigned char *)DRBG_PERS, strlen(DRBG_PERS) + 1)) != 0)
    {
        printString("mbedtls_crt_drbg_init\n");
        printHex(ret);
        fail();
    }

    printString("mbedtls_x509_crt_parse...\n");

    if ((ret = mbedtls_x509_crt_parse(cacert,
        (const unsigned char *)SSL_CA_PEM, strlen(SSL_CA_PEM) + 1)) != 0)
    {
        printString("mbedtls_x509_crt_parse\n");
        printHex(ret);
        fail();
    }

    printString("raw: tag:"); printHex(cacert->raw.tag);
    printString(" len:"); printHex(cacert->raw.len);
    putchar('\n');

    printString("tbs: tag:"); printHex(cacert->tbs.tag);
    printString(" len:"); printHex(cacert->tbs.len);
    putchar('\n');

    printString("version:"); printHex(cacert->version);
    putchar('\n');

    printString("serial: tag:"); printHex(cacert->serial.tag);
    printString(" len:"); printHex(cacert->serial.len);
    putchar('\n');

    printString("sig_oid: tag:"); printHex(cacert->sig_oid.tag);
    printString(" len:"); printHex(cacert->sig_oid.len);
    putchar('\n');

    printString("issuer_raw: tag:"); printHex(cacert->issuer_raw.tag);
    printString(" len:"); printHex(cacert->issuer_raw.len);
    putchar('\n');

    printString("subject_raw: tag:"); printHex(cacert->subject_raw.tag);
    printString(" len:"); printHex(cacert->subject_raw.len);
    putchar('\n');

    printString("issuer: oid: tag:"); printHex(cacert->issuer.oid.tag);
    printString(" len:"); printHex(cacert->issuer.oid.len);
    printString("\n      val: tag:"); printHex(cacert->issuer.val.tag);
    printString(" len:"); printHex(cacert->issuer.val.len);
    printString("\n      merged:"); printHex(cacert->issuer.next_merged);
    putchar('\n');

    printString("subject: oid: tag:"); printHex(cacert->subject.oid.tag);
    printString(" len:"); printHex(cacert->subject.oid.len);
    printString("\n      val: tag:"); printHex(cacert->subject.val.tag);
    printString(" len:"); printHex(cacert->subject.val.len);
    printString("\n      merged:"); printHex(cacert->subject.next_merged);
    putchar('\n');

    printString("valid_from:");
    printHex(cacert->valid_from.year); putchar(' ');
    printHex(cacert->valid_from.mon); putchar(' ');
    printHex(cacert->valid_from.day); putchar(' ');
    printHex(cacert->valid_from.hour); putchar(' ');
    printHex(cacert->valid_from.min); putchar(' ');
    printHex(cacert->valid_from.sec); putchar('\n');

    printString("pk:"); printHex(cacert->pk.pk_info->type); putchar(' ');
    printString(cacert->pk.pk_info->name);
    putchar('\n');

    printString("ext_types:"); printHex(cacert->ext_types); putchar('\n');
    printString("ca_istrue:"); printHex(cacert->ca_istrue); putchar('\n');
    printString("max_pathlen:"); printHex(cacert->max_pathlen); putchar('\n');
    printString("key_usage:"); printHex(cacert->key_usage); putchar('\n');
    printString("ns_cert_type:"); printHex(cacert->ns_cert_type); putchar('\n');

    printString("sig: tag:"); printHex(cacert->sig.tag);
    printString(" len:"); printHex(cacert->sig.len);
    putchar('\n');

    printString("sig_md:"); printHex(cacert->sig_md); putchar('\n');
    printString("sig_pk:"); printHex(cacert->sig_pk); putchar('\n');

    printString("mbedtls_ssl_config_defaults...\n");

    if ((ret = mbedtls_ssl_config_defaults(ssl_conf,
                    MBEDTLS_SSL_IS_CLIENT,
                    MBEDTLS_SSL_TRANSPORT_STREAM,
                    MBEDTLS_SSL_PRESET_DEFAULT)) != 0)
    {
        printString("mbedtls_ssl_config_defaults\n");
        printHex(ret);
        fail();
    }

    mbedtls_ssl_conf_ca_chain(ssl_conf, cacert, NULL);
    mbedtls_ssl_conf_rng(ssl_conf, mbedtls_ctr_drbg_random, ctr_drbg);
    mbedtls_ssl_conf_dbg(ssl_conf, debugPrintCallback, NULL);

    /*
     * It is possible to disable authentication by passing
     * MBEDTLS_SSL_VERIFY_NONE in the call to mbedtls_ssl_conf_authmode()
     */
    mbedtls_ssl_conf_authmode(ssl_conf, MBEDTLS_SSL_VERIFY_REQUIRED);

    if ((ret = mbedtls_ssl_setup(ssl, ssl_conf)) != 0)
    {
        printString("mbedtls_ssl_setup\n");
        printHex(ret);
        fail();
    }

    printString("Setting hostname...\n");

    mbedtls_ssl_set_hostname(ssl, HTTPS_SERVER_NAME);

    printString("Configuring bio calls...\n");

    mbedtls_ssl_set_bio(ssl, NULL, prvBlockingSend, prvBlockingRecv, NULL);

    printString("Starting handshake...\n");

    /* Start the handshake, the rest will be done in onReceive() */
    do
    {
        ret = mbedtls_ssl_handshake(ssl);
    }
    while (ret != 0 && (ret == MBEDTLS_ERR_SSL_WANT_READ ||
           ret == MBEDTLS_ERR_SSL_WANT_WRITE));
    if (ret < 0)
    {
        char *err = malloc(1024);
        mbedtls_strerror( ret, err, 1024 );
        printString("mbedtls_ssl_handshake\n");
        printString( err );
        putchar('\n');
        printHex(ret);
        fail();
    }

    printString("Success!\n");
    fail();
}
