#include "nes6502.h"
#include "Bus.h"
#include "Common.h"

#include <sstream>

uint8_t nes6502::fetch()
{
	if (instructions[opcode].addrmode != &nes6502::IMP)
		fetched = read(addr_abs);
	return fetched;
}

void nes6502::reset()
{
	addr_abs = 0xFFFC;
	pc = (read(addr_abs + 1) << 8) | read(addr_abs);
	// pc = 0xc000; // TODO: delete it

	reg_a = 0;
	reg_x = 0;
	reg_y = 0;
	sp = 0xFD;
	status_reg = 0 | U;

	addr_abs = 0;
	addr_rel = 0;
	fetched = 0;

	cycles = 8;
}

void nes6502::clock()
{
	if (cycles == 0)
	{
		opcode = read(pc++);

		cycles = instructions[opcode].cycle;

		uint8_t cycle1 = (this->*instructions[opcode].addrmode)();
		uint8_t cycle2 = (this->*instructions[opcode].opcode)();
		cycles += cycle1 & cycle2;
	}
	cycles--;
}

void nes6502::irq()
{
	if (getFlag(I))
	{
		write(0x0100 + sp, (pc >> 8) & 0x00FF);
		sp--;
		write(0x0100 + sp, pc & 0x00FF);
		sp--;

		setFlag(B, 0);
		setFlag(U, 1);
		setFlag(I, 1);
		write(0x0100 + sp, status_reg);
		sp--;

		pc = (uint16_t)read(0xFFFE) | ((uint16_t)read(0xFFFF) << 8);

		cycles = 7;
	}
}

void nes6502::nmi()
{
	write(0x0100 + sp, (pc >> 8) & 0x00FF);
	sp--;
	write(0x0100 + sp, pc & 0x00FF);
	sp--;

	setFlag(B, 0);
	setFlag(U, 1);
	setFlag(I, 1);
	write(0x0100 + sp, status_reg);
	sp--;

	pc = (uint16_t)read(0xFFFA) | ((uint16_t)read(0xFFFB) << 8);

	cycles = 8;
}

uint8_t nes6502::read(uint16_t addr) const
{
	return bus->cpuRead(addr);
}

void nes6502::write(uint16_t addr, uint8_t data) const
{
	bus->cpuWrite(addr, data);
}

uint8_t nes6502::getFlag(Flags flagName)
{
	return (status_reg & flagName) > 0;
}

void nes6502::setFlag(Flags flagName, uint8_t data)
{
	if (data)
		status_reg |= flagName;
	else
		status_reg &= ~flagName;
}

std::map<uint16_t, std::string> nes6502::dissamble(uint16_t start, uint16_t end) const
{
	uint16_t pos = start;

	auto next = [&]() -> std::string {
		return hex(read(pos++));
	};

	std::map<uint16_t, std::string> dump;

	while (pos < end - 1)
	{
		std::stringstream ss;
		uint16_t p = pos;
		ss << "0x" << hex(pos) << ": ";

		uint8_t opcode = read(pos++);
		Instruction inst = instructions[opcode];

		ss << inst.name << "[" << hex(opcode) << "] ";

		if (inst.addrmode == &nes6502::IMM)
		{
			ss << "#$" << next();
		}
		else if (inst.addrmode == &nes6502::IMP)
		{
		}
		else if (inst.addrmode == &nes6502::ZP0)
		{
			ss << "*" << next();
		}
		else if (inst.addrmode == &nes6502::ZPX)
		{
			ss << "*" << next() << ", X";
		}
		else if (inst.addrmode == &nes6502::ZPY)
		{
			ss << "*" << next() << ", Y";
		}
		else if (inst.addrmode == &nes6502::ABS)
		{
			auto lo = next();
			ss << next() << lo;
		}
		else if (inst.addrmode == &nes6502::ABX)
		{
			auto lo = next();
			ss << next() << lo << ", X";
		}
		else if (inst.addrmode == &nes6502::ABY)
		{
			auto lo = next();
			ss << next() << lo << ", Y";
		}
		else if (inst.addrmode == &nes6502::ACC)
		{
			ss << "A";
		}
		else if (inst.addrmode == &nes6502::REL)
		{
			ss << next();
		}
		else if (inst.addrmode == &nes6502::IND)
		{
			auto lo = next();
			ss << "(" << next() << lo << ")";
		}
		else if (inst.addrmode == &nes6502::IZX)
		{
			ss << "(" << next() << ", X)";
		}
		else if (inst.addrmode == &nes6502::IZY)
		{
			ss << "(" << next() << "), Y";
		}

		dump.insert({ p, ss.str() });
	}

	return dump;
}

