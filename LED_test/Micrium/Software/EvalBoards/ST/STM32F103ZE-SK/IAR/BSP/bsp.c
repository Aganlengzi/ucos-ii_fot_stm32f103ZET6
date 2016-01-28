/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                             (c) Copyright 2008; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                        BOARD SUPPORT PACKAGE
*
*                                     ST Microelectronics STM32
*                                              on the 
*                                 IAR STM32F103ZE-SK Evaluation Board
*
* Filename      : bsp.c
* Version       : V1.00
* Programmer(s) : FT
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_MODULE
#include <bsp.h>


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/
                                                                /* ---------------------- GPIOA Pins ---------------------- */
#define  BSP_GPIOA_PB_WAKEUP             DEF_BIT_00
                                                                /* ---------------------- GPIOC Pins ---------------------- */
#define  BSP_GPIOC_ADC_TRIM              DEF_BIT_04             /* GPIOC 04/ADC12 IN14 Potentiometer Input                  */
                                                                /* ---------------------- GPIOF Pins ---------------------- */
#define  BSP_GPIOF_LED1                  DEF_BIT_06             /* GPIOF 06: STAT 1 LED Green                               */
#define  BSP_GPIOF_LED2                  DEF_BIT_07             /* GPIOF 07: STAT 2 LED Yellow                              */
#define  BSP_GPIOF_LED3                  DEF_BIT_08             /* GPIOF 08: STAT 3 LED Red                                 */
#define  BSP_GPIOF_LED4                  DEF_BIT_09             /* GPIOF 09: STAT 4 LED Green                               */
                                                                /* GPIOF LEDs Inputs group                                  */
#define  BSP_GPIOF_LED_GRP               BSP_GPIOF_LED1       | \
                                         BSP_GPIOF_LED2       | \
                                         BSP_GPIOF_LED3       | \
                                         BSP_GPIOF_LED4        
                                                                /* ---------------------- GPIOG Pins ---------------------- */
#define  BSP_GPIOG_PB_USER               DEF_BIT_08             /* GPIOG 08: User Push button                               */

#define  BSP_GPIOG_JOY_RIGHT             DEF_BIT_07             /* GPIOG 07: Joystick right   switch contact                */
#define  BSP_GPIOG_JOY_LEFT              DEF_BIT_12             /* GPIOG 12: Joystick left    switch contact                */
#define  BSP_GPIOG_JOY_DOWN              DEF_BIT_13             /* GPIOG 13: Joystick down    switch contact                */
#define  BSP_GPIOG_JOY_UP                DEF_BIT_14             /* GPIOG 14: Joystick up      switch contact                */
#define  BSP_GPIOG_JOY_CENTER            DEF_BIT_15             /* GPIOG 15: Joystick center  switch contact                */
                                                                /* GPIOF Joystick Inputs group                              */
#define  BSP_GPIOG_JOY_GRP              (BSP_GPIOG_JOY_RIGHT  | \
                                         BSP_GPIOG_JOY_LEFT   | \
                                         BSP_GPIOG_JOY_DOWN   | \
                                         BSP_GPIOG_JOY_UP     | \
                                         BSP_GPIOG_JOY_CENTER)     
                                                                /* GPIOF port inputs group                                  */
#define  BSP_GPIOG_IN_GRP               (BSP_GPIOG_JOY_GRP    | \
                                         BSP_GPIOG_PB_USER)

#define  BSP_RCC_TO_VAL                  0x00000FFF             /* Max Timeout for RCC register                             */

/*
*********************************************************************************************************
*                                           EXTERNAL FUNCTIONS
*********************************************************************************************************
*/

extern  void  SCP1000_IntrHook (void);

/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  BSP_ADC_Init      (void);

static  void  BSP_Joy_Init      (void);

static  void  BSP_LED_Init      (void);

static  void  BSP_PB_Init       (void);

static  void  BSP_RCC_Init      (void);

/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
**********************************************************************************************************
**********************************************************************************************************
**                                         GLOBAL FUNCTIONS
**********************************************************************************************************
**********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            BSP_Init()
*
* Description : This function should be called by your application code before you make use of any of the
*               functions found in this module.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_Init (void)
{    
    BSP_RCC_Init();                                             /* Initialize the Reset and Control (RCC) module             */

    BSP_ADC_Init();                                             /* Initialize the I/O for the ADC                            */
    
    BSP_LED_Init();                                             /* Initialize the I/Os for the LEDs                          */

    BSP_PB_Init();                                              /* Initialize the I/Os for the PBs                           */

    BSP_Joy_Init();                                             /* Initialize the I/Os for the Joystick                      */
}

