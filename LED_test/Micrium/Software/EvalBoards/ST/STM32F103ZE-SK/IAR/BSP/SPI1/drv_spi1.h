/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2008
 *
 *    File name   : drv_spi1.h
 *    Description : SPI1 driver include file
 *
 *    History :
 *    1. Date        : August 29, 2008
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: #3 $
 **************************************************************************/
#include "stm32f10x_lib.h"
#include "arm_comm.h"

#ifndef __DRV_SPI1_H
#define __DRV_SPI1_H

// SCP1000 sensor CSB pin - PC2
#define SCP1000_CS_PORT         GPIOC
#define SCP1000_CS_MASK         GPIO_Pin_2

// LCD controller CS pin - PC3
#define LCD_CS_PORT             GPIOC
#define LCD_CS_MASK             GPIO_Pin_3

// SPI1 pins - PA5, PA6, PA7
#define SPI1_PORT               GPIOA
#define SPI1_SCLK_MASK          GPIO_Pin_5
#define SPI1_MISO_MASK          GPIO_Pin_6
#define SPI1_MOSI_MASK          GPIO_Pin_7

#define SPI1_MOSI_H()           SPI1_PORT->BSRR = SPI1_MOSI_MASK
#define SPI1_MOSI_L()           SPI1_PORT->BRR  = SPI1_MOSI_MASK
#define SPI1_CLK_H()            SPI1_PORT->BSRR = SPI1_SCLK_MASK
#define SPI1_CLK_L()            SPI1_PORT->BRR  = SPI1_SCLK_MASK
#define SPI1_CLK_XOR()          {if(0 != (SPI1_PORT->IDR & SPI1_SCLK_MASK))SPI1_CLK_L();else SPI1_CLK_H();}
#define SPI1_MIS0()             (0 != (SPI1_PORT->IDR & SPI1_MISO_MASK))


typedef enum _SPI1_Device_t
{
  SPI1_NO_DEV = 0, SCP1000_DEV, GLCD_DEV, SPI1_LAST_DEV
} SPI1_Device_t;

/*************************************************************************
 * Function Name: SPI1_ChipSelect
 * Parameters: SPI1_Device_t _Device
 * Return: Boolean
 *          TRUE  - succeed to open SPI diver
 *          FALSE - SPI diver already opened
 *
 * Description: SPI1 Chip select and lock the driver
 *
 *************************************************************************/
Boolean  SPI1_ChipSelect  (SPI1_Device_t _Device);

/*************************************************************************
 * Function Name: SPI1_ChipDeselect
 * Parameters: SPI1_Device_t _Device
 * Return: Boolean
 *          TRUE  - Close SPI driver
 *          FALSE - Driver is already closed or opened by other device
 *
 * Description: SPI1 Chip deselect and unlock the driver
 *
 *************************************************************************/
Boolean  SPI1_ChipDeselect  (SPI1_Device_t _Device);

/*************************************************************************
 * Function Name: SPI1_SetWordWidth
 * Parameters: ISPI1_Device_t _Device, Int32U _Width
 * Return: Boolean
 *
 * Description: Set SPI1 word width
 *
 *************************************************************************/
Boolean SPI1_SetWordWidth (SPI1_Device_t _Device, Int32U _Width);

/*************************************************************************
 * Function Name: SPI1_SetClockDelay
 * Parameters: SPI1_Device_t _Device, Int32U _Delay
 * Return: none
 *
 * Description: Set SPI1 clock
 *
 *************************************************************************/
void SPI1_SetClockDelay (SPI1_Device_t _Device, Int32U _Delay);


/*************************************************************************
 * Function Name: SPI1_SetClockFreq
 * Parameters: SPI1_Device_t _Device, Boolean _CPOL, Boolean _CPHA
 * Return: none
 *
 * Description: Set SPI1 mode
 *
 *************************************************************************/
void SPI1_SetMode (SPI1_Device_t _Device, Boolean _CPOL, Boolean _CPHA);

/*************************************************************************
 * Function Name: SPI1_Init
 * Parameters: none
 * Return: none
 *
 * Description: Init SPI1
 *
 *************************************************************************/
void SPI1_Init(void);

/*************************************************************************
 * Function Name: SPI1_TranserByte
 * Parameters: Int32U _Data
 * Return: Int32U
 *
 * Description: Transfer byte by SPI1
 *
 *************************************************************************/
Int32U SPI1_TranserByte (Int32U _Data);

#endif // __DRV_SPI1_H
