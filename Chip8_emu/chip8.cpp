#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <random>


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
			V[0xF] = V[(opcode & 0x00F0) >> 4] & 0x000F;				// Store the least significant bit prior to shift						// Could be a problem zone if emulator failed
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
			V[0xF] = (V[(opcode & 0x00F0) >> 4] & 0xF000) >> 12;		// Store the least significant bit prior to shift						// Could be a problem zone if emulator failed
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
		printf("HAVEN'T IMPLEMENTED THIS LINE OF CODE YET");
		pc += 2;
		break;

	// EXXX opcodes
	case 0xE000:
		switch (opcode & 0x000F) {
			
			// EX9E
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
}

void Chip8::setKeys() {
}

bool Chip8::loadGame(const char* game_file) {
	//just load the contents of the ROM into memory, will write later when I understand more and do not need to look at a guide to understand
}