uint8_t nes6502::IMM()
{
	addr_abs = pc++;
	return 0;
}

uint8_t nes6502::IMP()
{
	return 0;
}

uint8_t nes6502::ZP0()
{
	addr_abs = read(pc++) & 0xFF;
	return 0;
}

uint8_t nes6502::ZPX()
{
	addr_abs = (read(pc++) + reg_x) & 0xFF;
	return 0;
}

uint8_t nes6502::ZPY()
{
	addr_abs = (read(pc++) + reg_y) & 0xFF;
	return 0;
}

uint8_t nes6502::ABS()
{
	uint16_t lo = read(pc++);
	uint16_t hi = read(pc++);
	addr_abs = (hi << 8) | lo;
	return 0;
}

uint8_t nes6502::ABX()
{
	uint16_t lo = read(pc++);
	uint16_t hi = read(pc++);
	addr_abs = ((hi << 8) | lo) + reg_x;
	return (hi << 8) != (addr_abs & 0xFF00); // Page wrapping
}

uint8_t nes6502::ABY()
{
	uint16_t lo = read(pc++);
	uint16_t hi = read(pc++);
	addr_abs = ((hi << 8) | lo) + reg_y;
	return (hi << 8) != (addr_abs & 0xFF00); // Page wrapping
}

uint8_t nes6502::ACC()
{
	fetched = reg_a;
	return 0;
}

uint8_t nes6502::REL()
{
	addr_rel = read(pc++);
	if (addr_rel & 0x80)
		addr_rel |= 0xFF00;
	return 0;
}

uint8_t nes6502::IND()
{
	uint8_t lo = read(pc++);
	uint8_t hi = read(pc++);
	uint16_t ptr = (hi << 8) | lo;
	if (lo == 0xFF)
		addr_abs = (read((ptr + 1) & 0xFF) << 8) | read(ptr);
	else
		addr_abs = read(ptr + 1) << 8 | read(ptr);
	return 0;
}

uint8_t nes6502::IZX()
{
	uint8_t arg = read(pc++);
	uint8_t ptr_lo = read((arg + reg_x) & 0xFF);
	uint8_t ptr_hi = read((arg + reg_x + 1) & 0xff);
	addr_abs = ptr_hi << 8 | ptr_lo;
	return 0;
}

uint8_t nes6502::IZY()
{
	uint8_t arg = read(pc++);
	uint8_t lo = read(arg);
	uint8_t hi = read((arg + 1) & 0xFF);
	addr_abs = (hi << 8 | lo) + reg_y;
	return (hi << 8) != (addr_abs & 0xFF00); // Page wrapping
}

uint8_t nes6502::ADC()
{
	fetch();
	uint16_t val = reg_a + fetched + getFlag(C);

	bool a = !(reg_a & 0x80);
	bool b = !(fetched & 0x80);
	bool c = !(val & 0x80);

	bool is_v = a && b && !c || !a && !b && c;

	reg_a = val;

	setFlag(C, val > 0xFF);
	setFlag(Z, (val & 0xFF) == 0);
	setFlag(V, is_v);
	setFlag(N, val & 0x80);

	return 1;
}

