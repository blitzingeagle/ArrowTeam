/* 
 * File:   inputHandler.h
 * Author: Daniil Orekhov
 *
 * Created on January 17, 2018, 4:01 PM
 */

#ifndef INPUTHANDLER_H
#define	INPUTHANDLER_H

#include <xc.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "configBits.h"
#include "lcd.h"
#include "enums.h"

/*********************************VARIABLES*********************************/

const char keys[] = "123A456B789C*0#D"; //char array corresponding to the keypad

int newDietType; //type of diet for the operation being created
int newDietNum; //number to repeat the diet for the operation being created
int newDestination; //destination drawer for the operation being created

int currentInputMode = MODE_NO_INPUT; //current sub-mode of the MODE_INPUT machine mode

int operationNum = 0; //number of operations created
int displayedOperationNum = 0; //index of the operation currently displayed in the MODE_SHOW_INPUT
int operationReady = 0; //boolean to tell the main that the operation is ready to be created

char input[16]; //stores the user input
char message[40]; //the message to be displayed on the LCD

/***************************GETTERS AND SETTERS***************************/

void setInputMode(int newMode) { currentInputMode = newMode; }
int getInputMode(void) { return currentInputMode; }

int getNewDietType(void) { return newDietType; }
int getNewDietNum(void) { return newDietNum; }
int getNewDestination(void) { return newDestination; }

int getOperationNum(void) { return operationNum; }
void setOperationNum(int num) { operationNum = num; }

int getDisplayedOperationNum(void) { return displayedOperationNum; }

int isOperationReady(void) { return operationReady; }
void setOperationReady(int b) { operationReady = b; }

/***************************FUNCTIONS***************************/

void displayAskForMessage(int inputMode, int hasInput) {
    /* This function displays the message to ask for input on the LCD screen. 
     * Note that there is a while loop that continues infinitely until an the 
     * interrupt handler changes the mode of the input.
     *
     * Arguments: int inputMode - input mode for the message. If it is changed 
     *                      by the interrupt handles, the while loop is exited.
     *            int hasInput - boolean used to determine if the cursor should be displayed.
     * 
     * Returns: none
     * 
     */
    
    int len = strlen(message);
    char output[16];
    memset(input,0,strlen(input));
    
    if (!hasInput) __lcd_display_control(1, 0, 0);
    while (currentInputMode == inputMode){
        for (int i = 0; i<len-16 && getInputMode() == inputMode; i++) {
            strncpy(output, message+i, 16);
            printf("%s", output);
            __lcd_newline();
            printf("%s", input);
            __delay_ms(150);
            if (i == 0 | i == len - 17) __delay_ms(450);
            __lcd_clear();
        }
    }
    if (!hasInput) __lcd_display_control(1, 1, 0);
}

void askForOperationInput(void) {
    sprintf(message, "Press * to create a new operation. ");
    displayAskForMessage(MODE_EMPTY_INPUT, 0);
}

void askForDestination(void) {
    sprintf(message, "Operation %d: Enter the destination drawer. ", operationNum+1);
    displayAskForMessage(MODE_INPUT_DESTINATION, 1);
}

void askForDietType(void) {
    sprintf(message, "Operation %d: Enter the diet type index. ", operationNum+1);
    displayAskForMessage(MODE_INPUT_DIET, 1);
}

void askForDietNum(void) {
    sprintf(message, "Operation %d: Enter the diet quantity. ", operationNum+1);
    displayAskForMessage(MODE_INPUT_DIET_NUM, 1);
}

void showPrompt(void) {
    sprintf(message, "Press * to create a new operation. Press # to view created operations. ");
    displayAskForMessage(MODE_INPUT_PROMPT, 0);
}

void showInput(int num, int diet, int dietN, int dest) {
    __lcd_display_control(1, 0, 0);
    __lcd_clear();
    __lcd_home();
    while (num == displayedOperationNum && currentInputMode == MODE_SHOW_INPUT) { 
        printf("Operation %d", num+1);
        __lcd_newline();
        printf("%sx%d into %d", diet, dietN, dest);
        __delay_ms(500);
        __lcd_clear();
    }
    __lcd_display_control(1, 1, 0);
}

int dietNeedsNum(void) {
    return newDietType == R || newDietType == F || newDietType == RF 
            || newDietType == RL || newDietType == FL || newDietType == L;
}

