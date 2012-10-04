/* card.h includes definition and utilities for cards */

#ifndef CARD_H
#define CARD_H

typedef enum
{
	HEART = 1,
	CLUB = 2,
	DIAMOND = 4,
	SPADE = 8
} SUIT;

typedef struct {
	short int value;
	SUIT suit;
} Card;


short int getNumberOfDealtCards(void);
Card dealRandomCard(void);
SUIT getSuit(Card* card);
void createDeck(int);
void shuffleDeck(void);

#endif