uint8_t nes6502::AND()
{
	fetch();

	reg_a = reg_a & fetched;

	setFlag(Z, reg_a == 0);
	setFlag(N, reg_a & 0x80);

	return 1;
}

uint8_t nes6502::ASL()
{
	fetch();
	uint16_t temp = (uint16_t)fetched << 1;
	setFlag(C, (temp & 0xFF00) > 0);
	setFlag(Z, (temp & 0x00FF) == 0);
	setFlag(N, temp & 0x0080);
	if (instructions[opcode].addrmode == &nes6502::IMP) reg_a = temp & 0x00FF;
	else write(addr_abs, temp & 0x00FF);
	return 0;
}

uint8_t nes6502::BCC()
{
	branch(C, 0);
	return 0;
}

uint8_t nes6502::BCS()
{
	branch(C, 1);
	return 0;
}

uint8_t nes6502::BEQ()
{
	branch(Z, 1);
	return 0;
}

uint8_t nes6502::BIT()
{
	fetch();
	uint16_t temp = reg_a & fetched;
	setFlag(Z, (temp & 0x00FF) == 0);
	setFlag(N, fetched & (1 << 7));
	setFlag(V, fetched & (1 << 6));
	return 0;
}

uint8_t nes6502::BMI()
{
	branch(N, 1);
	return 0;
}

uint8_t nes6502::BNE()
{
	branch(Z, 0);
	return 0;
}

uint8_t nes6502::BPL()
{
	branch(N, 0);

	return 0;
}

uint8_t nes6502::BRK()
{
	pc++;

	setFlag(I, 1);
	write(0x0100 + sp, (pc >> 8) & 0x00FF);
	sp--;
	write(0x0100 + sp, pc & 0x00FF);
	sp--;

	setFlag(B, 1);
	write(0x0100 + sp, status_reg);
	sp--;
	setFlag(B, 0);

	pc = (uint16_t)read(0xFFFE) | ((uint16_t)read(0xFFFF) << 8);

	return 0;
}

uint8_t nes6502::BVC()
{
	branch(V, 0);
	return 0;
}

uint8_t nes6502::BVS()
{
	branch(V, 1);
	return 0;
}

uint8_t nes6502::CLC()
{
	setFlag(C, 0);
	return 0;
}

uint8_t nes6502::CLD()
{
	setFlag(D, 0);
	return 0;
}

uint8_t nes6502::CLI()
{
	setFlag(I, 0);
	return 0;
}

uint8_t nes6502::CLV()
{
	setFlag(V, 0);
	return 0;
}

uint8_t nes6502::CMP()
{
	fetch();
	uint16_t val = (uint16_t)reg_a - (uint16_t)fetched;

	setFlag(C, reg_a >= fetched);
	setFlag(Z, (val & 0x00FF) == 0x0000);
	setFlag(N, val & 0x0080);

	return 1;
}

uint8_t nes6502::CPX()
{
	fetch();
	uint16_t temp = (uint16_t)reg_x - (uint16_t)fetched;
	setFlag(C, reg_x >= fetched);
	setFlag(Z, (temp & 0x00FF) == 0x0000);
	setFlag(N, temp & 0x0080);
	return 0;
}

uint8_t nes6502::CPY()
{
	fetch();
	uint16_t temp = (uint16_t)reg_y - (uint16_t)fetched;
	setFlag(C, reg_y >= fetched);
	setFlag(Z, (temp & 0x00FF) == 0x0000);
	setFlag(N, temp & 0x0080);
	return 0;
}

uint8_t nes6502::DEC()
{
	fetch();
	uint8_t val = fetched - 1;
	write(addr_abs, val);

	setFlag(Z, val == 0);
	setFlag(N, val & 0x80);

	return 0;
}

uint8_t nes6502::DEX()
{
	reg_x--;

	setFlag(Z, reg_x == 0);
	setFlag(N, reg_x & 0x80);

	return 0;
}

