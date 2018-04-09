/* 
 * File:   RTC.c
 * Author: morris
 *
 * Created on February 24, 2018, 9:53 AM
 */

#include "RTC.h"

#include "I2C.h"

void RTC_set_time(char *setTime) {
    /* Writes the setTime array to the RTC memory.
     *
     * @param setTime - 2 digit hex encoded decimal in the format
     * {<seconds>, <minutes>, <hour>, <weekday>, <day>, <month>, <year>}
     * 
     * eg.
     * const char happynewyear[7] = {
     *  0x45, // 45 Seconds 
     *  0x59, // 59 Minutes
     *  0x23, // 24 hour mode, set to 23:00
     *  0x02, // Tues
     *  0x20, // 20th
     *  0x02, // Feb
     *  0x18  // 2018
     * };
     * 
     * Returns: none
     */
    
    I2C_Master_Start(); // Start condition
    I2C_Master_Write(0b11010000); //7 bit RTC address + Write
    I2C_Master_Write(0x00); // Set memory pointer to seconds
    
    /* Write array. */
    for(char i=0; i<7; i++) {
        I2C_Master_Write(setTime[i]);
    }
    
    I2C_Master_Stop(); //Stop condition
}

void RTC_read_time(char *time) {
    /**
     * Reads time from RTC memory
     * 
     * @param time - char array to hold the contents of the RTC
     */
    
    /* Reset RTC memory pointer. */
    I2C_Master_Start(); // Start condition
    I2C_Master_Write(0b11010000); // 7 bit RTC address + Write
    I2C_Master_Write(0x00); // Set memory pointer to seconds
    I2C_Master_Stop(); // Stop condition

    /* Read current time. */
    I2C_Master_Start(); // Start condition
    I2C_Master_Write(0b11010001); // 7 bit RTC address + Read
    for(char i = 0; i < 6; i++) {
        time[i] = I2C_Master_Read(ACK); // Read with ACK to continue reading
    }
    time[6] = I2C_Master_Read(NACK); // Final Read with NACK
    I2C_Master_Stop(); // Stop condition
}

time_t get_epoch_time(char *time) {
    struct tm t = {0};  // Initalize to all 0's
    t.tm_year = 100 + __bcd_to_num(time[6]);
    t.tm_mon = __bcd_to_num(time[5]);
    t.tm_mday = __bcd_to_num(time[4]);
    t.tm_hour = __bcd_to_num(time[2]);
    t.tm_min = __bcd_to_num(time[1]);
    t.tm_sec = __bcd_to_num(time[0]);
//    t.tm_year = 112;  // This is year-1900, so 112 = 2012
//    t.tm_mon = 8;
//    t.tm_mday = 15;
//    t.tm_hour = 21;
//    t.tm_min = 54;
//    t.tm_sec = 13;
    return mktime(&t);
}
