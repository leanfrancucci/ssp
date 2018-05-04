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
#if SSP_DEBUG == 1
#include <ctype.h>
#if SSP_PRINT_FORMAT == 1
#include <stdarg.h>
#include <stdio.h>
#endif
#endif

/* ----------------------------- Local macros ------------------------------ */
#define CB(x)                   ((SSPBase*)(x))
#define CTRN(x)                 ((SSPNodeTrn*)(x))
#define CNORM(x)                ((SSPNodeNormal*)(x))
#define ssp_isMatch(_me)        (*(_me)->pattern == '\0')
#define ssp_initSearch(_me)     (_me)->state = SSP_INSEARCH
#define ssp_isInPatt(_me, x)    (*(_me)->pattern == (x))
#define ssp_incPatt(_me)        ++ (_me)->pattern

/* ------------------------------- Constants ------------------------------- */
enum
{
    SSP_IDLE, SSP_INSEARCH
};

enum
{
    SSP_FOUND, SSP_NOT_FOUND
};

#if SSP_DEBUG == 1
static const char *sspmap[] =
{
    "-",    /* NUL  - 00 */
    "-",    /* SOH  - 01 */
    "-",    /* STX  - 02 */
    "-",    /* ETX  - .. */
    "-",    /* EOT  - .. */
    "-",    /* ENQ  - .. */
    "-",    /* ACK  - .. */
    "-",    /* BEL  - .. */
    "-",    /* BS   - .. */
    "-",    /* TAB  - .. */
    "\\n",  /* LF   - .. */
    "-",    /* VT   - .. */
    "-",    /* FF	- .. */
    "\\r",  /* CR	- .. */
    "-",    /* SO	- .. */
    "-",    /* SI   - .. */
    "-",    /* DLE  - .. */
    "-",    /* DC1  - .. */
    "-",    /* DC2  - .. */
    "-",    /* DC3  - .. */
    "-",    /* DC4  - .. */
    "-",    /* NAK  - .. */
    "-",    /* SYN  - .. */
    "-",    /* ETB  - .. */
    "-",    /* CAN  - .. */
    "-",    /* EM   - .. */
    "-",    /* SUB  - .. */
    "-",    /* ESC  - .. */
    "-",    /* FS   - .. */
    "-",    /* GS   - .. */
    "-",    /* RS   - .. */
    "-"     /* US   - 1F */
};
#endif

/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
/* ----------------------- Local function prototypes ----------------------- */
#if SSP_DEBUG == 1
static char *
ssp_mapChar(SSP *const me, unsigned char c)
{
    if (!isprint(c))
    {
        return (char *)sspmap[c];
    }

    me->strmap[0] = c;
    me->strmap[1] = '\0';
    return me->strmap;
}

#if SSP_PRINT_FORMAT == 1
static void
printFormat(SSP *const me, unsigned char out, char *fmt, ...)
{
    va_list args;

    (void)out;
    va_start(args, fmt);
    vsprintf(me->pfs, fmt, args);
    SSP_PUTS(out, me->pfs);
    va_end(args);
}
#endif
#endif

/**
 *  \brief
 *  Finds the c input in each of node's branch.
 *  Also, it set the current branch to be used.
 *
 *  \param c    input character.
 *
 *  \return
 *  SSP_FOUND if the received character is found in the node pattern,
 *  otherwise SSP_NOT_FOUND.
 */
static int
ssp_searchPatt(SSP *const me, unsigned char c)
{
    for (me->branch = CB(me->node)->branchTbl,
         me->pattern = me->branch->patt;
         me->pattern != (unsigned char *)0;
         ++me->branch, me->pattern = me->branch->patt)
    {
        if (*me->pattern == c)
        {
            return SSP_FOUND;
        }
    }
    return SSP_NOT_FOUND;
}

static void
ssp_deliver(SSP *const me, unsigned char c)
{
#if SSP_DEBUG == 1
    if (CB(me->node)->type == SSP_NTRN)
    {
        SSP_PRINT((0, "\tDeliver input '%s' (%2d)\n", ssp_mapChar(me, c), c));
    }
#endif
    if ((CB(me->node)->type == SSP_NTRN) && CTRN(me->node)->trnAction)
    {
        (*CTRN(me->node)->trnAction)(c);
    }
}

