/* player.c implementation for player-related attributes and methods */

#include <stdlib.h>
#include "card.h"
#include "player.h"
#include "lcd.h"
#include <util/delay.h>
#include "utility.h"

#define FALSE 0
#define TRUE 1

static Card playerHand [11];	/* The player's hand  can't contain more than 11 cards without going over */
short static int num_allocated = 0;	/* How large the array is */
short static int numberOfAces = 0; //Total number of aces
short static int acesAsOne=0; //Number of aces that are currently counted as 1


Card* player_getHand()
{
	return playerHand;
}

void player_addCard(Card c)
{
	lcd_write_ctrl(LCD_CLEAR);
	lcd_write_str("  *Got card*  ");
	lcd_gotoxy(2,1);
	lcd_write_int(c.value);
	if(c.suit == SPADE)
		lcd_write_str("SPADE");
	else if(c.suit == CLUB)
		lcd_write_str("CLUB");
	else if(c.suit == HEART)
		lcd_write_str("HEART");
	else if(c.suit == DIAMOND)
		lcd_write_str("DIAMOND");
	_delay_ms(30000);
	
	if(c.value == 1){ /* Eihän tämä ole viittaus? Ei siis saisi muuttaa kortin arvoa siellä pakassa */
		c.value = 11;
		numberOfAces++;
	}else if(c.value == 11 || c.value == 12 || c.value == 13 )
		c.value = 10;

	playerHand[num_allocated] = c;
	num_allocated++;
}

void player_clearHand()
{
	memset(playerHand, 0, 11);
	acesAsOne=0;
	numberOfAces = 0;
	num_allocated = 0;
	/*for(short int i=0; i<11;i++){
		playerHand[i]=0;
	}
	num_allocated = 0;*/
	
}

short int player_getHandsCurrentValue(){
	short int handValue = 0;
	short int acesHelper = numberOfAces; //acesHelper is essentially the number of aces counted as 11
	for(short int i=0; i<num_allocated;i++){
		handValue = handValue + playerHand[i].value;
	}
	while(acesHelper > 0 && handValue > 21){
		handValue = handValue - 10;
		acesHelper--;
	}
	acesAsOne = numberOfAces - acesHelper;
	return handValue;
}

short int player_isHandBust(){
	short int handValue = 0;
	short int acesHelper = numberOfAces;
	for(short int i=0; i<num_allocated;i++){
		handValue = handValue + playerHand[i].value;
	}
	while(acesHelper > 0 && handValue > 21){
		handValue = handValue - 10;
		acesHelper--;
	}
	acesAsOne = numberOfAces - acesHelper;
	if(handValue > 21)
		return TRUE;
	else
		return FALSE;
}

short int player_getAcesAsEleven(){
	return numberOfAces - acesAsOne;
}

short int player_getAcesAsOne(){
	return acesAsOne;
}
