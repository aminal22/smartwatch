Smartwatch System Integrated with Altera DE1 Board

### Overview
This project aims to create a smartwatch system integrated with the Altera DE1 board. The system will consist of both hardware and software components, combining sophisticated hardware design with intuitive software development.

### Hardware Phase
In the hardware phase, the focus is on designing and implementing a programmable System-on-Chip (SoC) architecture. This involves integrating various physical components such as LEDs, displays, buttons, etc., onto the Altera DE1 board. Additionally, IP blocks are integrated into the system using VHDL code generated with the Qsys System. These IP blocks include essential components like the NIOS II processor for embedded processing, PLL for clock generation, Timers for timekeeping functionality, JTAG UART for debugging, and System ID for identification purposes. Physical components on the FPGA board include an 8MB SDRAM for memory storage, red/green LEDs for visual feedback, 4 push buttons for user input, 10 switches for mode selection and configuration, and 4 7-segment displays for displaying time and stopwatch information.

### Software Development Phase
The software development phase involves programming the SoC using embedded C. This phase focuses on implementing the functionality of the smartwatch system. Specifically, the specifications include utilizing the push buttons for various functions such as reset, trigger, pause, and stop of the stopwatch, with the ability to display seconds and minutes. The switches are used for watch update and display switching between watch and stopwatch modes. The 7-segment displays are utilized for displaying time and stopwatch information, including minutes and seconds.

### Technologies Used
- Intel Quartus Prime: Used for FPGA development and synthesis.
- Altera DE1 Cyclone II: FPGA development board used for prototyping and testing.
- Qsys System: Utilized for integrating IP blocks into the SoC architecture.
- Nios II: Embedded processor used for executing software tasks and managing system operations.

By integrating sophisticated hardware design with intuitive software development, this project aims to create a functional and user-friendly smartwatch system that demonstrates the capabilities of the Altera DE1 board and showcases the potential of FPGA-based embedded systems.
