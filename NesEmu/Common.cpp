#include "Common.h"

std::string hex(uint8_t num)
{
	std::string str = "00";
	for (size_t i = 0; i < 2; i++)
	{
		str[1 - i] = "0123456789ABCDEF"[num % 16];
		num /= 16;
	}
	return str;
}
std::string hex(uint16_t num)
{
	std::string str = "0000";
	for (size_t i = 0; i < 4; i++)
	{
		str[3 - i] = "0123456789ABCDEF"[num % 16];
		num /= 16;
	}
	return str;
}