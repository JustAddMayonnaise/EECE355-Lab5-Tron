/************* ubc-de-media.c ***************/

/*
 * Version 5.00 March 18, 2014
 *
 * For use with V5 of the UBC DE Media Computer.
 *
 */

#include "ubc-de-media.h"


// //////////////////////////////////////////////////////////////////////////
//
//
// Global variable definitions for all I/O address pointers
//
//


volatile unsigned int *pLEDR		= (volatile unsigned int *)ADDR_LEDR;
volatile unsigned int *pLEDG		= (volatile unsigned int *)ADDR_LEDG;
volatile unsigned int *pSWITCH		= (volatile unsigned int *)ADDR_SWITCH;
volatile unsigned int *pKEY		= (volatile unsigned int *)ADDR_KEY;

volatile unsigned int *pHEX7SEG		= (volatile unsigned int *)ADDR_HEX7SEG;
volatile unsigned int *pHEX7SEGA	= (volatile unsigned int *)ADDR_HEX7SEGA;
volatile unsigned int *pHEX7SEGB	= (volatile unsigned int *)ADDR_HEX7SEGB;

volatile unsigned int *pPS2		= (volatile unsigned int *)ADDR_PS2;

volatile unsigned int *pGPIO0		= (volatile unsigned int *)ADDR_GPIO0;
volatile unsigned int *pGPIO1		= (volatile unsigned int *)ADDR_GPIO1;

volatile unsigned int *pCOUNTER		= (volatile unsigned int *)(ADDR_COUNTER+0);
volatile unsigned int *pCOUNTER_RELOAD	= (volatile unsigned int *)(ADDR_COUNTER+4);
volatile unsigned int *pCOUNTER_STATUS	= (volatile unsigned int *)(ADDR_COUNTER+8);

volatile unsigned int *pSNDCTL		= (volatile unsigned int *)(ADDR_AUDIO + 0);
volatile unsigned int *pSNDRDY		= (volatile unsigned int *)(ADDR_AUDIO + 4);
volatile          int *pSNDL		= (volatile          int *)(ADDR_AUDIO + 8);
volatile          int *pSNDR		= (volatile          int *)(ADDR_AUDIO +12);

volatile unsigned int *pTERMINAL	= (volatile unsigned int *)(ADDR_JTAG + 0);
volatile unsigned int *pTERMINALCTL	= (volatile unsigned int *)(ADDR_JTAG + 4);

volatile unsigned int *pRS232		= (volatile unsigned int *)(ADDR_RS232 + 0);
volatile unsigned int *pRS232CTL	= (volatile unsigned int *)(ADDR_RS232 + 4);

volatile unsigned int *pACONFIG		= (volatile unsigned int *)(ADDR_AVCONFIG + 0);
volatile unsigned int *pVCONFIG		= (volatile unsigned int *)(ADDR_AVCONFIG + 12);
volatile unsigned int *pSNDCONFIG	= (volatile unsigned int *)(ADDR_AVCONFIG + 8);


volatile unsigned int *pKEY_IRQENABLE	= (volatile unsigned int *)(ADDR_KEY+8);
volatile unsigned int *pKEY_EDGECAPTURE	= (volatile unsigned int *)(ADDR_KEY+12);




// //////////////////////////////////////////////////////////////////////////
//
//
// Graphics frame buffer core 
//
//

void initChars()
{
	volatile unsigned char *pSRAMstart;
	volatile unsigned char *pSRAMend;
	volatile unsigned char *pSRAM;
	
	pSRAMstart = getCharAddr(0,0);
	pSRAMend   = getCharAddr(MAX_X_CHARS-1,MAX_Y_CHARS-1);
	
	for( pSRAM = pSRAMstart; pSRAM <= pSRAMend; pSRAM++ ) {
		*pSRAM = 0;
	}
}

