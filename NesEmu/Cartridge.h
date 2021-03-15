#pragma once

#include <memory>
#include <vector>
#include <string>

class Mapper;

class Cartridge
{
public:
	std::vector<uint8_t> memPRG;
	std::vector<uint8_t> memCHR;
private:
	std::shared_ptr<Mapper> mapper;

	uint8_t mapperID = 0;
	uint8_t nPRGBank = 0;
	uint8_t nCHRBank = 0;


	bool m_imageValid = false;

public:
	enum Mirror
	{
		HORIZONTAL,
		VERTICAL
	} mirror;

	Cartridge(std::string filePath);

	bool cpuWrite(uint16_t addr, uint8_t data);
	bool cpuRead(uint16_t addr, uint8_t& data);

	bool ppuWrite(uint16_t addr, uint8_t data);
	bool ppuRead(uint16_t addr, uint8_t& data);

	void reset();

	bool imageValid() { return m_imageValid; }
};

