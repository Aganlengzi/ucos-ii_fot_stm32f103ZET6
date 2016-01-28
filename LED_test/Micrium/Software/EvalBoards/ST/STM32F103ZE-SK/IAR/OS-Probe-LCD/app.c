/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2007; Micrium, Inc.; Weston, FL
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
*                                            EXAMPLE CODE
*
*                                     ST Microelectronics STM32
*                                              on the 
*                                 IAR STM32F103ZE-SK Evaluation Board
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : FT
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include <includes.h>

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

#define  APP_PB_NONE                                  0        /* Push Button states                                        */
#define  APP_PB_1_PRESSED                             1
#define  APP_PB_2_PRESSED                             2
                                                               /* Application Colors 12-bits pallete                         */
#define  APP_COLOR_WHITE                          0xFFF
#define  APP_COLOR_BLACK                          0x000               
#define  APP_COLOR_BLUE                           0x00A               
#define  APP_COLOR_RED                            0xA00               
#define  APP_COLOR_GREEN                          0x0F0               
#define  APP_COLOR_YELLOW                         0xFF0
#define  APP_COLOR_ORANGE                         0xFA0

#define  APP_LCD_SCR_SIGN_ON                          1
#define  APP_LCD_SCR_OS_STAT                          2
#define  APP_LCD_SCR_TASK_INFO                        3
#define  APP_LCD_SCR_BOARD_INFO                       4

#define  APP_LCD_SCR_FIRST                  APP_LCD_SCR_SIGN_ON
#define  APP_LCD_SCR_LAST                   APP_LCD_SCR_BOARD_INFO

#define  APP_LCD_PAGE_MAX                             5        /* Max page per screen                                        */
#define  APP_LCD_PAGE_MIN                             1        /* Min page per screen                                        */

/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

                                                                /* ----------------- APPLICATION GLOBALS ------------------ */
                                                                /* -------- uC/OS-II APPLICATION TASK STACKS -------------- */
static  OS_STK         App_TaskStartStk[APP_CFG_TASK_START_STK_SIZE];
static  OS_STK         App_TaskPBStk[APP_CFG_TASK_PB_STK_SIZE];

#if (APP_CFG_LCD_EN == DEF_ENABLED)
static  OS_STK         App_TaskJoyStk[APP_CFG_TASK_JOY_STK_SIZE];
static  OS_STK         App_TaskLCDStk[APP_CFG_TASK_LCD_STK_SIZE];
#endif
                                                                /* ---------- uC/OS-II APPLICATION EVENTS ----------------- */
static  OS_EVENT      *App_StartMbox;

#if (APP_CFG_LCD_EN == DEF_ENABLED)
static  OS_EVENT      *App_LCDMbox1;
static  OS_EVENT      *App_LCDMbox2;
#endif

#if (APP_CFG_LCD_EN == DEF_ENABLED)
extern  FontType_t     Terminal_6_8_6;
extern  FontType_t     Terminal_9_12_6;
extern  FontType_t     Terminal_18_24_12;
#endif
                                                                /* -------------- uC/PROBE RELATED GLOBALS ---------------- */
#if (APP_CFG_PROBE_OS_PLUGIN_EN == DEF_ENABLED)
volatile  CPU_INT32U   App_ProbeCnts;
volatile  CPU_BOOLEAN  App_ProbeB1;
volatile  CPU_BOOLEAN  App_ProbeB2;
volatile  CPU_INT16U   App_ProbeADC;
#endif


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void            App_EventCreate              (void);
static  void            App_TaskCreate               (void);
static  void            App_TaskStart                (void       *p_arg);
static  void            App_TaskPB                   (void       *p_arg);

#if (APP_CFG_LCD_EN == DEF_ENABLED)
static  void            App_TaskLCD                  (void       *p_arg);
static  void            App_TaskJoy                  (void       *p_arg);

static  void            App_DispScr_SignOn           (void);
static  void            App_DispScr_OSStat           (void);
static  void            App_DispScr_OSTaskInfo       (CPU_INT08U  page);
static  void            App_DispScr_BoardInfo        (void);
#endif

#if (APP_CFG_PROBE_COM_EN       == DEF_ENABLED) || \
    (APP_CFG_PROBE_OS_PLUGIN_EN == DEF_ENABLED)
static  void            App_ProbeInit                (void);
#endif

