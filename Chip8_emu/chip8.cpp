#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <random>
#include <ctime>


#include "chip8.h"

unsigned char chip8_fontset[80] =
{
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

Chip8::Chip8() {

}

Chip8::~Chip8() {

}

void Chip8::init() {
	// Initalizing registers
	pc		= 0x200;			// Program counter starts at 0x200
	opcode	= 0;				// Reset current Opcode
	I		= 0;				// Reset index register
	sp		= 0;				// Reset stack pointer

	// Clear Display
	
	
	// Clear stack
	// Clear registers V0 - VF
	// Clear memory
	for (uint8_t i = 0; i < 16; i++) {
		stack[i]	= 0;
		key[i]		= 0;
		V[i]		= 0;
	}

	// Load fontset into memory
	for (uint8_t i = 0; i < 80) {
		memory[i] = chip8_fontset[i];
	}
}

void Chip8::emulateCycle() {
	std::clock_t start = std::clock();

	// Fetch Opcode
	opcode = memory[pc] << 8 | memory[pc + 1];

	// Decode Opcode & Execute Opcode
	// When decoding operation code, remember to add two to program counter unless it affects it directly, like a goto

	switch (opcode & 0xF000) {
	
	// 0XXX opcodes
	case 0x0000:
		switch (opcode & 0x000F) {
		
		// 00E0 - Clear the screen
		case 0x0000:
			for (uint8_t i = 0; i < 64 * 32; i++)
				gfx[i] = 0;
			drawFlag = true;
			pc += 2;
			break;
		
		// 00EE - Return from a subroutine
		case 0x000E:
			--sp;
			pc = stack[sp];
			pc += 2;
			break;

		default:
			printf("Unknown opcode: 0x%X\n", opcode);
			exit(3);
		}
		break;

	// 1NNN - Jump to address NNN
	case 0x1000:
		pc = opcode & 0x0FFF
		break;

	// 2NNN - Execute subroutine starting at address NNN
	case 0x2000:
		stack[sp] = pc;
		sp++;
		pc = opcode & 0x0FFF;
		break;

	// 3XNN - Skip following instruction if value of register VX equals NN
	case 0x3000:
		if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
			pc += 2;
		pc += 2;
		break;

	// 4XNN - Skip following instruction if value of register VX not equal to NN
	case 0x4000:
		if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
			pc += 2;
		pc += 2;
		break;

	// 5XY0 - Skip following instruction if register VX is equal to register VY
	case 0x5000:
		if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
			pc += 2;
		pc += 2;
		break;

	// 6XNN - Store NN to register VX
	case 0x6000:
		V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
		pc += 2;
		break;

	// 7XNN - Add NN to register VX
	case 0x7000:
		V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
		pc += 2;
		break;

	// 8XXX opcodes
	case 0x8000:
		switch (opcode & 0x000F) {
			
		// 8XY0 - Store the value of register VY to register VX
		case 0x0000:
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4]
			pc += 2;
			break;

		// 8XY1 - Set VX to (VX OR VY)
		case 0x0001:
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] | V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;

		// 8XY2 - Set VX to (VX AND VY)
		case 0x0002:
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] & V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;

		// 8XY3 - Set VX to (VX XOR VY)
		case 0x0003:
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] ^ V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;
		
		// 8XY4 - Add the value of VY to register VX
		case 0x0004:
			if (V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]))
				V[0xF] = 1;												// Carry one if we loop over
			else
				V[0xF] = 0;												// Else set it to 0
			V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;

		// 8XY5 - Subtract the value of VY from register VX
		case 0x0005:
			if (V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8])
				V[0xF] = 1;												// Carry one if we loop under
			else
				V[0xF] = 0;												// Else set it to 0
			V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
			pc +=2;
			break;

		// 8XY6 - Store the value of register VY shifted right bit by one bit in register VX
		case 0x0006:
			V[0xF] = V[(opcode & 0x00F0) >> 4] & 0x0001;				// Store the least significant bit prior to shift						// Could be a problem zone if emulator failed
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] >> 1;
			pc += 2;
			break;

		// 8XY7 - Set register VX to the value of VY minus VX
		case 0x0007:
			if (V[(opcode & 0x00F0) >> 4] < V[(opcode & 0x0F00) >> 8])
				V[0xF] = 1;												// Carry one if we loop under
			else
				V[0xF] = 0;												// Else set it to 0
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
			pc += 2;
			break;

		// 8XYE - Store the value of register VY shifted left bit by one bit in register VX
		case 0x000E:
			V[0xF] = (V[(opcode & 0x00F0) >> 4] & 0xF000) >> 15;		// Store the least significant bit prior to shift						// Could be a problem zone if emulator failed
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] << 1;
			pc += 2;
			break;

		default:
			printf("Unknown opcode: 0x%X\n", opcode);
			exit(3);
		}
		break;

	// 9XY0 - Skip following instruction if value of register VX is not equal to the value of register VY
	case 0x9000:
		if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
			pc += 2;
		pc += 2;
		break;

	// ANNN - Store memory address NNN in register I
	case 0xA000:
		I = opcode & 0x0FFF;
		pc += 2;
		break;

	// BNNN - Jump to address NNN + V0
	case 0xB000:
		pc = (opcode & 0x0FFF) + V[0x0];
		break;

	// CXNN - Sets VX to a random number, masked by NN.
	case 0xC000:
		V[(opcode & 0x0F00) >> 8] = (rand() % (0xFF + 1)) & (opcode & 0x00FF);
		pc += 2;
		break;

	// DXYN - Draw a sprite at position (VX, VY) with N bytes of sprite data starting at the address stored in I
	//		  and set VF to 01 if any pixels are cahnged to unset, and 00 otherwise
	case 0xD000:
		unsigned short x = V[(opcode & 0x0F00) >> 8];
		unsigned short y = V[(opcode & 0x00F0) >> 4];
		unsigned short height = opcode & 0x000F; // N number of sprite data
		unsigned short pixel;

		V[0xF] = 0;
		for (int yline = 0; yline < height; yline++) // we are drawing for n sprites
		{
			pixel = memory[I + yline]; //The pixel we are looking at need the memory address stored in there
			for (int xline = 0; xline < 8; xline++)
			{
				if ((pixel & (0x80 >> xline)) != 0) // makes
				{
					if (gfx[(x + xline + ((y + yline) * 64))] == 1)
					{
						V[0xF] = 1;
					}
					gfx[x + xline + ((y + yline) * 64)] ^= 1;
				}
			}
		}

		drawFlag = true;
		pc += 2;
		break;

	// EXXX opcodes
	case 0xE000:
		switch (opcode & 0x00FF) {
			
		// EX9E - Skip the following instruction if the key corresponding to the hex value currently stored
		//		  in VX is pressed
		case 0x009E:
			if (key[V[(opcode & 0x0F00) >> 8]] != 0)
				pc += 2;
			pc += 2;
			break;

		// EXA1 - Skip the following instruction if the key corresponding to the hex value currently stored
		//		  in VX is not pressed
		case 0x00A1:
			if (key[V[(opcode & 0x0F00) >> 8]] == 0)
				pc += 2;
			pc += 2;
			break;

		default:
			printf("Unknown opcode: 0x%X\n", opcode);
			exit(3);
		}
		break;

	// FXXX opcodes
	case 0xF000:
		switch (opcode & 0x00FF) {

		// FX07 - Store the current value of the delay timer in register VX
		case 0x0007:
			V[(opcode & 0x0F00) >> 8]) = delay_timer;
			pc += 2;
			break;

		// FX0A - Wait for a keypress and store the result in register VX
		case 0x000A:
			bool isKeyPressed = false;
			for (uint16_t keyNo = 0; key < 0xF; keyNo++)
				isKeyPressed = isKeyPressed || key[keyNo];

			if(isKeyPressed)
				pc += 2;
			break;

		// FX15 - Set the delay timer to the value of register VX
		case 0x0015:
			delay_timer = V[(opcode & 0x0F00) >> 8]);
			pc += 2;
			break;

		// FX18 - Set the sound timer to the value of register VX
		case 0x0018:
			sound_timer = V[(opcode & 0x0F00) >> 8]);
			pc += 2;
			break;

		// FX1E - Add the value stored in register VX to register I
		case 0x001E:
			I += V[(opcode & 0x0F00) >> 8]);
			pc += 2;
			break;

		// FX29 - Set I to the memory address of the sprite data corresponding to the hexadecimal digit 
		//		  stored in register VX
		case 0x0029:
			I = V[(opcode & 0x0F00) >> 8]) * 0x5;
			pc += 2;
			break;

		// FX33 - Store the binary-coded decimal equivalent of the value stored in register VX at
		//		  addresses I, I+1, I+2 (if number is 123 then mem[I] = 0x0001 , mem[I+1] = 0x0010, and meme[I+2] = 0x0011)
		case 0x0033:
			// First digit XNN / 100 = X (int division)
			// Second digit NXN / 10 = NX % 10 = X 
			// Third digit NNX % 10 = X
			memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
			memory[I] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
			memory[I] = V[(opcode & 0x0F00) >> 8] % 10;
			pc += 2;
			break;

		// FX55 - Store the values of registers V0 to VX (inclusive) in memory starting at address I
		//		  I is set to I + X + 1 after the operation
        case 0x0055:
			int range = (opcode & 0x0F00) >> 8;
			for (uint8_t registerNo = 0; registerNo <= range; i++) 
				memory[I + registerNo] = V[registerNo];
			I = I + range + 1;
			pc += 2;
			break;

		// FX65 - Fill registers V0 to VX with values stored in memory starting at address I
		//		  I is set to I + X + 1 after the operation
		case 0x0065:
			int range = (opcode & 0x0F00) >> 8;
			for (uint8_t registerNo = 0; registerNo <= range; i++)
				V[registerNo] = memory[I + registerNo];
			I = I + range + 1;
			pc += 2;
			break;

		default:
			printf("Unknown opcode: 0x%X\n", opcode);
			exit(3);
		}
		break;

	default:
		printf("Unknown opcode: 0x%X\n", opcode);
		exit(3);
	}


	// Update timers
	if (delay_timer > 0)
		--delay_timer;
	if (sound_timer > 0) {
		if (sound_timer == 1)
			printf("BEEP!\n");
		--sound_timer;
	}

	// Forcing 60fps
	while (std::clock() > (start + (1000/60))) {}
}