void initScreen()
{
	volatile unsigned int *pSRAMstart;
	volatile unsigned int *pSRAMend;
	volatile unsigned int *pSRAM;

	pSRAMstart = (unsigned int *)ADDR_VGA;
	pSRAMend   = (unsigned int *)(ADDR_VGA + 4*(1<<MAX_X_BITS)*(1<<MAX_Y_BITS) );

	for( pSRAM = pSRAMstart; pSRAM < pSRAMend; pSRAM++ ) {
		*pSRAM = 0;
	}

	initChars();
}

static void drawLineH( int y, unsigned short colour )
{
	volatile unsigned int *pPixelPair;
	int x;
	unsigned int colourPair = colour << 16 | colour;
	pPixelPair = (unsigned int *)getPixelAddr(0,y);
	for( x=0; x < MAX_X_PIXELS; x+=2 )
	{
		*(pPixelPair++) = colourPair;
	}
	if( MAX_X_PIXELS % 1 ) {
		unsigned short *pPixel = getPixelAddr( MAX_X_PIXELS-1, y );
		*pPixel = colour;
	}
}


void fillScreen( unsigned short colour )
{
	int x, y;
	for( y=0; y < MAX_Y_PIXELS; y++ )
		drawLineH( y, colour );
}


void drawPixel( int x, int y, unsigned short colour )
{
	volatile unsigned short *pPixel;
	pPixel = getPixelAddr(x,y);
	*pPixel = colour;
}


unsigned short makeColour( int r, int g, int b )
{
	int rr  = (r&63)>>1;
	int gg  = (g&63);
	int bb  = (b&63)>>1;
	int rgb = ( (rr<<11) | (gg<<5) | (bb) )  &  0xffff;
	return (unsigned short)rgb;
}


unsigned short *getPixelAddr( int x, int y )
{
	y = max(0,min(MAX_Y_PIXELS-1,y));
	x = max(0,min(MAX_X_PIXELS-1,x));
	return (unsigned short *)(ADDR_VGA + (((y<<MAX_X_BITS)|(x))<<1));
}


void drawChar( int x, int y, int character )
{
	volatile unsigned char *pChar;
	pChar = getCharAddr(x,y);
	*pChar = ( character & 0x7f );
}


unsigned char *getCharAddr( int x, int y )
{
	y = max(0,min(MAX_Y_CHARS-1,y));
	x = max(0,min(MAX_X_CHARS-1,x));
	return (unsigned char *)(ADDR_CHARBUF + (((y<<MAX_X_CHAR_BITS)|(x))<<0));
}




// //////////////////////////////////////////////////////////////////////////
//
//
// RS232 core 
//
//


unsigned int getRS232_nowait()
{
	return *pRS232;
}

#define rs232bytes_available(c) (c>>16)


int getcharRS232()
{
	unsigned int rs232word;

	do {
		rs232word = *pRS232;
	} while( rs232bytes_available(rs232word) == 0 );

	return rs232word & 0xff;
}


void putcharRS232( int c )
{
	unsigned int rs232word;

	do {
		rs232word = *pRS232CTL;
	} while( rs232bytes_available(rs232word) == 0 );

	*pRS232 = c;
}


void flushRS232()
{
	int tries = 100, nchars;

	while( tries-- ) {
		timedDelay( ONE_MS );
		nchars = 1000;
		while( nchars-- ) {
			getRS232_nowait();
		}
	}

}






// //////////////////////////////////////////////////////////////////////////
//
//
// JTAG core 
//
//

// internal helper function
static char itoa( int i )
{
	int x = i & 0xf;
	if( x > 9 )
		x = 'A' + x - 10;
	else
		x = '0' + x;
	return x;
}

void printbyteJTAG( int c )
{
	int hichar, lochar;

	hichar = itoa( (c & 0xf0) >>4 );
	lochar = itoa( c );

	putcharJTAG( '0' );
	putcharJTAG( 'x' );
	putcharJTAG( hichar );
	putcharJTAG( lochar );
	putcharJTAG( ' ' );
}

unsigned int getJTAG_nowait()
{
	return *pTERMINAL;
}

