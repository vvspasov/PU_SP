#include "stm32f10x.h" 
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_adc.h"
#include "misc.h"
#include "Delay.h"

#include "uartHandler.h"
void f_SYS_PA8(void);
void Timer(int ms);
void initSP(void);
void initADC(void);
//**********************************************
#define PW_up GPIO_SetBits(GPIOB, GPIO_Pin_4)
#define PW_dn GPIO_ResetBits(GPIOB, GPIO_Pin_4)
#define Cpl_up GPIO_SetBits(GPIOA, GPIO_Pin_15)
#define Cpl_dn GPIO_ResetBits(GPIOA, GPIO_Pin_15)
#define Cmn_up GPIO_SetBits(GPIOB, GPIO_Pin_5)
#define Cmn_dn GPIO_ResetBits(GPIOB, GPIO_Pin_5)
//**********************************************
#define KUpl_up GPIO_SetBits(GPIOB, GPIO_Pin_3)
#define KUpl_dn GPIO_ResetBits(GPIOB, GPIO_Pin_3)
#define KUmn_up GPIO_SetBits(GPIOB, GPIO_Pin_9)
#define KUmn_dn GPIO_ResetBits(GPIOB, GPIO_Pin_9)
//***********************************************
#define Kcb_up GPIO_SetBits(GPIOB, GPIO_Pin_0)
#define Kcb_dn GPIO_ResetBits(GPIOB, GPIO_Pin_0)
#define Kpk_up GPIO_SetBits(GPIOB, GPIO_Pin_8)
#define Kpk_dn GPIO_ResetBits(GPIOB, GPIO_Pin_8)
#define Kmk_up GPIO_SetBits(GPIOB, GPIO_Pin_7)
#define Kmk_dn GPIO_ResetBits(GPIOB, GPIO_Pin_7)
//**********************************************
#define KPpl_up GPIO_SetBits(GPIOB, GPIO_Pin_1)
#define KPpl_dn GPIO_ResetBits(GPIOB, GPIO_Pin_1)
#define KPmn_up GPIO_SetBits(GPIOB, GPIO_Pin_2)
#define KPmn_dn GPIO_ResetBits(GPIOB, GPIO_Pin_2)
//**********************************************



int del = 5000000;
void check_HW()
{//check Hardware
		Cmn_up;//ok
//		Delay(del);
//	  Cmn_dn;
//		Delay(del);
	
		Cpl_up;//ok
//		Delay(del);
//	  Cpl_dn;
//		Delay(del);
	
		PW_up;//ok
//		Delay(del);
//	  PW_dn;
//		Delay(del);
Delay(del);	
KUpl_up;
	
Delay(del);
KUmn_up;

Delay(del);
Kcb_up;	

Delay(del);
Kpk_up;	

Delay(del);
Kmk_up;	

Delay(del);
KPpl_up;	
Delay(del);
KPpl_dn;

Delay(del);
KPmn_up;	
Delay(del);
KPmn_dn;	
}
//Global for ADC
volatile long sum_A0 = 0;
volatile long sum_A1 = 0;
volatile long sum_A2 = 0;
volatile long sum_A3 = 0;
volatile long sum_A4 = 0;
volatile long sum_A5 = 0;
volatile int cnt_v0 = 0;
volatile int cnt_v1 = 0;
volatile int cnt_v2 = 0;
volatile int cnt_v3 = 0;
volatile int cnt_v4 = 0;
volatile int cnt_v5 = 0;	
volatile int val_A0 = 0;
volatile int val_A1 = 0;
volatile int val_A2 = 0;
volatile int val_A3 = 0;
volatile int val_A4 = 0;
volatile int val_A5 = 0;

