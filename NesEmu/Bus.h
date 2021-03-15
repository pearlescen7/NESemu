#pragma once

#include <array>
#include <memory>

#include "nes6502.h"
#include "nes2c02.h"
#include "Cartridge.h"

class Bus
{
public:
	nes6502 cpu;
	nes2c02 ppu;

private:
	std::array<uint8_t, 2048> cpuRam;
	std::shared_ptr<Cartridge> cartridge;
	size_t systemClockCounter = 0;

public:
	Bus()
		:cpu(this)
	{
		cpuRam.fill(0);
	}

	void cpuWrite(uint16_t addr, uint8_t data);
	uint8_t cpuRead(uint16_t addr);

	void insertCartridge(std::shared_ptr<Cartridge> cartridge);
	void reset();
	void clock();
};

