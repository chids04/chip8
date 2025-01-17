#include "cpu.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>

namespace Chip8 {

Cpu::Cpu(const std::string &path)
{
    sp = 0;
    opcode = 0;
    pc = 0x200; // Program starts at this address
    delayTimer = 0;
    soundTimer = 0;
    df = false;

    delayEnded = true;
    soundEnded = true;

    std::fill(memory.begin(), memory.end(), 0);
    std::fill(V.begin(), V.end(), 0);
    std::fill(stack.begin(), stack.end(), 0);

    for (uint8_t i = 0x00; i <= 0xF; i++) {
        key[i] = i;
    }

    constexpr std::array<uint8_t, 80> fonts = {
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

    std::copy(fonts.begin(), fonts.end(), memory.begin() + 0x050);

    loadGame(path);
    gen.seed(std::random_device()());
    initializeOpcodeTable();
}

void Cpu::loadGame(const std::string &path) {
    std::ifstream game(path, std::ios::binary);

    if (!game.is_open()) {
        std::cerr << "Invalid file path" << std::endl;
        return;
    }

    game.seekg(0, std::ios::end);
    std::streamsize gameSize = game.tellg();
    game.seekg(0, std::ios::beg);

    std::vector<char> buf(gameSize);
    game.read(buf.data(), gameSize);
    game.close();

    for (int i = 0; i < gameSize && (i + 512) < 4096; i++) {
        memory[512 + i] = static_cast<uint8_t>(buf[i]);
    }
}

uint8_t Cpu::getRand() {
    std::uniform_int_distribution<uint8_t> distribution(0, 255);
    return distribution(gen);
}

void Cpu::printRegisters(const std::string &message) const
{
    std::cout << message << "\n";
    for (size_t i = 0; i < V.size(); ++i) {
        std::cout << "V[" << i << "]=" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(V[i]) << "\n";
    }

    std::cout << "\nI=" << std::hex << std::setw(4) << I << "\nPC=" << std::hex << std::setw(4) << static_cast<int>(pc)
        << " \nSP=" << std::hex << std::setw(2) << static_cast<int>(sp) << std::endl;
}

void Cpu::initializeOpcodeTable() {
    opcodeTable[0x0] = [this]() { handle0x0(); };
    opcodeTable[0x1] = [this]() { handle0x1(); };
    opcodeTable[0x2] = [this]() { handle0x2(); };
    opcodeTable[0x3] = [this]() { handle0x3(); };
    opcodeTable[0x4] = [this]() { handle0x4(); };
    opcodeTable[0x5] = [this]() { handle0x5(); };
    opcodeTable[0x6] = [this]() { handle0x6(); };
    opcodeTable[0x7] = [this]() { handle0x7(); };
    opcodeTable[0x8] = [this]() { handle0x8(); };
    opcodeTable[0x9] = [this]() { handle0x9(); };
    opcodeTable[0xA] = [this]() { handle0xA(); };
    opcodeTable[0xB] = [this]() { handle0xB(); };
    opcodeTable[0xC] = [this]() { handle0xC(); };
    opcodeTable[0xD] = [this]() { handle0xD(); };
    opcodeTable[0xE] = [this]() { handle0xE(); };
    opcodeTable[0xF] = [this]() { handle0xF(); };
}

void Cpu::emulatecycle() {
    opcode = (memory[pc] << 8) | memory[pc + 1];
    pc += 2;


    uint8_t instruction = (opcode & GET_FIRST_NIB) >> 12;

    std::cout << std::hex << static_cast<int>(instruction) << "\n";
    printRegisters("registers before exection");

    if (opcodeTable.find(instruction) != opcodeTable.end()) {
        opcodeTable[instruction]();
        printRegisters("registers after execution");
    } else {
        std::cerr << "Unknown instruction: 0x" << std::hex << opcode << std::endl;
    }
}

// Opcode handling functions
void Cpu::handle0x0() {
    if ((opcode & 0x00FF) == 0xE0) {
        // Clear the screen
        std::fill(gfx.begin(), gfx.end(), 0);
        df = true;
    } else if ((opcode & 0x00FF) == 0xEE) {
        // Return from a subroutine
        sp--;
        pc = stack[sp];
    }
}

void Cpu::handle0x1() {
    pc = opcode & GET_12_BIT; // Jump to address
}

void Cpu::handle0x2() {
    stack[sp] = pc;
    sp++;
    pc = opcode & GET_12_BIT; // Call subroutine
}

void Cpu::handle0x3() {
    uint8_t x = (opcode & GET_SECOND_NIB) >> 8;
    uint8_t kk = opcode & GET_SEC_BYTE;
    if (V[x] == kk) {
        pc += 2; // Skip next instruction
    }
}

void Cpu::handle0x4() {
    uint8_t x = (opcode & GET_SECOND_NIB) >> 8;
    uint8_t kk = opcode & GET_SEC_BYTE;
    if (V[x] != kk) {
        pc += 2; // Skip next instruction
    }
}

void Cpu::handle0x5() {
    uint8_t x = (opcode & GET_SECOND_NIB) >> 8;
    uint8_t y = (opcode & GET_THIRD_NIB) >> 4;
    if (V[x] == V[y]) {
        pc += 2; // Skip next instruction
    }
}

void Cpu::handle0x6() {
    uint8_t x = (opcode & GET_SECOND_NIB) >> 8;
    uint8_t kk = opcode & GET_SEC_BYTE;
    V[x] = kk; // Set Vx = kk
}

void Cpu::handle0x7() {
    uint8_t x = (opcode & GET_SECOND_NIB) >> 8;
    uint8_t kk = opcode & GET_SEC_BYTE;
    V[x] += kk; // Add kk to Vx
}

void Cpu::handle0x8() {
    uint8_t x = (opcode & GET_SECOND_NIB) >> 8;
    uint8_t y = (opcode & GET_THIRD_NIB) >> 4;
    uint8_t z = opcode & GET_FOURTH_NIB;

    switch (z) {
        case 0x0: V[x] = V[y]; break;           // Set Vx = Vy
        case 0x1: V[x] |= V[y]; break;          // Set Vx = Vx OR Vy
        case 0x2: V[x] &= V[y]; break;          // Set Vx = Vx AND Vy
        case 0x3: V[x] ^= V[y]; break;          // Set Vx = Vx XOR Vy
        case 0x4: {                             // Add Vy to Vx with carry
            uint16_t result = V[x] + V[y];
            V[0xF] = (result > 255) ? 1 : 0;
            V[x] = result & 0xFF;
            break;
        }
        case 0x5: {                             // Subtract Vy from Vx with borrow
            V[0xF] = (V[x] > V[y]) ? 1 : 0;
            V[x] -= V[y];
            break;
        }
        case 0x6:                               // Shift Vx right
            V[0xF] = V[x] & 0x1;
            V[x] >>= 1;
            break;
        case 0x7: {                             // Set Vx = Vy - Vx with borrow
            V[0xF] = (V[y] > V[x]) ? 1 : 0;
            V[x] = V[y] - V[x];
            break;
        }
        case 0xE:                               // Shift Vx left
            V[0xF] = (V[x] & 0x80) >> 7;
            V[x] <<= 1;
            break;
    }
}

void Cpu::handle0x9() {
    uint8_t x = (opcode & GET_SECOND_NIB) >> 8;
    uint8_t y = (opcode & GET_THIRD_NIB) >> 4;
    if (V[x] != V[y]) {
        pc += 2; // Skip next instruction
    }
}

void Cpu::handle0xA() {
    I = opcode & GET_12_BIT; // Set I to address
}

void Cpu::handle0xB() {
    pc = V[0x0] + (opcode & GET_12_BIT); // Jump to address + V0
}

void Cpu::handle0xC() {
    uint8_t x = (opcode & GET_SECOND_NIB) >> 8;
    uint8_t kk = opcode & GET_SEC_BYTE;
    V[x] = getRand() & kk; // Set Vx = random & kk
}

void Cpu::handle0xD() {
    uint8_t x = V[(opcode & GET_SECOND_NIB) >> 8];
    uint8_t y = V[(opcode & GET_THIRD_NIB) >> 4];
    uint8_t height = opcode & 0x000F;
    uint8_t pixel;

    V[0xF] = 0;
    for (int yline = 0; yline < height; yline++) {
        pixel = memory[I + yline];
        for (int xline = 0; xline < 8; xline++) {
            if ((pixel & (0x80 >> xline)) != 0) {
                if (gfx[(x + xline + ((y + yline) * 64))] == 1) {
                    V[0xF] = 1;
                }
                gfx[x + xline + ((y + yline) * 64)] ^= 1;
            }
        }
    }
    df = true;
}

void Cpu::handle0xE() {
    uint8_t x = (opcode & GET_SECOND_NIB) >> 8;
    uint8_t keyState = key[V[x]];
    if ((opcode & GET_SEC_BYTE) == 0x9E) { // Skip next if key pressed
        if (keyState == 1) pc += 2;
    } else if ((opcode & GET_SEC_BYTE) == 0xA1) { // Skip next if key not pressed
        if (keyState == 0) pc += 2;
    }
}

void Cpu::handle0xF() {
    uint8_t x = (opcode & GET_SECOND_NIB) >> 8;
    switch (opcode & GET_SEC_BYTE) {
        case 0x07: V[x] = delayTimer; break; // Set Vx = delay timer
        case 0x0A: {                         // Wait for key press
            bool pressed = false;
            for (uint8_t i = 0; i < 16; i++) {
                if (key[i] == 1) {
                    V[x] = i;
                    pressed = true;
                    break;
                }
            }
            if (!pressed) pc -= 2; // Repeat instruction until key pressed
            break;
        }
        case 0x15: delayTimer = V[x]; break; // Set delay timer = Vx
        case 0x18: soundTimer = V[x]; break; // Set sound timer = Vx
        case 0x1E: I += V[x]; break;         // Add Vx to I
        case 0x29: I = memory[0x050 + (V[x] * 5)]; break; // Set I to sprite
        case 0x33: {                         // Store BCD representation
            memory[I] = V[x] / 100;
            memory[I + 1] = (V[x] / 10) % 10;
            memory[I + 2] = V[x] % 10;
            break;
        }
        case 0x55: {                         // Store registers V0 to Vx in memory
            for (uint8_t i = 0; i <= x; i++) {
                memory[I + i] = V[i];
            }
            break;
        }
        case 0x65: {                         // Read registers V0 to Vx from memory
            for (uint8_t i = 0; i <= x; i++) {
                V[i] = memory[I + i];
            }
            break;
        }
    }
}


} // namespace Chip8
