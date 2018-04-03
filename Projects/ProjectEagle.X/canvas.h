/* 
 * File:   canvas.h
 * Author: morris
 *
 * Created on March 9, 2018, 4:31 PM
 */

#ifndef CANVAS_H
#define	CANVAS_H

#define CONTAINER_X 35
#define CONTAINER_Y 60
#define CONTAINER_R 32

struct {
    char header_text[22];
    char footer_text[22];
    unsigned char selected_sector;
} Canvas;

void draw_frame(void);
void draw_container(void);
void draw_table(void);
void highlight_sector(unsigned char);

void update_header(void);
void update_footer(void);
void update_sheet_data(unsigned char B, unsigned char N, unsigned char S, unsigned char W);

#endif	/* CANVAS_H */

