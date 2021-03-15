#include "nes2c02.h"
#include "Cartridge.h"
#include <iostream>


nes2c02::nes2c02()
{
	fine_x = 0x00;
	addr_latch = 0x00;
	ppu_data_buffer = 0x00;
	status_reg.reg = 0x00;
	mask_reg.reg = 0x00;
	control_reg.reg = 0x00;
	vram_addr.reg = 0x0000;
	tram_addr.reg = 0x0000;
	scanline = 0;
	cycle = 0;
	bg_next_attrib = 0;
	bg_next_id = 0;
	bg_next_pattern_high = 0;
	bg_next_pattern_low = 0;
	bg_shifter_attrib_high = 0x0000;
	bg_shifter_attrib_low = 0x0000;
	bg_shifter_pattern_high = 0x0000;
	bg_shifter_pattern_low = 0x0000;

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 1024; j++)
		{
			nameTable[i][j] = 128;
		}
	}

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 4096; j++)
		{
			patternTable[i][j] = 128;
		}
	}

	for (int i = 0; i < 32; i++)
	{
		paletteTable[i] = 0;
	}

	//fill the buffer with some color to determine its size
	screenBuffer.create(256, 240, { 0,0,0,255 });

}

void nes2c02::insertCartridge(std::shared_ptr<Cartridge> cartridge)
{
	this->cartridge = cartridge;
}

void nes2c02::cpuWrite(uint16_t addr, uint8_t data)
{
	switch (addr)
	{
	case 0x0000:
		control_reg.reg = data;
		tram_addr.x_nametable = control_reg.x_nametable;
		tram_addr.y_nametable = control_reg.y_nametable;
		break;
	case 0x0001:
		mask_reg.reg = data;
		break;
	case 0x0002:
		//can't write into status reg
		break;
	case 0x0003:
		break;
	case 0x0004:
		break;
	case 0x0005:
		if (addr_latch == 0)
		{
			fine_x = data & 0x07;
			tram_addr.x_coarse = data >> 3;
			addr_latch = 1;
		}
		else
		{
			tram_addr.fine_y = data & 0x07;
			tram_addr.y_coarse = data >> 3;
			addr_latch = 0;
		}
		break;
	case 0x0006:
		//accumulate data in tram then write to vram
		if (addr_latch == 0)
		{
			//read high first, low second
			tram_addr.reg = (uint16_t)((data & 0x3F) << 8) | (tram_addr.reg & 0x00FF);
			addr_latch = 1;
		}
		else
		{
			tram_addr.reg = (uint16_t)data | (tram_addr.reg & 0xFF00);
			vram_addr = tram_addr;
			addr_latch = 0;
		}
		break;
	case 0x0007:
		ppuWrite(vram_addr.reg, data);

		//check inc mode and inc vram
		vram_addr.reg += (control_reg.inc_mode ? 32 : 1);
		break;
	}

}

uint8_t nes2c02::cpuRead(uint16_t addr)
{
	uint8_t data = 0x00;

	switch (addr)
	{

	case 0x0000:
		//ctrl can't be read
		break;
	case 0x0001:
		//mask can't be read
		break;
	case 0x0002:
		data = (status_reg.reg & 0xE0) | (ppu_data_buffer & 0x1F);
		status_reg.vblank = 0;
		addr_latch = 0;
		break;

	case 0x0003:
		break;
	case 0x0004:
		break;
	case 0x0005:
		//scroll can't be read
		break;
	case 0x0006:
		//ppu addr can't be read
		break;
	case 0x0007:

		data = ppu_data_buffer;
		ppu_data_buffer = ppuRead(vram_addr.reg);

		//no need to delay if reading from palette range
		if (vram_addr.reg >= 0x3F00) data = ppu_data_buffer;

		//check inc mode and inc vram
		vram_addr.reg += (control_reg.inc_mode ? 32 : 1);
		break;
	}

	return data;
}