int getcharJTAG()
{
	unsigned int jtagword;
	int data_ready;

	// wait until terminal sends a character
	do {
		jtagword = *pTERMINAL;
		data_ready = (jtagword>>15) & 1;
	} while( data_ready == 0 );

	return jtagword & 0xff;	// return the character

}

void putcharJTAG( int c )
{
	int num_bytes_available;

	// wait until terminal is ready to accept another character
	do {
		num_bytes_available = (*pTERMINALCTL) >> 16;
	} while( num_bytes_available == 0 );

	*pTERMINAL = c;		// output the character
}

void printstringJTAG( char *psz )
{
	while( *psz ) {
		putcharJTAG( *psz++ );
	}
}





// //////////////////////////////////////////////////////////////////////////
//
//
// PS2 core 
//
//

#define PS2_RESET		0xFF
#define PS2_ACK			0xFA
#define PS2_DEFAULTS		0xF6
#define PS2_ENABLE		0xF4
#define PS2_SETSAMPLERATE	0xF3
#define PS2_SETRESOLUTION	0xE8
#define PS2_TESTOK		0xAA

#define PS2_RESOLUTION_1_PER_MM	0x00
#define PS2_RESOLUTION_2_PER_MM	0x01
#define PS2_RESOLUTION_4_PER_MM	0x02
#define PS2_RESOLUTION_8_PER_MM	0x03

#define ps2bytes_available(c) (c>>16)



unsigned int getPS2_nowait()
{
	unsigned int ps2word = *pPS2;

	return ps2word;
}


int getcharPS2()
{
	unsigned int ps2word;

	do {
		ps2word = *pPS2;
	} while( (ps2bytes_available(ps2word) == 0) );
		// note: bit15 of ps2word does NOT indicate DATAVALID

	return ps2word & 0xff;
}


void putcharPS2( int c )
{
	*pPS2 = c;
}


void flushPS2()
{
	int tries = 20, nchars;

	while( tries-- ) {
		timedDelay( ONE_MS );
		nchars = 10000;
		while( nchars-- ) {
			getPS2_nowait();
		}
	}

}


static int sendPS2command( int command )
{
	putcharPS2( command );
	return getcharPS2();
}


static int sendPS2commandByte( int command, int byte )
{
	int c;
	
	c = sendPS2command( command );
	if( c != PS2_ACK ) goto cleanup;

	c = sendPS2command( byte );	

cleanup:
	return c;
}



// internal only routine
static int tryResetPS2()
{
	unsigned int c, tries;
	int device_found, STATUS = PS2_ERROR;

	flushPS2();

	c = sendPS2command( PS2_RESET );
	if( c != PS2_ACK ) goto cleanup;

	c = getcharPS2();
	if( c != PS2_TESTOK  ) goto cleanup;

	/* determine if mouse or kb */
	/* mouse returns 0x00, kb returns nothing */
	tries = 10;
	do {
		timedDelay( 2 * ONE_MS );
		c = getPS2_nowait();
		if( ps2bytes_available(c) > 0 ) break;
	} while( tries-- );

	c = c & 0xff;	// keep just character, if any

	if( tries > 0 && c == 0x00 ) {
		// we found a mouse !!
		printbyteJTAG( c );
		device_found = PS2_MOUSE;
	} else {
		printbyteJTAG( c );
		device_found = PS2_KB;
	}

	c = sendPS2command( PS2_DEFAULTS );
	if( c != PS2_ACK ) {
		STATUS = PS2_ERROR;
		goto cleanup;
	}

	if( device_found == PS2_MOUSE ) {
		// set low-resolution of 1-count per mm
		//c = sendPS2commandByte( PS2_SETRESOLUTION, PS2_RESOLUTION_1_PER_MM );
		//if( c != PS2_ACK ) goto cleanup;

		// send mouse movement data frequently (200Hz)
		// valid sample rates:  10, 20, 40, 60, 80, 100, 200
		c = sendPS2commandByte( PS2_SETSAMPLERATE, 200 );
		if( c != PS2_ACK ) goto cleanup;
	}

	// restore PS2 device to default settings and
	// enable the PS2 device to send data on events
	c = sendPS2command( PS2_ENABLE );
	if( c != PS2_ACK ) {
		STATUS = PS2_ERROR;
		goto cleanup;
	}

	STATUS = device_found;

cleanup:
	return STATUS;
	
}


