#include <stdlib.h>
#include <avr/io.h>
#include "utility.h"
#include "lcd.h"

#define FALSE 0
#define TRUE 1

static int leftReleased = TRUE;
static int rightReleased = TRUE;
static int middleReleased = TRUE;
static int upReleased = TRUE;
static int downReleased = TRUE;

short int leftButtonPressed(){
	if(!(PINA & (1 << PA1)) && leftReleased){
		leftReleased = FALSE;
		return TRUE;
	}else if(!(PINA & (1 << PA1)) && !leftReleased){
		return FALSE;
	}else{
		leftReleased = TRUE;
		return FALSE;
	}
	
}

short int rightButtonPressed(){
	if(!(PINA & (1 << PA3)) && rightReleased){
		rightReleased = FALSE;
		return TRUE;
	}else if(!(PINA & (1 << PA3)) && !rightReleased){
		return FALSE;
	}else{
		rightReleased = TRUE;
		return FALSE;
	}
}

short int upButtonPressed(){
	if(!(PINA & (1 << PA0)) && upReleased){
		upReleased = FALSE;
		return TRUE;
	}else if(!(PINA & (1 << PA0)) && !upReleased){
		return FALSE;
	}else{
		upReleased = TRUE;
		return FALSE;
	}
}

short int downButtonPressed(){
	if(!(PINA & (1 << PA4)) && downReleased){
		downReleased = FALSE;
		return TRUE;
	}else if(!(PINA & (1 << PA4)) && !downReleased){
		return FALSE;
	}else{
		downReleased = TRUE;
		return FALSE;
	}
}


short int middleButtonPressed(){
	if(!(PINA & (1 << PA2)) && middleReleased){
		middleReleased = FALSE;
		return TRUE;
	}else if(!(PINA & (1 << PA2)) && !middleReleased){
		return FALSE;
	}else{
		middleReleased = TRUE;
		return FALSE;
	}
}

void ledOn(){
	PORTA |= (1 << PA6);
}
void ledOff(){
	PORTA &= ~(1 << PA6);
}
