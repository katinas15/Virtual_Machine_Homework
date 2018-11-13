#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

ifstream fd("q1_encr.txt"); //encrypted failas

vector<unsigned char> decrRead() {
    ifstream input("decryptor.bin", std::ios::binary);     //nuskaitymas decryptor failo
    vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {}); // copies all data into buffer
    return buffer;
}

void INC(vector<unsigned char> buffer, int it, unsigned char regs[], bool &zeroFlag) {
    int Regx = buffer[it + 1] & 0x0F;    //paimami pirmi 4 bitai o kiti nutrinami ir gaunamas skaicius
    regs[Regx]++;
    if(regs[Regx] == 0) zeroFlag = true;
}

void DEC(vector<unsigned char> buffer, int it, unsigned char regs[], bool &zeroFlag) {
    int Regx = buffer[it + 1] & 0x0F;    //paimami pirmi 4 bitai o kiti nutrinami ir gaunamas skaicius
    regs[Regx]--;
    if(regs[Regx] == 0) zeroFlag = true;
}

void MOV(vector<unsigned char> buffer, int it, unsigned char regs[]) {
    int Regx = buffer[it + 1] & 0x0F;
    int Regy = (buffer[it + 1] & 0xF0) >> 4;
    regs[Regx] = regs[Regy];
}

void MOVC(vector<unsigned char> buffer, int it, unsigned char regs[]) {
    regs[0] = buffer[it + 1];    //i nulini registra kopijuojama konstanta
}

void LSL(vector<unsigned char> buffer, int it, unsigned char regs[], bool &zeroFlag) {
    int Regx = buffer[it + 1] & 0x0F;    //paimami pirmi 4 bitai o kiti nutrinami ir gaunamas skaicius
    regs[Regx] = regs[Regx] << 1;    //perstumiami bitai i kaire
    if(regs[Regx] == 0) zeroFlag = true;
}

void LSR(vector<unsigned char> buffer, int it, unsigned char regs[], bool &zeroFlag) {
    int Regx = buffer[it + 1] & 0x0F;    //paimami pirmi 4 bitai o kiti nutrinami ir gaunamas skaicius
    regs[Regx] = regs[Regx] >> 1;    //perstumiami bitai i desine
    if(regs[Regx] == 0) zeroFlag = true;
}

void JMP(vector<unsigned char> buffer, int &it, unsigned char regs[]) {
    signed char a = buffer[it + 1];    //baito konstata nusako kiek jumpint nuo esamos pozicijos
    it = it + a - 2; //-2 nes iskarto kitu ejimu prideda 2
}

void JZ(vector<unsigned char> buffer, int &it, unsigned char regs[], bool zeroFlag) {
    if (zeroFlag) JMP(buffer, it, regs);    
}

void JNZ(vector<unsigned char> buffer, int &it, unsigned char regs[], bool zeroFlag) {
    if (!zeroFlag) JMP(buffer, it, regs);    
}

void JFE(vector<unsigned char> buffer, int &it, unsigned char regs[], bool endOfFile) {
    if (endOfFile) JMP(buffer, it, regs);    //jump jei failo pabaiga
}

void RET() {
    exit(0);    //rip
}

void ADD(vector<unsigned char> buffer, int it, unsigned char regs[], bool &zeroFlag) {
    int Regx = buffer[it + 1] & 0x0F;
    int Regy = (buffer[it + 1] & 0xF0) >> 4;    //paimami nuo 7 iki 4 bito ir pastumiami per 4 i desine tam kad butu galima gauti skaiciu
    regs[Regx] = regs[Regx] + regs[Regy];
    if(regs[Regx] == 0) zeroFlag = true;
}

void SUB(vector<unsigned char> buffer, int it, unsigned char regs[], bool &zeroFlag) {
    int Regx = buffer[it + 1] & 0x0F;
    int Regy = (buffer[it + 1] & 0xF0) >> 4;    //paimami nuo 7 iki 4 bito ir pastumiami per 4 i desine tam kad butu galima gauti skaiciu
    regs[Regx] = regs[Regx] - regs[Regy];
    if(regs[Regx] == 0) zeroFlag = true;
}

