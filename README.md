# SMAUGT_M3_ICTEXPRESS
Repository for the review of ICT EXPRESS 

# Software and Hardware
- STM32CubeIDE 1.18.1 software for building source code, debugging the project, and running the project
- Terminal software (in my case, MobaXterm)
- Necleo-F207ZG (Cortex-M3) board and a USB cable 

# Testing environment
- Windows 11
  
# Steps for testing the project
1. Install STM32CubeIDE 1.18.1 or newer version

2. Clone the project shared in this repository

3. Launch the project with STM32CubeIDE. You can build the project with the Menu-Project-Build Project

4. Connect the target board with a Laptop via a USB cable
<img width="1679" height="949" alt="image" src="https://github.com/user-attachments/assets/9d5aefc3-1350-452e-9989-10e268197ead" />

5. Then, you can find which COM port is used for connecting the board and Laptop (in our case, COM4 port is used)
<img width="562" height="66" alt="image" src="https://github.com/user-attachments/assets/3b2012d7-ab1d-443e-8786-24b17e03819b" />

6. Lanch the MobaXterm and configure serial session with COM4 port and transmission speed as 115200
<img width="1127" height="821" alt="image" src="https://github.com/user-attachments/assets/7026d67b-7e39-4024-bdd6-5c4db2222d59" />

7. You can download the generated elf file to the target board with Menu-Run-Run

8. After the download is finished, the software is automatically executed and the result will be shown on the terminal
<img width="1271" height="1509" alt="image" src="https://github.com/user-attachments/assets/a8639ff0-d8b8-446e-96e2-8beb34a69269" />

# Testing information
- The test program reports the average timing of 100 executions. You can modify _TIMES_ definition in the main.c file for more executions

- The testing clock is configured as 30MHz and the source codes are build with optimization level -O3
  
- You can configure the security level of SMAUG-T in parameters.h file (SMAUG_MODE)

- You can configure which test will be executed in main.c file (_TEST_POLYMUL_ or not)

- You can configure the project setting at the project properties like below
<img width="980" height="1255" alt="image" src="https://github.com/user-attachments/assets/69b2d27a-5e46-439b-bb92-9d06612bf013" />


# The source code in this repository is released under the PolyForm Noncommercial License 1.0.0, which allows use, modification, and distribution for noncommercial purposes only; commercial use requires separate permission from Seog Chung Seo (scseo@kookmin.ac.kr)