int isProperInput(void) {
    if (currentInputMode == MODE_INPUT_DIET) 
        return (atoi(input) >= 1 && atoi(input) <= 18);
    else if (currentInputMode == MODE_INPUT_DIET_NUM) {
        if (newDietType == R || newDietType == F || newDietType == RF 
                || newDietType == RL || newDietType == FL)
            return (atoi(input) >= 1 && atoi(input) <= 2);
        else if (newDietType == L) return (atoi(input) >= 1 && atoi(input) <= 3);
        else return atoi(input) == 1;
    }
    else if (currentInputMode == MODE_INPUT_DESTINATION) 
        return (atoi(input) <= 16 && atoi(input) >= 1);
    
    return 0;
}

int isNumberInput(int keypress) {
    if (keypress == 0 || keypress == 1 || keypress == 2 ||
            keypress == 4 || keypress == 5 || keypress == 6 ||
            keypress == 8 || keypress == 9 || keypress == 10 ||
            keypress == 13) return 1;
    return 0;
}

void displayInputError(void) {
    __lcd_display_control(1, 0, 0);
    __lcd_clear();
    __lcd_home();
    printf("Value must be ");
    __lcd_newline();
    if (currentInputMode == MODE_INPUT_DIET) printf("from 1 to 18.");
    else if (currentInputMode == MODE_INPUT_DIET_NUM) {
        if (newDietType == R || newDietType == F || newDietType == RF ||
                newDietType == RL || newDietType == FL) printf("from 1 to 2 ");
        else if (newDietType == L) printf("from 1 to 3 ");
    }
    else if (currentInputMode == MODE_INPUT_DESTINATION) printf("from 1 to 16.");
    
    __delay_ms(1000);
    __lcd_display_control(1, 1, 0);
    memset(input,0,strlen(input));
}

void processNumberInput(char keypress) {
    if (isNumberInput(keypress) && strlen(input) < 2) 
        input[strlen(input)] = keys[keypress];
    else if (keypress == 15) input[strlen(input)-1] = '\0';
    else if (keypress == 7) {
        if (getOperationNum() == 0) currentInputMode = MODE_EMPTY_INPUT;
        else currentInputMode = MODE_INPUT_PROMPT;
    }
    else if (keypress == 11) {
        if (isProperInput()) {
            if (currentInputMode == MODE_INPUT_DESTINATION) {
                newDestination = atoi(input);
                currentInputMode = MODE_INPUT_DIET;
            }
            else if (currentInputMode == MODE_INPUT_DIET) {
                newDietType = atoi(input);
                if (dietNeedsNum()) currentInputMode = MODE_INPUT_DIET_NUM;
                else {
                    newDietNum = 1;
                    operationReady = 1;
                    currentInputMode = MODE_INPUT_PROMPT;
                }
            }
            else if (currentInputMode == MODE_INPUT_DIET_NUM) {
                newDietNum = atoi(input);
                operationReady = 1;
                currentInputMode = MODE_INPUT_PROMPT;
            }
        } else displayInputError();
    }
}

void processInputInterrupt(char keypress) {
    if (currentInputMode == MODE_EMPTY_INPUT) {
        if (keypress == 7) currentInputMode = MODE_NO_INPUT;
        else if (keypress == 12) currentInputMode = MODE_INPUT_DESTINATION;
    }
    else if (currentInputMode == MODE_INPUT_DESTINATION 
            || currentInputMode == MODE_INPUT_DIET 
            || currentInputMode == MODE_INPUT_DIET_NUM) {
        processNumberInput(keypress);
    }
    else if (currentInputMode == MODE_INPUT_PROMPT) {
        if (keypress == 12) currentInputMode = MODE_INPUT_DESTINATION;
        else if (keypress == 14) currentInputMode = MODE_SHOW_INPUT;
    }
    else if (currentInputMode == MODE_SHOW_INPUT) {
        if (keypress == 11) 
            if (operationNum - 1 == displayedOperationNum) {
                displayedOperationNum = 0;
                currentInputMode = MODE_INPUT_PROMPT;
            }
            else displayedOperationNum++;
        else if (keypress == 7) {
            displayedOperationNum = 0;
            currentInputMode = MODE_INPUT_PROMPT;
        }
    }
}

#endif	/* INPUTHANDLER_H */