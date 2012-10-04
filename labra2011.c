#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <util/delay.h>
#include "lcd.h"
#include "card.h"
#include "player.h"
#include "dealer.h"
#include "utility.h"

#define FALSE 0
#define TRUE 1
#define DRAW 2

void init(void) {

   		/* estet‰‰n kaikki keskeytykset */
		cli();

        /* kaiutin pinnit ulostuloksi */
        DDRE  |=  (1 << PE4) | (1 << PE5);
        /* pinni PE4 nollataan */
        PORTE &= ~(1 << PE4);
        /* pinni PE5 asetetaan */
        PORTE |=  (1 << PE5);   
        
        /* ajastin nollautuu, kun sen ja OCR1A rekisterin arvot ovat samat */
        TCCR1A &= ~( (1 << WGM11) | (1 << WGM10) );
        TCCR1B |=    (1 << WGM12);
        TCCR1B &=   ~(1 << WGM13);

        /* salli keskeytys, jos ajastimen ja OCR1A rekisterin arvot ovat samat */
        TIMSK |= (1 << OCIE1A);

        /* asetetaan OCR1A rekisterin arvoksi 0x3e (~250hz) */
        OCR1AH = 0x00;
        OCR1AL = 0x15;

        /* k‰ynnist‰ ajastin ja k‰yt‰ kellotaajuutena (16 000 000 / 1024) Hz */
        TCCR1B |= (1 << CS12) | (1 << CS10);

		/* n‰pp‰in pinnit sis‰‰ntuloksi */
		DDRA &= ~(1 << PA0);
		DDRA &= ~(1 << PA2);
		DDRA &= ~(1 << PA4);

		/* rele/led pinni ulostuloksi */
		DDRA |= (1 << PA6);

		/* lcd-n‰ytˆn alustaminen */
		lcd_init();
		lcd_write_ctrl(LCD_ON);
		lcd_write_ctrl(LCD_CLEAR);

}

short int askBet(short int);
short int checkHands(void);
short int calculateScore(short int, short int, short int);
short int player_wantsMore(void);
int getSeedView(void);
void showShowBetView(short int);
void showStartPlayerTurnView();
void showMainPlayerView();
void showStartDealersTurnView();
void showBlackjackView();
void showShowScoreView(short int);
void showLossView();
void ShowVictoryView();
short int multiply(short int);

