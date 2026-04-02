#include<iostream>
#include<cstdint>
#include<cstring>
#include<cstdlib>  
#include<ctime>   
#include<fstream>
#include<vector>

using namespace std;

class chip8
{
    public:
        uint8_t display[32][64];
        uint8_t keypad[16];
        uint8_t prevKeypad[16];
        
    private:
        uint8_t memory[4096];
        uint16_t I;
        uint8_t Vx[16];
        uint8_t delay;
        uint8_t sound;
        uint16_t stack[16];
        uint8_t sp;
        uint16_t pc;
        
        const uint8_t fontset[80] = {
            0xF0, 0x90, 0x90, 0x90, 0xF0, 
            0x20, 0x60, 0x20, 0x20, 0x70, 
            0xF0, 0x10, 0xF0, 0x80, 0xF0, 
            0xF0, 0x10, 0xF0, 0x10, 0xF0, 
            0x90, 0x90, 0xF0, 0x10, 0x10, 
            0xF0, 0x80, 0xF0, 0x10, 0xF0, 
            0xF0, 0x80, 0xF0, 0x90, 0xF0, 
            0xF0, 0x10, 0x20, 0x40, 0x40, 
            0xF0, 0x90, 0xF0, 0x90, 0xF0, 
            0xF0, 0x90, 0xF0, 0x10, 0xF0, 
            0xF0, 0x90, 0xF0, 0x90, 0x90, 
            0xE0, 0x90, 0xE0, 0x90, 0xE0, 
            0xF0, 0x80, 0x80, 0x80, 0xF0, 
            0xE0, 0x90, 0x90, 0x90, 0xE0, 
            0xF0, 0x80, 0xF0, 0x80, 0xF0, 
            0xF0, 0x80, 0xF0, 0x80, 0x80  
        };

        uint16_t fetch() {
            uint16_t opcode = memory[pc]<<8 | memory[pc+1];
            pc += 2;
            return opcode;
        }

