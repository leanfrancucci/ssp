/**
 * 	\file main.c
 */


#include <stdlib.h>
#include <stdio.h>
#include "ssp.h"


#define MAX_EXP_CHS			3


SSP_DCLR_NORMAL_NODE	root, 
	   					node_no,
	   					node_ok;

SSP_DCLR_TRN_NODE		node_trn;


static unsigned char buff[ 16 ], *pbuff;
static int ixbuff;


static
void
none( unsigned char pos )
{
}


static
void
patt_frm( unsigned char pos )
{
	pbuff = buff;
	ixbuff = 0;
	printf( "\t%s: expected at least %d characters\n", __FUNCTION__, 
														MAX_EXP_CHS );
}


static
void
patt_ok( unsigned char pos )
{
	int i;

	printf( "\t%s: #%d ch collected =", __FUNCTION__, ixbuff );

	for( i = 0, pbuff = buff; i < ixbuff; ++i, ++pbuff )
		printf( " %c", *pbuff );

	putchar( '\n' );
}


void
collect( unsigned char c )
{
	if( ixbuff < MAX_EXP_CHS )
	{
		printf( "\t%s: collect[%d] = %c (%2d)\n", __FUNCTION__, 
													ixbuff, c, c );
		*pbuff++ = c;
		++ixbuff;
	}
}


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


int
main( void )
{
	ssp_init( &root );

	ssp_do_search( '\r' );
	ssp_do_search( '\n' );

	ssp_do_search( 'n' );
	ssp_do_search( 'o' );
	ssp_do_search( '-' );

	ssp_do_search( 'a' );
	ssp_do_search( 'b' );
	ssp_do_search( 'c' );

	ssp_do_search( '+' );

	ssp_do_search( 'o' );
	ssp_do_search( 'k' );

	ssp_do_search( 'f' );
	ssp_do_search( 'r' );
	ssp_do_search( 'm' );

	ssp_do_search( 'a' );
	ssp_do_search( 'b' );
	ssp_do_search( 'c' );

	ssp_do_search( 'o' );
	ssp_do_search( 'k' );

	ssp_do_search( 'n' );
	ssp_do_search( 'o' );

	getchar();
}