#if (APP_CFG_PROBE_OS_PLUGIN_EN == DEF_ENABLED)
static  void            App_ProbeCallback            (void);
#endif

static  void            App_FormatDec                (CPU_INT08U  *s, 
                                                      CPU_INT32U   value, 
                                                      CPU_INT08U   digits);

/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : This the main standard entry point.
*
* Note(s)     : none.
*********************************************************************************************************
*/

int  main (void)
{
#if (OS_TASK_NAME_SIZE > 5)    
    CPU_INT08U  err;
#endif

    BSP_IntDisAll();                                            /* Disable all interrupts until we are ready to accept them */

    OSInit();                                                   /* Initialize "uC/OS-II, The Real-Time Kernel"              */

    OSTaskCreateExt((void (*)(void *)) App_TaskStart,           /* Create the start task                                    */
                    (void           *) 0,
                    (OS_STK         *)&App_TaskStartStk[APP_CFG_TASK_START_STK_SIZE - 1],
                    (INT8U           ) APP_CFG_TASK_START_PRIO,
                    (INT16U          ) APP_CFG_TASK_START_PRIO,
                    (OS_STK         *)&App_TaskStartStk[0],
                    (INT32U          ) APP_CFG_TASK_START_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

#if (OS_TASK_NAME_SIZE > 5)
    OSTaskNameSet(APP_CFG_TASK_START_PRIO, "Start", &err);
#endif

    OSStart();                                                  /* Start multitasking (i.e. give control to uC/OS-II)       */
}

/*
*********************************************************************************************************
*                                          App_TaskStart()
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Argument(s) : p_arg   is the argument passed to 'App_TaskStart()' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Notes       : (1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                   used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  App_TaskStart (void *p_arg)
{
    CPU_INT08U   os_err;
    CPU_INT08U   pb_state;
    CPU_INT08U   i;
    CPU_INT16U   dly;
    CPU_INT08U  *p_msg;

    
        
    (void)p_arg;                                                /* See Note #1                                              */

    BSP_Init();                                                 /* Initialize BSP functions                                 */
    OS_CPU_SysTickInit();                                       /* Initialize the SysTick.                              */

#if (OS_TASK_STAT_EN > 0)
    OSStatInit();                                               /* Determine CPU capacity                                   */
#endif

#if (APP_CFG_PROBE_COM_EN       == DEF_ENABLED) || \
    (APP_CFG_PROBE_OS_PLUGIN_EN == DEF_ENABLED)
    App_ProbeInit();    
#endif
    
    App_EventCreate();                                           /* Create application events                                */
    App_TaskCreate();                                            /* Create application tasks                                 */

    BSP_LED_Off(0);

    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.            */

        dly = (BSP_ADC_GetStatus(1) >> 3) + 10;

        p_msg = (CPU_INT08U *)(OSMboxPend(App_StartMbox,        /* Receive a message from the App_TaskPB() Task              */
                                          (OS_TICKS_PER_SEC / 3), 
                                          &os_err));

        if (os_err == OS_NO_ERR) {                              /* If no errros read the message                             */
            pb_state = (CPU_INT32U)p_msg;
            BSP_LED_Off(0);
        }
                
        switch (pb_state) {                                     /* Blinks the LEDs base on the status of the Push buttons    */
            default:
            case APP_PB_1_PRESSED:
                 for (i = 1; i <= 4; i++) {
                     BSP_LED_On(i);
                     OSTimeDlyHMSM(0, 0, 0, dly);
                     BSP_LED_Off(i);
                 }
                 break;

            case APP_PB_2_PRESSED:
                 BSP_LED_Toggle(0);
                 OSTimeDlyHMSM(0, 0, 0, dly);                 
                 break;
        }
    }
}

/*
*********************************************************************************************************
*                                      App_EventCreate
*
* Description:  Creates the application events.
*
* Argument(s) :  none.
*
* Return(s)   :  none.
*
* Caller(s)   :  App_TaskStart().
*
* Note(s)     :  none.
*********************************************************************************************************
*/

