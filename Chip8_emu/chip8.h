#ifndef CHIP_8_H
#define CHIP_8_H

#include <stdint.h>

class Chip8 {
private:
	uint16_t stack[16];				// Stack
	uint16_t sp;					// Stack pointer

	uint16_t memory[4096];			// 4k Memory
	uint16_t V[16];					// V registers (V0-VF)
	
	uint16_t pc;					// Program counter
	uint16_t opcode;				// Current op code
	uint16_t I;						// Index register

	uint8_t delay_timer;			// Delay timer
	uint8_t	sound_timer;			// Sound timer
	
public:

	unsigned char gfx[64 * 32];		// Graphics buffer
	unsigned char key[16];			// Keypad
	bool drawFlag;					// Boolean to know when to draw

	Chip8();						// Constructor
	~Chip8();						// Deconstructor

	void emulateCycle();			// To emulate a cycle
	void setKeys();					// To add keyboard inputs to the cycle
	void init();					// Start the emulator
	bool loadGame(const char*)		// The rom we will load up to test if the emulator works
};

#endif