void Chip8::setKeys() {
}

bool Chip8::loadGame(const char* game_file) {
	printf("Loading ROM: %s\n", file_path);

	// Open ROM file
	FILE* rom = fopen(file_path, "rb");
	if (rom == NULL) {
		std::cerr << "Failed to open ROM" << std::endl;
		return false;
	}

	// Get file size
	fseek(rom, 0, SEEK_END);
	long rom_size = ftell(rom);
	rewind(rom);

	// Allocate memory to store rom
	char* rom_buffer = (char*)malloc(sizeof(char) * rom_size);
	if (rom_buffer == NULL) {
		std::cerr << "Failed to allocate memory for ROM" << std::endl;
		return false;
	}

	// Copy ROM into buffer
	size_t result = fread(rom_buffer, sizeof(char), (size_t)rom_size, rom);
	if (result != rom_size) {
		std::cerr << "Failed to read ROM" << std::endl;
		return false;
	}

	// Copy buffer to memory
	if ((4096 - 512) > rom_size) {
		for (int i = 0; i < rom_size; ++i) {
			memory[i + 512] = (uint8_t)rom_buffer[i];   // Load into memory starting
														// at 0x200 (=512)
		}
	}
	else {
		std::cerr << "ROM too large to fit in memory" << std::endl;
		return false;
	}

	// Clean up
	fclose(rom);
	free(rom_buffer);

	return true;
}
