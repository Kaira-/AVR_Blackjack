/* necessary method prototypes and macros for dealer */

#ifndef DEALER_H
#define DEALER_H

#include "card.h"

Card* dealer_getHand();
void dealer_addCard(Card c);
short int dealer_getAcesAsOne();
short int dealer_getAcesAsEleven();

#endif
