# URC 2023

Important Commands

1. Change into `code` directory
2. Run
   ```bash
   conan build . -b missing
   ```
3. Change into `build/debug` directory
4. Change **FILE** and **SERIALPORT** with correct values then run
   ```bash
   nxpprog --control --binary="lpc4078_FILE.bin" --device="/dev/ttySERIALPORT"
   ```
