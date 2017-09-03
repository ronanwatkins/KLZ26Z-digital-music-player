/*
 * melodys.c
 *
 *  Created on: 7 Feb 2017
 *      Author: Ronan Watkins
 */

#include "notes.h"
#include "melodys.h"
#include "flash.h"

//arrays to hold notes and tempos for each tune
int melody[] = {
  NOTE_E7, NOTE_E7, 0, NOTE_E7,
  0, NOTE_C7, NOTE_E7, 0,
  NOTE_G7, 0, 0,  0,
  NOTE_G6, 0, 0, 0,

  NOTE_C7, 0, 0, NOTE_G6,
  0, 0, NOTE_E6, 0,
  0, NOTE_A6, 0, NOTE_B6,
  0, NOTE_AS6, NOTE_A6, 0,

  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, 0, NOTE_F7, NOTE_G7,
  0, NOTE_E7, 0, NOTE_C7,
  NOTE_D7, NOTE_B6, 0, 0,

  NOTE_C7, 0, 0, NOTE_G6,
  0, 0, NOTE_E6, 0,
  0, NOTE_A6, 0, NOTE_B6,
  0, NOTE_AS6, NOTE_A6, 0,

  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, 0, NOTE_F7, NOTE_G7,
  0, NOTE_E7, 0, NOTE_C7,
  NOTE_D7, NOTE_B6, 0, 0
};

int tempo[] = {
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
};

int underworld_melody[] = {
  NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
  NOTE_AS3, NOTE_AS4, 0,
  0,
  NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
  NOTE_AS3, NOTE_AS4, 0,
  0,
  NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
  NOTE_DS3, NOTE_DS4, 0,
  0,
  NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
  NOTE_DS3, NOTE_DS4, 0,
  0, NOTE_DS4, NOTE_CS4, NOTE_D4,
  NOTE_CS4, NOTE_DS4,
  NOTE_DS4, NOTE_GS3,
  NOTE_G3, NOTE_CS4,
  NOTE_C4, NOTE_FS4, NOTE_F4, NOTE_E3, NOTE_AS4, NOTE_A4,
  NOTE_GS4, NOTE_DS4, NOTE_B3,
  NOTE_AS3, NOTE_A3, NOTE_GS3,
  0, 0, 0
};

int underworld_tempo[] = {
  12, 12, 12, 12,
  12, 12, 6,
  3,
  12, 12, 12, 12,
  12, 12, 6,
  3,
  12, 12, 12, 12,
  12, 12, 6,
  3,
  12, 12, 12, 12,
  12, 12, 6,
  6, 18, 18, 18,
  6, 6,
  6, 6,
  6, 6,
  18, 18, 18, 18, 18, 18,
  10, 10, 10,
  10, 10, 10,
  3, 3, 3
};

int axel_melody[] = {
  NOTE_D4, NOTE_F4, NOTE_D4, NOTE_D4, NOTE_G4, NOTE_D4, NOTE_C4, NOTE_D4, NOTE_A4, NOTE_D4,
  NOTE_D4, NOTE_AS4, NOTE_A4, NOTE_F4, NOTE_D4, NOTE_A4, NOTE_D5,
  NOTE_D4, NOTE_C4, NOTE_C4, NOTE_A3, NOTE_E4, NOTE_D4
};

int axel_tempo[] = {
  4, 4, 6, 8, 4, 6, 6,
  4, 4, 6, 8, 4, 6, 6,
  6, 6, 8, 6, 6, 6, 8, 4, 6
};

int sailormoon_melody[] = {
  NOTE_C4, NOTE_F4, NOTE_GS4, NOTE_C5, NOTE_C5, NOTE_AS4, NOTE_AS4, NOTE_GS4, NOTE_G4, NOTE_AS4,
  NOTE_C4, NOTE_E4, NOTE_G4, NOTE_AS4, NOTE_AS4, NOTE_GS4, NOTE_GS4, NOTE_G4, NOTE_F4, NOTE_GS4,
  NOTE_C4, NOTE_F4,NOTE_GS4, NOTE_C5, NOTE_C5, NOTE_AS4, NOTE_AS4, NOTE_C5, NOTE_DS5, NOTE_CS5,
  NOTE_CS5, NOTE_C5, NOTE_AS4, NOTE_C5, NOTE_AS4, NOTE_GS4, NOTE_G4, NOTE_F4
};

int sailormoon_tempo[] = {
	4, 4, 4, 3, 4, 3, 4, 3, 3, 2,
	4, 4, 4, 3, 4, 3, 4, 3, 3, 2,
	4, 4, 4, 3, 4, 3, 4, 3, 3, 2,
	4, 4, 3, 3, 3, 3, 4, 3
};

