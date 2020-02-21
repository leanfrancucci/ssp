/* --------------------------------------------------------------------------
 *
 *                           String Search Parser (SSP)
 *                           --------------------------
 *
 *                      Suitable for processing AT commands
 *
 *                      Copyright (c) 2018 Leandro Francucci
 *
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Site: https://vortexmakes.com/
 * e-mail: lf@vortexmakes.com
 * --------------------------------------------------------------------------
 */

/**
 *  \file       ssp.c
 *  \brief      String Search Parser. This module implements a AT Hayes
 *              command parser.
 */

/* -------------------------- Development history -------------------------- */
/* -------------------------------- Authors -------------------------------- */
/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "ssp.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
enum
{
    SSP_IDLE, SSP_INSEARCH
};

enum
{
    SSP_FOUND, SSP_NOT_FOUND
};

/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
/* ----------------------- Local function prototypes ----------------------- */
static void
ssp_deliver(SSP *const me, unsigned char c)
{
    if ((me->node->type == SSP_NTRN) && ((SSPNodeTrn *)(me->node))->trnAction)
    {
        (*((SSPNodeTrn *)(me->node))->trnAction)(c);
    }
}

/**
 *  \brief
 *  This function is invoked on finding a match.
 *  Invokes the branch action, set the ssp state to idle and set the new
 *  node according to next node in the branch.
 *
 *  \note
 *  If the next node is set to NULL then the current node will remain
 *  unchanged.
 */
static void
ssp_match(SSP *const me)
{
    if (me->branch->branchAction)
    {
        (*me->branch->branchAction)((unsigned char)(me->pos + 1));
    }

    me->state = SSP_IDLE;
    if (me->branch->target != (const SSPBase *)0)
    {
        me->node = me->branch->target;
    }
}

/* ---------------------------- Global functions --------------------------- */
int
ssp_init(SSP *const me, const SSPNodeNormal *root)
{
    int r = 1;

    if ((me != (SSP *)0) && (root != (const SSPNodeNormal *)0))
    {
        me->node = (SSPBase *)root;
        me->branch = ((SSPBase *)root)->branchTbl;
        me->state = SSP_IDLE;
        r = 0;
    }
    return r;
}

SSPResult
ssp_doSearch(SSP *const me, unsigned char c)
{
    int isFound;
    SSPResult result = SSP_UNMATCH;        /* Search result */

    switch (me->state)
    {
        case SSP_IDLE:
            /* Find the c input in every edge of current node */
            /* Also, set the current edge to be used */
            for (isFound = SSP_NOT_FOUND, me->branch = me->node->branchTbl,
                 me->pattern = me->branch->patt;
                 me->pattern != (unsigned char *)0;
                 ++me->branch, me->pattern = me->branch->patt)
            {
                if (*me->pattern == c)
                {
                    isFound = SSP_FOUND;
                    break;
                }
            }
            ssp_deliver(me, c);

            if (isFound == SSP_FOUND)
            {
                me->pos = 0;
                ++me->pattern;
                if (*me->pattern == '\0')
                {
                    ssp_match(me);
                    result = SSP_MATCH;
                }
                else
                {
                    me->state = SSP_INSEARCH;
                    result = SSP_INIT_SEARCH;
                }
            }
            else
            {
                me->state = SSP_IDLE;
                result = SSP_UNMATCH;
            }
            break;
        case SSP_INSEARCH:
            ssp_deliver(me, c);

            if (*me->pattern == c)
            {
                ++me->pos;
                ++me->pattern;
                if (*me->pattern == '\0')
                {
                    ssp_match(me);
                    result = SSP_MATCH;
                }
                else
                {
                    result = SSP_SEARCH_CONTINUES;
                }
            }
            else if (*(me->pattern - 1) != c)
            {
                me->state = SSP_IDLE;
                result = SSP_UNMATCH;
            }
            else
            {
                result = SSP_DUPLICATED_CHAR;
            }
            break;
        default:
            break;
    }
    return result;
}

/* ------------------------------ End of file ------------------------------ */
