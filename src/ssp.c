/**
 *  \file       ssp.c
 *  \brief      String Search Parser. This module implements a AT Hayes 
 *              command parser.
 */

/* -------------------------- Development history -------------------------- */
/* -------------------------------- Authors -------------------------------- */
/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include <stdlib.h>
#include "ssp.h"
#if SSP_DEBUG == 1
#include <ctype.h>
#if SSP_PRINT_FORMAT == 1
#include <stdarg.h>
#include <stdio.h>
#endif
#endif

/* ----------------------------- Local macros ------------------------------ */
#define CB(x)               ((SSPBase*)(x))
#define CTRN(x)             ((SSPNodeTrn*)(x))
#define CNORM(x)            ((SSPNodeNormal*)(x))

#define ssp_isMatch()       (*pattern == '\0')
#define ssp_initSearch()    state = SSP_INSEARCH; result = SSP_INIT_SEARCH
#define ssp_isInPatt(x)     (*pattern == (x))
#define ssp_incPatt()       ++pattern

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
static const void *node;        /**	Points to current node */
static const SSPBranch *branch; /** Points to current branch */
static unsigned char *pattern;  /** Points to current pattern */
static int state;               /**	Maintains the current state of parser */
static int pos;                 /**	Current position in the pattern */
static SSPResult result;        /** Search result */

#if SSP_DEBUG == 1
static char strmap[2];
#if SSP_PRINT_FORMAT == 1
static char pfs[SSP_PRINT_FORMAT_SIZE];
#endif
#endif

/* ----------------------- Local function prototypes ----------------------- */
#if SSP_DEBUG == 1
static char *
ssp_mapChar(unsigned char c)
{
    if (!isprint(c))
    {
        return (char *)sspmap[c];
    }

    strmap[0] = c;
    strmap[1] = '\0';
    return strmap;
}

#if SSP_PRINT_FORMAT == 1
static void
printFormat(unsigned char out, char *fmt, ...)
{
    va_list args;

    (void)out;
    va_start(args, fmt);
    vsprintf(pfs, fmt, args);
    SSP_PUTS(out, pfs);
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
ssp_searchPatt(unsigned char c)
{
    for (branch = CB(node)->branchTbl, pattern = branch->patt; pattern != NULL;
         ++branch, pattern = branch->patt)
    {
        if (*pattern == c)
        {
            return SSP_FOUND;
        }
    }
    return SSP_NOT_FOUND;
}

static void
ssp_deliver(unsigned char c)
{
#if SSP_DEBUG == 1
    if (CB(node)->type == SSP_NTRN)
    {
        SSP_PRINT((0, "\tDeliver input '%s' (%2d)\n", ssp_mapChar(c), c));
    }
#endif
    if ((CB(node)->type == SSP_NTRN) && CTRN(node)->trnAction)
    {
        (*CTRN(node)->trnAction)(c);
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
ssp_notMatch(void)
{
    if (CB(node)->type == SSP_NTRN)
    {
        SSP_PRINT((0, "\tIn transparent node \"%s\"\n", CB(node)->name));
    }
    else
    {
        SSP_PRINT((0, "\tNot match. Changes to \"%s\" node\n",
                   CB(node)->name));
    }
    state = SSP_IDLE;
    result = SSP_UNMATCH;
}
#else
#define ssp_notMatch()         state = SSP_IDLE; result = SSP_UNMATCH
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
ssp_match(void)
{
    SSP_PRINT((0, "\tFind pattern \"%s\" from node \"%s\"\n",
               branch->patt,
               CB(node)->name));
    if (branch->branchAction)
    {
        (*branch->branchAction)((unsigned char)(pos + 1));
    }

    state = SSP_IDLE;
    if (branch->target != NULL)
    {
        node = branch->target;
    }

    SSP_PRINT((0, "\tChange to node \"%s\"\n", CB(node)->name));
    result = SSP_MATCH;
}

#if SSP_DEBUG == 1
static int
ssp_isEqual(unsigned char c)
{
    int r;

    r = (*(pattern - 1) == c);
    if (r)
    {
        SSP_PRINT((0, "\tRepeat input (match pos = %2d)\n", pos));
    }

    return r;
}
#else
#define ssp_isEqual(x)       (*(pattern - 1) == (x))
#endif

/* ---------------------------- Global functions --------------------------- */
int
ssp_init(SSP *const me, const SSPNodeNormal *root)
{
    int result = 1;

    if ((me != (SSP *)0) && (root != (const SSPNodeNormal *)0))
    {
        me->node = root;
        me->branch = CB(root)->branchTbl;
        me->state = SSP_IDLE;
        result = 0;
    }
    return result;
}

SSPResult 
ssp_doSearch(unsigned char c)
{
    int r;

    SSP_PRINT((0, "In: '%s' (%2d)\n", ssp_mapChar(c), c));
    switch (state)
    {
        case SSP_IDLE:
            r = ssp_searchPatt(c);
            ssp_deliver(c);

            if (r == SSP_FOUND)
            {
                pos = 0;

                SSP_PRINT((0, "\tFind in pattern \"%s\" from node \"%s\"",
                           branch->patt,
                           CB(node)->name));
                SSP_PRINT((0, " (match pos = %2d)\n", pos));

                ssp_incPatt();
                if (ssp_isMatch())
                {
                    ssp_match();
                }
                else
                {
                    ssp_initSearch();
                }
            }
            else
            {
                ssp_notMatch();
            }
            break;
        case SSP_INSEARCH:
            ssp_deliver(c);

            if (ssp_isInPatt(c))
            {
                ++pos;
                SSP_PRINT((0, "\tFind in pattern \"%s\" from node \"%s\"",
                           branch->patt,
                           CB(node)->name));
                SSP_PRINT((0, " (match pos = %2d)\n", pos));

                ssp_incPatt();
                if (ssp_isMatch())
                {
                    ssp_match();
                }
                else
                {
                    result = SSP_SEARCH_CONTINUES;
                }
            }
            else if (!ssp_isEqual(c))
            {
                ssp_notMatch();
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
