/* player.h includes methods and variables related to the player */

#ifndef PLAYER_H
#define PLAYER_H

#include "card.h"

Card* player_getHand();
void player_addCard(Card c);
short int player_getAcesAsOne(void);
short int player_getAcesAsEleven(void);

#endif
