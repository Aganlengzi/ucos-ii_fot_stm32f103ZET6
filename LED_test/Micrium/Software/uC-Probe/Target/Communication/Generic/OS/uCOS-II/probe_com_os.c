/*
*********************************************************************************************************
*                                       uC/Probe Communication
*
*                         (c) Copyright 2007-2008; Micrium, Inc.; Weston, FL
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
*                                       COMMUNICATION: GENERIC
*                                        Micrium uC/OS-II PORT
*
* Filename      : probe_com_os.c
* Version       : V2.20
* Programmer(s) : BAN
*********************************************************************************************************
* Note(s)       : (1) This file is the uC/OS-II layer for the uC/Probe Generic Communication Module.
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include  <probe_com.h>
#include  <ucos_ii.h>


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/


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

#if ((PROBE_COM_CFG_TERMINAL_REQ_EN == DEF_ENABLED) && (OS_SEM_EN > 0))
static  OS_EVENT  *ProbeCom_OS_TerminalSem;
#endif


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/

#if ((PROBE_COM_CFG_TERMINAL_REQ_EN == DEF_ENABLED) && (OS_SEM_EN == 0))
#error  "If Probe terminal requests are enabled, then semaphores MUST be enabled."
#endif


/*
*********************************************************************************************************
*                                         ProbeCom_OS_Init()
*
* Description : Create a semaphore for terminal output completion notification.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : ProbeCom_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if (PROBE_COM_CFG_TERMINAL_REQ_EN == DEF_ENABLED)
void  ProbeCom_OS_Init (void)
{
#if (OS_EVENT_NAME_SIZE > 10) && (OS_SEM_EN > 0)
    CPU_INT08U  err;
#endif


#if   (OS_SEM_EN > 0)
    ProbeCom_OS_TerminalSem = OSSemCreate(0);
#if   (OS_EVENT_NAME_SIZE > 13)
    OSEventNameSet(ProbeCom_OS_TerminalSem, (CPU_INT08U *)"uC/Probe Term", &err);
#elif (OS_EVENT_NAME_SIZE > 10)
    OSEventNameSet(ProbeCom_OS_TerminalSem, (CPU_INT08U *)"Probe Term", &err);
#endif
#endif
}
#endif


/*
*********************************************************************************************************
*                                    ProbeCom_OS_TerminalOutWait()
*
* Description : Wait for terminal output to complete.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : ProbeCom_TerminalOut().
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if (PROBE_COM_CFG_TERMINAL_REQ_EN == DEF_ENABLED)
void  ProbeCom_OS_TerminalOutWait (void)
{
#if (OS_SEM_EN > 0)
    INT8U  os_err;


    OSSemPend(ProbeCom_OS_TerminalSem, 0, &os_err);             /* Wait for output to complete.                         */

    (void)os_err;
#endif
}
#endif


/*
*********************************************************************************************************
*                                   ProbeCom_OS_TerminalOutSignal()
*
* Description : Signal terminal output completion.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : ProbeCom_CmdTerminalOut().
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if (PROBE_COM_CFG_TERMINAL_REQ_EN == DEF_ENABLED)
void  ProbeCom_OS_TerminalOutSignal (void)
{
#if (OS_SEM_EN > 0)
    OSSemPost(ProbeCom_OS_TerminalSem);                         /* Output has completed.                                 */
#endif
}
#endif
