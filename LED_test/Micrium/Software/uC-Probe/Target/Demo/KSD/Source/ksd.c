/*
*********************************************************************************************************
*                                        uC/Probe IAR Kickstart Kits Demo
*
*                          (c) Copyright 2003-2008; Micrium, Inc.; Weston, FL
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
*                                     uC/Probe  IAR Kickstart Kits Demo 
*
* Filename      : ksd.c
* Version       : V1.00
* Programmer(s) : FT
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define   KSD_MODULE
#include  <ksd.h>

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/
                                                                             /* ------ GLOBAL VARIABLES TO BE MONITORED  ------------------- */
static  CPU_INT32U   KSD_Probe_Dial;                                         /* Test variable for the Dial component demo                    */
static  CPU_INT32U   KSD_Probe_Picture_Ix;                                   /* Test variable for the Picture component demo                 */

static  CPU_BOOLEAN  KSD_Probe_LED_Sel;                                      /* LEDs select value                                            */
static  CPU_BOOLEAN  KSD_Probe_LED1;                                         /* LED 1 value                                                  */
static  CPU_BOOLEAN  KSD_Probe_LED2;                                         /* LED 2 value                                                  */
static  CPU_BOOLEAN  KSD_Probe_LED3;                                         /* LED 3 value                                                  */
                                                                             /* ------------------ KBD's TASKS STACK  ---------------------- */
static  OS_STK       KSD_TaskLEDStk[KSD_CFG_TASK_LED_STK_SIZE];              /* LED 1 and 2 selection task                                   */

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  KSD_TaskLED  (void *p_arg);

/*
*********************************************************************************************************
*                                                KSD_Init()
*
* Description : Initialize the IAR Kickstart Kits demo.
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/

void  KSD_Init (void)
{
    CPU_INT08U  os_err;

    
    (void)&KSD_Probe_Dial;                                            /* Avoid warning                                            */
    (void)&KSD_Probe_LED_Sel;
    (void)&KSD_Probe_LED1;
    (void)&KSD_Probe_LED2;
    (void)&KSD_Probe_LED3;
    (void)&KSD_Probe_Picture_Ix;

#if (OS_TASK_CREATE_EXT_EN > 0)                                       /* ------------------ CREATE KSD TASKS -------------------- */
#if (OS_STK_GROWTH == 1)
    os_err = OSTaskCreateExt( KSD_TaskLED,
                             (void *)0,
                             &KSD_TaskLEDStk[KSD_CFG_TASK_LED_STK_SIZE - 1],   
                              KSD_CFG_TASK_LED_PRIO,                            
                              KSD_CFG_TASK_LED_ID,                              
                             &KSD_TaskLEDStk[0],                                
                              KSD_CFG_TASK_LED_STK_SIZE,                      
                             (void *)0,                                          
                              OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);            
    
#else
    os_err = OSTaskCreateExt( KSD_TaskLEDSel,
                             (void *)0,
                             &KSD_TaskLEDSelStk[0],                                   
                              KSD_CFG_TASK_LED_SEL_PRIO,                              
                              KSD_CFG_TASK_LED_SEL_ID,                                
                             &KSD_TaskStartStk[KSD_CFG_TASK_LED_SEL_STK_SIZE - 1],    
                              KSD_CFG_TASK_LED_SEL_STK_SIZE,                          
                             (void *)0,                                              
                              OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);            


#endif
#else
#if (OS_STK_GROWTH == 1)
    os_err = OSTaskCreate( KSD_TaskLED,
                          (void *)0,
                          &KSD_TaskLEDStk[MS_CFG_TASK_LED_STK_SIZE - 1],
                           KSD_CFG_TASK_LED_PRIO);


#else
    os_err = OSTaskCreate( KSD_TaskLED,
                          (void *)0,
                          &KSD_TaskLEDStk[0],
                           KSD_CFG_TASK_LED_PRIO);
#endif
#endif

#if (OS_TASK_NAME_SIZE > 12)
    OSTaskNameSet(KSD_CFG_TASK_LED_PRIO, (CPU_INT08U *)"KSD LED Task", &os_err);
#endif

    
}

/*
*********************************************************************************************************
*                                                KSD_TaskLED()
*
* Description : Monitors the KSD_Probe_LED_sel value from uC/Probe.
*
*               If KSD_Probe_LED_Sel = 0 then 
*               - KSD_Probe_LED1 = 0, 
*               - KSD_Probe_LED2 = 1
*
*               If KSD_Probe_LED_Sel = 0 then 
*               - KSD_Probe_LED1 = 1, 
*               - KSD_Probe_LED2 = 0
*
*               Toggles the virtual LED3 at 1 hz
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/

static  void  KSD_TaskLED  (void *p_arg)
{
    CPU_INT32U  ctr;
    
 
    ctr            = 0;
    KSD_Probe_LED1 = 0;
    KSD_Probe_LED2 = 0;
    KSD_Probe_LED3 = 0;
        
    while(DEF_TRUE) {
        if(KSD_Probe_LED_Sel == DEF_OFF) {
            KSD_Probe_LED1 = DEF_OFF;
            KSD_Probe_LED2 = DEF_ON;
        } else {
            KSD_Probe_LED1 = DEF_ON;
            KSD_Probe_LED2 = DEF_OFF;
        }
        
        if (ctr > 100) {
            if (KSD_Probe_LED3 == DEF_OFF) {
               KSD_Probe_LED3 = DEF_ON;
            } else {
               KSD_Probe_LED3 = DEF_OFF;
            }
            ctr = 0;
        } else {
            ctr++;
        }
                
        OSTimeDlyHMSM(0, 0, 0, 10);        
    }
}
