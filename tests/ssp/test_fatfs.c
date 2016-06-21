/**
 *  \file       test_fatfs.c
 *  \brief      Unit test for fatfs module (only for learning test).
 */

/* -------------------------- Development history -------------------------- */
/* -------------------------------- Authors -------------------------------- */
/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include <stdio.h>
#include <string.h>
#include "unity_fixture.h"
#include "types.h"
#include "board.h"
#include "USB_MSD_driver.h"
#include "usbhost.h"
#include "sleep.h"
#include "ff.h"
#include "diskio.h"

/* ----------------------------- Local macros ------------------------------ */
enum
{
    PWR_ON, PWR_OFF
};

/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
TEST_GROUP(fatfs);

/* ---------------------------- Local variables ---------------------------- */
static FATFS fatfs;
static char filePath[30];
static char logicalDriveNumber[3];
static FRESULT fatfsCode;
static FIL file;
static FILINFO fileInfo;
static char fileData[16];

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
static void
setUsbPower(int control)
{
    if (control == PWR_ON)
    {
        TST_USBPWR_ON();
    }
    else
    {
        TST_USBPWR_OFF();
    }
    sleep(1000);
}

static int
waitingConnection(int control)
{
    int i;
    byte res;

    setUsbPower(control);
    for (i = 200; i > 0; --i)
    	if ((res = USB_MSDPlugAndPlay()) == errNOERR)
            break;
    return (int)res;
}

static int
setFileData(void)
{
    strcpy(fileData, "Hello World!");
    return strlen(fileData);
}

static void
openTestFile(void)
{
    waitingConnection(PWR_ON);

    fatfsCode = f_mount(&fatfs, logicalDriveNumber, 1);
    TEST_ASSERT_EQUAL(FR_OK, fatfsCode);

    fatfsCode = f_unlink(filePath);
    TEST_ASSERT_TRUE((FR_OK == fatfsCode) || (FR_NO_FILE == fatfsCode));

    fatfsCode = f_open(&file, filePath, 
                       FA_WRITE | FA_READ | FA_CREATE_ALWAYS);
    TEST_ASSERT_EQUAL(FR_OK, fatfsCode);
}

static void
deleteTestFile(void)
{
    fatfsCode = f_unlink(filePath);
    TEST_ASSERT_EQUAL(FR_OK, fatfsCode);
}

/* ---------------------------- Global functions --------------------------- */
TEST_SETUP(fatfs)
{
    sprintf(logicalDriveNumber, "%c:", USBDISK + '0');
    sprintf(filePath, "%c:/test.dat", USBDISK + '0');
}

TEST_TEAR_DOWN(fatfs)
{
}

TEST(fatfs, MountDrive)
{
    int res;

    res = waitingConnection(PWR_ON);
    TEST_ASSERT_EQUAL(errNOERR, res);

    fatfsCode = f_mount(&fatfs, logicalDriveNumber, 1);
    TEST_ASSERT_EQUAL(FR_OK, fatfsCode);
}

TEST(fatfs, MountDriveFails)
{
    setUsbPower(PWR_OFF);
    fatfsCode = f_mount(&fatfs, logicalDriveNumber, 1);
    TEST_ASSERT_EQUAL(FR_NOT_READY, fatfsCode);
}

TEST(fatfs, OpenFile)
{
    openTestFile();

    fatfsCode = f_stat(filePath, &fileInfo);
    TEST_ASSERT_EQUAL_STRING("TEST.DAT", fileInfo.fname);

    deleteTestFile();
}

TEST(fatfs, OpenFileFails)
{
    waitingConnection(PWR_ON);

    fatfsCode = f_mount(&fatfs, logicalDriveNumber, 1);
    TEST_ASSERT_EQUAL(FR_OK, fatfsCode);

    setUsbPower(PWR_OFF);
    fatfsCode = f_open(&file, filePath, 
                       FA_WRITE | FA_READ | FA_CREATE_ALWAYS);
    TEST_ASSERT_EQUAL(FR_DISK_ERR, fatfsCode);
}

TEST(fatfs, WriteFile)
{
    UINT bytesWritten, fileDataSize;

    fileDataSize = setFileData();
    openTestFile();

    fatfsCode = f_write(&file, fileData, fileDataSize, &bytesWritten);
    TEST_ASSERT_EQUAL(FR_OK, fatfsCode);
    TEST_ASSERT_EQUAL(bytesWritten, fileDataSize);

    deleteTestFile();
}

TEST(fatfs, WriteFileFails)
{
    UINT bytesWritten, fileDataSize;

    fileDataSize = setFileData();
    openTestFile();

    setUsbPower(PWR_OFF);
    fatfsCode = f_write(&file, fileData, fileDataSize, &bytesWritten);
    TEST_ASSERT_EQUAL(FR_DISK_ERR, fatfsCode);

    openTestFile();
    deleteTestFile();
}

/* ------------------------------ End of file ------------------------------ */
