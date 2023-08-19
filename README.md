# URC 2023

## How to Build and Flash

1. Change into the directory you want to build
2. Run
   ```bash
   conan build . -pr lpc4078 -s build_type=MinSizeRel -b missing -c tools.build:jobs=1
   ```
3. Change into `build/lpc4078/Debug` directory
4. Change **FILE** and **SERIALPORT** with correct values then run 
   **Windows:**
   ```bash
   nxpprog --control --binary="lpc4078_**FILE**.elf.bin" --device="COM**SERIALPORT**"
   ```
   **MAC**:
   **Linux**:

If this doesn't work then look here for updated instructions: https://libhal.github.io/2.2/

## How to Setup the Com tower

1. Connect the safe black battery to the antenna on the pole.
2. Plug in the Ryobi Battery to the arm
3. If the TP-Link_FC30 (wireless access point on the arm) isn't getting power through the Ryobi battery then we will need to plug it into a power bank instead
4. Ensure the esp is configured to connect to the tplink and your local computers server

## How to Configure your Network Settings When Connected to the TP-Link_FC30

1. Goto your network settings
2. Click on the TP-Link's network settings and edit them
3. Where it says anything about DHCP, disable this and set it to manual mode. This will let you set your static IP address
4. Set your Static IP address to 192.168.0.211
5. Set your subnet Mask to 255.255.255.0
6. Set your DNS to 192.168.0.1
7. Set your GateWay to 192.168.0.1
8. Leave routes blank for linux
9. If you need to know, the network is a /24 network which means you have an 8 bit network which means 255 (3 less then this) can be connected


## How to Start Up Local Server (Frontend and Backend)

1. Run the Following Commands
   ```bash
   cd joystick-serial-prototype-2022
   npm run start
   cd server
   npm run start
   ```
If this doesn't work then look here for updated instructions: https://github.com/SJSURoboticsTeam/joystick-serial-prototype-2022

## How to Configure Mission Control
  
1. Copy the backend URL
2. Paste it in the textbox that holds the localhost of the current port its running on
3. Click connect
