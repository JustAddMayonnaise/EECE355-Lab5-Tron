/* EECE355 Lab 5 Solution 
 * Date: April 1, 2014 
 * 
 * Student1Name: David Wong 
 * Student1Number: 44627107
 * Student1UserID: g7b9@ece.ubc.ca
 * Student1Email: ubcdavid@interchange.ubc.ca
 */ 

// This TRON game uses the following I/O on the DE2 board:
// 		- Player 1 uses SW 17 and 16
// 		- Player 2 uses SW 1 and 0
// 		- Scoreboard uses the 7 segment displays 
// 		- Start new game uses KEY3
// The bonus helper is a GREEN square that is on the screen. This helper enables the player to move faster.

#include "ubc-de-media.h"
#define TRUE 1
#define FALSE 0

// Checks if the position on the screen is a black pixel
// If pixel is black, return true, else return false
int isPixelBlack(int x, int y)
{
	unsigned short BLACK = makeColour(0, 0, 0);
	if(*getPixelAddr(x,y) == BLACK) return TRUE;
	return FALSE;
}

// Checks if the position on the screen is a bonus pixel
// If pixel is bonus, return true, else return bonus
int isPixelBonus(int x, int y)
{
	// The following is used to store colour values
	unsigned short BONUS = makeColour(0, 63, 0);	
	if(*getPixelAddr(x,y) == BONUS) return TRUE;
	return FALSE;
}

// This function provides movement for player 1.
// INPUT  : Switch 18 controls xy-direction, switch 17 positive negative
// OUTPUT : Returns TRUE or FALSE depending on if the player has encountered a pixel colour that is not black
int move_p1(int* xpos, int* ypos, int* vx, int* vy, int* p1_bonus, int* p2_bonus, unsigned short colour)
{
	unsigned short BONUS = makeColour(0, 63, 0);
	// If switch is up, it goes y-direction, else it goes x-direction
	// Checks for previous velocity to ensure user cannot go backwards
	if(*pSWITCH&131072 && *vy == 0)
	{
		*vx = 0;
		if(*pSWITCH&65536) *vy = -1; else *vy = 1;
	}
	if(!(*pSWITCH&131072) && *vx == 0)
	{
		*vy = 0;
		if(*pSWITCH&65536) *vx = -1; else *vx = 1;
	}
	int i;
	// This loop is to ensure that the player does not go through a non-black pixel when the velocity is doubled
	for(i = 0; i < *p1_bonus; i++)
	{
		// Updates the position
		*xpos += *vx;
		*ypos += *vy;
		// Checks if the next position is not black pixel and not bonus pixel. If not black pixel, player has crashed and return true.
		if(!isPixelBlack(*xpos, *ypos) && !isPixelBonus(*xpos, *ypos)) return TRUE;
		// Changes the flags for bonus
		if(isPixelBonus(*xpos, *ypos))
		{
			*p1_bonus = 2;
			*p2_bonus = 1;
			deleteBonus(*xpos, *ypos, BONUS);
		}
		// Continues the trail
		drawPixel(*xpos, *ypos, colour);
	}

	return FALSE;
}

// Provides the same function as move_p1(). However, it should be noted that the directions are reversed to ensure that the 
// players are moving towards the center at the beginning.
int move_p2(int* xpos, int* ypos, int* vx, int* vy, int* p1_bonus, int* p2_bonus, unsigned short colour)
{
	unsigned short BONUS = makeColour(0, 63, 0);
	// If switch 1 is up, it goes y-direction, else it goes x-direction
	if(*pSWITCH&2 && *vy == 0)
	{
		*vx = 0;
		if(*pSWITCH&1) *vy = 1; else *vy = -1;
	}
	if(!(*pSWITCH&2) && *vx == 0)
	{
		*vy = 0;
		if(*pSWITCH&1) *vx = 1; else *vx = -1;
	}
	int i;
	// This loop is to ensure that the player does not go through a non-black pixel when the velocity is doubled
	for(i = 0; i < *p2_bonus; i++)
	{
		// Updates the position
		*xpos += *vx;
		*ypos += *vy;
		// Checks if the next position is not black pixel and not bonus pixel. If not black pixel, player has crashed and return true.
		if(!isPixelBlack(*xpos, *ypos) && !isPixelBonus(*xpos, *ypos)) return TRUE;
		// Changes the flags for bonus
		if(isPixelBonus(*xpos, *ypos))
		{
			*p1_bonus = 1;
			*p2_bonus = 2;
			deleteBonus(*xpos, *ypos, BONUS);
		}
		// Continues the trail
		drawPixel(*xpos, *ypos, colour);
	}
	return FALSE;
}

// This function fills the edges of the screen with a colour.
// INPUT  : unsigned short
// OUTPUT : The edges of the screen will be coloured with the input unsigned short value
void fillEdges(unsigned short colour)
{
	int i,j;
	// Draws the x direction borders
	for( i=0; i<MAX_X_PIXELS; i++ ) 
	{
		drawPixel( i, 0, colour );
		drawPixel( i, 1, colour );
		drawPixel( i, MAX_Y_PIXELS-1, colour );
	}
	
	// Draws the y direction boarders
	for( j=0; j<MAX_Y_PIXELS-1; j++ ) {
		drawPixel( 0, j, colour );
		drawPixel( MAX_X_PIXELS-1, j, colour );
	}
}

