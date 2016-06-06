#ifndef __UBCDEMEDIA_H__
#define __UBCDEMEDIA_H__

/************* ubc-de-media.h ***************/

/*
 * Version 5.00 March 18, 2014
 *
 * For use with V5 of the UBC DE Media Computer.
 *
 */

// some useful macros
#define max(a,b)	(((a)<(b))?(b):(a))
#define min(a,b)	(((a)<(b))?(a):(b))




// //////////////////////////////////////////////////////////////////////////
//
//
// Global constant declarations for all I/O addresses
//
//

// Example usage:
//
//    volatile int *pLEDR = ADDR_LEDR;
//    *pLEDR = 0x8F;
//
//    int switch = *pSWITCH;
//    switch = switch & 0x04;   // isolate SW2
//
//
// NB: in some addresses below, bit 31 (0x80000000) is set
//     to force all load/store accesses to be uncached.

#define	ADDR_LEDR	0x90000000	/* output only, RED (DE1 10b, DE2 18b) */
#define	ADDR_LEDG	0x90000010	/* output only, GREEN (DE1 8b, DE2 9b) */
#define	ADDR_HEX7SEG	0x90000020	/* output only, HEX3 to HEX0 (32b) */
#define	ADDR_HEX7SEGA	0x90000020	/* output only, HEX3 to HEX0 (32b) */
#define	ADDR_HEX7SEGB	0x90000030	/* output only, HEX7 to HEX4 (32b), DE2 only */
#define	ADDR_SWITCH	0x90000040	/* input only, (DE1 10b, DE2 18b) */
#define	ADDR_KEY	0x90000050	/* input only, KEY3 to KEY0  (4b) */
#define	ADDR_GPIO0	0x90000060 	/* I/O connections to connector JP1/GPIO0 */
#define	ADDR_GPIO1	0x90000070 	/* I/O connections to connector JP2/GPIO1 */
#define	ADDR_COUNTER	0x90000080	/* input only, 50 MHz counter (32b) */

#define	ADDR_PS2	0x90000100	/* in/out, PS2 connector for kb or mouse (8b data) */

#define	ADDR_JTAG	0x90001000	/* send/recv chars to Terminal window */
#define	ADDR_RS232	0x90001040 	/* send/recv chars to RS232 connector */

#define ADDR_TIMER	0x90002000	/* timer controller */
#define	ADDR_SYSID	0x90002100	/* unique systemID */

#define	ADDR_AVCONFIG	0x90003000 	/* configure audio and vga */
#define	ADDR_AUDIO	0x90003040 	/* audio registers */
#define ADDR_LCD	0x90003050  /* 16x2 character LCD, DE2 only */

#define	ADDR_VGA 	0x88000000 	/* vga pixel framebuffer */
#define	ADDR_CHARBUF 	0x89000000 	/* vga character framebuffer */

#define	ADDR_SDRAM	0x00000000	/* SDRAM chip, 8MB, cached */
#define	ADDR_SDRAM_END	0x00800000

#define	ADDR_SRAM	0x08000000	/* SRAM chip, 512kB, also used for vga pixel framebuffer */
#define	ADDR_SRAM_END	0x08080000

#define ADDR_FLASH	0x01000000	/* Flash ROM, 4MB */
#define ADDR_FLASH_END	0x01400000



// Global variable declarations

extern volatile unsigned int *pLEDR;
extern volatile unsigned int *pLEDG;
extern volatile unsigned int *pSWITCH;
extern volatile unsigned int *pKEY;
extern volatile unsigned int *pHEX7SEG;
extern volatile unsigned int *pHEX7SEGA;
extern volatile unsigned int *pHEX7SEGB;
extern volatile unsigned int *pCOUNTER;
extern volatile unsigned int *pPS2;
extern volatile unsigned int *pSNDCTL;
extern volatile unsigned int *pSNDRDY;
extern volatile          int *pSNDL;
extern volatile          int *pSNDR;
extern volatile unsigned int *pTERMINAL;
extern volatile unsigned int *pTERMINALCTL;
extern volatile unsigned int *pRS232;
extern volatile unsigned int *pRS232CTL;
extern volatile unsigned int *pAVCONFIG;
extern volatile unsigned int *pSNDCONFIG;

extern volatile unsigned int *pGPIO0;
extern volatile unsigned int *pGPIO1;

extern volatile unsigned int *pCOUNTER_RELOAD;
extern volatile unsigned int *pCOUNTER_STATUS;

extern volatile unsigned int *pKEY_EDGECAPTURE;
extern volatile unsigned int *pKEY_IRQENABLE;

extern unsigned int last_counter_value;



// //////////////////////////////////////////////////////////////////////////
//
//
// HEX 7 SEGMENT display 
//
//