/*
*********************************************************************************************************
*                                       BSP_CPU_ClkFreq()
*
* Description : This function reads CPU registers to determine the CPU clock frequency of the chip in KHz.
*
* Argument(s) : none.
*
* Return(s)   : The CPU clock frequency, in Hz.
*
* Caller(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT32U  BSP_CPU_ClkFreq (void)
{
    static  RCC_ClocksTypeDef  rcc_clocks;


    RCC_GetClocksFreq(&rcc_clocks);

    return ((CPU_INT32U)rcc_clocks.HCLK_Frequency);
}

/*
*********************************************************************************************************
*********************************************************************************************************
*                                         OS CORTEX-M3 FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         OS_CPU_SysTickClkFreq()
*
* Description : Get system tick clock frequency.
*
* Argument(s) : none.
*
* Return(s)   : Clock frequency (of system tick).
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

INT32U  OS_CPU_SysTickClkFreq (void)
{
    INT32U  freq;


    freq = BSP_CPU_ClkFreq();
    return (freq);
}

/*
************************************************************************************************************
************************************************************************************************************
**                                              ADC SERVICES
************************************************************************************************************
************************************************************************************************************
*/

/*
*********************************************************************************************************
*                                           BSP_ADC_GetStatus()
*
* Description : This function initializes the board's ADC
*
* Argument(s) : adc_ch    is the channel of the ADC to probe.
*
* Return(s)   : The numerator of the binary fraction representing the result of the latest ADC conversion.
*               This value will be a 12-bit value between 0x0000 and 0x0FFF, inclusive.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT16U  BSP_ADC_GetStatus (CPU_INT08U  adc_ch)
{
    CPU_INT16U  result;


    result = 0;

    switch (adc_ch) {
        case 1:
             result = ADC_GetConversionValue(ADC1);
             break;

        case 2:
             result = ADC_GetConversionValue(ADC2);
             break;

        case 3:
             result = ADC_GetConversionValue(ADC3);
             break;
             
        default:
             break;
    }

    return (result);
}

/*
*********************************************************************************************************
*                                         BSP_ADC_Init
*
* Description : Initializes the board's ADC
*
* Argument(s) : none
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  BSP_ADC_Init (void)
{
    ADC_InitTypeDef   adc_init;
    GPIO_InitTypeDef  gpio_init;


    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    gpio_init.GPIO_Pin  = BSP_GPIOC_ADC_TRIM;
    gpio_init.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOC, &gpio_init);

    adc_init.ADC_Mode               = ADC_Mode_Independent;
    adc_init.ADC_ScanConvMode       = DISABLE;
    adc_init.ADC_ContinuousConvMode = ENABLE;
    adc_init.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_None;
    adc_init.ADC_DataAlign          = ADC_DataAlign_Right;
    adc_init.ADC_NbrOfChannel       = 1;
    ADC_Init(ADC1, &adc_init);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 1, ADC_SampleTime_13Cycles5);
    ADC_Cmd(ADC1, ENABLE);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/*
************************************************************************************************************
************************************************************************************************************
**                                             LED SERVICES
************************************************************************************************************
************************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         BSP_LED_Init()
*
* Description : Initializes the board's LEDs.
*
* Argument(s) : none
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  BSP_LED_Init (void)
{
    GPIO_InitTypeDef  gpio_init;


    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);

    gpio_init.GPIO_Pin   = BSP_GPIOF_LED_GRP;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOF, &gpio_init);
}

/*
*********************************************************************************************************
*                                             BSP_LED_On()
*
* Description : Turns on the LEDs on the board specified by led_id parameter.
*
* Argument(s) : led_id  is the number of the LED to control
*                       0    indicates that you want ALL the LEDs to be ON
*                       1    turns ON user LED1  on the board
*                       2    turns ON user LED2  on the board
*                       3    turns ON user LED3  on the board
*                       4    turns ON user LED4  on the board
*
* Return(s)  : none.
*
* Caller(s)  : Application.
*
* Note(s)    : none.
*********************************************************************************************************
*/

void  BSP_LED_On (CPU_INT08U led_id)
{
    switch (led_id) {
        case 0:
             GPIO_SetBits(GPIOF, BSP_GPIOF_LED_GRP);
             break;

        case 1:
             GPIO_SetBits(GPIOF, BSP_GPIOF_LED1);
             break;

        case 2:
             GPIO_SetBits(GPIOF, BSP_GPIOF_LED2);
             break;

        case 3:
             GPIO_SetBits(GPIOF, BSP_GPIOF_LED3);
             break;

        case 4:
             GPIO_SetBits(GPIOF, BSP_GPIOF_LED4);
             break;

        default:
             break;
    }
}

