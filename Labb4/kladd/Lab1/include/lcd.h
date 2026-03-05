#ifndef LCD_H_
#define LCD_H_


void LCD_Init(void);
void writeChar(char ch, int pos);
void writeLong(long i);
void writeTwoDigits(long i, int pos);
void toggleSegment(int pos);


#endif