// Write "0" to a bit to turn "off" a 7-segment display LED.
// Each bit position at address ADDR_HEX7SEG works as follows:
//     HEX3       HEX2        HEX1       HEX0
//     24          16           8          0
//    ----        ----        ----       ----
// 29|    |25  21|    |17  13|    |9   5|    |1
//   | 30 |      | 22 |      | 14 |     |  6 |
//    ----        ----        ----       ----
//   |    |      |    |      |    |     |    |
// 28|    |26  20|    |18  12|    |10  4|    |2
//    ----        ----        ----       ----
//     27          19          11         3
// Note that bits 31, 23, 15, and 7 are not used.
//

#define DIGIT_  0x00 /* blank space */
#define	DIGIT0	0x3F /* 0xC0 */
#define	DIGIT1	0x06 /* 0xF9 */
#define	DIGIT2	0x5B /* 0xA4 */
#define	DIGIT3	0x4F /* 0xB0 */
#define	DIGIT4	0x66 /* 0x99 */
#define	DIGIT5	0x6D /* 0x92 */
#define	DIGIT6	0x7D /* 0x82 */
#define	DIGIT7	0x07 /* 0xF8 */
#define	DIGIT8	0x7F /* 0x80 */
#define	DIGIT9	0x67 /* 0x98 */
#define	DIGITA	0x77 /* 0x88 */
#define	DIGITB	0x7C /* 0x83 */
#define	DIGITC	0x39 /* 0xC6 */
#define	DIGITD	0x5E /* 0xA1 */
#define	DIGITE	0x79 /* 0x86 */
#define	DIGITF	0x71 /* 0x8E */




// //////////////////////////////////////////////////////////////////////////
//
//
// VGA or LCD frame buffer core  
//
//

#define MAX_X_PIXELS	320
#define MAX_Y_PIXELS	240
#define	MAX_X_BITS	9	/* CEIL(LOG2(MAX_X_PIXELS)) */
#define	MAX_Y_BITS	8	/* CEIL(LOG2(MAX_Y_PIXELS)) */

#define MAX_X_CHARS	40
#define MAX_Y_CHARS	30
#define	MAX_X_CHAR_BITS	6	/* CEIL(LOG2(MAX_X_CHARS)) */
#define	MAX_Y_CHAR_BITS	5	/* CEIL(LOG2(MAX_Y_CHARS)) */

void
initChars();

void
initScreen();

void 
fillScreen( unsigned short colour );

void 
drawPixel( int x, int y, unsigned short colour );

unsigned short 
makeColour( int r, int g, int b );

unsigned short *
getPixelAddr( int x, int y );

void 
drawChar( int x, int y, int character );

unsigned char *
getCharAddr( int x, int y );




// //////////////////////////////////////////////////////////////////////////
//
//
// RS232 core 
//
//

unsigned int
getRS2332_nowait();

int
getcharRS232();

void
putcharRS232( int c );

void
flushRS232();



// //////////////////////////////////////////////////////////////////////////
//
//
// JTAG core 
//
//

unsigned int
getJTAG_nowait();

int
getcharJTAG();

void
putcharJTAG( int c );

void
printstringJTAG( char *psz );

void
printbyteJTAG( int c );




// //////////////////////////////////////////////////////////////////////////
//
//
// PS2 core 
//
//


#define PS2_ERROR	0
#define PS2_MOUSE	1
#define PS2_KB		2

#define MOUSE_BUTTON1	0x01
#define MOUSE_BUTTON2	0x04
#define MOUSE_BUTTON3	0x02


unsigned int
getPS2_nowait();

int
getcharPS2();

void
putcharPS2( int c );

void
flushPS2();

int
resetPS2();

int
getMouseMotion( int *pdx, int *pdy, int *pbuttons );

int
getMouseMotion_nowait( int *pdx, int *pdy, int *pbuttons );





// //////////////////////////////////////////////////////////////////////////
//
//
// 50 MHz Counter core 
//
//

#define ONE_MS 50000


void
timedDelay( int delay_amount );



// //////////////////////////////////////////////////////////////////////////
//
//
// Interrupts core 
//
//

/* Total number of IRQS in the system */
#define NUM_IRQS        13

/* IRQ numbers for each device */
#define IRQ_TIMER   0
#define IRQ_KEY     1
#define IRQ_COUNTER 3
#define IRQ_AUDIO   6
#define IRQ_PS2     7
#define IRQ_JTAG    8
#define IRQ_RS232   10
#define IRQ_GPIO0   11  // JP1
#define IRQ_GPIO1   12  // JP2



typedef void(*ptr_to_function)(void);


void
emptyISR();

void
initInterrupts();

void
disableInterrupts();

void
enableInterrupts();

void
disableInterrupt( int irq_number );

void
enableInterrupt( int irq_number );

void
unregisterISR( int irq_number );

void
registerISR( int irq_number, ptr_to_function isr );

#endif // __UBCDE1MEDIA_H__