void XOR(vector<unsigned char> buffer, int it, unsigned char regs[], bool &zeroFlag) {
    int Regx = buffer[it + 1] & 0x0F;
    int Regy = (buffer[it + 1] & 0xF0) >> 4;
    regs[Regx] = regs[Regx] ^ regs[Regy];
    if(regs[Regx] == 0) zeroFlag = true;
}

void OR(vector<unsigned char> buffer, int it, unsigned char regs[], bool &zeroFlag) {
    int Regx = buffer[it + 1] & 0x0F;
    int Regy = (buffer[it + 1] & 0xF0) >> 4;
    regs[Regx] = regs[Regx] | regs[Regy];
    if(regs[Regx] == 0) zeroFlag = true;
}

void IN(vector<unsigned char> buffer, int it, unsigned char regs[], bool &endOfFile, bool &zeroFlag) {
    int Regx = buffer[it + 1] & 0x0F;
    char read;
    fd >> read;   //nuskaitomas simbolis
    regs[Regx] = read; // pereliamas i atitinkama registra
    
    if (fd.eof()) endOfFile = true;  //jei tai failo pabaiga
    
    if(regs[Regx] == 0) zeroFlag = true;
}

void OUT(vector<unsigned char> buffer, int it, unsigned char regs[]) {
    int Regx = buffer[it + 1] & 0x0F; //surandama i kuri registra ideti simboli
    cout << regs[Regx];
}

void checkCommand(vector<unsigned char> buffer, int &it, unsigned char regs[], bool &endOfFile, bool &zeroFlag) {
    if (buffer[it] == 0x01)      INC(buffer, it, regs, zeroFlag);
    else if (buffer[it] == 0x02) DEC(buffer, it, regs, zeroFlag);
    else if (buffer[it] == 0x03) MOV(buffer, it, regs);
    else if (buffer[it] == 0x04) MOVC(buffer, it, regs);
    else if (buffer[it] == 0x05) LSL(buffer, it, regs, zeroFlag);
    else if (buffer[it] == 0x06) LSR(buffer, it, regs, zeroFlag);
    else if (buffer[it] == 0x07) JMP(buffer, it, regs);
    else if (buffer[it] == 0x08) JZ(buffer, it, regs, zeroFlag);
    else if (buffer[it] == 0x09) JNZ(buffer, it, regs, zeroFlag);
    else if (buffer[it] == 0x0A) JFE(buffer, it, regs, endOfFile);
    else if (buffer[it] == 0x0B) RET();
    else if (buffer[it] == 0x0C) ADD(buffer, it, regs, zeroFlag);
    else if (buffer[it] == 0x0D) SUB(buffer, it, regs, zeroFlag);
    else if (buffer[it] == 0x0E) XOR(buffer, it, regs, zeroFlag);
    else if (buffer[it] == 0x0F) OR(buffer, it, regs, zeroFlag);
    else if (buffer[it] == 0x10) IN(buffer, it, regs, endOfFile, zeroFlag);
    else if (buffer[it] == 0x11) OUT(buffer, it, regs);
}

int main() {
    unsigned char regs[16]; //registru masyvas
    vector<unsigned char> buffer = decrRead(); //nuskaityti faila ir viska ikelti i buffer
    int it = 0; //buffer pozicija
    bool endOfFile = false;    //end of file flag
    bool zeroFlag = false;
    while (1) {
        checkCommand(buffer, it, regs, endOfFile, zeroFlag); //patikrinama kokia tai komanda ir iskvieciama
        it += 2; //pereinama i kita komanda, einama kas 2 nes vienas yra komanda kitas yra baito konstanta
    }
    
    return 0;
}
/*========Output============
 It would appear that we have reached the limits of what
 it is possible to achieve with computer technology,
 although one should be careful with such statements,
 as they tend to sound pretty silly in 5 years.
 (John Von Neumann, circa 1949)
 ==========Output==========*/
