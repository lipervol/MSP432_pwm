#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <oledfont.h>

#define Config_GPIO_Port GPIO_PORT_P5
#define Config_GPIO_SDA GPIO_PIN0
#define Config_GPIO_SCL GPIO_PIN1

uint8_t OLED_CMD=0;
uint8_t OLED_DATA=1;

void GPIO_Write(uint16_t Config_GPIO_PIN,uint8_t Bit){
    MAP_GPIO_setAsOutputPin(Config_GPIO_Port,Config_GPIO_PIN);
    if(Bit) MAP_GPIO_setOutputHighOnPin(Config_GPIO_Port,Config_GPIO_PIN);
    else MAP_GPIO_setOutputLowOnPin(Config_GPIO_Port,Config_GPIO_PIN);
}

void I2C_Start(void){
    GPIO_Write(Config_GPIO_SDA,1);
    GPIO_Write(Config_GPIO_SCL,1);
    GPIO_Write(Config_GPIO_SDA,0);
    GPIO_Write(Config_GPIO_SCL,0);
}

void I2C_Stop(void){
    GPIO_Write(Config_GPIO_SDA,0);
    GPIO_Write(Config_GPIO_SCL,1);
    GPIO_Write(Config_GPIO_SDA,1);
    GPIO_Write(Config_GPIO_SCL,0);
}

int I2C_WaitACK(void){
    uint8_t ucErrTime=0;
    GPIO_Write(Config_GPIO_SDA,1);
    GPIO_Write(Config_GPIO_SCL,1);
    MAP_GPIO_setAsInputPinWithPullUpResistor(Config_GPIO_Port,Config_GPIO_SDA);
    while(!MAP_GPIO_getInputPinValue(Config_GPIO_Port,Config_GPIO_SDA))
    {
        ucErrTime++;
        if(ucErrTime>250)
        {
            I2C_Stop();
            return 0;
        }
    }
    GPIO_Write(Config_GPIO_SCL,0);
    return 1;
}

void I2C_SendByte(uint8_t Data){
    uint8_t i;
    for(i=0;i<8;i++){
        GPIO_Write(Config_GPIO_SCL,0);
        if(Data&0x80){
            GPIO_Write(Config_GPIO_SDA,1);
        }
        else{
            GPIO_Write(Config_GPIO_SDA,0);
        }
        GPIO_Write(Config_GPIO_SCL,1);
        GPIO_Write(Config_GPIO_SCL,0);
        Data<<=1;
    }
}

void OLED_WriteByte(uint8_t Data,uint8_t mode){
    I2C_Start();
    I2C_SendByte(0x78);
    I2C_WaitACK();
    if(mode){
        I2C_SendByte(0x40);
    }
    else{
        I2C_SendByte(0x00);
    }
    I2C_WaitACK();
    I2C_SendByte(Data);
    I2C_WaitACK();
    I2C_Stop();
}

void OLED_SetPos(uint8_t x,uint8_t y){
    OLED_WriteByte(0xb0+y,OLED_CMD);
    OLED_WriteByte(((x&0xf0)>>4)|0x10,OLED_CMD);
    OLED_WriteByte((x&0x0f),OLED_CMD);
}

void OLED_DisplayOn(void){
    OLED_WriteByte(0x8d,OLED_CMD);
    OLED_WriteByte(0x14,OLED_CMD);
    OLED_WriteByte(0xAF,OLED_CMD);
}

void OLED_DisplayOff(void){
    OLED_WriteByte(0x8d,OLED_CMD);
    OLED_WriteByte(0x10,OLED_CMD);
    OLED_WriteByte(0xAE,OLED_CMD);
}

void OLED_Clear(void){
    uint8_t i,n;
    for(i=0;i<8;i++){
        OLED_WriteByte(0xb0+i,OLED_CMD);
        OLED_WriteByte(0x00,OLED_CMD);
        OLED_WriteByte(0x10,OLED_CMD);
        for(n=0;n<130;n++) OLED_WriteByte(0x00,OLED_DATA);
      }
}

void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t sizey){
    uint8_t c=0,sizex=sizey/2;
    uint16_t i=0,size1;
    if(sizey==8) size1=6;
    else size1=(sizey/8+((sizey%8)?1:0))*(sizey/2);
    c=chr-' ';
    OLED_SetPos(x,y);
    for(i=0;i<size1;i++){
        if(i%sizex==0&&sizey!=8) OLED_SetPos(x,y++);
        if(sizey==8) OLED_WriteByte(F6x8[c][i],OLED_DATA);
        else if(sizey==16) OLED_WriteByte(F8x16[c][i],OLED_DATA);
        else return;
    }

}

void OLED_ShowString(uint8_t x,uint8_t y,uint8_t *chr,uint8_t sizey)
{
  uint8_t j=0;
  while (chr[j]!='\0')
  {
    OLED_ShowChar(x,y,chr[j++],sizey);
    if(sizey==8)x+=6;
    else x+=sizey/2;
  }
}

void OLED_ShowChinese(uint8_t x,uint8_t y,uint8_t no,uint8_t sizey)
{
  uint16_t i,size1=(sizey/8+((sizey%8)?1:0))*sizey;
  for(i=0;i<size1;i++)
  {
    if(i%sizey==0) OLED_SetPos(x,y++);
    if(sizey==16) OLED_WriteByte(HZ16x16[no][i],OLED_DATA);
    else return;
  }
}

void OLED_ShowChineseString(uint8_t x,uint8_t y,uint8_t *num,uint8_t len,uint8_t sizey,uint8_t sizex){
    uint8_t j=0;
    while (j<len)
    {
        OLED_ShowChinese(x,y,num[j++],sizey);
        x+=sizex;
    }
}

void OLED_DrawBMP(uint8_t x,uint8_t y,uint8_t sizex, uint8_t sizey,uint8_t BMP[])
{
  uint16_t j=0;
  uint8_t i,m;
  sizey=sizey/8+((sizey%8)?1:0);
  for(i=0;i<sizey;i++)
  {
    OLED_SetPos(x,i+y);
    for(m=0;m<sizex;m++)
    {
      OLED_WriteByte(BMP[j++],OLED_DATA);
    }
  }
}

