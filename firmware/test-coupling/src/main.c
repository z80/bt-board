
#include "ch.h"
#include "hal.h"

//*
//* Application entry point.
//*
int main(void)
{
    halInit();
    chSysInit();

    while ( 1 )
    {
        chThdSleepMilliseconds( 250 );
    }

    return 0;
}






