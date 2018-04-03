#include "canvas.h"

#include <math.h>

#include "GLCD_PIC.h"
#include "px_ascii.h"

void draw_frame(void) {
    glcdDrawRectangle(0, GLCD_SIZE_VERT, 0, 14, RED);
    glcdDrawRectangle(0, GLCD_SIZE_VERT, 14, 110, BLACK);
    glcdDrawRectangle(0, GLCD_SIZE_VERT, 114, 128, WHITE);
}

void draw_container(void) {
    unsigned char x = CONTAINER_X, y = CONTAINER_Y, r = CONTAINER_R;
    glcdDrawCircle(x, y, r, YELLOW);
    glcdDrawSpokes(x, y, r, WHITE);
    
    for(char i = 0; i < 8; i++) {
        float theta = 3*M_PI/2 + M_PI/8 + i*M_PI/4;
        unsigned char x1 = x + (char)round(2*r/3 * cos(theta));
        unsigned char y1 = y + (char)round(2*r/3 * sin(theta));
        overwrite_px_char(x1-2,y1-6,i+'1',GREY,BLACK);
    }
}

void draw_table(void) {
    glcdDrawRectangle(72, 117, 27, 98, GREY);
    glcdDrawRectangle(75, 120, 24, 95, WHITE);
    glcdDrawRectangle(76, 119, 25, 39, BLUE);
    glcdDrawRectangle(76, 119, 52, 53, GREY);
    glcdDrawRectangle(76, 119, 66, 67, GREY);
    glcdDrawRectangle(76, 119, 80, 81, GREY);
    overwrite_px_string(80, 26, "CMPT 1", WHITE, BLUE);
    overwrite_px_char(78, 40, 'B', BLACK, WHITE);
    overwrite_px_char(78, 54, 'N', BLACK, WHITE);
    overwrite_px_char(78, 68, 'S', BLACK, WHITE);
    overwrite_px_char(78, 82, 'W', BLACK, WHITE);
    overwrite_px_char(112, 40, '0', BLACK, WHITE);
    overwrite_px_char(112, 54, '0', BLACK, WHITE);
    overwrite_px_char(112, 68, '0', BLACK, WHITE);
    overwrite_px_char(112, 82, '0', BLACK, WHITE);
}

void highlight_sector(unsigned char sector) {
    float theta;
    unsigned char x1, y1, x2, y2;
    
    theta = 3*M_PI/2 + Canvas.selected_sector * M_PI/4;
    x1 = CONTAINER_X + (char)round(CONTAINER_R * cos(theta));
    y1 = CONTAINER_Y + (char)round(CONTAINER_R * sin(theta));
    x2 = CONTAINER_X + (char)round(CONTAINER_R * cos(theta+M_PI/4));
    y2 = CONTAINER_Y + (char)round(CONTAINER_R * sin(theta+M_PI/4));
    glcdDrawLine(CONTAINER_X, x1, CONTAINER_Y, y1, 0, WHITE);
    glcdDrawLine(CONTAINER_X, x2, CONTAINER_Y, y2, 0, WHITE);
    x1 = CONTAINER_X + (char)round(2*CONTAINER_R/3 * cos(theta+M_PI/8));
    y1 = CONTAINER_Y + (char)round(2*CONTAINER_R/3 * sin(theta+M_PI/8));
    overwrite_px_char(x1-2,y1-6,Canvas.selected_sector+'1',GREY,BLACK);
    
    theta = 3*M_PI/2 + sector * M_PI/4;
    x1 = CONTAINER_X + (char)round(CONTAINER_R * cos(theta));
    y1 = CONTAINER_Y + (char)round(CONTAINER_R * sin(theta));
    x2 = CONTAINER_X + (char)round(CONTAINER_R * cos(theta+M_PI/4));
    y2 = CONTAINER_Y + (char)round(CONTAINER_R * sin(theta+M_PI/4));
    glcdDrawLine(CONTAINER_X, x1, CONTAINER_Y, y1, 0, GREEN);
    glcdDrawLine(CONTAINER_X, x2, CONTAINER_Y, y2, 0, GREEN);
    x1 = CONTAINER_X + (char)round(2*CONTAINER_R/3 * cos(theta+M_PI/8));
    y1 = CONTAINER_Y + (char)round(2*CONTAINER_R/3 * sin(theta+M_PI/8));
    overwrite_px_char(x1-2,y1-6,sector+'1',GREEN,BLACK);
    
    Canvas.selected_sector = sector;
}

void update_header(void) {
    overwrite_px_string(1, 1, Canvas.header_text, BLACK, RED);
}

void update_footer(void) {
    overwrite_px_string(1, 115, Canvas.footer_text, BLACK, WHITE);
}

void update_sheet_data(unsigned char B, unsigned char N, unsigned char S, unsigned char W) {
    overwrite_px_char(112, 40, '0'+B, BLACK, WHITE);
    overwrite_px_char(112, 54, '0'+N, BLACK, WHITE);
    overwrite_px_char(112, 68, '0'+S, BLACK, WHITE);
    overwrite_px_char(112, 82, '0'+W, BLACK, WHITE);
}