void nes2c02::ppuWrite(uint16_t addr, uint8_t data)
{
	addr &= 0x3FFF;

	if (cartridge->ppuWrite(addr, data)) {}
	else if (addr >= 0x0000 && addr <= 0x1FFF)
	{
		size_t i = (addr & 0x1000) >> 12;
		size_t j = addr & 0x0FFF;
		patternTable[i][j] = data;
	}
	else if (addr >= 0x2000 && addr <= 0x3EFF)
	{
		addr &= 0x0FFF;
		if (cartridge->mirror == Cartridge::Mirror::VERTICAL)
		{
			if (addr >= 0x0000 && addr <= 0x03FF) nameTable[0][addr & 0x03FF] = data;
			if (addr >= 0x0400 && addr <= 0x07FF) nameTable[1][addr & 0x03FF] = data;
			if (addr >= 0x0800 && addr <= 0x0BFF) nameTable[0][addr & 0x03FF] = data;
			if (addr >= 0x0C00 && addr <= 0x0FFF) nameTable[1][addr & 0x03FF] = data;
		}
		else if (cartridge->mirror == Cartridge::Mirror::HORIZONTAL)
		{
			if (addr >= 0x0000 && addr <= 0x03FF) nameTable[0][addr & 0x03FF] = data;
			if (addr >= 0x0400 && addr <= 0x07FF) nameTable[0][addr & 0x03FF] = data;
			if (addr >= 0x0800 && addr <= 0x0BFF) nameTable[1][addr & 0x03FF] = data;
			if (addr >= 0x0C00 && addr <= 0x0FFF) nameTable[1][addr & 0x03FF] = data;
		}
	}
	else if (addr >= 0x3F00 && addr <= 0x3FFF)
	{
		addr &= 0x001F;

		if (addr == 0x0010) addr = 0x0000;
		if (addr == 0x0014) addr = 0x0004;
		if (addr == 0x0018) addr = 0x0008;
		if (addr == 0x001C) addr = 0x000C;
		paletteTable[addr] = data;
	}
}

uint8_t nes2c02::ppuRead(uint16_t addr)
{
	uint8_t temp = 0x00;
	addr &= 0x3FFF;

	if (cartridge->ppuRead(addr, temp)) {}
	else if (addr >= 0x0000 && addr <= 0x1FFF)
	{
		//if cartridge can't map
		temp = patternTable[(addr & 0x1000) >> 12][addr & 0x0FFF];
	}
	else if (addr >= 0x2000 && addr <= 0x3EFF)
	{
		//check cartridge mirroring to access name table
		addr &= 0x0FFF;
		if (cartridge->mirror == Cartridge::Mirror::VERTICAL)
		{
			if (addr >= 0x0000 && addr <= 0x03FF) temp = nameTable[0][addr & 0x03FF];
			if (addr >= 0x0400 && addr <= 0x07FF) temp = nameTable[1][addr & 0x03FF];
			if (addr >= 0x0800 && addr <= 0x0BFF) temp = nameTable[0][addr & 0x03FF];
			if (addr >= 0x0C00 && addr <= 0x0FFF) temp = nameTable[1][addr & 0x03FF];
		}
		else if (cartridge->mirror == Cartridge::Mirror::HORIZONTAL)
		{
			if (addr >= 0x0000 && addr <= 0x03FF) temp = nameTable[0][addr & 0x03FF];
			if (addr >= 0x0400 && addr <= 0x07FF) temp = nameTable[0][addr & 0x03FF];
			if (addr >= 0x0800 && addr <= 0x0BFF) temp = nameTable[1][addr & 0x03FF];
			if (addr >= 0x0C00 && addr <= 0x0FFF) temp = nameTable[1][addr & 0x03FF];
		}
	}
	else if (addr >= 0x3F00 && addr <= 0x3FFF)
	{
		//use palette memory
		addr &= 0x001F;
		//addr refers to bg color
		if (addr == 0x0010) addr = 0x0000;
		if (addr == 0x0014) addr = 0x0004;
		if (addr == 0x0018) addr = 0x0008;
		if (addr == 0x001C) addr = 0x000C;
		temp = paletteTable[addr] & (mask_reg.greyscale ? 0x30 : 0x3F);
	}

	return temp;
}