volatile unsigned long f = 0;
volatile int p = 0;
volatile int ind = 0;
int Perem = 0;
unsigned char KS = 0;
int PK = 0;
int PU, MU, SU, KZ, AVAR;
int cpuPU, cpuMU, cpuPUold, cpuMUold, cntPU, cntMU;
int not_end_command = 0;
void ControlStatus()
{
		if (Perem == 1)
		{
			KUpl_up;//On Rele KU+
			Delay(400000);
//_printf("%03x %03x %03x %03x %03x %03x %02x\r\n", val_A0, val_A1, val_A2, val_A3, val_A4, val_A5, Perem);				
			if (val_A1 > 0x080 && val_A0 < 0x010)
			{
				PK = 1;//Control+
			}
			if (PK == 0)
			{
				KUpl_dn;//Off Rele KU+
				Delay(400000);
				KUmn_up;//On rele KU-
			}
			Delay(400000);
//_printf("%03x %03x %03x %03x %03x %03x %02x\r\n", val_A0, val_A1, val_A2, val_A3, val_A4, val_A5, Perem);				
			if (val_A0 > 0x080 && val_A1 < 0x010)
			{
				PK = 2;//Control-
			}	
			if (PK == 0)
			{
				KUmn_dn;//Off Rele KU-
			}
			//***
			if (PK == 1)
			{
				Cpl_up;
			}
			if (PK == 2)
			{
				Cmn_up;
			}			
		}	
		if (Perem == 0)
		{
				KUpl_up;//On Rele KU+			
		}
}
void OB35(void)
{
	
}
int main()
{
//		SystemInit();			//0001- Clock configure	
//Настройка PLL
   RCC->CFGR  &= ~RCC_CFGR_PLLSRC;        //Источником сигнала для PLL выбран HSI с делением на 2
   RCC->CR  &= ~RCC_CR_PLLON;            //Отключить генератор PLL
   RCC->CFGR &= ~RCC_CFGR_PLLMULL;        //Очистить PLLMULL
   RCC->CFGR |=  RCC_CFGR_PLLMULL6;      //Коефициент умножения = 4
   RCC->CR  |=  RCC_CR_PLLON;            //Включить генератор PLL
   while((RCC->CR & RCC_CR_PLLRDY)==0) {} //Ожидание готовности PLL

   //Переключиться на тактирование от PLL
   RCC->CFGR &= ~RCC_CFGR_SW;            //Очистка битов выбора источника тактового сигнала
   RCC->CFGR |=  RCC_CFGR_SW_PLL;        //Выбрать источником тактового сигнала PLL
   while((RCC->CFGR&RCC_CFGR_SWS)!=0x08){}//Ожидание переключения на PLL
   //Настроить делитель для AHB
   RCC->CFGR &= ~RCC_CFGR_HPRE;          //Очистка битов предделителя "AHB Prescaler"
   RCC->CFGR |=  RCC_CFGR_HPRE_DIV1;      //Установить "AHB Prescaler" равным 1 	
//		SystemInit();			//0001- Clock configure		
	
		USART1_Init();
		_printf("001\r\n");		
		initSP();
		//f_SYS_PA8();	
		initADC();
	  Timer(0);
//check_HW();//	--- Test of Hardware
		PW_up;//On SF Power
		Perem = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_15);//Check JP3 - DC motor
		//Perem = 0;
		ControlStatus();//First status of Strelka (+ OR -)
		while(1)
		{				
			//				OB35();
				//PU = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12);		
				//MU = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13);		
				//SU = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_14);
				cpuPU = !GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12);		
				cpuMU = !GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13);
				if (cpuPU == 1 && cpuPUold == 0)
				{
					cntPU = 1;
				}
				if (cpuMU == 1 && cpuMUold == 0)
				{
					cntMU = 1;
				}
				cpuPUold = cpuPU;
				cpuMUold = cpuMU;
			  if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) && GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13))
				{
					not_end_command = 0;//finish command from CPU
				}				
				if (val_A2 < 0x20 && val_A3 < 0x20 && val_A4 < 0x20 && GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) && GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) && p == 0)
				{
					//if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) && !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_14) && p == 0 && not_end_command == 0)
					if (cntPU >= 20 && !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_14) && p == 0 && not_end_command == 0)
					{//perevod v plus +
						p = 1;
						PU = 0x01;
						AVAR = 0;
						KUpl_dn;
						KUmn_dn;
						Delay(100);
						if (Perem == 1)
						KPpl_up;//***
						else
						KPmn_up;//***	
					}
					//if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) && !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_14) && p == 0  && not_end_command == 0)
					if (cntMU >= 20 && !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_14) && p == 0  && not_end_command == 0)
					{//perevod v minus +
						p = 1;
						PU = 0x02;
						AVAR = 0;
						KUpl_dn;
						KUmn_dn;
						Delay(100);
						if (Perem == 1)
						KPmn_up;//***
						else
						KPpl_up;//***	
					}					
					if (p == 0)
					{
						if (KS == 0x01)
						{
							KUpl_up;
						}
						if (KS == 0x02)
						{
							if (Perem == 1) KUmn_up; else KUpl_up;
						}
					}				
				}			
				//Indication***
				//*************
				//*************	
				
				if (PU == 0x01 && p > 0)
				{
					if (ind % 8 == 0)
					{
						Cpl_up;//Perevod+
					}
					else Cpl_dn;
				}
				if (PU == 0x02 && p > 0)
				{
					if (ind % 8 == 0)
					{
						Cmn_up;//Perevod-
					}
					else Cmn_dn;
				}	
				PK = 0;
				if (val_A1 > 0x080 && val_A0 < 0x010)
				{
						PK = 1;
						KS = 1;
						KZ = 0;
				}	
				if (val_A0 > 0x080 && val_A1 < 0x010)
				{
						PK = 2;
						KS = 2;
						KZ = 0;
				}				
				if (val_A0 > 0x120 && val_A1 > 0x120)
				{
					if (KZ == 0) KZ = 1;
					if (p != 0) KZ = 0;
				}	
				else
				{
					KZ = 0;
				}
				if (PK == 0)
				{
					Kpk_dn;
					Kmk_dn;
					Cpl_dn;
					Cmn_dn;	
				}
				if (PK == 1)
				{
					Cpl_up;//Kontrol+
					Cmn_dn;
					Kpk_up;	
				}
				if (PK == 2)
				{
					Cmn_up;//Kontrol-
					Cpl_dn;
					Kmk_up;
				}			
				if (KZ > 0)
				{
					if (ind % 2 == 0)
					{
						Cpl_up;
						Cmn_up;
					}
					else
					{
						Cpl_dn;
						Cmn_dn;					
					}						
				}			
				if (KZ > 50)
				{
					KZ = 0;
					KS = 0;
					Cpl_dn;
					Cmn_dn;	
					KUpl_dn;//Off Rele KU+
					KUmn_dn;//Off Rele KU-						
				}
				if (AVAR == 1)
				{
					if (ind % 2 == 0)
					{
						PW_up;//On SF Power
					}
					else
					{
						PW_dn;//Off SF Power
					}
				}
				else
				{
					PW_up;//On SF Power
				}
				//Perevod***
				//**********
				//**********
				if (p != 0)
				{//Idet perevod
					if (Perem == 1)
					{
						if ((val_A2 < 0x10 || val_A3 < 0x10 || val_A4 < 0x10))
						{
								if (p > 2)
								{//Perevod nachalsa
									KPpl_dn;//***
									KPmn_dn;//***	
									not_end_command = 0x01;
									KS = PU;
									if (p < 5 || (val_A2 > 0x300 || val_A3 > 0x300 || val_A4 > 0x300))
									{							
										AVAR = 0x01;	
									}	
									p = 0;
									PU = 0;	
									cntPU = 0;
									cntMU = 0;									
									Delay(100);
								}
						}
						else
						{//Proverka raboti na frikciu
							if (p > 60)//10sek----30-3sek
							{
								p = 0;
								KPpl_dn;//***
								KPmn_dn;//***
								KS = PU;
								PU = 0;
								cntPU = 0;
								cntMU = 0;
								Delay(100);
								not_end_command = 0x01;
							}							
						}
					}
					if (Perem == 0)
					{//Postojannij tok
						if (val_A5 < 0x100 && p > 3)
						{
							KPpl_dn;//***
							KPmn_dn;//***	
							not_end_command = 0x01;
							KS = 1;
							if (p < 5)
							{
								AVAR = 0x01;	
							}
							p = 0;
							PU = 0;									
							Delay(100);
						}
						else
						{
							if (p > 100)
							{
								p = 0;
								KPpl_dn;//***
								KPmn_dn;//***
								KS = PU;
								PU = 0;
								cntPU = 0;
								cntMU = 0;
								Delay(100);
								not_end_command = 0x01;								
							}
						}
					}
				}			
				//KPmn_up;
