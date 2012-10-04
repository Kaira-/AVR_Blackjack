/* card.c includes definition and utilities for cards */

#include <stdlib.h>
#include "card.h"
#include "avr/eeprom.h"

Card deck[416]; //Eight decks
static int dealtCards[416]; 
static int number_dealtCards;

short int getNumberOfDealtCards(){
	return number_dealtCards;
}

void createDeck(int seed){
	srand(seed);
	
	for(short int k = 0; k < 8; k++){
		for(short int i = 0; i < 4; i++){
			for(short int j = 1; j < 14; j++){
				Card card;
				card.value = j;

				switch(i){
					case 0:
						card.suit = HEART;
						break;
					case 1:
						card.suit = DIAMOND;
						break;
					case 2:
						card.suit = CLUB;
						break;
					case 3:
						card.suit = SPADE;
						break;
				}

				deck[i*13+j-1 + k*52] = card; //Creates an 8 deck.. deck
		}
	}
	}
}

Card dealRandomCard()
{	
	short int tryAgain;
	short int randNum;

	do{
		tryAgain = 0;
		randNum = rand() % 415+1;
		for(short int i; i < number_dealtCards;i++){
			if(dealtCards[i] == randNum)
				tryAgain = 1;
		}
	}while(tryAgain); //If the card has already been dealt, try again

	dealtCards[number_dealtCards] = randNum;
	number_dealtCards++;

	return deck[randNum];
}

void shuffleDeck(){
	memset(dealtCards, 0, 416);
	number_dealtCards = 0;
}

SUIT getSuit(Card* card)
{
	return card->suit;
}