        void execute(uint16_t op) {
            uint8_t x   = (op & 0x0F00) >> 8;
            uint8_t y   = (op & 0x00F0) >> 4;
            uint8_t n   = (op & 0x000F);
            uint16_t nnn = (op & 0x0FFF);
            uint8_t kk  = (op & 0x00FF);

            switch((op & 0xF000) >> 12) {
                case 0x0:
                    if(op == 0x00E0) {
                        memset(display, 0, sizeof(display));
                    }
                    else if(op == 0x00EE) {
                        sp--;
                        pc = stack[sp];
                    }
                    break;

                case 0x1:
                    pc = nnn;
                    break;
                
                case 0x2:
                    stack[sp] = pc;
                    sp++;
                    pc = nnn;
                    break;

                case 0x3:
                    if(Vx[x] == kk) pc += 2;
                    break;
                
                case 0x4:
                    if(Vx[x] != kk) pc += 2;
                    break;
                
                case 0x5:
                    if((Vx[x] == Vx[y]) && (n == 0)) pc += 2;
                    break;

                case 0x6:
                    Vx[x] = kk;
                    break;
                
                case 0x7:
                    Vx[x] += kk;
                    break;
                
                case 0x8:
                    switch (n)
                    {
                        case 0x0:
                            Vx[x] = Vx[y];
                            break;

                        case 0x1:
                            Vx[x] |= Vx[y];
                            break;
                        
                        case 0x2:
                            Vx[x] &= Vx[y];
                            break;
                        
                        case 0x3:
                            Vx[x] ^= Vx[y];
                            break;

                        case 0x4:
                            if(Vx[x] + Vx[y] > 255) Vx[15] = 1;
                            else Vx[15] = 0;
                            Vx[x] += Vx[y];
                            break;
                        
                        case 0x5:
                            if(Vx[x] >= Vx[y]) Vx[15] = 1;
                            else Vx[15] = 0;
                            Vx[x] -= Vx[y];
                            break;

                        case 0x6:
                            Vx[15] = Vx[x] & 0x0001;
                            Vx[x] >>= 1;
                            break;
                        
                        case 0x7:
                            if(Vx[x] <= Vx[y]) Vx[15] = 1;
                            else Vx[15] = 0;
                            Vx[x] = Vx[y] - Vx[x];
                            break;

                        case 0xE:
                            Vx[15] = (Vx[x] & 0x80) >> 7;
                            Vx[x] <<= 1;
                            break;
                    }
                    break;
                    
                    case 0x9:
                        if(Vx[x] != Vx[y]) pc += 2;
                        break;

                    case 0xA:
                        I = nnn;
                        break;

                    case 0xB:
                        pc = nnn + Vx[0];
                        break;

                    case 0xC:
                        Vx[x] = (rand() % 256) & kk;
                        break;
                        
                    case 0xD: {
                        uint8_t xPos = Vx[x] % 64;
                        uint8_t yPos = Vx[y] % 32;
                        Vx[15] = 0;
                        for (int row = 0; row < n; row++) {
                            uint8_t spriteByte = memory[I + row];

                            for (int col = 0; col < 8; col++) {
                                uint8_t spritePixel = (spriteByte >> (7 - col)) & 1;
                                
                                if (spritePixel) {
                                    uint8_t xCoord = xPos + col;
                                    uint8_t yCoord = yPos + row;
                                    if (xCoord >= 64 || yCoord >= 32) continue; 
                                    if (display[yCoord][xCoord] == 1) {
                                        Vx[15] = 1;
                                    }
                                    display[yCoord][xCoord] ^= 1;
                                }
                            }
                        }
                        break;
                    }

                    case 0xE:
                        if(kk == 0x9E) {
                            if(keypad[Vx[x]] != 0) pc += 2;
                        } 
                        else if(kk == 0xA1) {
                            if(keypad[Vx[x]] == 0) pc += 2;
                        }
                        break;
                    
                    
                    case 0xF:
                        switch(kk)
                        {
                            case 0x07:
                                Vx[x] = delay;
                                break;

                            case 0x0A: {
                                bool keyPressed = false;
                                for(int i = 0; i < 16; i++) {
                                    if(prevKeypad[i] == 0 && keypad[i] == 1) {
                                        Vx[x] = i;          
                                        keyPressed = true;  
                                        break;            
                                    }
                                }
                                if (!keyPressed) pc -= 2; 
                                break;
                            }

                            case 0x15:
                                delay = Vx[x];
                                break;

                            case 0x18:
                                sound = Vx[x];
                                break;

                            case 0x1E:
                                I += Vx[x];
                                break;

                            case 0x29:
                                I = Vx[x] * 5;
                                break;

                            case 0x33:
                                memory[I] = Vx[x]/100;
                                memory[I+1] = (Vx[x]/10) % 10;
                                memory[I+2] = Vx[x] % 10;
                                break;

                            case 0x55:
                                for(int i = 0; i <= x; i++) {
                                    memory[I+i] = Vx[i];
                                }
                                break;

                            case 0x65:
                                for(int i = 0; i <= x; i++) {
                                    Vx[i] =  memory[I+i];
                                }
                                break;
                        }
            }
        }

    public:
        void initialise(){
            srand(time(nullptr));

            pc = 0x200;  

            I = 0;
            sp = 0;
            
            delay = 0;
            sound = 0;

            memset(display, 0, sizeof(display));
            memset(stack, 0, sizeof(stack));
            memset(Vx, 0, sizeof(Vx));
            memset(memory, 0, sizeof(memory));

            for (int i = 0; i < 80; i++) memory[i] = fontset[i];
        }

        void cycle(){
            uint16_t opcode = fetch();
            execute(opcode);
        }

        bool loadROM(const char* filename) {
            ifstream file(filename, ios::binary | ios::ate);

            if (file.is_open()) {
                streamsize size = file.tellg();
                if (size > (4096 - 0x200)) {
                    cout<<"Error: ROM is too large to fit in memory."<<endl;
                    return false;
                }
                file.seekg(0, ios::beg);
                vector<char> buffer(size);
                file.read(buffer.data(), size);
                memcpy(&memory[0x200], buffer.data(), size);
                return true;
            } 
            
            cout<<"Error: Could not open file "<<filename<<endl;
            return false;
        }

        void updateTimers() {
            if (delay > 0) delay--;
            if (sound > 0) sound--;
        }
};
