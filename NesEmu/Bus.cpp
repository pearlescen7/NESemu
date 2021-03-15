#include "Bus.h"

void Bus::cpuWrite(uint16_t addr, uint8_t data)
{
	if (cartridge->cpuWrite(addr, data))
	{

	}
	else if (addr >= 0x0000 && addr <= 0x1FFF)
		cpuRam[addr & 0x07FF] = data;
	else if (addr >= 0x2000 && addr <= 0x3FFF)
		ppu.cpuWrite(addr & 0x0007, data);
}

uint8_t Bus::cpuRead(uint16_t addr)
{
	uint8_t data = 0;
	if (cartridge->cpuRead(addr, data))
		return data;
	else if (addr >= 0x0000 && addr <= 0x1FFF)
		return cpuRam[addr & 0x07FF];
	else if (addr >= 0x2000 && addr <= 0x3FFF)
		return ppu.cpuRead(addr & 0x0007);

	return data;
}

void Bus::insertCartridge(std::shared_ptr<Cartridge> cartridge)
{
	this->cartridge = cartridge;
	ppu.insertCartridge(cartridge);
}

void Bus::reset()
{
	cartridge->reset();
	cpu.reset();
	ppu.reset();
	systemClockCounter = 0;
}

void Bus::clock()
{
	ppu.clock();
	if (systemClockCounter % 3)
		cpu.clock();
	if (ppu.nmi)
	{
		ppu.nmi = false;
		cpu.nmi();
	}
	systemClockCounter++;
}