int resetPS2()
{
	int ps2_status;
	do {
		ps2_status = tryResetPS2();
	} while( ps2_status == PS2_ERROR );

	return ps2_status;
}


#define isXneg(c)	(((c)&0x10)?1:0)
#define isYneg(c)	(((c)&0x20)?1:0)
#define isMoveNeg(c)	(((c)&0x80)?1:0)


static int last_ps2_byte = 0;

static int getMouseMotionMaybeWait2( int *pdx, int *pdy, int *pbuttons, int wait )
{
	int valid, c2_sign_mismatch, c3_sign_mismatch;
	int c1, c2, c3;
	int cc1, cc2, cc3;
	int dx, dy;


	// standard mouse protocol is to send 3 byte packets
	// c1: button and movement info
	// c2: x movement amount
	// c3: y movement amount
	//
	// c1 bits:
	//   bit7 - Y overflow
	//   bit6 - X overflow
	//   bit5 - Y sign bit
	//   bit4 - X sign bit
	//   bit3 - always '1'
	//   bit2 - middle button
	//   bit1 - right button
	//   bit0 - left button

// get bytes from the mouse.
// sometimes we get erroneous data from the mouse.
// i think it is because of a hardware bug.
// a common bug is the same byte value appearing twice
// in a row. try to detect & reject these bad packets.

// get first byte from mouse
	if( wait ) {
		c1 = cc1 = getcharPS2();
	} else {
		c1 = cc1 = getPS2_nowait();
		if( ps2bytes_available(c1) == 0 ) {
			goto cleanup;
		}
		c1 = cc1 = (c1 & 0xff);
	}

// check for bad values in first byte
	if( c1 == last_ps2_byte )	goto cleanup;
	if( !(c1 & 0x08) )		goto cleanup;
	if( c1 == PS2_ACK )		goto cleanup;

	// disallow 2 or more buttons at the same time
	if( ((c1&0x07)==0x07) )		goto cleanup;
	if( ((c1&0x07)==0x06) )		goto cleanup;
	if( ((c1&0x07)==0x05) )		goto cleanup;
	if( ((c1&0x07)==0x03) )		goto cleanup;

// get second byte from mouse

	c2 = cc2 = getcharPS2();
	if( c1 == c2 ) c2 = cc2 = getcharPS2();
	last_ps2_byte = cc2;

	c2_sign_mismatch = 1;
	if(  isXneg(c1) &&  isMoveNeg(c2) ) c2_sign_mismatch=0;
	if( !isXneg(c1) && !isMoveNeg(c2) ) c2_sign_mismatch=0;
	if( c2_sign_mismatch )		goto cleanup;

// get third byte from mouse

	c3 = cc3 = getcharPS2();
	last_ps2_byte = cc3;

	c3_sign_mismatch = 1;
	if(  isYneg(c1) &&  isMoveNeg(c3) ) c3_sign_mismatch=0;
	if( !isYneg(c1) && !isMoveNeg(c3) ) c3_sign_mismatch=0;
	if( c3_sign_mismatch && (c2 == c3) ) {
		c3 = cc3 = getcharPS2();
		if(  isYneg(c1) &&  isMoveNeg(c3) ) c3_sign_mismatch=0;
		if( !isYneg(c1) && !isMoveNeg(c3) ) c3_sign_mismatch=0;
	}
	if( c3_sign_mismatch )		goto cleanup;


// got entire mouse packet.
// verify it a little bit.


	// bit3 of c1 is always '1'.  this is not a guarantee
	// that we're perfectly synchronized with the mouse,
	// but it's close enough

	if( c1 & 0x08 ) {

		dx = isMoveNeg(c2) ? (0xffffff00|c2) : c2;
		dy = isMoveNeg(c3) ? (0xffffff00|c3) : c3;

		*pdx = dx;
		*pdy = dy;
		*pbuttons = c1 & (0x07);
#if 0
		if( c1&6 ) {
			printbyteJTAG( cc1 );
			printbyteJTAG( cc2 );
			printbyteJTAG( cc3 );
			putcharJTAG( '1' );
			putcharJTAG( '\n' );
		}
#endif
		return 1;	// indicates mouse motion
	}

cleanup:

#if 0
	printbyteJTAG( cc1 );
	printbyteJTAG( cc2 );
	printbyteJTAG( cc3 );
	putcharJTAG( '0' );
	putcharJTAG( '\n' );
#endif

	*pdx = 0;
	*pdy = 0;
	*pbuttons = 0;

	return 0; 	// indicates error or no motion
}

