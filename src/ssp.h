/**
 * 	\file ssp.h
 *
 * 	String search parser interface.
 */


#ifndef __SSP_H__
#define __SSP_H__


#include "sspcfg.h"


#if SSP_EN_NODE_NAME == 1
	#define mkbase(t,n)				{t,n##_tbl,#n}
#else
	#define mkbase(t,n)				{t,n##_tbl}
#endif


/**
 *	This macro creates a normal node.
 *
 *	\note
 *
 *	See SSP_NNORM_T structure definition for more information.
 *
 * 	\param name		node name. Represents a normal node structure.
 */

#define SSP_CREATE_NORMAL_NODE( name )									\
																		\
								extern const SSP_BR_T name##_tbl[];		\
																		\
								const SSP_NNORM_T name =				\
								{										\
									mkbase(SSP_NNORM,name)				\
								}


/**
 *	This macro creates a transparent node.
 *
 *	\note
 *
 *	See SSP_NTRN_T structure definition for more information.
 *
 * 	\param name		node name. Represents a transparent node structure.
 * 	\param c		pointer to collection action. This argument is 
 *					optional, thus it could be declared as NULL.
 */

#define SSP_CREATE_TRN_NODE( name, c )									\
																		\
								extern const SSP_BR_T name##_tbl[];		\
																		\
								const SSP_NTRN_T name =					\
								{										\
									mkbase(SSP_NTRN,name),				\
									c									\
								}


/**
 *	This macro creates a node branch table. Use the 
 *	'SSP_END_BR_TABLE' macro to terminate the branch table.
 *
 * 	\param name		node name.
 */

#define SSP_CREATE_BR_TABLE( name )										\
																		\
								const SSP_BR_T name##_tbl[]={

/**
 * 	\brief
 *	This macro defines a tree branch or node transition.
 *
 *	Example:
 *	\code
 *	SSP_CREATE_NORMAL_NODE( root );
 *	SSP_CREATE_BR_TABLE( root )
 *		{	"ok",			// pattern "ok"
 *			NULL, 			// branch action is not used
 *			&node_ok	},	// next tree node
 *		{ "no", NULL, &node_no	},
 *	SSP_END_BR_TABLE
 *	\endcode
 *
 *	\sa
 *	SSP_BR_T structure definition for more information.
 *
 * 	\param patt		pattern to search. String terminated in '\\0'
 * 	\param bract	pointer to action function. This function is invoked 
 * 					when the pattern is found. This argument is optional, 
 * 					thus it could be declared as NULL.
 * 	\param target	pointer to target node.
 */

#define SSPBR( patt, bract, target )		\
						{ (unsigned char*)patt, bract, target }


/**
 *	This macro is used to terminate a state transition table.
 */

#define SSP_END_BR_TABLE		{ NULL, NULL, NULL }};


/**@{
 *
 * 	Declares a previously created node to be used as a global object.  
 */

#define SSP_DCLR_NORMAL_NODE	extern const SSP_NNORM_T
#define SSP_DCLR_TRN_NODE		extern const SSP_NTRN_T

/*@}*/


/**
 *	Defines SSP debug interface.
 */

#define sspput_string(ch,x)		printf("%s",(x))
#define sspput_char(ch,x)		putc((x),stdout)

#if SSP_DEBUG == 1
#define sspdprint(x)			print_format##x
#else
#define sspdprint(x)
#endif


enum
{
	SSP_NNORM,
	SSP_NTRN
};


typedef void ( *SSP_TRNA_T )( unsigned char c );
typedef void ( *SSP_BRA_T )( unsigned char pos );


/**
 * 	\brief 
 * 	Maintains the basic information of a node.
 */

typedef struct ssp_base_t
{
	/**	
	 *	Contains the type of a particular node and can have 
	 *	the following values:
	 *
	 *	- \b SSP_NNORM: 		normal node.
	 *	- \b SSP_NTRN: 			transparent node.
	 */

	unsigned char type;

	/**	
	 *	Points to node's branch table.
	 */
	
	const struct ssp_br_t *brtbl;

	/**	
	 *	Node name. 
	 *	String terminated in '\\0' that represents the name 
	 *	of node. It's generally used for debugging.
	 */

	char *name;
} SSP_BASE_T;


/**
 * 	\brief
 * 	Describes the tree branch or node transition. 
 */

typedef struct ssp_br_t
{
	/**
	 * 	Pattern to search.
	 * 	String terminated in '\\0'.
	 */

	unsigned char *patt;

	/**
	 * 	Points to action function.
	 * 	This function is invoked when the pattern is found.
	 */
	SSP_BRA_T bract;

	/**
	 * 	Points to target node.
	 */

	const void *target;
} SSP_BR_T;


typedef struct ssp_nnorm_t
{
	struct ssp_base_t base;
} SSP_NNORM_T;


typedef struct ssp_ntrn_t
{
	struct ssp_base_t base;

	/**
	 * 	Points to action function.
	 * 	This function is invoked on arriving a input character.
	 */

	SSP_TRNA_T trnact;
} SSP_NTRN_T;


/**
 * 	\brief
 * 	Initialization function.
 *	Also, could be used to reset the parser.
 *
 * 	\param root		pointer to root node. The topmost node in a tree.
 */

void ssp_init( const SSP_NNORM_T *root );


/**
 * 	\brief
 * 	Find a character in the tree.
 *
 * 	\param c		input character.
 */

void ssp_do_search( unsigned char c );


#endif
