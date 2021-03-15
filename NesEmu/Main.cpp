#include <memory>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <fstream>

#include <SFML/Graphics.hpp>

#include "Common.h"
#include "Cartridge.h"
#include "Bus.h"

#include "NesScreen.h"

#include <cstdlib>

int main()
{
#if 1
	NesScreen nes("..\\tests\\nestest.nes");
	nes.init();
	nes.printImage("out.txt");
	bool run = true;
	while (run)
	{
		run = nes.update();
	}
#endif

#if 0
	Bus nes;
	auto cart = std::make_shared<Cartridge>("..\\tests\\nestest.nes");
	nes.insertCartridge(cart);
	nes.reset();
	auto ram = nes.cpu.dissamble(0x0000, 0xFFFF);
	while (true)
	{
		uint16_t pc_prev = nes.cpu.pc;
		while(pc_prev == nes.cpu.pc)
			nes.cpu.clock();

		std::cout << std::hex << "A:" << int(nes.cpu.reg_a) << " X:" << int(nes.cpu.reg_x) << " Y:" << int(nes.cpu.reg_y) << "\n" <<
			"PC:" << int(nes.cpu.pc) << " SP: " << int(nes.cpu.sp) << "\n" <<
			"Cycle:" << int(nes.cpu.cycles) << "\n";
		uint8_t p = nes.cpu.status_reg;
		std::string c = "N V U B D I Z C";

		for(int i = 0; i < 16; i+=2)
		{
			if (!(p & 0b1000'0000))
				c[i] = tolower(c[i]);
			p <<= 1;
		}
		std::cout << c << "\n\n";

		auto it = ram.find(nes.cpu.pc);
		for (int i = 0; i < 7 && it != ram.begin(); i++)
			it--;

		for (int i = 0; i < 16; i++)
		{
			std::cout << (it->first == nes.cpu.pc ? "->" : "  ") << it->second << "\n";
			it++;
		}

		std::cin.get();
		system("cls");
	}
#endif

	return 0;
}