int getMouseMotion( int *pdx, int *pdy, int *pbuttons )
{
	return getMouseMotionMaybeWait2( pdx, pdy, pbuttons, 1 );
}

int getMouseMotion_nowait( int *pdx, int *pdy, int *pbuttons )
{
	return getMouseMotionMaybeWait2( pdx, pdy, pbuttons, 0 );
}





// //////////////////////////////////////////////////////////////////////////
//
//
// 50 MHz Counter core 
//
//

// Init counter:   *pCOUNTER = value;
// Read counter:   value = *pCOUNTER;
// When counter overflows, reload value is *pCOUNTER_RELOAD = (0 - 1000);
// Enable interrupts:  *pCOUNTER_STATUS = 1;
// ANY WRITE: clear interrupt
// eg, clear interrupt and disable, *pCOUNTER_STATUS = 0; 
// eg, clear interrupt and  enable, *pCOUNTER_STATUS = 1; 



unsigned int last_counter_value = 0;



// delay_amount is number of 50MHz cycles to pause since the last timedDelay call.
// for 120Hz updates, delay_amount = 50MHz/120 = 416,667


void timedDelay( int delay_amount )
{
	unsigned int current_counter;
	unsigned int elapsed_time;

	// by using the COUNTER, the time delay here is independent
	// of CPU speed.

	// notice this delay loop is written very carefully.
	// it reads the COUNTER just once per iteration.
	// also, it computes a time difference since the
	// last counter reading; this difference is important
	// and should automatically account for wrap-arounds.
	// if you rewrite the arithmetic in this loop, it may
	// not account for wrap-arounds correctly.

	do {
		current_counter = *pCOUNTER;
		elapsed_time = current_counter - last_counter_value;
	} while ( elapsed_time < delay_amount );

	last_counter_value = current_counter;
}



// //////////////////////////////////////////////////////////////////////////
//
//
// Interrupts core 
//
//



/* Things the user should not need to call or change */


/* Macros to run rdctl & wrctl assembly instructions */
#define RDCTL(RETURNLOC,CTLREG) __asm volatile ("rdctl %0, " CTLREG "\n" : "=r" (RETURNLOC))
#define WRCTL(WRITEVAL,CTLREG)  __asm volatile ("wrctl " CTLREG ", %0\n" : : "r" (WRITEVAL))

/* Control registers used in interrupt handling */
#define CTL_STATUS   "ctl0"
#define CTL_IENABLE  "ctl3"
#define CTL_IPENDING "ctl4"

//static void (*isr_table[NUM_IRQS])();	/* ISR table; table of functions used to service interrupts */
static ptr_to_function isr_table[NUM_IRQS];


static int getPendingInterrupts()
{
	int ipending;
	/* ipending is a bit vector of pending interrupts */
	RDCTL( ipending, CTL_IPENDING );
	return ipending;
}


void exceptionHandler()
{
	int i, ipending;
	
	ipending = getPendingInterrupts();
	
	/* Basic exception handler.  Service all pending interrupts, then return. */
	/* Note: when this routine ends, it goes back to the assembly routine named */
	/* the_exception, which will uses the ERET instruction to restore interrupts. */
	
	for(i = 0; i < NUM_IRQS; i++){ /* if a bit in ipending is set, jump to the ISR */
		if( ipending & (1<<i) ) {
			isr_table[i](); /* call ISR for each pending IRQ in table */
		}
	}
}

