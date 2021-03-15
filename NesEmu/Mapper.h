#pragma once

#include <cinttypes>

class Mapper
{
protected:
	uint8_t nPRGBank = 0;
	uint8_t nCHRBank = 0;
public:
	Mapper(uint8_t nPRGBank, uint8_t nCHRBank)
		: nPRGBank(nPRGBank), nCHRBank(nCHRBank) {}

	virtual bool cpuMapWrite(uint16_t addr, uint32_t& mapped_addr) = 0;
	virtual bool cpuMapRead(uint16_t addr, uint32_t& mapped_addr) = 0;

	virtual bool ppuMapWrite(uint16_t addr, uint32_t& mapped_addr) = 0;
	virtual bool ppuMapRead(uint16_t addr, uint32_t& mapped_addr) = 0;
	
	virtual void reset() = 0;
};