uint8_t nes6502::DEY()
{
	reg_y--;
	setFlag(Z, reg_y == 0x00);
	setFlag(N, reg_y & 0x80);
	return 0;
}

uint8_t nes6502::EOR()
{
	fetch();
	reg_a = reg_a ^ fetched;
	setFlag(Z, reg_a == 0x00);
	setFlag(N, reg_a & 0x80);
	return 1;
}

uint8_t nes6502::INC()
{
	fetch();
	uint8_t val = fetched + 1;
	write(addr_abs, val);

	setFlag(Z, val == 0);
	setFlag(N, val & 0x80);

	return 0;
}

uint8_t nes6502::INX()
{
	reg_x++;

	setFlag(Z, reg_x == 0);
	setFlag(N, reg_x & 0x80);

	return 0;
}

uint8_t nes6502::INY()
{
	reg_y++;
	setFlag(Z, reg_y == 0x00);
	setFlag(N, reg_y & 0x80);
	return 0;
}

uint8_t nes6502::JMP()
{
	pc = addr_abs;
	return 0;
}

uint8_t nes6502::JSR()
{
	uint16_t pushed = pc - 1;
	write(0x0100 + sp--, (pushed >> 8) & 0xFF);
	write(0x0100 + sp--, pushed & 0xFF);
	pc = addr_abs;

	return 0;
}

uint8_t nes6502::LDA()
{
 	fetch();
	reg_a = fetched;
	setFlag(Z, reg_a == 0);
	setFlag(N, reg_a & 0x80);

	return 1;
}

uint8_t nes6502::LDX()
{
	fetch();
	reg_x = fetched;
	setFlag(Z, reg_x == 0x00);
	setFlag(N, reg_x & 0x80);
	return 1;
}

uint8_t nes6502::LDY()
{
	fetch();
	reg_y = fetched;
	setFlag(Z, reg_y == 0x00);
	setFlag(N, reg_y & 0x80);
	return 1;
}

uint8_t nes6502::LSR()
{
	fetch();
	uint8_t temp = fetched >> 1;
	if (instructions[opcode].addrmode == &nes6502::ACC)
		reg_a = temp;
	else
		write(addr_abs, temp);

	setFlag(C, fetched & 0x0001);
	setFlag(Z, (temp & 0x00FF) == 0x0000);
	setFlag(N, temp & 0x80);

	return 0;
}

uint8_t nes6502::NOP()
{
	switch (opcode) {
	case 0x1C:
	case 0x3C:
	case 0x5C:
	case 0x7C:
	case 0xDC:
	case 0xFC:
		return 1;
		break;
	}
	return 0;
}

uint8_t nes6502::ORA()
{
	fetch();
	reg_a = reg_a | fetched;
	setFlag(Z, reg_a == 0x00);
	setFlag(N, reg_a & 0x80);
	return 1;
}

uint8_t nes6502::PHA()
{
	write(0x0100 + sp, reg_a);
	sp--;
	return 0;
}

uint8_t nes6502::PHP()
{
	write(0x0100 + sp--, status_reg | B | U);
	setFlag(B, 0);
	setFlag(U, 0);
	return 0;
}

uint8_t nes6502::PLA()
{
	reg_a = read(0x0100 + ++sp);
	setFlag(Z, reg_a == 0);
	setFlag(N, reg_a & 0xFF);
	return 0;
}

uint8_t nes6502::PLP()
{
	sp++;
	status_reg = read(0x0100 + sp);
	setFlag(U, 1);
	return 0;
}

uint8_t nes6502::ROL()
{
	fetch();
	uint16_t temp = (uint16_t)fetched << 1 | getFlag(C);
	setFlag(C, temp & 0xFF00);
	setFlag(Z, (temp & 0x00FF) == 0);
	setFlag(N, temp & 0x0080);

	if (instructions[opcode].addrmode == &nes6502::IMP) reg_a = temp & 0x00FF;
	else write(addr_abs, temp & 0x00FF);

	return 0;
}

