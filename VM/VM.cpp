#include "pch.h"
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <string>
#include <sstream>
#include <bitset>

using namespace std;

ifstream fd("q1_encr.txt"); //encrypted failas

vector<unsigned char> decrRead() {
	ifstream input("decryptor.bin", std::ios::binary);     //nuskaitymas decrypted failo
	vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {}); // copies all data into buffer
	return buffer;
}

void MOVC(vector<unsigned char> buffer, int it, unsigned char regs[]) {
	regs[0] = buffer[it + 1];
}

void LSL(vector<unsigned char> buffer, int it, unsigned char regs[]) {
	int Regx = buffer[it + 1] & 0x0F;
	regs[Regx] = regs[Regx] << 1;
}

void JMP(vector<unsigned char> buffer, int &it, unsigned char regs[]) {
	signed char a = buffer[it + 1];
	it = it + a - 2; //-2 nes iskarto kitu ejimu prideda 2
}

void JFE(vector<unsigned char> buffer, int &it, unsigned char regs[], bool endOfFile) {
	if (endOfFile) JMP(buffer, it, regs);
}

void RET() {
	exit(0);
}

void SUB(vector<unsigned char> buffer, int it, unsigned char regs[]) {
	int Regx = buffer[it + 1] & 0x0F; //surandama i kuri registra ideti simboli, nutrinama nereikalinga dalis
	int Regy = (buffer[it + 1] & 0xF0) >> 4;
	regs[Regx] = regs[Regx] - regs[Regy];
}

void XOR(vector<unsigned char> buffer, int it, unsigned char regs[]) {
	int Regx = buffer[it + 1] & 0x0F; //surandama i kuri registra ideti simboli, nutrinama nereikalinga dalis
	int Regy = (buffer[it + 1] & 0xF0) >> 4;
	regs[Regx] = regs[Regx] ^ regs[Regy];
}

void OR(vector<unsigned char> buffer, int it, unsigned char regs[]) {
	int Regx = buffer[it + 1] & 0x0F; //surandama i kuri registra ideti simboli, nutrinama nereikalinga dalis
	int Regy = (buffer[it + 1] & 0xF0) >> 4;
	regs[Regx] = regs[Regx] | regs[Regy];
}

void IN(vector<unsigned char> buffer, int it, unsigned char regs[], bool &endOfFile) {
	if (fd.eof()) endOfFile = true;  //jei tai failo pabaiga
	else {
		int Regx = buffer[it + 1] & 0x0F; //surandama i kuri registra ideti simboli, nutrinama nereikalinga dalis
		char read;
		fd >> read;   //nuskaitomas simbolis
		regs[Regx] = read; // pereliamas i atitinkama registra
	}
}

void OUT(vector<unsigned char> buffer, int it, unsigned char regs[]) {
	int Regx = buffer[it + 1] & 0x0F; //surandama i kuri registra ideti simboli, nutrinama nereikalinga dalis
	cout << regs[Regx];
}

void checkCommand(vector<unsigned char> buffer, int &it, unsigned char regs[], bool &endOfFile) {
	//nustatoma kokia komanda iskviesti
	if (buffer[it] == 0x04)      MOVC(buffer, it, regs);
	else if (buffer[it] == 0x05) LSL(buffer, it, regs);
	else if (buffer[it] == 0x07) JMP(buffer, it, regs);
	else if (buffer[it] == 0x0A) JFE(buffer, it, regs, endOfFile);
	else if (buffer[it] == 0x0B) RET();
	else if (buffer[it] == 0x0D) SUB(buffer, it, regs);
	else if (buffer[it] == 0x0E) XOR(buffer, it, regs);
	else if (buffer[it] == 0x0F) OR(buffer, it, regs);
	else if (buffer[it] == 0x10) IN(buffer, it, regs, endOfFile);
	else if (buffer[it] == 0x11) OUT(buffer, it, regs);
}

int main() {
	unsigned char regs[16]; //registru masyvas
	vector<unsigned char> buffer = decrRead(); //nuskaityti faila ir viska ikelti i buffer
	int it = 0; //buffer pozicija
	bool endOfFile = false;

	while (1) {
		checkCommand(buffer, it, regs, endOfFile); //patikrinama kokia tai komanda ir iskvieciama
		it += 2; //pereinama i kita kokmanda, einama kas 2 nes buffer vienas yra kokmanda kitas yra baito konstanta
	}

	return 0;
}
/*It would appear that we have reached the limits of what 
it is possible to achieve with computer technology,
although one should be careful with such statements, 
as they tend to sound pretty silly in 5 years.
(John Von Neumann, circa 1949)*/