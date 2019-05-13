#include "chip8.h"

Chip8 myChip8;

int main(int argc, char** argv) {
	myChip8.init();
	myChip8.loadGame("..\\roms\\PONG");

	for (;;) {
		
		myChip8.emulateCycle();

		if (myChip8.drawFlag) {
			//draw some graphics
		}

		myChip8.setKeys();
	}
}