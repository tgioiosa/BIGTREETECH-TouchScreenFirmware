//TG Modified
#ifndef _LCD_H_
#define _LCD_H_

#include <stdint.h>
#include "variants.h"  // for STM32_HAS_FSMC etc...

#ifdef STM32_HAS_FSMC

  typedef struct
  {
    volatile uint16_t LCD_REG;
    volatile uint16_t LCD_RAM;
  } LCD_TypeDef;

//TG - 0x6000000 is address of FSMC Memory Bank 1 used for NOR/PSRAM it is 256MBytes in size
//     two FSMC Bank 1 registers are used to write to the LCD module's internal registers,
//     first is LCD_REG in which you write the address of the LCD module register to be written
//     second is LCD_RAM where you put the data to send to the LCD module chosen register at <LCD_REG>
//     Once you write LCD_RED and LCD_RAM, the FSMC takes care of sending to the LCD module.
//
//     Usually LCD_REG is at 0x60000000 and LCD_RAM is at 0x60020000 for Bank 1, but it seems that
//     BTT chose LCD_REG = 0x60FFFFFE and LCD_RAM = 0x61000000
#ifdef MKS_TFT35_V1_0  // different LCD base address for MKS_TFT35_V1_0
  #define LCD_BASE        ((uint32_t)(0x60000000 | 0x003fffe))   // 1111 1111 1111 1111 1111 1110
#else
  #define LCD_BASE        ((uint32_t)(0x60000000 | 0x00FFFFFE))  // 1111 1111 1111 1111 1111 1110
#endif

  #define LCD             ((LCD_TypeDef *) LCD_BASE)

  #define LCD_WR_REG(regval) do{ LCD->LCD_REG = regval; }while(0)
  #define LCD_WR_DATA(data)  do{ LCD->LCD_RAM = data; }while(0)

#else
  #error "don't support LCD-GPIO yet"
#endif

void LCD_HardwareConfig(void);
uint16_t LCD_RD_DATA(void);

#endif
