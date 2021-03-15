#pragma once

#include <string>
#include <SFML/Graphics.hpp>

#include "Bus.h"

class NesScreen
{
private:
	sf::RenderWindow window;
	Bus bus;
	std::shared_ptr<Cartridge> cart;
	std::map<uint16_t, std::string> image;
	sf::Font font;
	bool stepMode = true;

	void renderRegisters();
	void renderScreen();
	void renderCode();
	void renderNametables();
public:
	NesScreen(std::string cartridgePath);

	void init();
	bool update();
	void printImage(std::string filename);
};