uint8_t nes6502::ROR()
{
	fetch();
	uint16_t temp = (uint16_t)(getFlag(C) << 7) | (fetched >> 1);

	if (instructions[opcode].addrmode == &nes6502::ACC)
		reg_a = temp & 0x00FF;
	else
		write(addr_abs, temp & 0x00FF);

	setFlag(C, fetched & 0x01);
	setFlag(Z, (temp & 0x00FF) == 0x00);
	setFlag(N, temp & 0x80);

	return 0;
}

uint8_t nes6502::RTI()
{
	sp++;
	status_reg = read(0x0100 + sp);
	status_reg &= ~B;
	status_reg &= ~U;

	sp++;
	pc = (uint16_t)read(0x0100 + sp);
	sp++;
	pc |= (uint16_t)read(0x0100 + sp) << 8;
	return 0;
}

uint8_t nes6502::RTS()
{
	sp++;
	pc = (uint16_t)read(0x0100 + sp);
	sp++;
	pc |= (uint16_t)read(0x0100 + sp) << 8;
	pc++;

	return 0;
}

uint8_t nes6502::SBC()
{
	fetch();
	uint16_t temp = (uint16_t)reg_a + (((uint16_t)fetched) ^ 0x00FF) + (uint16_t)getFlag(C);

	bool a = !(reg_a & 0x80);
	bool b = !(fetched & 0x80);
	bool c = !(temp & 0x80);

	uint8_t is_v = a && !b && !c || !a && b && c;

	setFlag(C, temp & 0xFF00);
	setFlag(Z, (temp & 0x00FF) == 0);
	setFlag(N, temp & 0x0080);
	setFlag(V, is_v);

	reg_a = temp & 0x00FF;

	return 1;
}

uint8_t nes6502::SEC()
{
	setFlag(C, 1);
	return 0;
}

uint8_t nes6502::SED()
{
	setFlag(D, 1);
	return 0;
}

uint8_t nes6502::SEI()
{
	setFlag(I, 1);
	return 0;
}

uint8_t nes6502::STA()
{
	write(addr_abs, reg_a);
	return 0;
}

uint8_t nes6502::STX()
{
	write(addr_abs, reg_x);
	return 0;
}

uint8_t nes6502::STY()
{
	write(addr_abs, reg_y);
	return 0;
}

uint8_t nes6502::TAX()
{
	reg_x = reg_a;
	setFlag(Z, reg_x == 0x00);
	setFlag(N, reg_x & 0x80);
	return 0;
}

uint8_t nes6502::TAY()
{
	reg_y = reg_a;
	setFlag(Z, reg_y == 0x00);
	setFlag(N, reg_y & 0x80);
	return 0;
}

uint8_t nes6502::TSX()
{
	reg_x = sp;

	setFlag(Z, reg_x == 0);
	setFlag(N, reg_x & 0x80);

	return 0;
}

uint8_t nes6502::TXA()
{
	reg_a = reg_x;

	setFlag(Z, reg_a == 0);
	setFlag(N, reg_x & 0x80);

	return 0;
}

uint8_t nes6502::TXS()
{
	sp = reg_x;
	return 0;
}

uint8_t nes6502::TYA()
{
	reg_a = reg_y;
	setFlag(Z, reg_a == 0x00);
	setFlag(N, reg_a & 0x80);
	return 0;
}

uint8_t nes6502::XXX()
{
	return 0;
}

void nes6502::branch(Flags flag, uint8_t condition)
{
	if (getFlag(flag) == condition)
	{
		addr_abs = pc + addr_rel;
		cycles++;
		if ((addr_abs & 0xFF00) != (pc & 0xFF00))
			cycles++;
		pc = addr_abs;
	}
}
