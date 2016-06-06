/************* ubc-de-media.s ***************/

/* 
 * Version 5.00 March 18, 2014
 *
 * For use with V5 of the UBC DE Media Computer.
 *
 */

/* I/O addresses, eg:
 *    movia r23, ADDR_LEDR
 *    movi  r2, 0x08
 *    stwio r2, 0(r23)
 */
.equ	ADDR_LEDR,		0x90000000	/* output only, RED (DE1 10b, DE2 18b) */
.equ	ADDR_LEDG,		0x90000010	/* output only, GREEN (DE1 8b, DE2 9b) */
.equ	ADDR_HEX7SEG,	0x90000020	/* output only, HEX3 to HEX0 (32b) */
.equ	ADDR_HEX7SEGA,	0x90000020	/* output only, HEX3 to HEX0 (32b) */
.equ	ADDR_HEX7SEGB,	0x90000030	/* output only, HEX7 to HEX4 (32b), DE2 only */
.equ	ADDR_SWITCH,	0x90000040	/* input only, (DE1 10b, DE2 18b) */
.equ	ADDR_KEY,		0x90000050	/* input only, KEY3 to KEY0  (4b) */
.equ	ADDR_GPIO0, 	0x90000060 	/* I/O connections to connector JP1/GPIO0 */
.equ	ADDR_GPIO1, 	0x90000070 	/* I/O connections to connector JP2/GPIO1 */
.equ	ADDR_COUNTER,	0x90000080	/* input only, 50 MHz counter (32b) */

.equ	ADDR_PS2,		0x90000100	/* in/out, PS2 connector for kb or mouse (8b data)*/

.equ	ADDR_JTAG,		0x90001000	/* send/recv chars to Terminal window */
.equ	ADDR_RS232, 	0x90001040 	/* send/recv chars to RS232 connector */

.equ	ADDR_TIMER,		0x90002000	/* timer controller */
.equ	ADDR_SYSID,		0x90002100	/* unique systemID */

.equ	ADDR_AVCONFIG,	0x90003000	/* configure audio/video */
.equ	ADDR_AUDIO, 	0x90003040 	/* audio device */
.equ	ADDR_LCD,		0x90003050	/* 16x2 character LCD, DE2 only */

.equ	ADDR_VGA, 		0x88000000 	/* vga pixel framebuffer */
.equ	ADDR_CHARBUF,	0x89000000 	/* vga character framebuffer */


.equ	ADDR_SDRAM,		0x00000000	/* SDRAM chip, 8MB, cached */
.equ	ADDR_SDRAM_END,	0x00800000

.equ	ADDR_SRAM,		0x08000000	/* SRAM chip, 512kB, also used for vga pixel framebuffer */
.equ	ADDR_SRAM_END,	0x08080000

.equ 	ADDR_FLASH,		0x01000000	/* Flash ROM, 4MB */
.equ 	ADDR_FLASH_END,	0x01400000
 

.equ	STACK_END,	ADDR_DRAM_END    /* easy to relocate stack if memory system changes */


/* Address offsets for preferred form of I/O, eg:
 *    movia r23, IOBASE
 *    movi  r2, 0x08
 *    stwio r2, LEDR(r23)
 */
.equ	IOBASE,		ADDR_LEDR
.equ	LEDR,		(ADDR_LEDR	- IOBASE)
.equ	LEDG,		(ADDR_LEDG	- IOBASE)
.equ	SWITCH,		(ADDR_SWITCH	- IOBASE)
.equ	KEY,		(ADDR_KEY	- IOBASE)
.equ	HEX7SEG,		(ADDR_HEX7SEG	- IOBASE)
.equ	HEX7SEGA,		(ADDR_HEX7SEGA	- IOBASE)
.equ	HEX7SEGB,		(ADDR_HEX7SEGB	- IOBASE)
.equ	COUNTER,		(ADDR_COUNTER	- IOBASE)
.equ	PS2,		(ADDR_PS2	- IOBASE)

.equ	SNDCTL,		(ADDR_AUDIO - IOBASE + 0)
.equ	SNDRDY,		(ADDR_AUDIO - IOBASE + 4)
.equ	SNDL,		(ADDR_AUDIO - IOBASE + 8)
.equ	SNDR,		(ADDR_AUDIO - IOBASE + 12)

.equ	TERMINAL,		(ADDR_JTAG  - IOBASE)
.equ	TERMINALCTL,	(ADDR_JTAG  - IOBASE + 4)

.equ	RS232,		(ADDR_RS232 - IOBASE)
.equ	RS232CTL,		(ADDR_RS232 - IOBASE + 4)

.equ	GPIO0,		(ADDR_GPIO0 - IOBASE)
.equ	GPIO0DIR,		(ADDR_GPIO0 - IOBASE + 4)
.equ	GPIO1,		(ADDR_GPIO1 - IOBASE)
.equ	GPIO1DIR,		(ADDR_GPIO1 - IOBASE + 4)

.equ	LCDCTL,		(ADDR_LCD - IOBASE)
.equ	LCDDATA,		(ADDR_LCD - IOBASE + 4)

.equ	COUNTER_RELOAD,	(ADDR_COUNTER - IOBASE + 4)
.equ	COUNTER_STATUS,	(ADDR_COUNTER - IOBASE + 8)

.equ	TIMER_STATUS,	(ADDR_TIMER - IOBASE + 0)
.equ	TIMER_CONTROL,	(ADDR_TIMER - IOBASE + 4)
.equ	TIMER_START_LOW,	(ADDR_TIMER - IOBASE + 8)
.equ	TIMER_START_HIGH,	(ADDR_TIMER - IOBASE + 12)
.equ	TIMER_VALUE_LOW,	(ADDR_TIMER - IOBASE + 16)
.equ	TIMER_VALUE_HIGH,	(ADDR_TIMER - IOBASE + 20)

/* 
 * Each bit position at address ADDR_HEX7SEG works as follows:
 *
 *    HEX3        HEX2        HEX1       HEX0
 *     24          16           8          0
 *    ----        ----        ----       ----
 * 29|    |25  21|    |17  13|    |9   5|    |1
 *   | 30 |      | 22 |      | 14 |     |  6 |
 *    ----        ----        ----       ----
 *   |    |      |    |      |    |     |    |
 * 28|    |26  20|    |18  12|    |10  4|    |2
 *    ----        ----        ----       ----
 *     27          19          11         3
 * Note that bits 31, 23, 15, and 7 are not used.
 *
 * Write "0" to a bit to turn "OFF" a 7-segment display LED.
 *
 */
.equ   DIGIT_,	0x00 /* blank space */
.equ   DIGIT0,	0x3f /*0xC0*/
.equ   DIGIT1,	0x06 /*0xF9*/
.equ   DIGIT2,	0x5b /*0xA4*/
.equ   DIGIT3,	0x4f /*0xB0*/
.equ   DIGIT4,	0x66 /*0x99*/
.equ   DIGIT5,	0x6d /*0x92*/
.equ   DIGIT6,	0x7d /*0x82*/
.equ   DIGIT7,	0x07 /*0xF8*/
.equ   DIGIT8,	0x7f /*0x80*/
.equ   DIGIT9,	0x67 /*0x98*/
.equ   DIGITA,	0x77 /*0x88*/
.equ   DIGITB,	0x7c /*0x83*/
.equ   DIGITC,	0x39 /*0xC6*/
.equ   DIGITD,	0x5e /*0xA1*/
.equ   DIGITE,	0x79 /*0x86*/
.equ   DIGITF,	0x71 /*0x8E*/

