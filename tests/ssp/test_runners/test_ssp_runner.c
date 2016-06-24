/**
 *  \file       test_ssp_runner.c
 *  \brief      Test runner of ssp module
 */

/* -------------------------- Development history -------------------------- */
/* -------------------------------- Authors -------------------------------- */
/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "unity_fixture.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */
TEST_GROUP_RUNNER(ssp)
{
	RUN_TEST_CASE(ssp, OutOfTree);
	RUN_TEST_CASE(ssp, StartSearch);
	RUN_TEST_CASE(ssp, FoundPattern);
	RUN_TEST_CASE(ssp, FoundLongPattern);
	RUN_TEST_CASE(ssp, RepeatsCharInPattern);
	RUN_TEST_CASE(ssp, BreakSearchPattern);
	RUN_TEST_CASE(ssp, OnFoundPatternCallsAction);
	RUN_TEST_CASE(ssp, CallsActionInTransparentNode);
	RUN_TEST_CASE(ssp, FoundPatternInTransparentNode);
	RUN_TEST_CASE(ssp, TravelingDifferentNodeTypes);
}

/* ------------------------------ End of file ------------------------------ */