static  void  App_EventCreate (void)
{
#if (OS_EVENT_NAME_SIZE > 9)  
    CPU_INT08U  os_err;
#endif    

    App_StartMbox = OSMboxCreate((void *)0);                   /* Create MBOX for communication between App_TaskPB()         */
                                                               /* ... and App_TaskStart()                                    */
#if (OS_EVENT_NAME_SIZE > 9)    
    OSEventNameSet(App_StartMbox, (INT8U *)"Start Mbox", &os_err);
#endif    


#if(APP_CFG_LCD_EN == DEF_ENABLED)     
    App_LCDMbox1   = OSMboxCreate((void *)0);                   /* Create MBOX 1, 2 for communication between App_TaskJoy()    */
    App_LCDMbox2   = OSMboxCreate((void *)0);                   /* ... and App_TaskLCD()                                      */

#if (OS_EVENT_NAME_SIZE > 10)    
    OSEventNameSet(App_LCDMbox1, (INT8U *)"LCD Mbox 1", &os_err);
    OSEventNameSet(App_LCDMbox1, (INT8U *)"LCD Mbox 2", &os_err);
#endif    
#endif    

}
/*
*********************************************************************************************************
*                                      App_TaskCreate()
*
* Description :  This function creates the application tasks.
*
* Argument(s) :  none.
*
* Return(s)   :  none.
*
* Caller(s)   :  App_TaskStart().
*
* Note(s)     :  none.
*********************************************************************************************************
*/

