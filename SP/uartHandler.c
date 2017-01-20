#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "Delay.h"
void USART1_Init()
{
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
	
    /* enable usart clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
   
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
 
    //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
  
     USART_StructInit(&USART_InitStructure);
     USART_InitStructure.USART_BaudRate = 115200;//BaudRate;
     USART_Init(USART1, &USART_InitStructure);
//USART1->BRR = 0x1A0;	//115200 for Frequency 48Mhz		 
//USART1->BRR = 0x1A0;	//115200 for Frequency 48Mhz	

//		 USART1->CR1 		|= USART_CR1_RXNEIE;//RX IE---Error falchure
     USART_Cmd(USART1, ENABLE);
	 
     NVIC_EnableIRQ(USART1_IRQn);
}
void PA8_OUT()
{//For RS485
		GPIO_InitTypeDef gpio1;	  
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    gpio1.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio1.GPIO_Pin = GPIO_Pin_8;
    gpio1.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio1);
}
void USART1_RS485_Init()
{
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
	
	  PA8_OUT();//PA8 - as output
		GPIO_ResetBits(GPIOA, GPIO_Pin_8);//PA8 = 0;	
	
    /* enable usart clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
   
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
 
    //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

   
     USART_StructInit(&USART_InitStructure);
     USART_InitStructure.USART_BaudRate = 0;//BaudRate;
     USART_Init(USART1, &USART_InitStructure);
USART1->BRR = 0x1A0;	//115200 for Frequency 48Mhz		 
		 
		 USART1->CR1 		|= USART_CR1_RXNEIE;//RX IE
     USART_Cmd(USART1, ENABLE);
	 

     NVIC_EnableIRQ(USART1_IRQn);
}
//********************************************
void _usart_transmit_char( unsigned char data )
{
  while(!(USART1->SR & USART_SR_TXE));//Bit 7 TXE: Transmit data register empty
  USART1->DR = data;
}
//***
void _usart_transmit_hex(int data, int size)
{
	char symbol;
	_usart_transmit_char(0x30);//0
	_usart_transmit_char(0x78);//x
	symbol = (data >> 12);
	if (symbol < 0x0A)
	{
		if (symbol != 0 || size >= 4)
			_usart_transmit_char(symbol + 0x30);//0
	}
	else
	{
		_usart_transmit_char(symbol + 0x41 - 0x0A);//A			
	}
	symbol = (data >> 8 & 0x000F);
	if (symbol < 0x0A)
	{
		if (symbol != 0 || size >= 3)
			_usart_transmit_char(symbol + 0x30);//0
	}
	else
	{
		_usart_transmit_char(symbol + 0x41 - 0x0A);//A			
	}
	symbol = (data >> 4 & 0x000F);
	if (symbol < 0x0A)
	{
		if (symbol != 0 || size >= 2)
			_usart_transmit_char(symbol + 0x30);//0
	}
	else
	{
		_usart_transmit_char(symbol + 0x41 - 0x0A);//A			
	}
	symbol = data & 0x000F;
	if (symbol < 0x0A)
	{
		_usart_transmit_char(symbol + 0x30);//0
	}
	else
	{
		_usart_transmit_char(symbol + 0x41 - 0x0A);//A			
	}
}
//***
void _usart_transmit_dec(int data, int size)
{
	int symbol;
	int _data = data;
	symbol = _data / 100000;
	_data = _data - symbol*100000;
	if (symbol != 0 || size >= 6)
	{
		size = 6;
		_usart_transmit_char(symbol + 0x30);//0
	}
	symbol = _data / 10000;
	_data = _data - symbol*10000;
	if (symbol != 0 || size >= 5)
	{
		size = 5;
		_usart_transmit_char(symbol + 0x30);//0
	}
	symbol = _data / 1000;
	_data = _data - symbol*1000;
	if (symbol != 0 || size >= 4)
	{
		size = 4;
		_usart_transmit_char(symbol + 0x30);//0
	}
	symbol = _data / 100;
	_data = _data - symbol*100;
	if (symbol != 0 || size >= 3)
	{
		size = 3;
		_usart_transmit_char(symbol + 0x30);//0
	}
	symbol = _data / 10;
	_data = _data - symbol*10;
	if (symbol != 0 || size >= 2)
	{
		size = 2;
		_usart_transmit_char(symbol + 0x30);//0
	}
	symbol = _data;
	_usart_transmit_char(symbol + 0x30);//0
}
//***
void _printf(char *first,...)
{
	char **p=&first;
	int i = 0;//Counter of char position in string[0]
	int iPtr = 0;//Index of variable
	int iSize = 1;
	int s = 0;
	_usart_transmit_char(38);
	while(*(p[0]+i))
	{
		if (*(p[0] + i) == '%')
		{//Check output for variable
			i++;
			if (*(p[0] + i) == '0')
			{//status to Fill zero
				i++;
				iSize = *(p[0] + i) - 48;//status to Fill zero
				i++;
			}
			if (*(p[0] + i) == 'x')
			{//Transmit Hex value!!!!!!!!!!!!!!!!!!!!!!
				iPtr++;
				_usart_transmit_hex((int)*(p+iPtr), iSize);
			}
			if (*(p[0] + i) == 'd')
			{//Transmit Dec value!!!!!!!!!!!!!!!!!!!!!!
				iPtr++;
				_usart_transmit_dec((int)*(p+iPtr), iSize);
			}			
			if (*(p[0] + i) == 's')
			{//Transmit String value!!!!!!!!!!!!!!!!!!!
				iPtr++;
				s = 0;
				while(*((p+iPtr)[0] + s))
				{
					_usart_transmit_char(*((p+iPtr)[0] + s));
					s++;
				}
			}
		}
		else
		{//Output string[0]
			_usart_transmit_char(*(p[0] + i));
		}
		i++;
	}
}
//*********************************************
#define GPIO_RTS 	GPIOA
#define GPIO_Pin_RTS 	GPIO_Pin_8
//*********************************************
void RS485_transmit_char( unsigned char data )
{
	GPIO_SetBits(GPIO_RTS, GPIO_Pin_RTS);//FOR RS485
	_usart_transmit_char(data);
	Delay(1000);
	GPIO_ResetBits(GPIO_RTS, GPIO_Pin_RTS);//FOR RS485
}
//***
void RS485_transmit_hex(int data, int size)
{
	GPIO_SetBits(GPIO_RTS, GPIO_Pin_RTS);//FOR RS485
	_usart_transmit_hex(data, size);
	Delay(1000);
	GPIO_ResetBits(GPIO_RTS, GPIO_Pin_RTS);//FOR RS485	
}
//***
void RS485_transmit_dec(int data, int size)
{
	GPIO_SetBits(GPIO_RTS, GPIO_Pin_RTS);//FOR RS485
	_usart_transmit_dec(data, size);
	Delay(1000);
	GPIO_ResetBits(GPIO_RTS, GPIO_Pin_RTS);//FOR RS485	
}
//***
void RS485_printf(char *first,...)
{
  char **p=&first;
	int i = 0;//Counter of char position in string[0]
	int iPtr = 0;//Index of variable
	int iSize = 1;
	int s = 0;
	GPIO_SetBits(GPIO_RTS, GPIO_Pin_RTS);//FOR RS485
	_usart_transmit_char(38);
	while(*(p[0]+i))
	{
		if (*(p[0] + i) == '%')
		{//Check output for variable
			i++;
			if (*(p[0] + i) == '0')
			{//status to Fill zero
				i++;
				iSize = *(p[0] + i) - 48;//status to Fill zero
				i++;
			}
			if (*(p[0] + i) == 'x')
			{//Transmit Hex value!!!!!!!!!!!!!!!!!!!!!!
				iPtr++;
				_usart_transmit_hex((int)*(p+iPtr), iSize);
			}
			if (*(p[0] + i) == 'd')
			{//Transmit Dec value!!!!!!!!!!!!!!!!!!!!!!
				iPtr++;
				_usart_transmit_dec((int)*(p+iPtr), iSize);
			}			
			if (*(p[0] + i) == 's')
			{//Transmit String value!!!!!!!!!!!!!!!!!!!
				iPtr++;
				s = 0;
				while(*((p+iPtr)[0] + s))
				{
					_usart_transmit_char(*((p+iPtr)[0] + s));
					s++;
				}
			}
		}
		else
		{//Output string[0]
			_usart_transmit_char(*(p[0] + i));
		}
		i++;
	}	
	
	Delay(1000);
	GPIO_ResetBits(GPIO_RTS, GPIO_Pin_RTS);//FOR RS485	
}
