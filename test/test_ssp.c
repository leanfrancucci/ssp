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
#include <string.h>
#include "unity.h"
#include "ssp.h"
#include "Mock_tree_actions.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static SSP sspObj;

SSP_DCLR_NORMAL_NODE root, node_no, node_ok;
SSP_DCLR_TRN_NODE node_trn;

SSP_CREATE_NORMAL_NODE(root);
SSP_CREATE_BR_TABLE(root)
SSPBR("ok",     NULL,       &node_ok),
SSPBR("no",     NULL,       &node_no),
SSP_END_BR_TABLE

SSP_CREATE_NORMAL_NODE(node_ok);
SSP_CREATE_BR_TABLE(node_ok)
SSPBR("frm",    pattFrm,    &node_trn),
SSPBR("error",  NULL,       NULL),
SSP_END_BR_TABLE

SSP_CREATE_TRN_NODE(node_trn, collect);
SSP_CREATE_BR_TABLE(node_trn)
SSPBR("ok",     pattOk,     &root),
SSPBR("+",      NULL,       &root),
SSP_END_BR_TABLE

SSP_CREATE_NORMAL_NODE(node_no);
SSP_CREATE_BR_TABLE(node_no)
SSPBR("+",      NULL,       &root),
SSPBR("-",      pattFrm,    &node_trn),
SSP_END_BR_TABLE

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
static SSPResult
parseAndCheck(SSP *const me, const char *str, int mode,
              SSPResult expectedResult)
{
    SSPResult result;
    const char *p;
    int i;

    for (i = strlen(str), p = str; i > 0; --i, ++p)
    {
        result = ssp_doSearch(me, *p);
        if (mode == 1)
        {
            TEST_ASSERT_EQUAL(expectedResult, result);
        }
    }
    return result;
}

static SSPResult
parseString(SSP *const me, const char *str)
{
    return parseAndCheck(me, str, 0, 0);
}

static SSPResult
parseStringInTransparentNode(SSP *const me, const char *str)
{
    return parseAndCheck(me, str, 1, SSP_UNMATCH);
}

/* ---------------------------- Global functions --------------------------- */
void
setUp(void)
{
    ssp_init(&sspObj, &root);
    Mock_tree_actions_Init();
}

void
tearDown(void)
{
    Mock_tree_actions_Verify();
    Mock_tree_actions_Destroy();
}

void
test_Initialize(void)
{
    int result;

    result = ssp_init(&sspObj, &root);
    TEST_ASSERT_EQUAL(result, 0);
}

void
test_WrongInitParameters(void)
{
    int result;

    result = ssp_init((SSP *const)0, &root);
    TEST_ASSERT_EQUAL(result, 1);

    result = ssp_init(&sspObj, (const SSPNodeNormal *)0);
    TEST_ASSERT_EQUAL(result, 1);
}

void
test_OutOfTree(void)
{
    SSPResult result;

    result = ssp_doSearch(&sspObj, '\r');
    TEST_ASSERT_EQUAL(SSP_UNMATCH, result);
}

void
test_StartSearch(void)
{
    SSPResult result;

    result = ssp_doSearch(&sspObj, 'n');
    TEST_ASSERT_EQUAL(SSP_INIT_SEARCH, result);
}

void
test_FoundPattern(void)
{
    SSPResult result;

    result = ssp_doSearch(&sspObj, 'n');
    result = ssp_doSearch(&sspObj, 'o');
    TEST_ASSERT_EQUAL(SSP_MATCH, result);
}

void
test_FoundLongPattern(void)
{
    SSPResult result;

    result = parseString(&sspObj, "oker");
    TEST_ASSERT_EQUAL(SSP_SEARCH_CONTINUES, result);
}

void
test_RepeatsCharInPattern(void)
{
    SSPResult result;

    result = parseString(&sspObj, "oooo");
    TEST_ASSERT_EQUAL(SSP_DUPLICATED_CHAR, result);
}

void
test_BreakSearchPattern(void)
{
    SSPResult result;

    result = parseString(&sspObj, "ol");
    TEST_ASSERT_EQUAL(SSP_UNMATCH, result);
}

void
test_OnFoundPatternCallsAction(void)
{
    SSPResult result;

    pattFrm_Expect(3);
    result = parseString(&sspObj, "okfrm");
    TEST_ASSERT_EQUAL(SSP_MATCH, result);
}

void
test_CallsActionInTransparentNode(void)
{
    SSPResult result;

    pattFrm_Expect(3);
    collect_Expect('0');
    collect_Expect('1');

    result = parseString(&sspObj, "okfrm");
    TEST_ASSERT_EQUAL(SSP_MATCH, result);

    result = parseStringInTransparentNode(&sspObj, "01");
    TEST_ASSERT_EQUAL(SSP_UNMATCH, result);
}

void
test_FoundPatternInTransparentNode(void)
{
    SSPResult result;

    pattFrm_Expect(3);
    collect_Expect('0');
    collect_Expect('1');
    collect_Expect('o');
    collect_Expect('k');
    pattOk_Expect(2);

    result = parseString(&sspObj, "okfrm");
    result = parseStringInTransparentNode(&sspObj, "01");
    result = parseString(&sspObj, "ok");
    TEST_ASSERT_EQUAL(SSP_MATCH, result);
}

void
test_TravelingDifferentNodeTypes(void)
{
    SSPResult result;

    pattFrm_Expect(1);
    collect_Expect('a');
    collect_Expect('b');
    collect_Expect('c');
    collect_Expect('+');
    pattFrm_Expect(3);
    collect_Expect('a');
    collect_Expect('b');
    collect_Expect('c');
    collect_Expect('o');
    collect_Expect('k');
    pattOk_Expect(2);

    result = parseString(&sspObj, "\r\nno-abc+okfrmabcokno");
    TEST_ASSERT_EQUAL(SSP_MATCH, result);
}

/* ------------------------------ End of file ------------------------------ */
