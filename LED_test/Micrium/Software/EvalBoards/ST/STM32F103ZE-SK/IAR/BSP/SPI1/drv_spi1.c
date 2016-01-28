/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2008
 *
 *    File name   : drv_spi1.c
 *    Description : SPI1 diver
 *
 *    History :
 *    1. Date        : August 29, 2008
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: #3 $
 **************************************************************************/
#include <assert.h>
#include "drv_spi1.h"

typedef struct _SPI_Dev_t
{
  Int32U DataWidth;
  Int32U Delay;
  Int32U CPHA;
  Int32U CPOL;
} SPI_Dev_t;

static SPI_Dev_t _SPI_Dev[SPI1_LAST_DEV];

static Int32U _SPI1_Lock;
static SPI1_Device_t _SPI1_Device;

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
Boolean  SPI1_ChipSelect  (SPI1_Device_t _Device)
{
Boolean _Lock;
  _Lock = AtomicExchange(1,&_SPI1_Lock);
  if(  _Lock
     && _Device != _SPI1_Device)
  {
    return(FALSE);
  }
  switch (_Device)
  {
  case SCP1000_DEV:
    if(_SPI_Dev[SCP1000_DEV].CPOL)
    {
      SPI1_CLK_H();
    }
    else
    {
      SPI1_CLK_L();
    }
    GPIO_ResetBits(SCP1000_CS_PORT,SCP1000_CS_MASK);
    break;
  case GLCD_DEV:
    if(_SPI_Dev[GLCD_DEV].CPOL)
    {
      SPI1_CLK_H();
    }
    else
    {
      SPI1_CLK_L();
    }
    GPIO_ResetBits(LCD_CS_PORT,LCD_CS_MASK);
    break;
  default:
    assert(0);
  }
  _SPI1_Device = _Device;
  return(TRUE);
}

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
Boolean  SPI1_ChipDeselect  (SPI1_Device_t _Device)
{
Boolean _Lock;
  if(_SPI1_Device != _Device)
  {
    return(FALSE);
  }
  _Lock = AtomicExchange(0,&_SPI1_Lock);
  if(0 == _Lock)
  {
    return(FALSE);
  }
  switch (_Device)
  {
  case SCP1000_DEV:
    GPIO_SetBits(SCP1000_CS_PORT,SCP1000_CS_MASK);
    break;
  case GLCD_DEV:
    GPIO_SetBits(LCD_CS_PORT,LCD_CS_MASK);
    break;
  default:
    assert(0);
  }
  _SPI1_Device = SPI1_NO_DEV;
  return(TRUE);
}

/*************************************************************************
 * Function Name: SPI1_SetWordWidth
 * Parameters: SPI1_Device_t _Device, Int32U _Width
 * Return: Boolean
 *
 * Description: Set SPI1 word width
 *
 *************************************************************************/
Boolean SPI1_SetWordWidth (SPI1_Device_t _Device, Int32U _Width)
{
  if(   (8 != _Width)
     && (9 != _Width))
  {
    return(FALSE);
  }

  _SPI_Dev[_Device].DataWidth = _Width;
  return(TRUE);
}

/*************************************************************************
 * Function Name: SPI1_SetClockDelay
 * Parameters: SPI1_Device_t _Device, Int32U _Delay
 * Return: none
 *
 * Description: Set SPI1 clock
 *
 *************************************************************************/
void SPI1_SetClockDelay (SPI1_Device_t _Device, Int32U _Delay)
{
  _SPI_Dev[_Device].Delay = _Delay;
}

/*************************************************************************
 * Function Name: SPI1_SetClockFreq
 * Parameters: SPI1_Device_t _Device, Boolean _CPOL, Boolean _CPHA
 * Return: none
 *
 * Description: Set SPI1 mode
 *
 *************************************************************************/
void SPI1_SetMode (SPI1_Device_t _Device, Boolean _CPOL, Boolean _CPHA)
{
  _SPI_Dev[_Device].CPHA = _CPHA;
  _SPI_Dev[_Device].CPOL = _CPOL;
}

/*************************************************************************
 * Function Name: SPI1_Init
 * Parameters: none
 * Return: none
 *
 * Description: Init SPI1
 *
 *************************************************************************/
void SPI1_Init(void)
{
GPIO_InitTypeDef GPIO_InitStructure;
  // Enable GPIO clock and release reset
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC,
                         ENABLE);
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC,
                         DISABLE);

  // Configure LCD_CS GPIO Pin as push-pull output in high level
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Pin = LCD_CS_MASK;
  GPIO_Init(LCD_CS_PORT, &GPIO_InitStructure);
  GPIO_SetBits(LCD_CS_PORT,LCD_CS_MASK);

  // Configure SPC1000_CS GPIO Pin as push-pull output in high level
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Pin = SCP1000_CS_MASK;
  GPIO_Init(SCP1000_CS_PORT, &GPIO_InitStructure);
  GPIO_SetBits(SCP1000_CS_PORT,SCP1000_CS_MASK);

  // Configure SPI1_SCLK and SPI1_MOSI GPIOs Pins as push-pull outputs
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Pin = SPI1_SCLK_MASK | SPI1_MOSI_MASK;
  GPIO_Init(SPI1_PORT, &GPIO_InitStructure);

  // Configure SPI1_MISO GPIO Pin as input floating
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin = SPI1_MISO_MASK;
  GPIO_Init(SPI1_PORT, &GPIO_InitStructure);

  for(Int32U i = 0; i < SPI1_LAST_DEV; i++)
  {
    _SPI_Dev[i].DataWidth = 8;
    _SPI_Dev[i].Delay = 0;
    _SPI_Dev[i].CPOL = 0;
    _SPI_Dev[i].CPHA = 0;
  }
}

/*************************************************************************
 * Function Name: SPI1_TranserByte
 * Parameters: Int32U _Data
 * Return: Int32U
 *
 * Description: Transfer byte by SPI1
 *
 *************************************************************************/
Int32U SPI1_TranserByte (Int32U _Data)
{
Int32U _InData = 0;
Int32U _Mask;
Int32U _DataWidth = _SPI_Dev[_SPI1_Device].DataWidth-1;
Boolean _CPHA =  _SPI_Dev[_SPI1_Device].CPHA;

  for (_Mask = 1UL << _DataWidth; _Mask; _Mask>>= 1)
  {
    if(_CPHA)
    {
      // Set Data
      if (_Mask & _Data)
      {
        SPI1_MOSI_H();
      }
      else
      {
        SPI1_MOSI_L();
      }
      for(volatile Int32U i = _SPI_Dev[_SPI1_Device].Delay; i; i--);
      // Clock change
      SPI1_CLK_XOR();
      _InData <<= 1;
      // Get Data
      if (SPI1_MIS0())
      {
        ++_InData;
      }
      for(volatile Int32U i = _SPI_Dev[_SPI1_Device].Delay; i; i--);
      // Clock change
      SPI1_CLK_XOR();
    }
    else
    {
      // Clock change
      SPI1_CLK_XOR();
      // Set Data
      if (_Mask & _Data)
      {
        SPI1_MOSI_H();
      }
      else
      {
        SPI1_MOSI_L();
      }
      for(volatile Int32U i = _SPI_Dev[_SPI1_Device].Delay; i; i--);
      _InData <<= 1;
      // Clock change
      SPI1_CLK_XOR();
      // Clock Low
      // Get Data
      if (SPI1_MIS0())
      {
        ++_InData;
      }
      for(volatile Int32U i = _SPI_Dev[_SPI1_Device].Delay; i; i--);
    }
  }

  return(_InData);
}
