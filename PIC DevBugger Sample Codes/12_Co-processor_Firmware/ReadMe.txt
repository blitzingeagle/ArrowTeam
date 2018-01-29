Boot --> The main co-processor firmware. Contains startup code that branches to the independent programs

KeypadEncoder --> Keypad encoder firmware that uses RB1, RB7:4 on the primary PIC to transmit keypress data

KeypadUART --> Broadcasts keypress data via the UART bus

USBConverter --> Bidirectional USB to serial converter that shows up as a virtual COM port on a PC
	**NOTE: This folder contains ALL the source files for this functionality.
		Thus, any changes here will be reflected in Boot and vice versa.**