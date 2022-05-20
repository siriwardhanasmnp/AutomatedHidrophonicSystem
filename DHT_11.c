/*
 * DHT_11.c
 *
 * Created: 4/20/2021 9:05:37 AM
 *  Author: Navodya
 */ 

#include "functions.h"

//Mode Selection
void mode_select(){
	while(mode_logic!=1){
		
		if(PINA & 0x01){
			Mode=1;
		mode_logic =1;}

		else{
			if(PINA & 0x02){
				Mode=2;
			mode_logic =1;}
			
			else{
				if(PINA & 0x04){
					Mode=3;
				mode_logic =1;}
			}
		}
	}
	if(mode_logic==1)
	PORTD |= 0x10;
}
