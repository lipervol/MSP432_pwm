#ifndef __OLED_H
#define __OLED_H

void GPIO_Write(uint16_t Config_GPIO_PIN,uint8_t Bit);
void I2C_Start(void);
void I2C_Stop(void);
int I2C_WaitACK(void);
void I2C_SendByte(uint8_t Data);
void OLED_WriteByte(uint8_t Data,uint8_t mode);
void OLED_SetPos(uint8_t x,uint8_t y);
void OLED_DisplayOn(void);
void OLED_DisplayOff(void);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t sizey);
void OLED_ShowString(uint8_t x,uint8_t y,uint8_t *chr,uint8_t sizey);
void OLED_ShowChinese(uint8_t x,uint8_t y,uint8_t no,uint8_t sizey);
void OLED_ShowChineseString(uint8_t x,uint8_t y,uint8_t *num,uint8_t len,uint8_t sizey,uint8_t sizex);
void OLED_DrawBMP(uint8_t x,uint8_t y,uint8_t sizex, uint8_t sizey,uint8_t BMP[]);

#endif
