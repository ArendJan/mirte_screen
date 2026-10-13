# MIRTE screen

application to write text to a connected SSD1306 OLED screen.

## Build
```bash
mkdir build
cd build
cmake ..
make
```

## Usage
```bash
./mirte_screen "Some text to show" # default usage, will be shown for 10s
./mirte_screen -f file_to_read.txt -t 2 # read file every 2s and update screen
./mirte_screen "text to show forever" -t -1
./mirte_screen -p /dev/ttyACM0 ... # select port to use
```