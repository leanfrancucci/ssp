/**
 *  \file       all_tests.c
 *  \brief      Test runner of ssp module
 */

/* -------------------------- Development history -------------------------- */
/* -------------------------------- Authors -------------------------------- */
/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include <stdio.h>
#include "unity_fixture.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
static void
waitForKeyPress(void)
{
    getchar();
}

static void 
runAllTests(void)
{
	RUN_TEST_GROUP(ssp);
}

/* ---------------------------- Global functions --------------------------- */
int
main(int argc, const char *argv[])
{
	UnityMain(argc, argv, runAllTests);

	waitForKeyPress();
	return 0;
}

/* ------------------------------ End of file ------------------------------ */