int main(void) 
{

		/* alusta laitteen komponentit */
		init();

		//initialize stuff
		int seed;
		short int playerBust = FALSE;
		short int dealerBust = FALSE;
		short int playerWon = FALSE;
		short int bet = 0;
		short int score = 20;		//player starts with 20 credits
		short int maybeBlackjack;
		
		lcd_write_ctrl(LCD_CLEAR);
		ledOn();
		lcd_write_str("  WELCOME TO");
		lcd_gotoxy(0,1);
		lcd_write_str("POCKET BLACKJACK");  
		_delay_ms(50000);
		
		ledOff();
		seed = getSeedView();
		createDeck(seed);
		ledOn();

		lcd_write_ctrl(LCD_CLEAR);
		lcd_write_str("Up/Down to bet");
		lcd_gotoxy(0,1);
		lcd_write_str("Right selects");
		_delay_ms(30000);
		ledOff();

		//Main game loop
        while (1) {

			lcd_write_ctrl(LCD_CLEAR);
			bet = askBet(score);		
			ledOn();
			/* "Bet is: X" */	
			showShowBetView(bet);
			/* Let's start! | First card is | FOO | Second card is.. | BAR */							
			showStartPlayerTurnView();
			maybeBlackjack = TRUE;

			ledOff();
			
			do{
				//Main player view
				showMainPlayerView();
				
				
				if(!player_wantsMore())
					break;

				maybeBlackjack = FALSE;				
				ledOn();
				player_addCard(dealRandomCard());
				playerBust = player_isHandBust(); //return TRUE if player bust
				if(playerBust) 
					break;
				ledOff();


			}while(1);

			ledOn();
			lcd_write_ctrl(LCD_CLEAR);
			if(!playerBust){
				lcd_write_str("You stayed at ");
				lcd_write_int(player_getHandsCurrentValue());
			}else
				lcd_write_str("OH NO! Bust!");
			_delay_ms(30000);
			
			if(!playerBust){
				
				/* Its dealer's turn now! */
				showStartDealersTurnView();
				do{				
					if(!dealer_wantsMore()){
						lcd_write_ctrl(LCD_CLEAR);
						lcd_write_str("Dealer cant play!");
						_delay_ms(30000);
						break;
					}else{
						lcd_write_ctrl(LCD_CLEAR);
						lcd_write_str("Dealer must play");
						_delay_ms(30000);
					}
					
					dealer_addCard(dealRandomCard());
					lcd_write_ctrl(LCD_CLEAR);
					lcd_write_str("Dealers hand: ");
					lcd_write_int(dealer_getHandsCurrentValue());
					_delay_ms(30000);
					dealerBust = dealer_isHandBust();
					if(dealerBust){
						lcd_write_ctrl(LCD_CLEAR);
						lcd_write_str("DEALER BUST!");
						_delay_ms(30000);
						break;
					}

				}while(1);
			}

			if(playerBust){
				playerWon = FALSE;
				lcd_write_ctrl(LCD_CLEAR);
				lcd_write_str("You lost..");
				lcd_gotoxy(0,1);
				lcd_write_str("   bust");
				_delay_ms(30000);
			}else if(player_getHandsCurrentValue() == 21 && maybeBlackjack){
				/* You got blackjack? That's super awesome!! */
				showBlackjackView();
				score = score + multiply(bet);
			}else if(dealerBust){
				playerWon = TRUE;
				lcd_write_ctrl(LCD_CLEAR);
				lcd_write_str(" You won!");			
				lcd_gotoxy(0,1);
				lcd_write_str("Dealer bust!");
				_delay_ms(30000);
			}else{
				playerWon = checkHands(); //Return TRUE if player won
				if(playerWon == 2){
				lcd_write_ctrl(LCD_CLEAR);
				lcd_write_str("Draw!");
				_delay_ms(30000);
				}else{
					if(playerWon){
						lcd_write_ctrl(LCD_CLEAR);
						lcd_write_str("You won!");
						_delay_ms(30000);
					}else{
						lcd_write_ctrl(LCD_CLEAR);
						lcd_write_str("You lost!");
						_delay_ms(30000);
					}
				}
				score = calculateScore(playerWon, bet, score);
			}

			
			showShowScoreView(score);
			if(score < 1){
				/* Aww too bad! :( */
				showLossView();
				score = 20;
			}if(score > 990){
				/* INCREDIBLE! */
				showVictoryView();
				score = 20;
			}		

			/* End the round, reset hands and variables */
			player_clearHand();
			dealer_clearHand();
			playerBust = FALSE;
			dealerBust = FALSE;
			playerWon = FALSE;
			bet = 0;
			if(getNumberOfDealtCards() > 220){
				shuffleDeck();
				lcd_write_ctrl(LCD_CLEAR);
				lcd_write_str("Shuffling deck..");
				_delay_ms(30000);
			}
			ledOff();
		}
}

ISR(TIMER1_COMPA_vect) {

	/* vaihdetaan kaiutin pinnien tilat XOR operaatiolla */
 	PORTE ^= (1 << PE4) | (1 << PE5); 
}

short int player_wantsMore(){
	while(1){
		if(upButtonPressed())
			return TRUE;
		else if(downButtonPressed())
			return FALSE;
	}
}


short int askBet(short int score)
{
	short int bet = 1;
	lcd_write_str("^");
	lcd_gotoxy(3,0);
	lcd_write_int(score);
	lcd_gotoxy(7,0);
	lcd_write_str("Points");
	lcd_gotoxy(15,0);
	lcd_write_str(">");
	lcd_gotoxy(0,1);
	lcd_write_str("v");
	lcd_gotoxy(3,1);
	lcd_write_int(bet);
	lcd_gotoxy(7,1);
	lcd_write_str("Bet");
	lcd_gotoxy(15,1);
	lcd_write_str(">");
	while(1){
		if(upButtonPressed() && bet < 9 && bet < score){
			bet++;
			lcd_gotoxy(3,1);
			lcd_write_int(bet);

		}else if(downButtonPressed() && bet > 1){
			bet--;
			lcd_gotoxy(3,1);
			lcd_write_int(bet);
		}
		

		if(rightButtonPressed())
			break;
	}
	lcd_write_ctrl(LCD_CLEAR);
	return bet;
}

short int checkHands(){

	lcd_write_ctrl(LCD_CLEAR);
	lcd_gotoxy(2,0);
	lcd_write_str("You");
	lcd_gotoxy(8,0);
	lcd_write_str("Dealer");
	lcd_gotoxy(3,1);
	lcd_write_int(player_getHandsCurrentValue());
	lcd_gotoxy(12,1);
	lcd_write_int(dealer_getHandsCurrentValue());
	_delay_ms(30000);

	if(player_getHandsCurrentValue() > dealer_getHandsCurrentValue())
		return TRUE;
	else if(player_getHandsCurrentValue() == dealer_getHandsCurrentValue())
		return DRAW;
	else
		return FALSE;

}

short int calculateScore(short int playerWon, short int bet, short int score){

	if(playerWon != 2){
		if(playerWon)
			score = score + bet;
		else
			score = score - bet;
	}

	return score;
}