static  void  App_TaskCreate (void)
{
#if (OS_TASK_NAME_SIZE > 8)        
    CPU_INT08U      err;
#endif
    
    OSTaskCreateExt((void (*)(void *)) App_TaskPB,
                    (void           *) 0,
                    (OS_STK         *)&App_TaskPBStk[APP_CFG_TASK_PB_STK_SIZE - 1],
                    (INT8U           ) APP_CFG_TASK_PB_PRIO,
                    (INT16U          ) APP_CFG_TASK_PB_PRIO,
                    (OS_STK         *)&App_TaskPBStk[0],
                    (INT32U          ) APP_CFG_TASK_PB_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

#if (OS_TASK_NAME_SIZE > 12)
    OSTaskNameSet(APP_CFG_TASK_PB_PRIO, "Push Buttons", &err);
#endif
    
#if (APP_CFG_LCD_EN == DEF_ENABLED)
    OSTaskCreateExt((void (*)(void *)) App_TaskLCD,
                    (void           *) 0,
                    (OS_STK         *)&App_TaskLCDStk[APP_CFG_TASK_LCD_STK_SIZE - 1],
                    (INT8U           ) APP_CFG_TASK_LCD_PRIO,
                    (INT16U          ) APP_CFG_TASK_LCD_PRIO,
                    (OS_STK         *)&App_TaskLCDStk[0],
                    (INT32U          ) APP_CFG_TASK_LCD_STK_SIZE,
                    (void           *) 0,
                    (INT8U           )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

#if (OS_TASK_NAME_SIZE > 8)
    OSTaskNameSet(APP_CFG_TASK_LCD_PRIO, "LCD Disp", &err);
#endif

    OSTaskCreateExt((void (*)(void *)) App_TaskJoy,
                    (void           *) 0,
                    (OS_STK         *)&App_TaskJoyStk[APP_CFG_TASK_JOY_STK_SIZE - 1],
                    (INT8U           ) APP_CFG_TASK_JOY_PRIO,
                    (INT16U          ) APP_CFG_TASK_JOY_PRIO,
                    (OS_STK         *)&App_TaskJoyStk[0],
                    (INT32U          ) APP_CFG_TASK_JOY_STK_SIZE,
                    (void           *) 0,
                    (INT8U           )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

#if (OS_TASK_NAME_SIZE > 8)
    OSTaskNameSet(APP_CFG_TASK_JOY_PRIO, "Joystick", &err);
#endif
#endif
    
}

/*
*********************************************************************************************************
*                                             App_TaskPB()
*
* Description : This task monitors the state of the push buttons and passes messages to AppTaskUserIF()
*
* Argument(s) : p_arg   is the argument passed to 'App_TaskPB()' by 'OSTaskCreate()'.
*
* Return(s)  : none.
*
* Caller(s)  : This is a task.
*
* Note(s)    : none.
*********************************************************************************************************
*/

static  void  App_TaskPB (void *p_arg)
{
    CPU_BOOLEAN  pb1_prev;
    CPU_BOOLEAN  pb1;
    CPU_BOOLEAN  pb2_prev;
    CPU_BOOLEAN  pb2;
    CPU_INT08U   msg;


    (void)p_arg;

    pb1_prev = DEF_FALSE;
    pb2_prev = DEF_FALSE;
    msg      = APP_PB_NONE;

    while (DEF_TRUE) {
        pb1      = BSP_PB_GetStatus(1);
        pb2      = BSP_PB_GetStatus(2);

        if (pb1 == DEF_TRUE && pb1_prev == DEF_FALSE) {           
            msg = APP_PB_1_PRESSED;
            OSMboxPost(App_StartMbox, (void *)msg);
        }

        if (pb2 == DEF_TRUE && pb2_prev == DEF_FALSE) {
            msg = APP_PB_2_PRESSED;
            OSMboxPost(App_StartMbox, (void *)msg);
        }

        pb1_prev = pb1;
        pb2_prev = pb2;

        OSTimeDlyHMSM(0, 0, 0, 150);
    }
}

/*
*********************************************************************************************************
*                                         App_TaskJoy()
*
* Description : Monitors the stat of the Joystick and passes messages to App_TaskLCD()
*
* Argument(s) : p_arg   is the argument passed to 'App_TaskJoy()' by 'OSTaskCreate()'.
*
* Return(s)  : none.
*
* Caller(s)  : This is a task.
*
* Note(s)    : none.
*********************************************************************************************************
*/

#if (APP_CFG_LCD_EN     == DEF_ENABLED)
static  void  App_TaskJoy (void *p_arg)
{
    CPU_BOOLEAN  joy_pos;
    CPU_BOOLEAN  joy_pos_prev;
    CPU_INT08U   lcd_scr_msg;
    CPU_INT08U   lcd_page_msg;
    

    (void)p_arg;

    joy_pos_prev = BSP_JOY_NONE;
    lcd_scr_msg  = APP_LCD_SCR_FIRST;
    lcd_page_msg = APP_LCD_PAGE_MIN;

    while (DEF_TRUE) {
        joy_pos = BSP_Joy_GetPos();

        if (joy_pos == BSP_JOY_RIGHT && joy_pos_prev != BSP_JOY_RIGHT) {           
            if (lcd_scr_msg == APP_LCD_SCR_LAST) {
                lcd_scr_msg = APP_LCD_SCR_FIRST;
            } else {
                lcd_scr_msg++;   
            }
            OSMboxPost(App_LCDMbox1, (void *)lcd_scr_msg);
        }

        if (joy_pos == BSP_JOY_LEFT && joy_pos_prev != BSP_JOY_LEFT) {           
            if (lcd_scr_msg == APP_LCD_SCR_FIRST) {
                lcd_scr_msg = APP_LCD_SCR_LAST;
            } else {
                lcd_scr_msg--;   
            }
            OSMboxPost(App_LCDMbox1, (void *)lcd_scr_msg);
        }

        if (joy_pos == BSP_JOY_UP && joy_pos_prev != BSP_JOY_UP) {           
            if (lcd_page_msg == APP_LCD_PAGE_MAX) {
                lcd_page_msg = APP_LCD_PAGE_MAX;
            } else {
                lcd_page_msg++;   
            }
            OSMboxPost(App_LCDMbox2, (void *)lcd_page_msg);
            }

        if (joy_pos == BSP_JOY_DOWN && joy_pos_prev != BSP_JOY_DOWN) {           
            if (lcd_page_msg == APP_LCD_PAGE_MIN) {
                lcd_page_msg = APP_LCD_PAGE_MIN;
            } else {
                lcd_page_msg--;   
            }
            OSMboxPost(App_LCDMbox2, (void *)lcd_page_msg);
        }
                
        joy_pos_prev = joy_pos;

        OSTimeDlyHMSM(0, 0, 0, 150);
    }
}
#endif

/*
*********************************************************************************************************
*                                         App_TaskLCD()
*
* Description : This task updates the LCD screen based on messages passed by App_TaskJoy().
*
* Arguments   : p_arg   is the argument passed to 'App_TaskLCD()' by 'OSTaskCreate()'.
*
* Return(s)  : none.
*
* Caller(s)  : This is a task.
*
* Note(s)    : none.
*********************************************************************************************************
*/

#if (APP_CFG_LCD_EN     == DEF_ENABLED)
static  void  App_TaskLCD (void *p_arg)
{
    CPU_INT08U  *p_msg;
    CPU_INT08U   os_err;
    CPU_INT32U   nstate;
    CPU_INT32U   pstate;
    CPU_INT08U   page;


    (void)p_arg;
    
    SPI1_Init();                                                /* Color LCD initialization code                             */
  
    GLCD_PowerUpInit((pInt8U)0);
    GLCD_Backlight(BACKLIGHT_ON);
    GLCD_TextSetPos(0,0);
 
    nstate   = APP_LCD_SCR_FIRST;
    pstate   = APP_LCD_SCR_FIRST;
    page     = 1;

    
    while (DEF_TRUE) {
        p_msg = (CPU_INT08U *)(OSMboxPend(App_LCDMbox1, OS_TICKS_PER_SEC / 4, &os_err));
        if (os_err == OS_NO_ERR) {
            nstate = (CPU_INT32U)p_msg;
        }

        if (nstate != pstate) {    
            printf("\f");
            pstate  = nstate;
        }

        p_msg = (CPU_INT08U *)(OSMboxPend(App_LCDMbox2, OS_TICKS_PER_SEC / 4, &os_err));
        if (os_err == OS_NO_ERR) {
            printf("\f");
            page  = (CPU_INT32U)p_msg;
        }
        
        switch (nstate) {
            case APP_LCD_SCR_BOARD_INFO:
                 App_DispScr_BoardInfo();
                 break;

            case APP_LCD_SCR_OS_STAT:
                 App_DispScr_OSStat();
                 break;

            case APP_LCD_SCR_TASK_INFO:
                 App_DispScr_OSTaskInfo(page - 1);
                 break;

            case APP_LCD_SCR_SIGN_ON:
            default:
                 App_DispScr_SignOn();            
                 break;
        }
        OSTimeDlyHMSM(0, 0, 0, 150);
    }
}
#endif

/*
*********************************************************************************************************
*                                         App_DispScr_SignOn()
*                                         App_DispScr_OSStat()
*                                       App_DispScr_OSTaskInfo()
*                                       App_DispScr_BoardInfo()
*
* Descrition:  These functions each display one of the screens used in the demonstration.
*
* Arguments :  none.
*
* Returns   :  none.
*
* Caller(s) : App_TaskLCD()
*
* Note(s)   : none
*********************************************************************************************************
*/

#if (APP_CFG_LCD_EN == DEF_ENABLED)
static  void App_DispScr_SignOn (void)
{    
    GLCD_TextSetPos(0 , 1);

    GLCD_SetFont(&Terminal_9_12_6, APP_COLOR_ORANGE, APP_COLOR_WHITE);
    printf(" Micrium uC/OS-II   \n\n\r");    
    
    GLCD_SetFont(&Terminal_9_12_6, APP_COLOR_RED, APP_COLOR_WHITE);
    printf(" STM32F103ZE-SK     \n\r");           
    printf(" Cortex-M3          \n\n\n\n\r");          
        
    GLCD_SetFont(&Terminal_6_8_6, APP_COLOR_RED, APP_COLOR_WHITE);
    printf(" OS Version: ");    
    GLCD_SetFont(&Terminal_6_8_6, APP_COLOR_BLUE, APP_COLOR_WHITE);    
    printf("%.2f \n\r", (CPU_FP32)OSVersion() / 100.0);

    GLCD_SetFont(&Terminal_6_8_6, APP_COLOR_RED, APP_COLOR_WHITE);
    printf(" Tick Rate : ");
    GLCD_SetFont(&Terminal_6_8_6, APP_COLOR_BLUE, APP_COLOR_WHITE);        
    printf("%d \n\r",(CPU_INT32U)OS_TICKS_PER_SEC); 

    GLCD_SetFont(&Terminal_6_8_6, APP_COLOR_RED, APP_COLOR_WHITE);
    printf(" IAR Ver   : ");   
    GLCD_SetFont(&Terminal_6_8_6, APP_COLOR_BLUE, APP_COLOR_WHITE);           
    printf("%d \n\r",(CPU_INT32U)(CPU_INT32U)__VER__); 
}

static  void App_DispScr_OSStat (void)
{
    
    GLCD_TextSetPos(0 , 1);

    GLCD_SetFont(&Terminal_9_12_6, APP_COLOR_ORANGE, APP_COLOR_WHITE);
    printf(" uC/OS-II Stat  \n\n\n\r");    

    GLCD_SetFont(&Terminal_6_8_6, APP_COLOR_RED, APP_COLOR_WHITE);
    printf("#Ticks   : ");        
    GLCD_SetFont(&Terminal_6_8_6, APP_COLOR_BLUE, APP_COLOR_WHITE);    
    printf("%d \n\r",(CPU_INT32U)OSTime); 
    
    GLCD_SetFont(&Terminal_6_8_6, APP_COLOR_RED, APP_COLOR_WHITE);
    printf("#CtxSw   : ");        
    GLCD_SetFont(&Terminal_6_8_6, APP_COLOR_BLUE, APP_COLOR_WHITE);    
    printf("%d \n\r",(CPU_INT32U)OSCtxSwCtr); 
    
    GLCD_SetFont(&Terminal_6_8_6, APP_COLOR_RED, APP_COLOR_WHITE);
    printf("#Tasks   : ");        
    GLCD_SetFont(&Terminal_6_8_6, APP_COLOR_BLUE, APP_COLOR_WHITE);    
    printf("%d \n\r",(CPU_INT32U)OS_MAX_TASKS); 
    
    GLCD_SetFont(&Terminal_6_8_6, APP_COLOR_RED, APP_COLOR_WHITE);
    printf("CPU Usage: ");        
    GLCD_SetFont(&Terminal_6_8_6, APP_COLOR_BLUE, APP_COLOR_WHITE);    
    printf("%d \n\r",(CPU_INT32U)OSCPUUsage);     
}

static  void App_DispScr_OSTaskInfo (CPU_INT08U page)
{
    OS_TCB     *p_tcb;
    CPU_INT32U  prio;
    CPU_INT08U  nbr_page;
    CPU_INT08U  i;
    CPU_INT08U  j;
    CPU_CHAR    nbr_str[4];

    
    GLCD_SetFont(&Terminal_6_8_6, APP_COLOR_ORANGE, APP_COLOR_WHITE);
    GLCD_TextSetPos(0 , 1);
    printf("uC/OS-II Task Info  \n\r");    

    GLCD_SetFont(&Terminal_6_8_6, APP_COLOR_BLACK, APP_COLOR_WHITE);
    printf("Prio | Task Name \n\r");        

    GLCD_SetFont(&Terminal_6_8_6, APP_COLOR_BLUE, APP_COLOR_WHITE);    
    printf("\n\r");    

    p_tcb    = &OSTCBTbl[0];
    nbr_page = ((OSTaskCtr - 1) / 6);
    i        = 0;
    j        = 0;

    if (page > nbr_page) {
        page = nbr_page;
    }
    
    while (p_tcb != NULL) {
        if (i > page * 6) {
            if (j >= 6) {
                break;
            }                    
            prio = p_tcb->OSTCBPrio;
            Str_Copy(&nbr_str[0], "xxxx |");     
            App_FormatDec(&nbr_str[0], prio, 4);
            GLCD_SetFont(&Terminal_6_8_6, APP_COLOR_BLUE, APP_COLOR_WHITE);
            printf("%s", nbr_str); 
        
            GLCD_SetFont(&Terminal_6_8_6, APP_COLOR_RED, APP_COLOR_WHITE);
            printf("%s \n\r", p_tcb->OSTCBTaskName); 
            j++;
        }
        p_tcb = p_tcb->OSTCBPrev;        
        i++;
    }

    printf("\n\r"); 
    printf("Page # %d of %d \n\n\n\r", page + 1, nbr_page + 1);                 
    
    if (nbr_page > 1)
        if (page == 0 ) {        
            printf("Up->Next");      
        } else if (page == nbr_page){
            printf("Down->Last");          
        } else {
            printf("Up->Next.  Down->Last");              
        }
}

static  void  App_DispScr_BoardInfo (void)
{  
    GLCD_TextSetPos(0 , 1);

    GLCD_SetFont(&Terminal_9_12_6, APP_COLOR_ORANGE, APP_COLOR_WHITE);
    printf(" STM32F103ZE-SK  \n\n\n\r");    
    
    GLCD_SetFont(&Terminal_6_8_6, APP_COLOR_RED, APP_COLOR_WHITE);
    printf("CPU Speed:  ");        
    GLCD_SetFont(&Terminal_6_8_6, APP_COLOR_BLUE, APP_COLOR_WHITE);    
    printf("%d Mhz \n\r", (CPU_INT32U)BSP_CPU_ClkFreq() / 1000000L);
    
    GLCD_SetFont(&Terminal_6_8_6, APP_COLOR_RED, APP_COLOR_WHITE);
    printf("ADC Value:  ");      
    GLCD_SetFont(&Terminal_6_8_6, APP_COLOR_BLUE, APP_COLOR_WHITE);    
    printf("%4d \n\r", (CPU_INT32U)BSP_ADC_GetStatus(1));
        
    GLCD_SetFont(&Terminal_6_8_6, APP_COLOR_RED, APP_COLOR_WHITE);
    printf("PB1 State:  ");        

    GLCD_SetFont(&Terminal_6_8_6, APP_COLOR_BLUE, APP_COLOR_WHITE);    

    if (BSP_PB_GetStatus(1) == DEF_TRUE) {
        printf("ON \n\r");    
    } else {
        printf("OFF \n\r");    
    }
    
    GLCD_SetFont(&Terminal_6_8_6, APP_COLOR_RED, APP_COLOR_WHITE);
    printf("PB2 State:  ");        

    GLCD_SetFont(&Terminal_6_8_6, APP_COLOR_BLUE, APP_COLOR_WHITE);    

    if (BSP_PB_GetStatus(2) == DEF_TRUE) {
        printf("ON \n\r");    
    } else {
        printf("OFF \n\r");    
    }
}
#endif

/*
*********************************************************************************************************
*                                         App_ProbeInit()
*
* Description : Initializes the uC/Probe comunnication module in the application.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : App_TaskStart()
*
* Return(s)   : none.
*********************************************************************************************************
*/
#if (APP_CFG_PROBE_COM_EN       == DEF_ENABLED) || \
    (APP_CFG_PROBE_OS_PLUGIN_EN == DEF_ENABLED)
static void  App_ProbeInit()
{
#if (APP_CFG_PROBE_OS_PLUGIN_EN == DEF_ENABLED)    
    (void)App_ProbeCnts;                                       /* Avoid compiler warning for the variables that         */
    (void)App_ProbeB1;                                         /* ... will be updated in the call back function         */    
    (void)App_ProbeB2;                                         /* ... of uC/OS plugin                                   */
    (void)App_ProbeADC;


    OSProbe_Init();
    OSProbe_SetCallback(App_ProbeCallback);
    OSProbe_SetDelay(50);
#endif    

#if (APP_CFG_PROBE_COM_EN == DEF_ENABLED)    
    ProbeCom_Init();                                            /* Initialize the uC/Probe communications module            */

#if (PROBE_COM_CFG_RS232_EN == DEF_ENABLED)
    ProbeRS232_Init(115200);
    ProbeRS232_RxIntEn();
#endif

#if (APP_CFG_KSD_EN == DEF_ENABLED)
    KSD_Init();
#endif    
#endif       
}
#endif

/*
*********************************************************************************************************
*                                         App_ProbeCallback()
*
* Description : This function is called by the uC/Probe uC/OS-II plug-in after updating task information.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : OSProbe_Task().
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if (APP_CFG_PROBE_OS_PLUGIN_EN == DEF_ENABLED)
static  void  App_ProbeCallback (void)
{

    App_ProbeCnts++;

    App_ProbeB1  = BSP_PB_GetStatus(1);
    App_ProbeB2  = BSP_PB_GetStatus(2);
    App_ProbeADC = (BSP_ADC_GetStatus(1) * 100) / 0x1000;

}
#endif

/*
*********************************************************************************************************
*                                      App_FormatDec()
*
* Description: This function converts a decimal value to ASCII (with leading zeros)
*
* Arguments  : s       is a pointer to the destination ASCII string
*              value   is the value to convert (assumes an unsigned value)
*              digits  is the desired number of digits
*
* Return(s)  : none.
*
* Caller(s)  : Application.
*
* Note(s)    : none.
*********************************************************************************************************
*/

static  void  App_FormatDec (CPU_INT08U  *s, 
                             CPU_INT32U  value, 
                             CPU_INT08U  digits)
{
    CPU_INT08U      i;
    CPU_INT32U      mult;


    mult = 1;
    for (i = 0; i < (digits - 1); i++) {
        mult *= 10;
    }
    while (mult > 0) {
        *s++   = value / mult + '0';
        value %= mult;
        mult  /= 10;
    }
}


/*
*********************************************************************************************************
*********************************************************************************************************
**                                         uC/OS-II APP HOOKS
*********************************************************************************************************
*********************************************************************************************************
*/

#if (OS_APP_HOOKS_EN > 0)
/*
*********************************************************************************************************
*                                      TASK CREATION HOOK (APPLICATION)
*
* Description : This function is called when a task is created.
*
* Argument(s) : ptcb   is a pointer to the task control block of the task being created.
*
* Note(s)     : (1) Interrupts are disabled during this call.
*********************************************************************************************************
*/

void  App_TaskCreateHook (OS_TCB *ptcb)
{
#if (APP_CFG_PROBE_OS_PLUGIN_EN == DEF_ENABLED) && (OS_PROBE_HOOKS_EN > 0)
    OSProbe_TaskCreateHook(ptcb);
#endif
}

/*
*********************************************************************************************************
*                                    TASK DELETION HOOK (APPLICATION)
*
* Description : This function is called when a task is deleted.
*
* Argument(s) : ptcb   is a pointer to the task control block of the task being deleted.
*
* Note(s)     : (1) Interrupts are disabled during this call.
*********************************************************************************************************
*/

void  App_TaskDelHook (OS_TCB *ptcb)
{
    (void)ptcb;
}

/*
*********************************************************************************************************
*                                      IDLE TASK HOOK (APPLICATION)
*
* Description : This function is called by OSTaskIdleHook(), which is called by the idle task.  This hook
*               has been added to allow you to do such things as STOP the CPU to conserve power.
*
* Argument(s) : none.
*
* Note(s)     : (1) Interrupts are enabled during this call.
*********************************************************************************************************
*/

#if OS_VERSION >= 251
void  App_TaskIdleHook (void)
{
}
#endif

/*
*********************************************************************************************************
*                                        STATISTIC TASK HOOK (APPLICATION)
*
* Description : This function is called by OSTaskStatHook(), which is called every second by uC/OS-II's
*               statistics task.  This allows your application to add functionality to the statistics task.
*
* Argument(s) : none.
*********************************************************************************************************
*/

void  App_TaskStatHook (void)
{
}

/*
*********************************************************************************************************
*                                        TASK SWITCH HOOK (APPLICATION)
*
* Description : This function is called when a task switch is performed.  This allows you to perform other
*               operations during a context switch.
*
* Argument(s) : none.
*
* Note(s)     : (1) Interrupts are disabled during this call.
*
*               (2) It is assumed that the global pointer 'OSTCBHighRdy' points to the TCB of the task that
*                   will be 'switched in' (i.e. the highest priority task) and, 'OSTCBCur' points to the
*                  task being switched out (i.e. the preempted task).
*********************************************************************************************************
*/

#if OS_TASK_SW_HOOK_EN > 0
void  App_TaskSwHook (void)
{
#if (APP_CFG_PROBE_OS_PLUGIN_EN == DEF_ENABLED) && (OS_PROBE_HOOKS_EN > 0)
    OSProbe_TaskSwHook();
#endif
}
#endif

/*
*********************************************************************************************************
*                                     OS_TCBInit() HOOK (APPLICATION)
*
* Description : This function is called by OSTCBInitHook(), which is called by OS_TCBInit() after setting
*               up most of the TCB.
*
* Argument(s) : ptcb    is a pointer to the TCB of the task being created.
*
* Note(s)     : (1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/

#if OS_VERSION >= 204
void  App_TCBInitHook (OS_TCB *ptcb)
{
    (void)ptcb;
}
#endif

/*
*********************************************************************************************************
*                                        TICK HOOK (APPLICATION)
*
* Description : This function is called every tick.
*
* Argument(s) : none.
*
* Note(s)     : (1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/

#if OS_TIME_TICK_HOOK_EN > 0
void  App_TimeTickHook (void)
{
#if (APP_CFG_PROBE_OS_PLUGIN_EN == DEF_ENABLED) && (OS_PROBE_HOOKS_EN > 0)
    OSProbe_TickHook();
#endif
}
#endif
#endif