/*
*********************************************************************************************************
*                                             BSP_LED_Off()
*
* Description : Turns off the LEDs on the board specified by led_id parameter.
*
* Argument(s) : led_id  is the number of the LED to turn OFF
*                       0    indicates that you want ALL the LEDs to be OFF
*                       1    turns OFF user LED1  on the board
*                       2    turns OFF user LED2  on the board
*                       3    turns OFF user LED3  on the board
*                       4    turns OFF user LED4  on the board

*
* Return(s)  : none
*
* Caller(s)  : Application.
*
* Note(s)    : none.

*********************************************************************************************************
*/

void  BSP_LED_Off (CPU_INT08U led_id)
{
    switch (led_id) {
        case 0:
             GPIO_ResetBits(GPIOF, BSP_GPIOF_LED_GRP);
             break;

        case 1:
             GPIO_ResetBits(GPIOF, BSP_GPIOF_LED1);
             break;

        case 2:
             GPIO_ResetBits(GPIOF, BSP_GPIOF_LED2);
             break;

        case 3:
             GPIO_ResetBits(GPIOF, BSP_GPIOF_LED3);
             break;

        case 4:
             GPIO_ResetBits(GPIOF, BSP_GPIOF_LED4);
             break;

        default:
             break;
    }
}

/*
*********************************************************************************************************
*                                             BSP_LED_Toggle()
*
* Description : Turns off the LEDs on the board specified by led_id parameter.
*
* Argument(s) : led_id  is the number of the LED to toggle.
*                       0    indicates that you want ALL the LEDs to be toggled
*                       1    toggles user LED1  on the board
*                       2    toggles user LED2  on the board
*                       3    toggles user LED3  on the board
*                       4    toggles user LED4  on the board
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_LED_Toggle (CPU_INT08U led_id)
{
    CPU_INT16U  reg_val;


    switch (led_id) {
        case 0:
             reg_val  =  GPIO_ReadOutputData(GPIOF);
             reg_val ^=  BSP_GPIOF_LED_GRP;
             reg_val &=  BSP_GPIOF_LED_GRP;
             GPIO_SetBits(GPIOF, reg_val);
             reg_val  = (~reg_val);
             reg_val &= BSP_GPIOF_LED_GRP;                          
             GPIO_ResetBits(GPIOF, reg_val);
             break;

        case 1:
             reg_val  =  GPIO_ReadOutputData(GPIOF);
             reg_val ^=  BSP_GPIOF_LED1;
             GPIO_SetBits(  GPIOF,  reg_val   & BSP_GPIOF_LED1);
             GPIO_ResetBits(GPIOF, (~reg_val) & BSP_GPIOF_LED1);
             break;

        case 2:
             reg_val  =  GPIO_ReadOutputData(GPIOF);
             reg_val ^=  BSP_GPIOF_LED2;
             GPIO_SetBits(  GPIOF,  reg_val   & BSP_GPIOF_LED2);
             GPIO_ResetBits(GPIOF, (~reg_val) & BSP_GPIOF_LED2);
             break;

        case 3:
             reg_val  =  GPIO_ReadOutputData(GPIOF);
             reg_val ^=  BSP_GPIOF_LED3;
             GPIO_SetBits(  GPIOF,  reg_val   & BSP_GPIOF_LED3);
             GPIO_ResetBits(GPIOF, (~reg_val) & BSP_GPIOF_LED3);
             break;

        case 4:
             reg_val  =  GPIO_ReadOutputData(GPIOF);
             reg_val ^=  BSP_GPIOF_LED4;
             GPIO_SetBits(  GPIOF,  reg_val   & BSP_GPIOF_LED4);
             GPIO_ResetBits(GPIOF, (~reg_val) & BSP_GPIOF_LED4);
             break;

        default:
             break;
    }
}



/*
************************************************************************************************************
************************************************************************************************************
**                                             PB SERVICES
************************************************************************************************************
************************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         BSP_PB_GetStatus()
*
* Description : Gets the status of any push button on the board.
*
* Argument(s) : pb_id       is the number of the push button to probe
*                           1    probe the PB1 'USER'   push button
*                           2    probe the PB2 'WKUP'   push button
*
* Return(s)   : The current status of the push button.
*                           DEF_TRUE    if the push button is pressed.
*                           DEF_FALSE   if the push button is not pressed.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_BOOLEAN  BSP_PB_GetStatus (CPU_INT08U  pb_id)
{
    CPU_BOOLEAN  status;
    CPU_INT32U   reg_val;


    status = DEF_FALSE;    
    
    switch (pb_id) {
        case 1:
             reg_val = GPIO_ReadInputDataBit(GPIOG, BSP_GPIOG_PB_USER);
             if (reg_val == 0) {
               status = DEF_TRUE;
             }
             break;

        case 2:
             reg_val = GPIO_ReadInputDataBit(GPIOA, BSP_GPIOA_PB_WAKEUP);
             if (reg_val != 0) {
               status = DEF_TRUE;
             }
             break;

        default:
             break;
    }

    return (status);
}

/*
*********************************************************************************************************
*                                         BSP_PB_Init()
*
* Description : Initializes the board's PB
*
* Argument(s) : none
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  BSP_PB_Init (void)
{
    GPIO_InitTypeDef  gpio_init;


    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);

    gpio_init.GPIO_Pin  = BSP_GPIOA_PB_WAKEUP;
    gpio_init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpio_init);

    gpio_init.GPIO_Pin  = BSP_GPIOG_PB_USER;
    gpio_init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOG, &gpio_init);

}

/*
************************************************************************************************************
************************************************************************************************************
**                                            JOYSTICK SERVICES
************************************************************************************************************
************************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             BSP_Joy_GetPos()
*
* Description : Get the position of the Joystick on the board.
*
* Argument(s) : none.
*
* Return(s)   : The current position of the Joystick.
*
*                           BSP_JOY_NONE     If the Joystick is not being pressed.
*                           BSP_JOY_UP       If the Joystick is toggled up.
*                           BSP_JOY_DOWN     If the Joystick is toggled down.
*                           BSP_JOY_RIGHT    If the Joystick is toggled right.
*                           BSP_JOY_LEFT     If the Joystick is toggled left.
*                           BSP_JOY_CENTER   If the Joystick is being pressed. 
*
* Caller(s)  : Application.
*
* Note(s)    : none.
*********************************************************************************************************
*/