// Displays the score of the game on the HEX
void displayScore(int p1, int p2)
{
	if(p2 == 0) *pHEX7SEGA = DIGIT0;
	else if (p2 == 1) *pHEX7SEGA = DIGIT1;
	else if (p2 == 2) *pHEX7SEGA = DIGIT2;
	else if (p2 == 3) *pHEX7SEGA = DIGIT3;
	else if (p2 == 4) *pHEX7SEGA = DIGIT4;
	else if (p2 == 5) *pHEX7SEGA = DIGIT5;
	else if (p2 == 6) *pHEX7SEGA = DIGIT6;
	else if (p2 == 7) *pHEX7SEGA = DIGIT7;
	else if (p2 == 8) *pHEX7SEGA = DIGIT8;
	else if (p2 == 9) *pHEX7SEGA = DIGIT9;
	
	if(p1 == 0) *pHEX7SEGB = DIGIT0;
	else if (p1 == 1) *pHEX7SEGB = DIGIT1;
	else if (p1 == 2) *pHEX7SEGB = DIGIT2;
	else if (p1 == 3) *pHEX7SEGB = DIGIT3;
	else if (p1 == 4) *pHEX7SEGB = DIGIT4;
	else if (p1 == 5) *pHEX7SEGB = DIGIT5;
	else if (p1 == 6) *pHEX7SEGB = DIGIT6;
	else if (p1 == 7) *pHEX7SEGB = DIGIT7;
	else if (p1 == 8) *pHEX7SEGB = DIGIT8;
	else if (p1 == 9) *pHEX7SEGB = DIGIT9;
}

// Draws the bonus helper pixels on the screen.
// The bonus helper should double the player's speed which is controlled in the move_p1 and move_p2 function.
void drawBonus(int x, int y, unsigned short colour)
{
	int i;
	int j;

	for (i = 0; i < 5; i++)
	{
		for(j = 0; j < 5; j++)
		{
			if(isPixelBlack(x+i,y+j)) drawPixel(x+i,y+j,colour);
		}
	}
}

void deleteBonus(int x, int y, unsigned short colour)
{
	int i;
	int j;
	unsigned short BLACK = makeColour(0, 0, 0);
	for (i = -5; i < 5; i++)
	{
		for(j = -5; j < 5; j++)
		{
			if(isPixelBonus(x+i,y+j)) drawPixel(x+i,y+j,BLACK);
		}
	}
}

int main()
{
	// The following is used to store colour values
	unsigned short BLACK = makeColour(0, 0, 0);
	unsigned short RED   = makeColour(63, 0, 0);
	unsigned short BLUE  = makeColour(0, 0, 63);
	unsigned short WHITE = makeColour(63, 63, 63);
	unsigned short BONUS = makeColour(0, 63, 0);

	// The following is used to store the player's position
	int p1_xpos;
	int p1_ypos;
	int p2_xpos;
	int p2_ypos;

	// The following is used to store the velocity of the player
	int p1_vx;
	int p1_vy;
	int p2_vx;
	int p2_vy;
	
	// The following provides the flag for bonus helper
	// Default value should be equal to 1 to provide regular velocity
	// When player obtains bonus, value should change to 2 to double their speed
	int p1_bonus;
	int p2_bonus;
	
	// The following stores the information if the player has crashed
	int p1_crash;
	int p2_crash;
	
	// Flag to enable to play again
	int play_again;
	
	// Stores player scores
	int p1_score;
	int p2_score;

	initScreen();

	while(1)
	{
		// Displays the score. Resets the score to 0 for the start of a new game
		p1_score = 0;
		p2_score = 0;
		displayScore(p1_score, p2_score);
		
		// Changes the flag to play again back to default value of FALSE.
		// This allows the player to decide to play again from the KEY3 button
		play_again = FALSE;

		while(p1_score < 9 && p2_score < 9)
		{
			// Resets the screen back to the original display
			fillScreen(BLACK);
			fillEdges(WHITE);

			//Draw bonuses
			drawBonus(100,80,BONUS);
			drawBonus(100,160,BONUS);
			drawBonus(200,80,BONUS);
			drawBonus(200,160,BONUS);
			drawBonus(55,80,BONUS);
			drawBonus(55,160,BONUS);
			drawBonus(255,80,BONUS);
			drawBonus(255,160,BONUS);
			
			// Changes the crash flag to FALSE
			p1_crash = FALSE;
			p2_crash = FALSE;

			// Starting position of the players
			p1_xpos = 79;
			p1_ypos = 120;

			p2_xpos = 240;
			p2_ypos = 120;
			
			// Starting velocity of the players. This allows the player to initially go in any direction
			p1_vx = 0;
			p1_vy = 0;
			
			p2_vx = 0;
			p2_vy = 0;
			
			// Bonus flag to control the velocity of players. The default value is 1 for default speed.
			p1_bonus = 1;
			p2_bonus = 1;
			
			// The following WHILE loop enables continuous movement of the players until someone has crashed.
			while(p1_crash != TRUE && p2_crash != TRUE)
			{
				p1_crash = move_p1(&p1_xpos, &p1_ypos, &p1_vx, &p1_vy, &p1_bonus, &p2_bonus, RED);
				p2_crash = move_p2(&p2_xpos, &p2_ypos, &p2_vx, &p2_vy, &p1_bonus, &p2_bonus, BLUE);

				// Delay so that the TRON doesn't move too fast
				timedDelay(50000*30); 
			}

			// The following IF ELSE statement updates the score for the player after someone has crashed
			if(p1_crash && p2_crash)
			{
				// No one gets a point
				p1_score += 0;
				p2_score += 0;
			}
			else if(p1_crash)
			{
				// Player 2 gets a point
				p2_score += 1;
			}
			else
			{
				// Player 1 gets a point
				p1_score += 1;
			}
			
			// Updates the score
			displayScore(p1_score, p2_score);

		}

		// Fills the screen of the winner's colour
		if(p1_score == 9) fillScreen(RED); else fillScreen(BLUE);

		// Waits for someone to press KEY3 to start a new game
		while(!play_again)
		{
			if(*pKEY&8) play_again = TRUE;
		}
	}

	return 0;
}