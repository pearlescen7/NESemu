#include "Cartridge.h"
#include "Mapper.h"

#include <iostream>
#include <fstream>

#include "Mapper_000.h"

Cartridge::Cartridge(std::string filePath)
{
	struct nesHeader
	{
		char name[4];
		uint8_t nPRGRom;
		uint8_t nCHRRom;
		uint8_t flags6;
		uint8_t flags7;
		uint8_t flags8;
		uint8_t flags9;
		uint8_t flags10;
		uint8_t padding[5];
	} header;

	std::ifstream file(filePath, std::ifstream::binary);
	if (file.fail())
	{
		m_imageValid = false;
		std::cout << "[ERROR] File with path '" << filePath << "' does not exist." << std::endl;
		return;
	}

	file.read((char*)(&header), sizeof(nesHeader));

	if (header.flags6 & 0b00000100)
		file.seekg(512, std::ios_base::cur);

	this->mapperID = ((header.flags7 >> 4) << 4) | (header.flags6 >> 4);
	this->mirror = (header.flags6 & 0x01) ? VERTICAL : HORIZONTAL;

	this->nPRGBank = header.nPRGRom;
	this->memPRG.resize((size_t)nPRGBank * 16384);
	file.read((char*)memPRG.data(), memPRG.size());

	this->nCHRBank = header.nCHRRom;
	this->memCHR.resize((size_t)(nCHRBank == 0 ? 1 : nCHRBank) * 8192);
	file.read((char*)memCHR.data(), memCHR.size());

	switch (this->mapperID)
	{
	case 0:
		this->mapper = std::make_shared<Mapper_000>(nPRGBank, nCHRBank);
		break;
	default:
		this->mapper = nullptr;
		std::cout << "[ERROR] Mapper_" << (int)mapperID << " is not added to the emulator." << std::endl;
		break;
	}


	this->m_imageValid = true;
	file.close();
}

bool Cartridge::cpuWrite(uint16_t addr, uint8_t data)
{
	uint32_t mapped_addr = 0;

	if (mapper->cpuMapWrite(addr, mapped_addr))
	{
		memPRG[mapped_addr] = data;
		return true;
	}

	return false;
}

bool Cartridge::cpuRead(uint16_t addr, uint8_t& data)
{
	uint32_t mapped_addr = 0;

	if (mapper->cpuMapRead(addr, mapped_addr))
	{
		data = memPRG[mapped_addr];
		return true;
	}

	return false;
}

bool Cartridge::ppuWrite(uint16_t addr, uint8_t data)
{
	uint32_t mapped_addr = 0;

	if (mapper->ppuMapWrite(addr, mapped_addr))
	{
		memCHR[mapped_addr] = data;
		return true;
	}

	return false;
}

bool Cartridge::ppuRead(uint16_t addr, uint8_t& data)
{
	uint32_t mapped_addr = 0;

	if (mapper->ppuMapRead(addr, mapped_addr))
	{
		data = memCHR[mapped_addr];
		return true;
	}

	return false;
}

void Cartridge::reset()
{
	if (mapper != nullptr)
		mapper->reset();
}
