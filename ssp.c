/**
 * 	\file ssp.c
 *	
 *	This module implements a AT Hayes command parser.
 */


#include <stdlib.h>
#include "ssp.h"


#define CB(x)					((SSP_BASE_T*)(x))
#define CTRN(x)					((SSP_NTRN_T*)(x))
#define CNORM(x)				((SSP_NNORM_T*)(x))

#define ssp_is_match()			(*ppatt == '\0')
#define ssp_init_search()		(state = SSP_INSEARCH)
#define ssp_is_in_patt( x )		(*ppatt == (x))
#define ssp_inc_patt()			++ppatt


enum
{
	SSP_IDLE, SSP_INSEARCH
};


enum
{
	SSP_FOUND, SSP_NOT_FOUND
};


static const void *pnd;			/**	Points to current node */
static const SSP_BR_T *pbr;		/** Points to current branch */
static unsigned char *ppatt;	/** Points to current pattern */
static int state;				/**	Maintains the current state of parser */
static int pos;					/**	Current position in the pattern */


#if SSP_DEBUG == 1 && SSP_PRINT_FORMAT == 1

#include <stdarg.h>
#include <stdio.h>

static char pfs[ SSP_PRINT_FORMAT_SIZE ];

static
void
print_format( unsigned char out, char *fmt, ... )
{
	va_list args;

	(void)out;
	va_start( args, fmt );
	vsprintf( pfs, fmt, args );
	sspput_string( out, pfs );
	va_end( args );
}
#endif


#if SSP_DEBUG == 1

#include <ctype.h>

static char strmap[ 2 ];
static char *sspmap[] =
{
	"-", 	/* NUL 	- 00 */
	"-", 	/* SOH 	- 01 */
	"-", 	/* STX 	- 02 */
	"-", 	/* ETX 	- .. */
	"-", 	/* EOT 	- .. */
	"-", 	/* ENQ 	- .. */
	"-", 	/* ACK 	- .. */
	"-", 	/* BEL 	- .. */
	"-", 	/* BS  	- .. */
	"-", 	/* TAB 	- .. */
	"\\n",	/* LF  	- .. */
	"-", 	/* VT 	- .. */
	"-", 	/* FF	- .. */
	"\\r", 	/* CR	- .. */
	"-", 	/* SO	- .. */
	"-", 	/* SI 	- .. */
	"-", 	/* DLE 	- .. */
	"-", 	/* DC1 	- .. */
	"-", 	/* DC2 	- .. */
	"-", 	/* DC3 	- .. */
	"-", 	/* DC4 	- .. */
	"-", 	/* NAK 	- .. */
	"-", 	/* SYN 	- .. */
	"-", 	/* ETB 	- .. */
	"-", 	/* CAN 	- .. */
	"-", 	/* EM 	- .. */
	"-", 	/* SUB 	- .. */
	"-", 	/* ESC 	- .. */
	"-", 	/* FS 	- .. */
	"-", 	/* GS 	- .. */
	"-", 	/* RS 	- .. */
	"-"  	/* US 	- 1F */
};


static
char *
ssp_map_char( unsigned char c )
{
	if( !isprint( c ) )
		return sspmap[ c ];

	strmap[ 0 ] = c;
	strmap[ 1 ] = '\0';
	return strmap;
}
#endif


/**
 * 	\brief
 * 	Finds the c input in each of node's branch.
 * 	Also, it set the current branch to be used.
 *
 * 	\param c 	input character.
 *
 * 	\return
 * 	SSP_FOUND if the received character is found in the node pattern, 
 * 	otherwise SSP_NOT_FOUND.
 */

static
int
ssp_search_patt( unsigned char c )
{
   for( pbr = CB( pnd )->brtbl, ppatt = pbr->patt; ppatt != NULL; 
		   								++pbr, ppatt = pbr->patt )
      if( *ppatt == c )
         return SSP_FOUND;
   return SSP_NOT_FOUND;
}


static
void
ssp_deliver( unsigned char c )
{
#if SSP_DEBUG == 1
	if( CB( pnd )->type == SSP_NTRN )
		sspdprint(( 0, "\tDeliver input '%s' (%2d)\n", ssp_map_char( c ), c ));
#endif
	if( CB( pnd )->type == SSP_NTRN && CTRN( pnd )->trnact )
		( *CTRN( pnd )->trnact )( c );
}


/**
 * 	Sets the current node.
 * 	If the target node of the current branch is set to NULL then the current 
 * 	node will be left as is. This could be useful to define a transparent 
 * 	node.
 */

#if SSP_DEBUG == 1
static
void
ssp_not_match( void )
{
	if( CB( pnd )->type == SSP_NTRN )
		sspdprint(( 0, "\tIn transparent node \"%s\"\n", CB( pnd )->name ));
	else
		sspdprint(( 0, "\tNot match. Changes to \"%s\" node\n", 
											CB( pnd )->name ));
	state = SSP_IDLE;
}
#else
#define ssp_not_match()			(state = SSP_IDLE)
#endif



/**
 * 	\brief
 * 	This function is invoked on finding a match.
 * 	Invokes the branch action, set the ssp state to idle and set the new 
 * 	node according to next node in the branch.
 *
 * 	\note
 * 	If the next node is set to NULL then the current node will remain
 * 	unchanged.
 */

static
void
ssp_match( void )
{
	sspdprint(( 0, "\tFind pattern \"%s\" from node \"%s\"\n", 
												pbr->patt, 
												CB( pnd )->name ));
	if( pbr->bract )
		( *pbr->bract )( (unsigned char)( pos + 1 ) );

	state = SSP_IDLE;
	if( pbr->target != NULL )
		pnd = pbr->target;

	sspdprint(( 0, "\tChange to node \"%s\"\n", CB( pnd )->name ));
}


#if SSP_DEBUG == 1
static
int
ssp_is_equal( unsigned char c )
{
	int r;
	
	r = ( *( ppatt - 1 ) == c );
	if( r )
		sspdprint(( 0, "\tRepeat input (match pos = %2d)\n", pos ));

	return r;
}
#else
#define ssp_is_equal( x )		(*( ppatt - 1 ) == (x))
#endif


void
ssp_init( const SSP_NNORM_T *root )
{
	pnd = root;
	pbr = CB( pnd )->brtbl;
	state = SSP_IDLE;
}


void
ssp_do_search( unsigned char c )
{
	int r;

	sspdprint(( 0, "In: '%s' (%2d)\n", ssp_map_char( c ), c ));
	switch( state )
	{
		case SSP_IDLE:
			r = ssp_search_patt( c );
			ssp_deliver( c );

			if( r == SSP_FOUND )
			{
				pos = 0;

				sspdprint(( 0, "\tFind in pattern \"%s\" from node \"%s\"",
												pbr->patt, 
												CB( pnd )->name ));
				sspdprint(( 0, " (match pos = %2d)\n", pos ));

				ssp_inc_patt();
				if( ssp_is_match() )
					ssp_match();
				else
					ssp_init_search();
			}
			else
				ssp_not_match();
			break;
		case SSP_INSEARCH:
			ssp_deliver( c );

			if( ssp_is_in_patt( c ) )
			{
				++pos;
				sspdprint(( 0, "\tFind in pattern \"%s\" from node \"%s\"",
												pbr->patt, 
												CB( pnd )->name ));
				sspdprint(( 0, " (match pos = %2d)\n", pos ));

				ssp_inc_patt();
				if( ssp_is_match() )
					ssp_match();
			}
			else if( !ssp_is_equal( c ) )
				ssp_not_match();
			break;
		default:
			break;
	}
}
