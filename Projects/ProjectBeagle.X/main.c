/* 
 * File:   main.c
 * Author: Morris Chen
 *
 * Created on February 02, 2018, 2:32 PM
 */

#include "main.h"
#include "GLCD_PIC.h"
#include "px_ascii.h"

Operation operations[8];
int currentMachineMode = MODE_STANDBY;
static bit SEND;

/* Initialize local variables. */
unsigned char mem[3]; // Initialize array to check for triple-A sequence
unsigned char counter = 0; // Increments each time a byte is sent
unsigned char keypress; // Stores the data corresponding to the last key press
unsigned char data; // Holds the data to be sent/received


void draw_px_char(int x, int y, px_char p) {
    uint32_t m = 0b1;
    for(int i = 0; i < 32; i++) {
        if(m & p.part[0]) glcdDrawPixel(x+i%PX_CHAR_WIDTH, y+i/PX_CHAR_WIDTH, BLACK);
        if(m & p.part[1]) glcdDrawPixel(x+(i+32)%PX_CHAR_WIDTH, y+(i+32)/PX_CHAR_WIDTH, BLACK);

        m <<= 1;
    }
}

void main(void) {
    
    // <editor-fold defaultstate="collapsed" desc="Machine Configuration">
    /********************************* PIN I/O ********************************/
    /* Write outputs to LATx, read inputs from PORTx. Here, all latches (LATx)
     * are being cleared (set low) to ensure a controlled start-up state. */  
    LATA = 0x00;
    LATB = 0x00; 
    LATC = 0x00;
    LATD = 0x00;
    LATE = 0x00;

    /* After the states of LATx are known, the data direction registers, TRISx
     * are configured. 0 --> output; 1 --> input. Default is  1. */
    TRISA = 0xFF; // All inputs (this is the default, but is explicated here for learning purposes)
    TRISB = 0xFF;
    TRISC = 0x00;
    TRISD = 0x00; // All output mode on port D for the LCD
    TRISE = 0x00;
    
    /************************** A/D Converter Module **************************/
    ADCON0 = 0x00;  // Disable ADC
    ADCON1 = 0b00001111; // Set all A/D ports to digital (pg. 222)
    // </editor-fold>
    
    INT1IE = 1; // Enable RB1 (keypad data available) interrupt
    ei(); // Enable all interrupts
    
    /* Initialize LCD. */
    initLCD();
    initGLCD();
  
    /* Initialize I2C Master with 100 kHz clock. */  
    I2C_Master_Init(100000); 
    //I2C_Master_Start(); // Start condition
    
   // I2C_Master_Write(0b00010000); // 7-bit Arduino slave address + write
    //I2C_Master_Stop();
    
    //SEND = 1;
    
    glcdDrawRectangle(0, GLCD_SIZE_VERT, 0, 128, WHITE);
    print_px_string(0, 14, "HELLO WORLD!");
    
    unsigned char time[7]; // Create a byte array to hold time read from RTC
    unsigned char i; // Loop counter
    
    while (1) {
        if (currentMachineMode == MODE_STANDBY) enterStandby();
            
        while (currentMachineMode == MODE_INPUT) {
            if (getInputMode() == MODE_EMPTY_INPUT) askForOperationInput();
            else if (getInputMode() == MODE_INPUT_DESTINATION) askForDestination();
            else if (getInputMode() == MODE_INPUT_DIET) askForDietType();
            else if (getInputMode() == MODE_INPUT_DIET_NUM) askForDietNum();
            else if (getInputMode() == MODE_INPUT_PROMPT) showPrompt();
            else if (getInputMode() == MODE_SHOW_INPUT) {
                int opNum = getDisplayedOperationNum();
                Operation operation = operations[opNum];
                showInput(opNum, DIETS[operation.dietType - 1], operation.dietNum, operation.destination);
            }
        }
    }
}

void enterStandby(void) {
    setOperationNum(0);
    
    char output[16];
    char message[] = "Please load fasteners and press any button to start. ";
    int len = strlen(message);
    __lcd_clear();
    __lcd_home();
    
    __lcd_display_control(1, 0, 0);
    
    while (currentMachineMode == MODE_STANDBY) {
        for (int i = 0; i<len-16 && currentMachineMode == MODE_STANDBY; i++) {
            strncpy(output, message+i, 16);
            printf("%s", output);
            __delay_ms(150);
            if (i == 0 | i == len - 17) __delay_ms(450);
            __lcd_clear();
        }
    }
    __lcd_display_control(1, 1, 0);
}




void createOperation(void) { 
    operations[getOperationNum()].destination = getNewDestination();
    operations[getOperationNum()].dietNum = getNewDietNum();
    operations[getOperationNum()].dietType = getNewDietType();
    setOperationNum(getOperationNum() + 1);
    setOperationReady(0);
}

void interrupt interruptHandler(void){
    /* This function is mapped to the interrupt vector, and so is called when
     * any interrupt occurs to handle it. Note that if any interrupt is enabled,
     * and it does not have a handler to clear it, then unexpected behavior will
     * occur, perhaps even causing the PIC to reset.
     *
     * Arguments: none
     * 
     * Returns: none
     */
    
    if (INT1IF) {
        
        unsigned char keypress = (PORTB & 0xF0) >> 4;
        
        if (currentMachineMode == MODE_STANDBY) {
            __lcd_clear();
            __lcd_home();
            currentMachineMode = MODE_INPUT;
            setInputMode(MODE_EMPTY_INPUT);
        }
        else if (currentMachineMode == MODE_INPUT) {
            processInputInterrupt(keypress);
            if (getInputMode() == MODE_NO_INPUT) currentMachineMode = MODE_STANDBY;
            if (isOperationReady()) createOperation();
        }
        
        /*
        if(SEND){
            data = keys[keypress];
            
            I2C_Master_Start(); // Start condition
            I2C_Master_Write(0b00010000); // 7-bit Arduino slave address + write
            I2C_Master_Write(data); // Write key press data
            I2C_Master_Stop();
        }*/
        
        INT1IF = 0;
    }
    
}