//				SU = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11);
				if (f == 0)// && ind % 10 == 0)// && 
				{
					//ind = 0;
//					p = 0;
					_printf("%03x %03x %03x %03x %03x %03x %02d\r\n", val_A0, val_A1, val_A2, val_A3, val_A4, val_A5, p);	
				}
		}
}
//********************
void TIM4_IRQHandler()
{
	#define SampleTime ADC_SampleTime_1Cycles5
	int des;
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);	
	des = f % 8;
	if (f == 0)
	{//Clear data
		cnt_v0 = 0;
		cnt_v1 = 0;
		cnt_v2 = 0;
		cnt_v3 = 0;
		cnt_v4 = 0;
		cnt_v5 = 0;
		sum_A0 = 0;
		sum_A1 = 0;
		sum_A2 = 0;
		sum_A3 = 0;
		sum_A4 = 0;
		sum_A5 = 0;	
	}	
	f++;//0,02ms	
	if (des == 1)
	{//startA0, A1
			ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, SampleTime);//A0	
			ADC_RegularChannelConfig(ADC2, ADC_Channel_4, 1, SampleTime);//A1			
			ADC_SoftwareStartConvCmd(ADC1, ENABLE);	
			ADC_SoftwareStartConvCmd(ADC2, ENABLE);	
	}
	if (des == 2)
	{//readA0, A1
			sum_A0 = sum_A0 + ADC_GetConversionValue(ADC1);
			cnt_v0++;
			sum_A1 = sum_A1 + ADC_GetConversionValue(ADC2);
			cnt_v1++;	
	}
	if (des == 3)
	{//startA2, A3
			ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 1, SampleTime);//A2	
			ADC_RegularChannelConfig(ADC2, ADC_Channel_2, 1, SampleTime);//A3			
			ADC_SoftwareStartConvCmd(ADC1, ENABLE);	
			ADC_SoftwareStartConvCmd(ADC2, ENABLE);	
	}
	if (des == 4)
	{//readA2, A3
			sum_A2 = sum_A2 + ADC_GetConversionValue(ADC1);
			cnt_v2++;
			sum_A3 = sum_A3 + ADC_GetConversionValue(ADC2);
			cnt_v3++;	
	}
	if (des == 5)
	{//startA4, A5
			ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, SampleTime);//A4	
			ADC_RegularChannelConfig(ADC2, ADC_Channel_0, 1, SampleTime);//A5			
			ADC_SoftwareStartConvCmd(ADC1, ENABLE);	
			ADC_SoftwareStartConvCmd(ADC2, ENABLE);	
	}
	if (des == 6)
	{//readA4, A5
			sum_A4 = sum_A4 + ADC_GetConversionValue(ADC1);
			cnt_v4++;
			sum_A5 = sum_A5 + ADC_GetConversionValue(ADC2);
			cnt_v5++;			
	}	
	if (f > 5000 && des == 7)
	{//100ms
			f = 0;		
			val_A0 = sum_A0/cnt_v0;
			val_A1 = sum_A1/cnt_v1;
			val_A2 = sum_A2/cnt_v2;
			val_A3 = sum_A3/cnt_v3;
			val_A4 = sum_A4/cnt_v4;
			val_A5 = sum_A5/cnt_v5;
			val_A5 = val_A5 - 0x800;
			if (val_A5 < 0) val_A5 = 0;
			if (p!= 0) p++;
			if (KZ!=0) KZ++;
			ind++;
	}
	if (f % 500 == 0)
	{//10ms
		if (cntPU != 0) cntPU++;
		if (cntPU > 20) cntPU = 20;//200ms
		if (cntMU != 0) cntMU++;
		if (cntMU > 20) cntMU = 20;//200ms
	}
}
void f_SYS_PA8()
{//testedOK
	/*
	//New code---SYSCLK to output PA8
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	//GPIOA->MODER |= GPIO_MODER_MODER8_0;//GPIm----
	GPIOA->MODER |= GPIO_MODER_MODER8_1;//10: Alternate function mode
	GPIOA->AFR[1] |= 0x00000000;//MCO(AF0) Alternative function [1] - AFRH 
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR8;//High Speed
	
	RCC->CFGR |= RCC_CFGR_MCO_SYSCLK;
	//RCC->CFGR |= RCC_CFGR_MCO_HSE;	
	*/
	 GPIO_InitTypeDef gpio1;
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	 gpio1.GPIO_Mode = GPIO_Mode_AF_PP;
	 gpio1.GPIO_Pin = GPIO_Pin_8;
	 gpio1.GPIO_Speed = GPIO_Speed_50MHz;
	 GPIO_Init(GPIOA, &gpio1);
	 RCC->CFGR |= RCC_CFGR_MCO_SYSCLK;
	 //RCC->CFGR |= RCC_CFGR_MCO_HSE;
	 //RCC->CFGR |= RCC_CFGR_MCO_PLL;
}
void Timer(int ms)
{
			//4800*10000 - 1000ms
			//4800*10		 - 1ms
			//480*1     - 0,02ms***
			TIM_TimeBaseInitTypeDef timer;

			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

			TIM_TimeBaseStructInit(&timer);
			timer.TIM_Prescaler = 480;

			timer.TIM_Period = 1;//10000;
			TIM_TimeBaseInit(TIM4, &timer);	
		
			TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
			TIM_Cmd(TIM4, ENABLE);
			NVIC_EnableIRQ(TIM4_IRQn);	
}
void initADC()
{
			GPIO_InitTypeDef port;
			ADC_InitTypeDef ADC_InitStructure;	
			
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_ADC2 | RCC_APB2Periph_GPIOA, ENABLE);
			
			GPIO_StructInit(&port);
			port.GPIO_Mode = GPIO_Mode_AIN;
			port.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
			port.GPIO_Speed = GPIO_Speed_2MHz;
			GPIO_Init(GPIOA, &port);	
			
			/* ADC1 configuration ------------------------------------------------------*/
			ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
			ADC_InitStructure.ADC_ScanConvMode = ENABLE;
			ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
			ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
			ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
			ADC_InitStructure.ADC_NbrOfChannel = 1; 

			ADC_Init(ADC1, &ADC_InitStructure); //Initialise ADC1
		
			ADC_Cmd(ADC1, ENABLE); //Enable ADC1
			/* Enable ADC1 reset calibration register */ 
			ADC_ResetCalibration(ADC1);
			/* Check the end of ADC1 reset calibration register */
			while(ADC_GetResetCalibrationStatus(ADC1));
			/* Start ADC1 calibaration */
			ADC_StartCalibration(ADC1);
			/* Check the end of ADC1 calibration */
			while(ADC_GetCalibrationStatus(ADC1));
			
			/* ADC2 configuration ------------------------------------------------------*/
			ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
			ADC_InitStructure.ADC_ScanConvMode = ENABLE;
			ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
			ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
			ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
			ADC_InitStructure.ADC_NbrOfChannel = 1; 			
			ADC_Init(ADC2, &ADC_InitStructure); //Initialise ADC1
		
			ADC_Cmd(ADC2, ENABLE); //Enable ADC1
			/* Enable ADC2 reset calibration register */ 
			ADC_ResetCalibration(ADC2);
			/* Check the end of ADC1 reset calibration register */
			while(ADC_GetResetCalibrationStatus(ADC2));
			/* Start ADC2 calibaration */
			ADC_StartCalibration(ADC2);
			/* Check the end of ADC2 calibration */
			while(ADC_GetCalibrationStatus(ADC2));
}
void initSP()
{
		  GPIO_InitTypeDef port;
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);	
			/* Disable the Serial Wire Jtag Debug Port SWJ-DP */
			GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
	
			GPIO_StructInit(&port);
			port.GPIO_Mode = GPIO_Mode_Out_PP;
			port.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
			GPIO_Init(GPIOB, &port);

			port.GPIO_Mode = GPIO_Mode_Out_PP;	
			port.GPIO_Pin = GPIO_Pin_15;
			GPIO_Init(GPIOA, &port);	
	
			port.GPIO_Mode = GPIO_Mode_IPU;//GPIO_Mode_IN_FLOATING;	
			port.GPIO_Pin = GPIO_Pin_10 |GPIO_Pin_11 |GPIO_Pin_12 | GPIO_Pin_13;
			GPIO_Init(GPIOB, &port);	
	
			port.GPIO_Mode = GPIO_Mode_IPU;//GPIO_Mode_IN_FLOATING;	
			port.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
			GPIO_Init(GPIOC, &port);	
			
			PW_dn;//ok
			Cpl_dn;
			Cmn_dn;	
}
//D4 - PA15	--- (VD14) - "Cpl" 	-ok
//D5 - PB4  --- (VD13) - "PW" 	-ok
//D6 - PB5  --- (VD12) - "Cmn" 	-ok


//B7 - PB3  --- () -	KUpl	 	-ok
//B6 - PB9  --- () -  KUmn 	-ok

//E6 - PB0  --- () -  Kcb
//B5 - PB8 	--- () -  Kpk -ok
//B4 - PB7 	--- () -  Kmk -ok

//B0 - PB1  --- () -	KPpl -ok
//B1 - PB2  --- () - 	KPmn -ok

//C7 - PC15 --- JP3 	(DC motor)
//C6 - PC14 --- kSU
//D0 - PB12 --- kPU		 -ok
//D1 - PB13 --- kMU    -ok

//B2 - PB10 --- KPpl_ON
//B3 - PB11 --- KPmn_ON

//from SF
//D7 - PB6  --- (KO2) - 		 -ok