CPU_INT08U  BSP_Joy_GetPos (void) 
{
    CPU_INT32U  reg_val;
    
    
    reg_val  =  GPIO_ReadInputData(GPIOG);
    
    if (DEF_BIT_IS_CLR(reg_val, BSP_GPIOG_JOY_GRP      )  == DEF_YES) {
        return BSP_JOY_NONE;
    }
    
    if (DEF_BIT_IS_SET(reg_val, BSP_GPIOG_JOY_UP   )  == DEF_YES) {
        return BSP_JOY_UP;
    }
    
    if (DEF_BIT_IS_SET(reg_val, BSP_GPIOG_JOY_DOWN )  == DEF_YES) {
        return BSP_JOY_DOWN;
    }

    if (DEF_BIT_IS_SET(reg_val, BSP_GPIOG_JOY_RIGHT)  == DEF_YES) {
        return BSP_JOY_RIGHT;
    }

    if (DEF_BIT_IS_SET(reg_val, BSP_GPIOG_JOY_LEFT )  == DEF_YES) {
        return BSP_JOY_LEFT;
    }
    
    if (DEF_BIT_IS_SET(reg_val, BSP_GPIOG_JOY_CENTER )  == DEF_YES) {
        return BSP_JOY_CENTER;
    }

    return BSP_JOY_NONE;
}

/*
*********************************************************************************************************
*                                             BSP_Joy_GetStatus()
*
* Description : Get the status of the Joystick at a given position.
*
* Argument(s) :   joy_pos.   Position of the Joystick.
*
*                             BSP_JOY_UP       If the Joystick is toggled up.
*                             BSP_JOY_DOWN     If the Joystick is toggled down.
*                             BSP_JOY_RIGHT    If the Joystick is toggled right.
*                             BSP_JOY_LEFT     If the Joystick is toggled left.
*                             BSP_JOY_CENTER   If the Joystick is being pressed. 
*
* Return(s)   :  The current Joystick status at a given position.
*
*                             DEF_YES          If the Joystick is     in the position specifies by joy_position
*                             DEF_NO           If the Joystick is not in the position specifies by joy_position
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT08U  BSP_Joy_GetStatus (CPU_INT08U  joy_pos) 
{
    CPU_INT08U joy_status;

    
    joy_status = BSP_Joy_GetPos();    

    if (joy_status == joy_pos)
        return DEF_YES;
    else
        return DEF_NO;
}

/*
*********************************************************************************************************
*                                         BSP_Joy_Init()
*
* Description : Initializes the Joystick's I/O.
*
* Argument(s) : none
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  BSP_Joy_Init      (void)
{
    GPIO_InitTypeDef  gpio_init;
    
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);

    gpio_init.GPIO_Pin  = BSP_GPIOG_JOY_GRP;
    gpio_init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOG, &gpio_init);

}


/*
******************************************************************************************************************************
******************************************************************************************************************************
*                                  uC/Probe Plug-In for uC/OS-II Functions
******************************************************************************************************************************
******************************************************************************************************************************
*/