int steveuniverse_melody[] = {
	NOTE_C4, NOTE_A4, NOTE_G4, NOTE_F4, NOTE_G4,  NOTE_E4,
	NOTE_C4, NOTE_A4, NOTE_G4, NOTE_F4, NOTE_E4,  NOTE_F4,
	NOTE_C4, NOTE_A4, NOTE_G4, NOTE_F4, NOTE_G4,  NOTE_CS4,
	NOTE_C4, NOTE_A4, NOTE_G4, NOTE_F4, NOTE_CS4, NOTE_C4,
	NOTE_C4, NOTE_A4, NOTE_G4, NOTE_F4, NOTE_DS4, NOTE_CS4,
	NOTE_A3, NOTE_G3, NOTE_G4, NOTE_F4, NOTE_DS4, NOTE_F4,
	NOTE_G4, NOTE_F4, NOTE_G4, NOTE_CS4, NOTE_F4, NOTE_G4,
	NOTE_F4, NOTE_CS4, NOTE_C4
};

int steveuniverse_tempo[] = {
	2, 3, 3, 4, 4, 2,
	3, 4, 4, 3, 3, 2,
	3, 4, 4, 3, 4, 4,
	2, 3, 3, 4, 4, 2,
	2, 3, 3, 4, 4, 2,
	3, 4, 4, 3, 3, 2,
	3, 4, 4, 3, 4, 4,
	2, 3, 3
};

void program_mario_melody(){
	int MARIO_MELODY_ADDRESS = 0x1FC00;
	int i=0, j=0;

	for(i=0, j=0; i<78; i+=2, j+=4){
		//program 2 integers from the array to flash at a time
		unsigned int data = (unsigned int)((melody[i]) + (melody[i+1]<<16));
		//program contents of data into flash
		//address is incremented by 4 bytes every loop
		flash_program(MARIO_MELODY_ADDRESS+j, data);
	}
}

void program_mario_tempo() {
	int MARIO_TEMPO_ADDRESS = 0x1FCC0;
	int i=0, j=0;

	for(i=0, j=0; i<78; i+=2, j+=4){
		unsigned int data = (unsigned int)((tempo[i]) + (tempo[i+1]<<16));
		flash_program(MARIO_TEMPO_ADDRESS+j, data);
	}
}

void program_underworld_melody() {
	int UNDERWORLD_MELODY_ADDRESS = 0x1FD80;
	int i=0, j=0;

	for(i=0, j=0; i<56; i+=2, j+=4){
		unsigned int data = (unsigned int)((underworld_melody[i]) + (underworld_melody[i+1]<<16));
		flash_program(UNDERWORLD_MELODY_ADDRESS+j, data);
	}
}

void program_underworld_tempo() {
	int UNDERWORLD_TEMPO_ADDRESS = 0x1FE10;
	int i=0, j=0;

	for(i=0, j=0; i<56; i+=2, j+=4){
		unsigned int data = (unsigned int)((underworld_tempo[i]) + (underworld_tempo[i+1]<<16));
		flash_program(UNDERWORLD_TEMPO_ADDRESS+j, data);
	}
}

void program_axel_melody() {
	int AXEL_MELODY_ADDRESS = 0x1FE80;
	int i=0, j=0;

	for(i=0, j=0; i<23; i+=2, j+=4){
		unsigned int data = (unsigned int)((axel_melody[i]) + (axel_melody[i+1]<<16));
		flash_program(AXEL_MELODY_ADDRESS+j, data);
	}
}

void program_axel_tempo() {
	int AXEL_TEMPO_ADDRESS = 0x1FF20;
	int i=0, j=0;

	for(i=0, j=0; i<23; i+=2, j+=4){
		unsigned int data = (unsigned int)((axel_tempo[i]) + (axel_tempo[i+1]<<16));
		flash_program(AXEL_TEMPO_ADDRESS+j, data);
	}
}

void program_sailormoon_melody() {
	int SAILORMOON_MELODY_ADDRESS = 0x1F44C;
	int i=0, j=0;

	for(i=0, j=0; i<38; i+=2, j+=4){
		unsigned int data = (unsigned int)((sailormoon_melody[i]) + (sailormoon_melody[i+1]<<16));
		flash_program(SAILORMOON_MELODY_ADDRESS+j, data);
	}
}

void program_sailormoon_tempo() {
	int SAILORMOON_TEMPO_ADDRESS = 0x1F4D0;
	int i=0, j=0;

	for(i=0, j=0; i<38; i+=2, j+=4){
		unsigned int data = (unsigned int)((sailormoon_tempo[i]) + (sailormoon_tempo[i+1]<<16));
		flash_program(SAILORMOON_TEMPO_ADDRESS+j, data);
	}
}

void program_steveuniverse_melody() {
	int STEVEUNIVERSE_MELODY_ADDRESS = 0x1F44C;
	int i=0, j=0;

	for(i=0, j=0; i<45; i+=2, j+=4){
		unsigned int data = (unsigned int)((steveuniverse_melody[i]) + (steveuniverse_melody[i+1]<<16));
		flash_program(STEVEUNIVERSE_MELODY_ADDRESS+j, data);
	}
}

void program_steveuniverse_tempo() {
	int STEVEUNIVERSE_TEMPO_ADDRESS = 0x1F570;
	int i=0, j=0;

	for(i=0, j=0; i<45; i+=2, j+=4){
		unsigned int data = (unsigned int)((steveuniverse_tempo[i]) + (steveuniverse_tempo[i+1]<<16));
		flash_program(STEVEUNIVERSE_TEMPO_ADDRESS+j, data);
	}
}