static void clearInterruptEnable( int irq_number )
{
	int ienabled;
	RDCTL( ienabled, CTL_IENABLE );
	ienabled = ienabled & ( ~(1 << irq_number) );
	WRCTL( ienabled, CTL_IENABLE );
}

static void setInterruptEnable( int irq_number )
{
	int ienabled;
	RDCTL( ienabled, CTL_IENABLE );
	ienabled = ienabled | (1 << irq_number );
	WRCTL( ienabled, CTL_IENABLE );
}



/* Functions the programmer can use */

void emptyISR()
{
}

void disableInterrupts()
{
	WRCTL( 0, CTL_STATUS );
}

void enableInterrupts()
{
	WRCTL( 1, CTL_STATUS );
}

#define assert(expn) do {if(!(expn)) return;} while(0)

void disableInterrupt( int irq_number )
{
	assert( irq_number < NUM_IRQS );
	disableInterrupts();
	clearInterruptEnable( irq_number );
	enableInterrupts();
}

void enableInterrupt( int irq_number )
{
	assert( irq_number < NUM_IRQS );
	disableInterrupts();
	setInterruptEnable( irq_number );
	enableInterrupts();
}

void unregisterISR( int irq_number )
{	
	assert( irq_number < NUM_IRQS );
	disableInterrupts();
	clearInterruptEnable( irq_number );
	isr_table[irq_number] = emptyISR;
	enableInterrupts();
}

void registerISR( int irq_number, ptr_to_function isr )
{
	assert( irq_number < NUM_IRQS );
	
	disableInterrupts();
	isr_table[irq_number] = isr;
	//setInterruptEnable( irq_number );	// do not enable by default
	enableInterrupts();
}

#undef assert


/* Turn off all interrupts, point to our exception handler, then enable interrupts */
/* Disables all interrupt enables, so this should be done before registering ISRs  */
void initInterrupts()
{
	int i;
	disableInterrupts();
	for( i=0; i < NUM_IRQS; i++ )	
		isr_table[i] = emptyISR;		/* clear ISR table */
	WRCTL( 0, CTL_IENABLE );	/* clear all individual interrupt enables */
	enableInterrupts();
}



#if 0
/* The assembly language code below handles CPU reset processing */
void the_reset (void) __attribute__ ((section (".reset")));
void the_reset (void)
/************************************************************************************
 * Reset code. By giving the code a section attribute with the name ".reset" we     *
 * allow the linker program to locate this code at the proper reset vector address. *
 * This code just calls the main program.                                           *
 ***********************************************************************************/
{
	asm (".set		noat");					// Magic, for the C compiler
	asm (".set		nobreak");				// Magic, for the C compiler
	asm ("movia		r2, main");				// Call the C language main program
	asm ("jmp		r2"); 
}
#endif


/* The assembly language code below handles CPU exception processing. This
 * code should not be modified; instead, the C language code in the function
 * interrupt_handler() can be modified as needed for a given application.
 */
