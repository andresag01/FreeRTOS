#include "FreeRTOS.h"
#include "portable.h"

#include "partest.h"

void vParTestSetLED( unsigned portBASE_TYPE uxLED, signed portBASE_TYPE xValue )
{
	( void ) uxLED;
	( void ) xValue;
}

void vParTestToggleLED( unsigned portBASE_TYPE uxLED )
{
	( void ) uxLED;
}
