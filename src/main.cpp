#include <iostream>
#include <main.hpp>
#include <string>
#include <tmx_cpp/modules/SSD1306_oled.hpp>
#include <tmx_cpp/tmx.hpp>
#include <vector>

std::vector<std::tuple<int, int, int>> i2c_ports = {
    {4, 5, 0}, {2, 3, 1},{10, 11, 1}, };

int main(int argc, char **argv) {
  // read arguments

  // if arg starts with /dev/ then use that as the port
  // if arg is a filename, then read the file and use the text in the file as
  // the text to send
  if (argc < 2) {
    std::cout << "Usage: " << argv[0]
              << " <text to send> [-p <port>] [-t <time=10s>] [-f <file>]"
              << std::endl;
    std::cout << "If no port is specified, the program will search for "
                 "available ports"
              << std::endl;
    return 1;
  }

  std::string port;
  std::string text;
  int time = 10;
  bool file_mode = false;
  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];
    if (arg == "-p" && i + 1 < argc) {
      port = argv[i + 1];
      i++;
    } else if (arg == "-t" && i + 1 < argc) {
      // Handle time argument
      try {
        time = std::stoi(argv[i + 1]);
      } catch (const std::invalid_argument &e) {
        std::cerr << "Invalid time argument: " << argv[i + 1] << std::endl;
        return 1;
      } catch (const std::out_of_range &e) {
        std::cerr << "Time argument out of range: " << argv[i + 1] << std::endl;
        return 1;
      }
      i++;
    } else if (arg == "-f" && i + 1 < argc) {
      file_mode = true;
      text = argv[i + 1];
      i++;
    } else {
      text = arg;
    }
  }

  if (port.empty()) {
    std::cout << "No port specified, searching for available ports"
              << std::endl;
    port = find_port();
    if (port.empty()) {
      std::cout << "No available ports found" << std::endl;
      return 1;
    }
  } else {
    std::cout << "Using specified port: " << port << std::endl;
  }
  auto tmx = std::make_shared<tmx_cpp::TMX>([&]() {}, port);
  std::cout << "Using port: " << port << std::endl;

  tmx->sendMessage(tmx_cpp::MESSAGE_TYPE::GET_PICO_UNIQUE_ID, {});
  tmx->setScanDelay(1000 / 10);
  std::shared_ptr<tmx_cpp::SSD1306_module> ssd1306;
  for (const auto &[sda, scl, bus] : i2c_ports) {
    std::cout << "Trying I2C pins: SDA=" << sda << ", SCL=" << scl
              << ", Bus=" << bus << std::endl;
    tmx->setI2CPins(sda, scl, bus,true);
    auto ssd1306_test =
        std::make_shared<tmx_cpp::SSD1306_module>(bus, 0x3C, 128, 64);
    tmx->module_sys->add_mod(ssd1306_test);
    if (ssd1306_test->send_text("SSD1306", 500ms)) {
      std::cout << "Connected to SSD1306 on SDA=" << sda << ", SCL=" << scl
                << ", Bus=" << bus << std::endl;
      ssd1306 = ssd1306_test;
      break;
    } else {
      std::cout << "Failed to connect to SSD1306 on SDA=" << sda
                << ", SCL=" << scl << ", Bus=" << bus << std::endl;
    }
  }
  if(!ssd1306) {
    std::cout << "Failed to connect to SSD1306 on any I2C pins" << std::endl;
    return 1;
  }

  do {
    write_text(text, file_mode, text, *tmx, ssd1306);

    //   sleep 10s
    std::this_thread::sleep_for(std::chrono::seconds(time));
  } while (file_mode);
}

std::string find_port() {
  std::vector<tmx_cpp::TMX::serial_port> ports =
      tmx_cpp::TMX::get_available_ports();
  for (const auto &port : ports) {
    if (tmx_cpp::TMX::is_accepted_port(port) &&
        tmx_cpp::TMX::check_port(port.port_name)) {
      return port.port_name;
    }
  }
  return "";
}

void write_text(std::string text, bool file_mode, std::string filename,
                tmx_cpp::TMX &tmx,
                std::shared_ptr<tmx_cpp::SSD1306_module> ssd1306) {
  if (file_mode) {
    text = read_file(filename);
  }
  std::cout << "Text to send: " << text << std::endl;
  ssd1306->send_text(text, 500ms);
}

std::string read_file(const std::string &filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Could not open file: " << filename << std::endl;
    return "";
  }
  std::string content((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
  file.close();
  return content;
}