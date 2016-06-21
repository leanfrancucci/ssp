/**
 *  \file       all_tests.c
 *  \brief      Test runner of fatfs module (only for learning test)
 */

/* -------------------------- Development history -------------------------- */
/* -------------------------------- Authors -------------------------------- */
/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "unity_fixture.h"
#include "board.h"
#include "pin_mux.h"
#include "putchar.h"
#include "board.h"
#include "usbhost.h"

/* ----------------------------- Local macros ------------------------------ */
#define forever()	for(;;)

/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
static
void
bsp_init( void )
{
    BOARD_InitPins();
    BOARD_BootClockRUN();
    outchar_init();
    sleep_init();
    usbhost_init();
	TST_USBPWR_INIT(LOGIC_USBPWR_OFF);
}

static 
void 
runAllTests(void)
{
	RUN_TEST_GROUP(fatfs);
}

/* ---------------------------- Global functions --------------------------- */
int
main(void)
{
	static int argc;
	static const char *argv[1];

	argc = 1;
	argv[0] = "fatfs";

	bsp_init();
	UnityMain(argc, argv, runAllTests);

	forever();
	return 0;
}

/* ------------------------------ End of file ------------------------------ */