//selects the right color from the palette and constructs a sf::color object
sf::Color nes2c02::getColorFromPalette(uint8_t palette, uint8_t pixel)
{
	uint8_t* selected_color = ppuPalette[ppuRead(0x3F00 + (palette << 2) + pixel) & 0x3F];
	sf::Color color_obj(selected_color[0], selected_color[1], selected_color[2], selected_color[3]);
	return color_obj;
}

sf::Image& nes2c02::getScreenBuffer()
{
	return screenBuffer;
}

void nes2c02::clock()
{
	//Lambda functions to simplify the implementation
	// auto rendering = [&]() {return mask_reg.bg_show || mask_reg.sprite_show; };

	auto IncScrollX = [&]()
	{
		if (mask_reg.bg_show || mask_reg.sprite_show)
		{
			if (vram_addr.x_coarse == 31)
			{
				vram_addr.x_coarse = 0;
				vram_addr.x_nametable = ~vram_addr.x_nametable;
			}
			else
			{
				vram_addr.x_coarse++;
			}
		}
	};

	auto IncScrollY = [&]()
	{
		if (mask_reg.bg_show || mask_reg.sprite_show)
		{
			if (vram_addr.fine_y < 7)
			{
				vram_addr.fine_y++;
			}
			else
			{
				vram_addr.fine_y = 0;
				if (vram_addr.y_coarse == 29)
				{
					vram_addr.y_coarse = 0;
					vram_addr.y_nametable = ~vram_addr.y_nametable;
				}
				else if (vram_addr.y_coarse == 31)
				{
					vram_addr.y_coarse = 0;
				}
				else
				{
					vram_addr.y_coarse++;
				}
			}
		}
	};

	auto ResetToTempAddressX = [&]()
	{
		if (mask_reg.bg_show || mask_reg.sprite_show)
		{
			vram_addr.x_coarse = tram_addr.x_coarse;
			vram_addr.x_nametable = tram_addr.x_nametable;
		}
	};

	auto ResetToTempAddressY = [&]()
	{
		if (mask_reg.bg_show || mask_reg.sprite_show)
		{
			vram_addr.y_coarse = tram_addr.y_coarse;
			vram_addr.y_nametable = tram_addr.y_nametable;
			vram_addr.fine_y = tram_addr.fine_y;
		}
	};

	auto LoadBGShifters = [&]()
	{
		//read the next byte to low byte
		bg_shifter_pattern_low = (bg_shifter_pattern_low & 0xFF00) | bg_next_pattern_low;
		bg_shifter_pattern_high = (bg_shifter_pattern_high & 0xFF00) | bg_next_pattern_high;

		//check if low and high is set
		bg_shifter_attrib_low = (bg_shifter_attrib_low & 0xFF00) | ((bg_next_attrib & 0b01) ? 0xFF : 0x00);
		bg_shifter_attrib_high = (bg_shifter_attrib_high & 0xFF00) | ((bg_next_attrib & 0b10) ? 0xFF : 0x00);
	};

	auto UpdateBGShifters = [&]()
	{
		if (mask_reg.bg_show)
		{
			bg_shifter_attrib_high <<= 1;
			bg_shifter_attrib_low <<= 1;
			bg_shifter_attrib_high <<= 1;
			bg_shifter_attrib_low <<= 1;
		}
	};

	if (scanline >= -1 && scanline < 240)
	{
		if (scanline == 0 && cycle == 0)  cycle = 1;
		if (scanline == -1 && cycle == 1) status_reg.vblank = 0;

		if ((cycle >= 2 && cycle < 258) || (cycle >= 321 && cycle < 338))
		{
			UpdateBGShifters();
			switch ((cycle - 1) % 8)
			{
			case 0:
				LoadBGShifters();
				bg_next_id = ppuRead(0x2000 | (vram_addr.reg & 0x0FFF));
				break;
			case 2:
				bg_next_attrib = ppuRead(0x23C0 | (vram_addr.y_nametable << 11)
										 | (vram_addr.x_nametable << 10)
										 | ((vram_addr.y_coarse >> 2) << 3)
										 | (vram_addr.x_coarse >> 2));
				if (vram_addr.y_coarse & 0x02) bg_next_attrib >>= 4;
				if (vram_addr.x_coarse & 0x02) bg_next_attrib >>= 2;
				bg_next_attrib &= 0x03;
				break;
			case 4:
				bg_next_pattern_low = ppuRead((control_reg.bg_patterntable << 12) + ((uint16_t)bg_next_id << 4) + (vram_addr.fine_y));
				break;
			case 6:
				bg_next_pattern_high = ppuRead((control_reg.bg_patterntable << 12) + ((uint16_t)bg_next_id << 4) + (vram_addr.fine_y) + 8);
				break;
			case 7:
				IncScrollX();
				break;
			}
		}

		if (cycle == 256) IncScrollY();

		if (cycle == 257)
		{
			LoadBGShifters();
			ResetToTempAddressX();
		}

		if (cycle == 338 || cycle == 340) bg_next_id = ppuRead(0x2000 | (vram_addr.reg & 0x0FFF));

		if (scanline == -1 && cycle >= 280 && cycle < 305) ResetToTempAddressY();
	}

	if (scanline == 240) {}

	if (scanline >= 241 && scanline < 261)
	{
		if (scanline == 241 && cycle == 1)
		{
			status_reg.vblank = 1;
			if (control_reg.generate_nmi) nmi = true;
		}
	}

	uint8_t bg_pixel = 0x00;
	uint8_t bg_palette = 0x00;

	if (mask_reg.bg_show)
	{
		uint16_t shift_select_bit = 0x8000 >> fine_x;

		uint8_t low_pixel = (bg_shifter_pattern_low & shift_select_bit) > 0;
		uint8_t high_pixel = (bg_shifter_pattern_high & shift_select_bit) > 0;
		bg_pixel = (high_pixel << 1) | low_pixel;

		uint8_t bg_low_pal = (bg_shifter_attrib_low & shift_select_bit) > 0;
		uint8_t bg_high_pal = (bg_shifter_attrib_high & shift_select_bit) > 0;
		bg_palette = (bg_high_pal << 1) | bg_low_pal;
	}

	//Draw to buffer pixel by pixel x:(cycle -1), y:scanline;
	if ((cycle >= 1) && (cycle < 257) && (scanline >= 0) && (scanline < 240))
	{
		// TODO!!
		// size_t i = (cycle - 1) / 4;
		// size_t j = scanline / 4;
		// screenBuffer.setPixel(cycle - 1, scanline, sf::Color(patternTable[0][j*32+i], 0, 0, 255));
		screenBuffer.setPixel(cycle - 1, scanline, getColorFromPalette(bg_palette, bg_pixel));
	}

	cycle++;
	if (cycle >= 341)
	{
		cycle = 0;
		scanline++;
		if (scanline >= 261)
		{
			scanline = -1;
			frame_complete = true;
		}
	}
}

void nes2c02::reset()
{
	fine_x = 0x00;
	addr_latch = 0x00;
	ppu_data_buffer = 0x00;
	status_reg.reg = 0x00;
	mask_reg.reg = 0x00;
	control_reg.reg = 0x00;
	vram_addr.reg = 0x0000;
	tram_addr.reg = 0x0000;
	scanline = 0;
	cycle = 0;
	bg_next_attrib = 0;
	bg_next_id = 0;
	bg_next_pattern_high = 0;
	bg_next_pattern_low = 0;
	bg_shifter_attrib_high = 0x0000;
	bg_shifter_attrib_low = 0x0000;
	bg_shifter_pattern_high = 0x0000;
	bg_shifter_pattern_low = 0x0000;
}