/**
 *  \brief
 *  Sets the current node.
 *
 *  If the target node of the current branch is set to NULL then the current
 *  node will be left as is. This could be useful to define a transparent
 *  node.
 */
#if SSP_DEBUG == 1
static void
ssp_notMatch(SSP *const me)
{
    if (CB(me->node)->type == SSP_NTRN)
    {
        SSP_PRINT((0, "\tIn transparent node \"%s\"\n", CB(me->node)->name));
    }
    else
    {
        SSP_PRINT((0, "\tNot match. Changes to \"%s\" node\n",
                   CB(me->node)->name));
    }
    me->state = SSP_IDLE;
}
#else
#define ssp_notMatch(_me)       (_me)->state = SSP_IDLE
#endif

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
    SSP_PRINT((0, "\tFind pattern \"%s\" from node \"%s\"\n",
               me->branch->patt,
               CB(me->node)->name));
    if (me->branch->branchAction)
    {
        (*me->branch->branchAction)((unsigned char)(me->pos + 1));
    }

    me->state = SSP_IDLE;
    if (me->branch->target != (const SSPBase *)0)
    {
        me->node = me->branch->target;
    }

    SSP_PRINT((0, "\tChange to node \"%s\"\n", CB(me->node)->name));
}

#if SSP_DEBUG == 1
static int
ssp_isEqual(SSP *const me, unsigned char c)
{
    int r;

    r = (*(me->pattern - 1) == c);
    if (r)
    {
        SSP_PRINT((0, "\tRepeat input (match pos = %2d)\n", me->pos));
    }

    return r;
}
#else
#define ssp_isEqual(_me, x)     (*((_me)->pattern - 1) == (x))
#endif

/* ---------------------------- Global functions --------------------------- */
int
ssp_init(SSP *const me, const SSPNodeNormal *root)
{
    int r = 1;

    if ((me != (SSP *)0) && (root != (const SSPNodeNormal *)0))
    {
        me->node = root;
        me->branch = CB(root)->branchTbl;
        me->state = SSP_IDLE;
        r = 0;
    }
    return r;
}

SSPResult
ssp_doSearch(SSP *const me, unsigned char c)
{
    int r;
    SSPResult result;        /** Search result */

    SSP_PRINT((0, "In: '%s' (%2d)\n", ssp_mapChar(me, c), c));
    switch (me->state)
    {
        case SSP_IDLE:
            r = ssp_searchPatt(me, c);
            ssp_deliver(me, c);

            if (r == SSP_FOUND)
            {
                me->pos = 0;

                SSP_PRINT((0, "\tFind in pattern \"%s\" from node \"%s\"",
                           me->branch->patt,
                           CB(me->node)->name));
                SSP_PRINT((0, " (match pos = %2d)\n", me->pos));

                ssp_incPatt(me);
                if (ssp_isMatch(me))
                {
                    ssp_match(me);
                    result = SSP_MATCH;
                }
                else
                {
                    ssp_initSearch(me);
                    result = SSP_INIT_SEARCH;
                }
            }
            else
            {
                ssp_notMatch(me);
                result = SSP_UNMATCH;
            }
            break;
        case SSP_INSEARCH:
            ssp_deliver(me, c);

            if (ssp_isInPatt(me, c))
            {
                ++me->pos;
                SSP_PRINT((0, "\tFind in pattern \"%s\" from node \"%s\"",
                           me->branch->patt,
                           CB(me->node)->name));
                SSP_PRINT((0, " (match pos = %2d)\n", me->pos));

                ssp_incPatt(me);
                if (ssp_isMatch(me))
                {
                    ssp_match(me);
                    result = SSP_MATCH;
                }
                else
                {
                    result = SSP_SEARCH_CONTINUES;
                }
            }
            else if (!ssp_isEqual(me, c))
            {
                ssp_notMatch(me);
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
