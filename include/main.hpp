#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <tmx_cpp/modules/SSD1306_oled.hpp>
#include <tmx_cpp/tmx.hpp>
#include <vector>
std::string find_port();
std::string read_file(const std::string &filename);
void write_text(std::string text, bool file_mode, std::string filename,
                tmx_cpp::TMX &tmx,
                std::shared_ptr<tmx_cpp::SSD1306_module> ssd1306);