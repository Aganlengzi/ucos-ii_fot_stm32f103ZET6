
#include "stm32f10x.h"
#include <includes.h>
#include <ucos_ii.h>
#include <os_cpu.h>


static OS_STK task1_stk[TASK1_STK_SIZE];
static OS_STK task2_stk[TASK2_STK_SIZE];

void led_config(void);
static void startup_task(void);
static void task1(void*p_arg);
static void task2(void*p_arg);
void delay(__IO u32 nCount);

static void startup_task()
{
	//RCC_ClocksTypeDef rcc_clocks;
	//RCC_GetClocksFreq(&rcc_clocks);
	//SysTick_Config(rcc_clocks.HCLK_Frequency / OS_TICKS_PER_SEC);
	led_config();
	OS_CPU_SysTickInit();
	OSTaskCreate(task1, (void *)0, &task1_stk[TASK1_STK_SIZE - 1], 7);
	OSTaskCreate(task2, (void *)0, &task2_stk[TASK2_STK_SIZE - 1], 6);
	
	
	
	//OSTaskDel(OS_PRIO_SELF);
}

void led_config(void){
	GPIO_InitTypeDef GPIO_InitStructure; 
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOG, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_SetBits(GPIOD, GPIO_Pin_13);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;			 //??LED?? D2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
	GPIO_SetBits(GPIOG, GPIO_Pin_14);
}



void task1(void *p_arg)
{
	while(1)
	{
		delay(0x0fffef);
		GPIO_ResetBits(GPIOD,GPIO_Pin_13);
		delay(0x0fffef);
		GPIO_SetBits(GPIOD, GPIO_Pin_13);
		OSTimeDly(200);
	}
}

void task2(void *p_arg)
{
	while(1)
	{
		delay(0x08ffef);
		GPIO_ResetBits(GPIOG,GPIO_Pin_14);
		delay(0x08ffef);
		GPIO_SetBits(GPIOG, GPIO_Pin_14);
		OSTimeDly(500);
	}
}


void delay(__IO u32 nCount){

	while(nCount--)
		;

}

int main(void)
{
	/*
	led_config();
	while(1){
		
		delay(0x0fffef);
		
		GPIO_ResetBits(GPIOD,GPIO_Pin_13);
		GPIO_SetBits(GPIOG, GPIO_Pin_14);
		delay(0x0fffef);
		GPIO_ResetBits(GPIOG,GPIO_Pin_14);
		GPIO_SetBits(GPIOD, GPIO_Pin_13);
		
	}
	*/
	
	OSInit();
	startup_task();
	OSStart();
	
}

