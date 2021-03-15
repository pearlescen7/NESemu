#include "Mapper_000.h"

bool Mapper_000::cpuMapWrite(uint16_t addr, uint32_t& mapped_addr)
{
    if (addr >= 0x8000 && addr <= 0xFFFF)
    {
        mapped_addr = addr & (nPRGBank > 1 ? 0x7FFF : 0x3FFF);
        return true;
    }
    return false;
}

bool Mapper_000::cpuMapRead(uint16_t addr, uint32_t& mapped_addr)
{
    if (addr >= 0x8000 && addr <= 0xFFFF)
    {
        mapped_addr = addr & (nPRGBank > 1 ? 0x7FFF : 0x3FFF);
        return true;
    }
    return false;
}

bool Mapper_000::ppuMapWrite(uint16_t addr, uint32_t& mapped_addr)
{
    if (addr >= 0x0000 && addr <= 0x1FFF && nCHRBank == 0)
    {
        mapped_addr = addr;
        return true;
    }
    return false;
}

bool Mapper_000::ppuMapRead(uint16_t addr, uint32_t& mapped_addr)
{
    if (addr >= 0x0000 && addr <= 0x1FFF)
    {
        mapped_addr = addr;
        return true;
    }
    return false;
}

void Mapper_000::reset()
{
}
