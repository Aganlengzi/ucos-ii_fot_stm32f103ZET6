/*
*********************************************************************************************************
*                                      uC/Probe Communication
*
*                           (c) Copyright 2007; Micrium, Inc.; Weston, FL
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
*
* Filename      : ksd.h
* Version       : V1.00
* Programmer(s) : FT
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                 MODULE
*
* Note(s) : (1) This header file is protected from multiple pre-processor inclusion through use of the
*               KSD_PRESENT pre-processor macro definition.
*********************************************************************************************************
*/

#ifndef  KSD_PRESENT                                 /* See Note #1.                                         */
#define  KSD_PRESENT


/*
*********************************************************************************************************
*                                           IAR KICKSTART KITS DEMO VERSION NUMBER
*
* Note(s) : (1) (a) The IAR kickstart kits demo (KSD) module software version is denoted as follows :
*
*                       Vx.yy
*
*                           where
*                                  V     denotes 'Version' label
*                                  x     denotes major software version revision number
*                                  yy    denotes minor software version revision number
*
*               (b)  The IAR kickstart kits demo module software version label #define is formatted as follows :
*
*                       ver = x.yy * 100
*
*                           where
*                                  ver   denotes software version number scaled as
*                                        an integer value
*                                  x.yy  denotes software version number
*********************************************************************************************************
*/

#define  KSD_VERSION         100u                    /* See Note #1.                                         */

/*
*********************************************************************************************************
*                                               EXTERNS
*********************************************************************************************************
*/

#ifdef   KSD_MODULE
#define  KSD_EXT
#else
#define  KSD_EXT  extern
#endif

/*
*********************************************************************************************************
*                                        IAR KICKSTART KITS DEMO INCLUDE FILES
*********************************************************************************************************
*/

#include  <cpu.h>
#include  <lib_def.h>
#include  <lib_mem.h>
#include  <lib_str.h>

#include  <ucos_ii.h>

#include  <app_cfg.h>

/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*                                     (Target Independent Functions)
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            INITIALIZATION
*********************************************************************************************************
*/

void  KSD_Init              (void);

/*
*********************************************************************************************************
*                                          CONFIGURATION ERRORS
*********************************************************************************************************
*/

#if     !defined(KSD_CFG_TASK_LED_PRIO)
#error  "KSD_CFG_TASK_LED_PRIO                      not #define'd in 'app_cfg.h'                  "
#error  "         [This will be the priority for the uC/Probe IAR KickStart Demo (KSD) LED Task]  "
#endif

#if     !defined(KSD_CFG_TASK_LED_STK_SIZE)
#error  "KSD_CFG_TASK_LED_STK_SIZE                  not #define'd in 'app_cfg.h'                  "
#error  "         [This will be the stack size for the uC/Probe IAR KickStart Demo (KSD) LED Task]"
#endif

#endif


