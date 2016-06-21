/**
 *  \file       test_fatfs_runner.c
 *  \brief      Test runner of fatfs module (only for learning test)
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
TEST_GROUP_RUNNER(fatfs)
{
	RUN_TEST_CASE(fatfs, MountDrive);
	RUN_TEST_CASE(fatfs, MountDriveFails);
	RUN_TEST_CASE(fatfs, OpenFile);
	RUN_TEST_CASE(fatfs, OpenFileFails);
	RUN_TEST_CASE(fatfs, WriteFile);
	RUN_TEST_CASE(fatfs, WriteFileFails);
}

/* ------------------------------ End of file ------------------------------ */