void the_exception (void) __attribute__ ((section (".exceptions")));
void the_exception (void)
/*******************************************************************************
 * Exceptions code. By giving the code a section attribute with the name       *
 * ".exceptions" we allow the linker program to locate this code at the proper *
 * exceptions vector address.                                                  *
 * This code calls the interrupt handler and later returns from the exception. *
 ******************************************************************************/
{
	asm ( ".set		noat" );				// Magic, for the C compiler
	asm ( ".set		nobreak" );				// Magic, for the C compiler
	asm ( "subi		sp, sp, 128" );
	asm ( "stw		et, 96(sp)" );
	asm ( "rdctl		et, ctl4" );
	asm ( "beq		et, r0, SKIP_EA_DEC" );		// Interrupt is not external         
	asm ( "subi		ea, ea, 4" );				/* Must decrement ea by one instruction  
									 * for external interupts, so that the 
									 * interrupted instruction will be run */
	
	asm ( "SKIP_EA_DEC:" );
	asm (	"stw	r1,  4(sp)" );				// Save all registers 
	asm (	"stw	r2,  8(sp)" );
	asm (	"stw	r3,  12(sp)" );
	asm (	"stw	r4,  16(sp)" );
	asm (	"stw	r5,  20(sp)" );
	asm (	"stw	r6,  24(sp)" );
	asm (	"stw	r7,  28(sp)" );
	asm (	"stw	r8,  32(sp)" );
	asm (	"stw	r9,  36(sp)" );
	asm (	"stw	r10, 40(sp)" );
	asm (	"stw	r11, 44(sp)" );
	asm (	"stw	r12, 48(sp)" );
	asm (	"stw	r13, 52(sp)" );
	asm (	"stw	r14, 56(sp)" );
	asm (	"stw	r15, 60(sp)" );
	asm (	"stw	r16, 64(sp)" );
	asm (	"stw	r17, 68(sp)" );
	asm (	"stw	r18, 72(sp)" );
	asm (	"stw	r19, 76(sp)" );
	asm (	"stw	r20, 80(sp)" );
	asm (	"stw	r21, 84(sp)" );
	asm (	"stw	r22, 88(sp)" );
	asm (	"stw	r23, 92(sp)" );
	asm (	"stw	r25, 100(sp)" );				// r25 = bt (skip r24 = et, because it is saved above)
	asm (	"stw	r26, 104(sp)" );				// r26 = gp
	// skip r27 because it is sp, and there is no point in saving this
	asm (	"stw	r28, 112(sp)" );				// r28 = fp
	asm (	"stw	r29, 116(sp)" );				// r29 = ea
	asm (	"stw	r30, 120(sp)" );				// r30 = ba
	asm (	"stw	r31, 124(sp)" );				// r31 = ra
	asm (	"addi	fp,  sp, 128" );
	
	asm (	"call	exceptionHandler" );			// Call the C language interrupt handler
	
	asm (	"ldw	r1,  4(sp)" );				// Restore all registers
	asm (	"ldw	r2,  8(sp)" );
	asm (	"ldw	r3,  12(sp)" );
	asm (	"ldw	r4,  16(sp)" );
	asm (	"ldw	r5,  20(sp)" );
	asm (	"ldw	r6,  24(sp)" );
	asm (	"ldw	r7,  28(sp)" );
	asm (	"ldw	r8,  32(sp)" );
	asm (	"ldw	r9,  36(sp)" );
	asm (	"ldw	r10, 40(sp)" );
	asm (	"ldw	r11, 44(sp)" );
	asm (	"ldw	r12, 48(sp)" );
	asm (	"ldw	r13, 52(sp)" );
	asm (	"ldw	r14, 56(sp)" );
	asm (	"ldw	r15, 60(sp)" );
	asm (	"ldw	r16, 64(sp)" );
	asm (	"ldw	r17, 68(sp)" );
	asm (	"ldw	r18, 72(sp)" );
	asm (	"ldw	r19, 76(sp)" );
	asm (	"ldw	r20, 80(sp)" );
	asm (	"ldw	r21, 84(sp)" );
	asm (	"ldw	r22, 88(sp)" );
	asm (	"ldw	r23, 92(sp)" );
	asm (	"ldw	r24, 96(sp)" );
	asm (	"ldw	r25, 100(sp)" );				// r25 = bt
	asm (	"ldw	r26, 104(sp)" );				// r26 = gp
	// skip r27 because it is sp, and we did not save this on the stack
	asm (	"ldw	r28, 112(sp)" );				// r28 = fp
	asm (	"ldw	r29, 116(sp)" );				// r29 = ea
	asm (	"ldw	r30, 120(sp)" );				// r30 = ba
	asm (	"ldw	r31, 124(sp)" );				// r31 = ra
	
	asm (	"addi	sp,  sp, 128" );
	
	asm (	"eret" );
}