int getSeedView(){
	int seed;
	short int hasBeenPressed = FALSE;
	lcd_write_ctrl(LCD_CLEAR);
	lcd_write_str("Press any key");
	lcd_gotoxy(0,1);
	lcd_write_str(" to continue! ");
	do{
		if(hasBeenPressed && (upButtonPressed() || downButtonPressed() || middleButtonPressed() || leftButtonPressed() || rightButtonPressed()))
			break;
		else if(!(PINA & (1 << PA0))){ //up button
			seed = seed + 2;
			hasBeenPressed = TRUE;
		}
		else if(!(PINA & (1 << PA4))){ //down button
			seed = seed + 3;
			hasBeenPressed = TRUE;
		}
		else if(!(PINA & (1 << PA1))){ //left button
			seed = seed + 4;
			hasBeenPressed = TRUE;
		}
		else if(!(PINA & (1 << PA3))){ //right button
			seed = seed + 5;
			hasBeenPressed = TRUE;
		}
		else if(!(PINA & (1 << PA2))){ //middle button
			seed = seed + 6;
			hasBeenPressed = TRUE;
		}else
			seed++;

	}while(1);
	return seed;
}

short int multiply(short int val)
{
	short int x = (short int)(val * 1.5);

	return x;
}

/* From now on these functions are basically ONLY for showing stuff on the screen. Therefore they're not very interesting */

void showShowBetView(short int bet){
	lcd_write_ctrl(LCD_CLEAR);
	lcd_gotoxy(0,0);
	lcd_write_str("Bet is: ");
	lcd_write_int(bet);
	_delay_ms(30000);
}

void showStartPlayerTurnView(){
	lcd_write_ctrl(LCD_CLEAR);
	lcd_write_str("Let's start!");
	lcd_gotoxy(0,1);
	lcd_write_str("First card..");
	_delay_ms(30000);

	player_addCard(dealRandomCard());
	lcd_write_ctrl(LCD_CLEAR);
	lcd_write_str("Second card..");
	_delay_ms(30000);
	player_addCard(dealRandomCard());
}

void showMainPlayerView(){
	lcd_write_ctrl(LCD_CLEAR);
	lcd_write_str("D");
	lcd_gotoxy(0,1);
	lcd_write_str("S");
	lcd_gotoxy(6,0);				
	lcd_write_int(player_getHandsCurrentValue());
	lcd_gotoxy(9,0);
	lcd_write_str("#A11: ");
	lcd_write_int(player_getAcesAsEleven());
	lcd_gotoxy(9,1);
	lcd_write_str(" #A1: ");
	lcd_write_int(player_getAcesAsOne());
}

void showStartDealersTurnView(){
	lcd_write_ctrl(LCD_CLEAR);
	lcd_write_str("*Dealer's turn!*");
	lcd_gotoxy(0,1);
	lcd_write_str("****************");
	_delay_ms(30000);

	lcd_write_ctrl(LCD_CLEAR);
	dealer_addCard(dealRandomCard());
	dealer_addCard(dealRandomCard());
	lcd_write_ctrl(LCD_CLEAR);
	lcd_write_str("Dealers hand: ");
	lcd_write_int(dealer_getHandsCurrentValue());
	_delay_ms(30000);
}

void showBlackjackView(){
	lcd_write_ctrl(LCD_CLEAR);
	lcd_write_str("!BLACKJACK!!");
	lcd_gotoxy(0,1);
	lcd_write_str("!!!!!!!!!!!!");
	_delay_ms(20000);
	sei();
	ledOff();
	_delay_ms(10000);				
	cli();
	ledOn();
	_delay_ms(10000);
	sei();
	ledOff();
	_delay_ms(10000);
	cli();
	ledOn();
}

void showShowScoreView(short int score){
	lcd_write_ctrl(LCD_CLEAR);
	lcd_write_str(" Current score");
	lcd_gotoxy(2,1);
	lcd_write_str("**");
	lcd_gotoxy(7,1);
	lcd_write_int(score);
	lcd_gotoxy(12,1);
	lcd_write_str("**");
	_delay_ms(30000);
}

void showLossView(){
	lcd_write_ctrl(LCD_CLEAR);
	lcd_write_str("(- .-')");
	lcd_gotoxy(0,1);
	lcd_write_str("Try again?");
	_delay_ms(30000);
}

void showVictoryView(){
	lcd_write_ctrl(LCD_CLEAR);
	lcd_write_str(" !! ( ^o^) !!");
	lcd_gotoxy(0,1);
	lcd_write_str("  INCREDIBLE!");
	sei();
	_delay_ms(10000);
	cli();
	_delay_ms(30000);
}
