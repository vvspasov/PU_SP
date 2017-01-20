void USART1_Init(void);
void USART1_RS485_Init(void);

void _usart_transmit_char( unsigned char data);
void _usart_transmit_hex(int data, int size);
void _usart_transmit_dec(int data, int size);
void _printf(char *first,...);
void RS485_transmit_char( unsigned char data );
void RS485_transmit_hex(int data, int size);
void RS485_transmit_dec(int data, int size);
void RS485_printf(char *first,...);

