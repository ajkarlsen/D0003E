#include <avr/io.h>

void togglePin4(){
	PORTE = ( PORTE  | 0x10) - ( PORTE  & 0x10);
}

void togglePin6(){
	PORTE = ( PORTE  | 0x40) - ( PORTE  & 0x40);
}


void disablePin4(){
	PORTE &= 0xEF;
}

void disablePin6(){
	PORTE &= 0xBF;
}





