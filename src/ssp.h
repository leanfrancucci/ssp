/**
 *  \file   ssp.h
 *  \brief  String Search Parser interface.
 */

/* -------------------------- Development history -------------------------- */
/* -------------------------------- Authors -------------------------------- */
/* --------------------------------- Notes --------------------------------- */
/* --------------------------------- Module -------------------------------- */
#ifndef __SSP_H__
#define __SSP_H__

/* ----------------------------- Include files ----------------------------- */
#include "sspcfg.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
#if SSP_EN_NODE_NAME == 1
#define MK_BASE(t,n)    {t, n##_tbl, #n}
#else
#define MK_BASE(t,n)    {t, n##_tbl}
#endif

/**
 *  \brief
 *	This macro creates a normal node.
 *
 *  \param[in] name     node name. Represents a normal node structure.
 *
 *	\note
 *	See SSPNodeNormal structure definition for more information.
 */
#define SSP_CREATE_NORMAL_NODE(name) \
            extern const SSPBranch name##_tbl[]; \
            const SSPNodeNormal name = \
            { \
                MK_BASE(SSP_NNORM, name) \
            }

/**
 *  \brief
 *	This macro creates a transparent node.
 *
 *  \param[in] name		node name. Represents a transparent node structure.
 *  \param[in] c		pointer to collection action. This argument is
 *					    optional, thus it could be declared as NULL.
 *
 *	\note
 *	See SSPNodeTrn structure definition for more information.
 */
#define SSP_CREATE_TRN_NODE(name, c) \
            extern const SSPBranch name##_tbl[]; \
            const SSPNodeTrn name = \
            { \
                MK_BASE(SSP_NTRN, name), \
                c \
            }

/**
 *  \brief
 *	This macro creates a node branch table. Use the
 *	'SSP_END_BR_TABLE' macro to terminate the branch table.
 *
 *  \param name[in]		node name.
 */
#define SSP_CREATE_BR_TABLE(name) \
    const SSPBranch name##_tbl[]= \
    {

/**
 *  \brief
 *	This macro defines a tree branch or node transition.
 *
 *  \param patt[in]		pattern to search. String terminated in '\\0'
 *  \param branchAction[in]	pointer to action function. This function is 
 *                      invoked when the pattern is found. This argument is 
 *                      optional, thus it could be declared as NULL.
 *  \param target[in]	pointer to target node.
 *
 *  \usage
 *	\code
 *	SSP_CREATE_NORMAL_NODE( root );
 *	SSP_CREATE_BR_TABLE( root )
 *		{	"ok",			// pattern "ok"
 *			NULL,           // branch action is not used
 *			&node_ok	},	// next tree node
 *		{ "no", NULL, &node_no	},
 *	SSP_END_BR_TABLE
 *	\endcode
 *
 *	\sa
 *	SSPBranch structure definition for more information.
 */
#define SSPBR(patt, branchAction, target) \
    {(unsigned char*)patt, branchAction, (SSPBase *)target}

/**
 *  \brief
 *	This macro is used to terminate a state transition table.
 */
#define SSP_END_BR_TABLE        {NULL, NULL, NULL}};

/**@{
 *  \brief
 *  Declares a previously created node to be used as a global object.
 */
#define SSP_DCLR_NORMAL_NODE    extern const SSPNodeNormal
#define SSP_DCLR_TRN_NODE       extern const SSPNodeTrn
/*@}*/

#if SSP_DEBUG == 1
/*
 *	Defines SSP debug interface.
 */
#define SSP_PUTS(ch,x)      printf("%s", (x))
#define SSP_PUTC(ch,x)      putc((x), stdout)
#define SSP_PRINT(x)        print_format##x
#else
#define SSP_PRINT(x)
#endif

/* -------------------------------- Constants ------------------------------ */
enum
{
    SSP_NNORM,
    SSP_NTRN
};

typedef enum SSPResult
{
    SSP_MATCH,
    SSP_UNMATCH,
    SSP_INIT_SEARCH,
    SSP_SEARCH_CONTINUES,
    SSP_DUPLICATED_CHAR
} SSPResult;

/* ------------------------------- Data types ------------------------------ */
typedef void (*SSPTrnAction)(unsigned char c);
typedef void (*SSPBranchAction)(unsigned char pos);

/**
 *  \brief
 *  Maintains the basic information of a node.
 */
typedef struct SSPBase SSPBase;
struct SSPBase
{
    /**
     *  \brief
     *  Type of node.
     *	Contains the type of a particular node and can have the following 
     *	values:
     *
     *	- \b SSP_NNORM: normal node.
     *	- \b SSP_NTRN:  transparent node.
     */
    unsigned char type;

    /**
     *  \brief
     *	Points to node's branch table.
     */
    const struct SSPBranch *branchTbl;

    /**
     *  \brief
     *	Node name.
     *	String terminated in '\\0' that represents the name of node. It's 
     *	generally used for debugging.
     */
    char *name;
};

/**
 *  \brief
 *  Describes the tree branch or node transition.
 */
typedef struct SSPBranch SSPBranch;
struct SSPBranch
{
    /**
     *  \brief
     *  Pattern to search.
     *  String terminated in '\\0'.
     */
    unsigned char *patt;

    /**
     *  \brief
     *  Points to action function.
     *  This function is invoked when the pattern is found.
     */
    SSPBranchAction branchAction;

    /**
     *  \brief
     *  Points to target node.
     */
    const SSPBase *target;
};

typedef struct SSPNodeNormal SSPNodeNormal;
struct SSPNodeNormal
{
    /**
     *  \brief
     *	Maintains the basic information of node.
     */
    SSPBase base;
};

typedef struct SSPNodeTrn SSPNodeTrn;
struct SSPNodeTrn
{
    /**
     *  \brief
     *	Maintains the basic information of node.
     */
    SSPBase base;

    /**
     *  \brief
     *  Points to action function.
     *  This function is invoked on arriving a input character.
     */
    SSPTrnAction trnAction;
};

/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
/**
 *  \brief
 *  Initialization function.
 *	Also, could be used to reset the parser.
 *
 *  \param root		pointer to root node. The topmost node in a tree.
 */
void ssp_init(const SSPNodeNormal *root);

/**
 *  \brief
 *  Find a character in the tree.
 *
 *  \param      c input character.
 *  \return     Result of search.
 */
SSPResult ssp_doSearch(unsigned char c);

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
