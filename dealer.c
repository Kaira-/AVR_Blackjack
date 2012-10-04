/* necessary function implementations, variables etc for dealer */

#include <stdlib.h>
#include "dealer.h"
#include "card.h"
#include "lcd.h"
#include <util/delay.h>
#include "utility.h"

#define FALSE 0
#define TRUE 1

static Card dealerHand [10];	/* not visible outside this file, can't contain more than 10 cards with minimal values before going over threshold */
short static int num_allocated = 0;
short static int numberOfAces=0; //Total number of aces
short static int acesAsOne=0; //Number of aces that are counted as 11

Card* dealer_getHand()
{
	return dealerHand;
}

void dealer_addCard(Card c)
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
	
	if(c.value == 1){ //If the card is an ace, value it as 11 for now
		c.value = 11;
		numberOfAces++;
	}else if(c.value == 11 || c.value == 12 || c.value == 13 ) //Big cards are valued as 10
		c.value = 10;

	dealerHand[num_allocated] = c;
	num_allocated++;

}

void dealer_clearHand()
{
	memset(dealerHand, 0, 11);
	acesAsOne=0;
	numberOfAces = 0;
	num_allocated = 0;
	
}

short int dealer_wantsMore(){
	short int handValue = 0;
	short int acesHelper = numberOfAces; //Aceshelper holds the total number of aces - which pretty much means the #aces valued as 11
	for(short int i=0; i<num_allocated;i++){
		handValue = handValue + dealerHand[i].value;
	}

	while(acesHelper > 0 && handValue > 21){
		handValue = handValue - 10;
		acesHelper--;
	}

	if(handValue < 17){
		return TRUE;
	}else{
		return FALSE;
	}
}

short int dealer_getHandsCurrentValue(){
	short int handValue = 0;
	short int acesHelper = numberOfAces;
	for(short int i=0; i<num_allocated;i++){
		handValue = handValue + dealerHand[i].value;
	}

	while(acesHelper > 0 && handValue > 21){
		handValue = handValue - 10;
		acesHelper--;
	}

	acesAsOne = numberOfAces - acesHelper;
	return handValue;
}

short int dealer_isHandBust(){
	short int handValue = 0;
	short int acesHelper = numberOfAces;
	for(short int i=0; i<num_allocated;i++){
		handValue = handValue + dealerHand[i].value;
	}

	while(acesHelper > 0 && handValue > 21){
		handValue = handValue - 10;
		acesHelper--;
	}

	acesAsOne = numberOfAces - acesHelper;

	if(handValue > 21){
		return TRUE;
	}else{
		return FALSE;
	}
}

short int dealer_getAcesAsEleven(){
	return numberOfAces - acesAsOne;
}

short int dealer_getAcesAsOne(){
	return acesAsOne;
}

