#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>

class Bus;

class nes6502
{
private:
	enum Flags
	{
		C = 1,
		Z = 2,
		I = 4,
		D = 8,
		B = 16,
		U = 32,
		V = 64,
		N = 128,
	};

	struct Instruction
	{
		std::string name;
		uint8_t(nes6502::* opcode)(void) = nullptr;
		uint8_t(nes6502::* addrmode)(void) = nullptr;
		uint8_t cycle = 0;
	};

public:
	uint8_t  reg_a = 0;
	uint8_t  reg_x = 0;
	uint8_t  reg_y = 0;
	uint8_t  sp = 0;
	uint16_t pc = 0;
	uint8_t	 status_reg = 0;
	uint8_t	 cycles = 0;

private:
	uint16_t addr_abs = 0;
	uint16_t  addr_rel = 0;
	uint8_t  fetched = 0;
	uint8_t	 opcode = 0;
	Bus* bus;
	std::vector<Instruction> instructions =
	{
		{"BRK", &nes6502::BRK, &nes6502::IMP, 7}, {"ORA", &nes6502::ORA, &nes6502::IZX, 6}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"ORA", &nes6502::ORA, &nes6502::ZP0, 3}, {"ASL", &nes6502::ASL, &nes6502::ZP0, 5}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"PHP", &nes6502::PHP, &nes6502::IMP, 3}, {"ORA", &nes6502::ORA, &nes6502::IMM, 2}, {"ASL", &nes6502::ASL, &nes6502::ACC, 2}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"ORA", &nes6502::ORA, &nes6502::ABS, 4}, {"ASL", &nes6502::ASL, &nes6502::ABS, 6}, {"???", &nes6502::XXX, &nes6502::IMP, 8},
		{"BPL", &nes6502::BPL, &nes6502::REL, 2}, {"ORA", &nes6502::ORA, &nes6502::IZY, 5}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"ORA", &nes6502::ORA, &nes6502::ZPX, 4}, {"ASL", &nes6502::ASL, &nes6502::ZPX, 6}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"CLC", &nes6502::CLC, &nes6502::IMP, 2}, {"ORA", &nes6502::ORA, &nes6502::ABY, 4}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"ORA", &nes6502::ORA, &nes6502::ABX, 4}, {"ASL", &nes6502::ASL, &nes6502::ABX, 7}, {"???", &nes6502::XXX, &nes6502::IMP, 8},
		{"JSR", &nes6502::JSR, &nes6502::ABS, 6}, {"AND", &nes6502::AND, &nes6502::IZX, 6}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"BIT", &nes6502::BIT, &nes6502::ZP0, 3}, {"AND", &nes6502::AND, &nes6502::ZP0, 3}, {"ROL", &nes6502::ROL, &nes6502::ZP0, 6}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"PLP", &nes6502::PLP, &nes6502::IMP, 4}, {"AND", &nes6502::AND, &nes6502::IMM, 2}, {"ROL", &nes6502::ROL, &nes6502::ACC, 2}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"BIT", &nes6502::BIT, &nes6502::ABS, 4}, {"AND", &nes6502::AND, &nes6502::ABS, 4}, {"ROL", &nes6502::ROL, &nes6502::ABS, 6}, {"???", &nes6502::XXX, &nes6502::IMP, 8},
		{"BMI", &nes6502::BMI, &nes6502::REL, 2}, {"AND", &nes6502::AND, &nes6502::IZY, 5}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"AND", &nes6502::AND, &nes6502::ZPX, 4}, {"ROL", &nes6502::ROL, &nes6502::ZPX, 6}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"SEC", &nes6502::SEC, &nes6502::IMP, 2}, {"AND", &nes6502::AND, &nes6502::ABY, 4}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"AND", &nes6502::AND, &nes6502::ABX, 4}, {"ROL", &nes6502::ROL, &nes6502::ABX, 7}, {"???", &nes6502::XXX, &nes6502::IMP, 8},
		{"RTI", &nes6502::RTI, &nes6502::IMP, 6}, {"EOR", &nes6502::EOR, &nes6502::IZX, 6}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"EOR", &nes6502::EOR, &nes6502::ZP0, 3}, {"LSR", &nes6502::LSR, &nes6502::ZP0, 5}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"PHA", &nes6502::PHA, &nes6502::IMP, 3}, {"EOR", &nes6502::EOR, &nes6502::IMM, 2}, {"LSR", &nes6502::LSR, &nes6502::ACC, 2}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"JMP", &nes6502::JMP, &nes6502::ABS, 3}, {"EOR", &nes6502::EOR, &nes6502::ABS, 4}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"LSR", &nes6502::LSR, &nes6502::ABS, 6},
		{"BVC", &nes6502::BVC, &nes6502::REL, 2}, {"EOR", &nes6502::EOR, &nes6502::IZY, 5}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"EOR", &nes6502::EOR, &nes6502::ZPX, 4}, {"LSR", &nes6502::LSR, &nes6502::ZPX, 6}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"CLI", &nes6502::CLI, &nes6502::IMP, 2}, {"EOR", &nes6502::EOR, &nes6502::ABY, 4}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"EOR", &nes6502::EOR, &nes6502::ABX, 4}, {"LSR", &nes6502::LSR, &nes6502::ABX, 7}, {"???", &nes6502::XXX, &nes6502::IMP, 8},
		{"RTS", &nes6502::RTS, &nes6502::IMP, 6}, {"ADC", &nes6502::ADC, &nes6502::IZX, 6}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"ADC", &nes6502::ADC, &nes6502::ZP0, 3}, {"ROR", &nes6502::ROR, &nes6502::ZP0, 5}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"PLA", &nes6502::PLA, &nes6502::IMP, 4}, {"ADC", &nes6502::ADC, &nes6502::IMM, 2}, {"ROR", &nes6502::ROR, &nes6502::ACC, 2}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"JMP", &nes6502::JMP, &nes6502::IND, 5}, {"ADC", &nes6502::ADC, &nes6502::ABS, 4}, {"ROR", &nes6502::ROR, &nes6502::ABS, 6}, {"???", &nes6502::XXX, &nes6502::IMP, 8},
		{"BVS", &nes6502::BVS, &nes6502::REL, 2}, {"ADC", &nes6502::ADC, &nes6502::IZY, 5}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"ADC", &nes6502::ADC, &nes6502::ZPX, 4}, {"ROR", &nes6502::ROR, &nes6502::ZPX, 6}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"SEI", &nes6502::SEI, &nes6502::IMP, 2}, {"ADC", &nes6502::ADC, &nes6502::ABY, 4}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"ADC", &nes6502::ADC, &nes6502::ABX, 4}, {"ROR", &nes6502::ROR, &nes6502::ABX, 7}, {"???", &nes6502::XXX, &nes6502::IMP, 8},
		{"???", &nes6502::XXX, &nes6502::IMP, 8}, {"STA", &nes6502::STA, &nes6502::IZX, 6}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"STY", &nes6502::STY, &nes6502::ZP0, 3}, {"STA", &nes6502::STA, &nes6502::ZP0, 3}, {"STX", &nes6502::STX, &nes6502::ZP0, 3}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"DEY", &nes6502::DEY, &nes6502::IMP, 2}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"TXA", &nes6502::TXA, &nes6502::IMP, 2}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"STY", &nes6502::STY, &nes6502::ABS, 4}, {"STA", &nes6502::STA, &nes6502::ABS, 4}, {"STX", &nes6502::STX, &nes6502::ABS, 4}, {"???", &nes6502::XXX, &nes6502::IMP, 8},
		{"BCC", &nes6502::BCC, &nes6502::REL, 2}, {"STA", &nes6502::STA, &nes6502::IZY, 6}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"STY", &nes6502::STY, &nes6502::ZPX, 4}, {"STA", &nes6502::STA, &nes6502::ZPX, 4}, {"STX", &nes6502::STX, &nes6502::ZPY, 5}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"TYA", &nes6502::TYA, &nes6502::IMP, 2}, {"STA", &nes6502::STA, &nes6502::ABY, 5}, {"TXS", &nes6502::TXS, &nes6502::IMP, 2}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"STA", &nes6502::STA, &nes6502::ABX, 5}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"???", &nes6502::XXX, &nes6502::IMP, 8},
		{"LDY", &nes6502::LDY, &nes6502::IMM, 2}, {"LDA", &nes6502::LDA, &nes6502::IZX, 6}, {"LDX", &nes6502::LDX, &nes6502::IMM, 2}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"LDY", &nes6502::LDY, &nes6502::ZP0, 3}, {"LDA", &nes6502::LDA, &nes6502::ZP0, 3}, {"LDX", &nes6502::LDX, &nes6502::ZP0, 3}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"TAY", &nes6502::TAY, &nes6502::IMP, 2}, {"LDA", &nes6502::LDA, &nes6502::IMM, 2}, {"TAX", &nes6502::TAX, &nes6502::IMP, 2}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"LDY", &nes6502::LDY, &nes6502::ABS, 4}, {"LDA", &nes6502::LDA, &nes6502::ABS, 4}, {"LDX", &nes6502::LDX, &nes6502::ABS, 4}, {"???", &nes6502::XXX, &nes6502::IMP, 8},
		{"BCS", &nes6502::BCS, &nes6502::REL, 2}, {"LDA", &nes6502::LDA, &nes6502::IZY, 5}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"LDY", &nes6502::LDY, &nes6502::ZPX, 4}, {"LDA", &nes6502::LDA, &nes6502::ZPX, 4}, {"LDX", &nes6502::LDX, &nes6502::ZPY, 4}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"CLV", &nes6502::CLV, &nes6502::IMP, 2}, {"LDA", &nes6502::LDA, &nes6502::ABY, 4}, {"TSX", &nes6502::TSX, &nes6502::IMP, 2}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"LDY", &nes6502::LDY, &nes6502::ABX, 4}, {"LDA", &nes6502::LDA, &nes6502::ABX, 4}, {"LDX", &nes6502::LDX, &nes6502::ABY, 4}, {"???", &nes6502::XXX, &nes6502::IMP, 8},
		{"CPY", &nes6502::CPY, &nes6502::IMM, 2}, {"CMP", &nes6502::CMP, &nes6502::IZX, 6}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"CPY", &nes6502::CPY, &nes6502::ZP0, 3}, {"CMP", &nes6502::CMP, &nes6502::ZP0, 3}, {"DEC", &nes6502::DEC, &nes6502::ZP0, 5}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"INY", &nes6502::INY, &nes6502::IMP, 2}, {"CMP", &nes6502::CMP, &nes6502::IMM, 2}, {"DEX", &nes6502::DEX, &nes6502::IMP, 2}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"CPY", &nes6502::CPY, &nes6502::ABS, 4}, {"CMP", &nes6502::CMP, &nes6502::ABS, 4}, {"DEC", &nes6502::DEC, &nes6502::ABS, 6}, {"???", &nes6502::XXX, &nes6502::IMP, 8},
		{"BNE", &nes6502::BNE, &nes6502::REL, 2}, {"CMP", &nes6502::CMP, &nes6502::IZY, 5}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"CMP", &nes6502::CMP, &nes6502::ZPX, 4}, {"DEC", &nes6502::DEC, &nes6502::ZPX, 6}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"CLD", &nes6502::CLD, &nes6502::IMP, 2}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"CMP", &nes6502::CMP, &nes6502::ABX, 4}, {"DEC", &nes6502::DEC, &nes6502::ABX, 7}, {"???", &nes6502::XXX, &nes6502::IMP, 8},
		{"CPX", &nes6502::CPX, &nes6502::IMM, 2}, {"SBC", &nes6502::SBC, &nes6502::IZX, 6}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"CPX", &nes6502::CPX, &nes6502::ZP0, 3}, {"SBC", &nes6502::SBC, &nes6502::ZP0, 3}, {"INC", &nes6502::INC, &nes6502::ZP0, 5}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"INX", &nes6502::INX, &nes6502::IMP, 2}, {"SBC", &nes6502::SBC, &nes6502::IMM, 2}, {"NOP", &nes6502::NOP, &nes6502::IMP, 2}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"CPX", &nes6502::CPX, &nes6502::ABS, 4}, {"SBC", &nes6502::SBC, &nes6502::ABS, 4}, {"INC", &nes6502::INC, &nes6502::ABS, 6}, {"???", &nes6502::XXX, &nes6502::IMP, 8},
		{"BEQ", &nes6502::BEQ, &nes6502::REL, 2}, {"SBC", &nes6502::SBC, &nes6502::IZY, 5}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"SBC", &nes6502::SBC, &nes6502::ZPX, 4}, {"INC", &nes6502::INC, &nes6502::ZPX, 6}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"SED", &nes6502::SED, &nes6502::IMP, 2}, {"SBC", &nes6502::SBC, &nes6502::ABY, 4}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"???", &nes6502::XXX, &nes6502::IMP, 8}, {"SBC", &nes6502::SBC, &nes6502::ABX, 5}, {"INC", &nes6502::INC, &nes6502::ABX, 7}, {"???", &nes6502::XXX, &nes6502::IMP, 8}
	};

	uint8_t IMM();	uint8_t IMP();
	uint8_t ZP0();	uint8_t ZPX();
	uint8_t ZPY();	uint8_t ABS();
	uint8_t ABX();	uint8_t ABY();
	uint8_t ACC();	uint8_t REL();
	uint8_t IND();	uint8_t IZX();
	uint8_t IZY();

	uint8_t ADC();	uint8_t AND();	uint8_t ASL();	uint8_t BCC();
	uint8_t BCS();	uint8_t BEQ();	uint8_t BIT();	uint8_t BMI();
	uint8_t BNE();	uint8_t BPL();	uint8_t BRK();	uint8_t BVC();
	uint8_t BVS();	uint8_t CLC();	uint8_t CLD();	uint8_t CLI();
	uint8_t CLV();	uint8_t CMP();	uint8_t CPX();	uint8_t CPY();
	uint8_t DEC();	uint8_t DEX();	uint8_t DEY();	uint8_t EOR();
	uint8_t INC();	uint8_t INX();	uint8_t INY();	uint8_t JMP();
	uint8_t JSR();	uint8_t LDA();	uint8_t LDX();	uint8_t LDY();
	uint8_t LSR();	uint8_t NOP();	uint8_t ORA();	uint8_t PHA();
	uint8_t PHP();	uint8_t PLA();	uint8_t PLP();	uint8_t ROL();
	uint8_t ROR();	uint8_t RTI();	uint8_t RTS();	uint8_t SBC();
	uint8_t SEC();	uint8_t SED();	uint8_t SEI();	uint8_t STA();
	uint8_t STX();	uint8_t STY();	uint8_t TAX();	uint8_t TAY();
	uint8_t TSX();	uint8_t TXA();	uint8_t TXS();	uint8_t TYA();
	uint8_t XXX();

	void branch(Flags flag, uint8_t condition);

public:
	nes6502(Bus* bus)
		: bus(bus)
	{}

	uint8_t fetch();

	void reset();
	void clock();
	void irq();
	void nmi();

	uint8_t read(uint16_t addr) const;
	void write(uint16_t addr, uint8_t data) const;

	uint8_t getFlag(Flags flagName);
	void setFlag(Flags flagName, uint8_t data);
	
	std::map<uint16_t, std::string> dissamble(uint16_t start, uint16_t end) const;
};