/*
*********************************************************************************************************
*                                   OSProbe_TmrInit()
*
* Description : This function is called to by uC/Probe Plug-In for uC/OS-II to initialize the
*               free running timer that is used to make time measurements.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : OSProbe_Init()
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if (APP_CFG_PROBE_OS_PLUGIN_EN == DEF_ENABLED) && \
    (OS_PROBE_HOOKS_EN          == 1)
void  OSProbe_TmrInit (void)
{
    TIM_TimeBaseInitTypeDef  tim_init;


#if (OS_PROBE_TIMER_SEL == 2)

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    tim_init.TIM_Period        = 0xFFFF;
    tim_init.TIM_Prescaler     = 0x00;
    tim_init.TIM_ClockDivision = 0x0;
    tim_init.TIM_CounterMode   = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM2, &tim_init);
    TIM_SetCounter(TIM2, 0);
    TIM_PrescalerConfig(TIM2, 256, TIM_PSCReloadMode_Immediate);
    TIM_Cmd(TIM2, ENABLE);

#elif (OS_PROBE_TIMER_SEL == 4)

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    tim_init.TIM_Period        = 0xFFFF;
    tim_init.TIM_Prescaler     = 0x00;
    tim_init.TIM_ClockDivision = 0x0;
    tim_init.TIM_CounterMode   = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM4, &tim_init);
    TIM_SetCounter(TIM4, 0);
    TIM_PrescalerConfig(TIM4, 256, TIM_PSCReloadMode_Immediate);
    TIM_Cmd(TIM4, ENABLE);

#endif
}
#endif


/*
*********************************************************************************************************
*                                        OSProbe_TmrRd()
*
* Description : This function is called to read the current counts of a 32 bit free running timer.
*
* Argument(s) : none
*
* Return(s)   : The 16 or 32 bit count of the timer assuming the timer is an UP counter.
*
* Caller(s)   : OSProbe_TimeGetCycles()
*
* Note(s)     : None.
*********************************************************************************************************
*/

#if (APP_CFG_PROBE_OS_PLUGIN_EN == DEF_ENABLED) && \
    (OS_PROBE_HOOKS_EN          == 1)
CPU_INT32U  OSProbe_TmrRd (void)
{
#if (OS_PROBE_TIMER_SEL == 2)

    return ((CPU_INT32U)TIM_GetCounter(TIM2));

#elif (OS_PROBE_TIMER_SEL == 4)

    return ((CPU_INT32U)TIM_GetCounter(TIM4));

#endif
}
#endif


/*
*********************************************************************************************************
*                                         BSP_RCC_Init()
*
* Description : Initializes the RCC module. Set the FLASH memmory timing and the system clock dividers
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  BSP_RCC_Init (void) 
{    
    CPU_INT32U  rcc_to;                                          /* RCC registers timeout                                    */
        
                    
    RCC_DeInit();                                                /*  Reset the RCC clock config to the default reset state   */

    RCC_HSEConfig(RCC_HSE_ON);                                   /*  HSE Oscillator ON                                       */        
    
    
    rcc_to = BSP_RCC_TO_VAL;
    
    while ((rcc_to > 0) &&
           (RCC_WaitForHSEStartUp() != SUCCESS)) {               /* Wait until the oscilator is stable                       */
        rcc_to--;
    }
      
    FLASH_SetLatency(FLASH_Latency_2);
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);         /* Fcpu = (PLL_src * PLL_MUL) = (8 Mhz / 1) * (9) = 72Mhz   */ 
    RCC_PLLCmd(ENABLE);

    rcc_to = BSP_RCC_TO_VAL;

    while ((rcc_to > 0) &&
           (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)) {
        rcc_to--;
    }

    RCC_HCLKConfig(RCC_SYSCLK_Div1);                             /* Set system clock dividers                                */                            
    RCC_PCLK2Config(RCC_HCLK_Div1);    
    RCC_PCLK1Config(RCC_HCLK_Div2);    
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);

    FLASH_SetLatency(FLASH_Latency_2);                           /* Embedded Flash Configuration                             */
    FLASH_HalfCycleAccessCmd(FLASH_HalfCycleAccess_Disable);
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
  
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

}