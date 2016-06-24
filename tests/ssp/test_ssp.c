/**
 *  \file       test_ssp.c
 *  \brief      Unit test for ssp module (only for learning test).
 */

/* -------------------------- Development history -------------------------- */
/* -------------------------------- Authors -------------------------------- */
/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include <stdlib.h>
#include <stdio.h>
#include "unity_fixture.h"
#include "ssp.h"

/* ----------------------------- Local macros ------------------------------ */
#define MAX_EXP_CHS     3

/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
TEST_GROUP(ssp);

/* ---------------------------- Local variables ---------------------------- */
static unsigned char buff[16], *pbuff;
static int ixbuff;
SSP_DCLR_NORMAL_NODE root, node_no, node_ok;
SSP_DCLR_TRN_NODE node_trn;

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
static void
none(unsigned char pos)
{
	(void)pos;
}

static void
patt_frm(unsigned char pos)
{
	(void)pos;
	pbuff = buff;
	ixbuff = 0;
	printf("\t%s: expected at least %d characters\n", __FUNCTION__, 
														MAX_EXP_CHS);
}

static void
patt_ok(unsigned char pos)
{
	int i;

	(void)pos;
	printf("\t%s: #%d ch collected =", __FUNCTION__, ixbuff);

	for (i = 0, pbuff = buff; i < ixbuff; ++i, ++pbuff)
		printf(" %c", *pbuff);

	putchar('\n');
}

void
collect( unsigned char c )
{
	if (ixbuff < MAX_EXP_CHS)
	{
		printf("\t%s: collect[%d] = %c (%2d)\n", __FUNCTION__, 
													ixbuff, c, c);
		*pbuff++ = c;
		++ixbuff;
	}
}

/* ================================ Tree =================================== */
SSP_CREATE_NORMAL_NODE( root );
SSP_CREATE_BR_TABLE( root )
	SSPBR( "ok",		NULL, 		&node_ok	),
	SSPBR( "no",		NULL, 		&node_no	),
SSP_END_BR_TABLE

SSP_CREATE_NORMAL_NODE( node_ok );
SSP_CREATE_BR_TABLE( node_ok )
	SSPBR( "frm",		patt_frm, 	&node_trn	),
	SSPBR( "error",		NULL, 		NULL 		),
SSP_END_BR_TABLE

SSP_CREATE_TRN_NODE( node_trn, collect );
SSP_CREATE_BR_TABLE( node_trn )
	SSPBR( "ok",		patt_ok, 	&root 		),
	SSPBR( "+",			NULL, 		&root 		),
SSP_END_BR_TABLE

SSP_CREATE_NORMAL_NODE( node_no );
SSP_CREATE_BR_TABLE( node_no )
	SSPBR( "+",			NULL, 		&root 		),
	SSPBR( "-",			patt_frm,	&node_trn	),
SSP_END_BR_TABLE

/* ---------------------------- Global functions --------------------------- */
TEST_SETUP(ssp)
{
    ssp_init(&root);
}

TEST_TEAR_DOWN(ssp)
{
}

TEST(ssp, OutOfTree)
{
    SSPResult result;

	result = ssp_doSearch('\r');
    TEST_ASSERT_EQUAL(SSP_UNMATCH, result);
}

TEST(ssp, StartSearch)
{
    SSPResult result;

	result = ssp_doSearch('n');
    TEST_ASSERT_EQUAL(SSP_INIT_SEARCH, result);
}

TEST(ssp, FoundPattern)
{
    SSPResult result;

	result = ssp_doSearch('n');
	result = ssp_doSearch('o');
    TEST_ASSERT_EQUAL(SSP_MATCH, result);
}

TEST(ssp, FoundLongPattern)
{
    SSPResult result;

	result = ssp_doSearch('o');
	result = ssp_doSearch('k');
	result = ssp_doSearch('e');
	result = ssp_doSearch('r');
    TEST_ASSERT_EQUAL(SSP_SEARCH_CONTINUES, result);
}

/* ------------------------------ End of file ------------------------------ */
