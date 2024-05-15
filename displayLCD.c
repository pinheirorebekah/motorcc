#include "displayLCD.h"

static void reverse( char s[] );
void myitoa(int n, char s[]);

void LCD_Init(void)
{
	// Garanta que todas as portas anal�gicas ANx usadas sejam digitais

	// Configura os pinos de controle (RW,RS,E) como sa�da.
	//n�o usado // 	CFG_RW = 0;
	CFG_RS = 1;
	CFG_E = 1;

	// Configura os pinos de dados (D4,D5,D6,D7) como sa�da.
	CFG_D4=1;
	CFG_D5=1;
	CFG_D6=1;
	CFG_D7=1;

// Inicio da rotina de inicializa��o do display LCD
// Aguardar tempos e enviar 3 vezes o comando 0b0011XXXX como descrito no datasheet
// do controlador HD44780 ou SPLC780D

	//Seleciona pinos de Controle para escrita do comando
	//n�o usado // 	RW_PIN = 0;	//escrita=0, leitura=1
	RS_PIN = 0;		// comando=0, dados=1
	E_PIN =  1; 	// desabilita=0, habilita=1

	//selecioan pinos de dados com comando de inicializa��o 0b0011 no Nibble-High
	//(os 4 bits de dados mais significativos), os bits do Nibble-Low s�o Don't Care 
	D7=0;
	D6=0;
	D5=1;
	D4=1;

//Passo 1: Envio do comando de inicializa��o 0b0011

	// Delay inicial de 15ms ap�s alimentar o LCD: Vdd > 4,5V
	DELAY_5ms();
	DELAY_5ms();
	DELAY_5ms();

	//Habilita leitura do comando pelo LCD, atrav�s do pino de enable, para tanto,
	//um pulso de 1ms com borda de descida � efetuado
	E_PIN = 0;      			
        DELAY_SINC();
	E_PIN = 1;
	DELAY_SINC();

//Passo 2: Envio do comando de inicializa��o 0b0011

	// Delay de pelo menos 4.1ms
	DELAY_5ms();

	//Pulso no pino de Enable para habilitar leitura do LCD na borda de descida
	E_PIN = 0;      			
        DELAY_SINC();
	E_PIN = 1;
	DELAY_SINC();

//Passo 3: Envio do comando de inicializa��o 0b0011

	// Delay de pelo menos 100us
	DELAY_5ms();

	//Pulso no pino de Enable para habilitar leitura do LCD na borda de descida
	E_PIN = 0;      			
    DELAY_SINC();
	E_PIN = 1;
	DELAY_SINC();

//Passo 4: Envio da Configura��o 0b0010 no Nibble-High
	D7=0;
	D6=0;
	D5=1;
	D4=0;


	//Pulso no pino de Enable para habilitar leitura do LCD na borda de descida
	E_PIN = 0;      			
        DELAY_SINC();
	E_PIN = 1;
	DELAY_SINC();


//Envio dos comandos de Configura��o do LCD
	LCD_EscreveComando(0x28);	// Function Set: 4bit interface, 2 lines, Font 5x7  
	LCD_EscreveComando(0x06);	// Entry mode set: Increment, Shift OFF
	LCD_EscreveComando(0x0E);	// Display Control: Display ON, Cursor ON, Blink OFF
	LCD_EscreveComando(0x01);	// Clear display
}


/******************************************************************************
 * Funcao:		void LCD_EscreveComando(unsigned char cmd)
 * Entrada:		cmd: comando que ser� enviado para o LCD
 * Sa�da:		Nenhuma (void)
 * Descri��o:	(Write a command to the LCD)
 *	Escreve um comando para o display de LCD.
 *****************************************************************************/
void LCD_EscreveComando(unsigned char cmd)
{
	//Seleciona pinos de Controle para escrita do comando
	//n�o usado // 	RW_PIN = 0;	//escrita=0, leitura=1
	RS_PIN = 0;		// comando=0, dados=1
	E_PIN =  1; 	// desabilita=0, habilita=1

	//Seleciona pinos de Dados: Nibble-High
	if(cmd&BIT4){D4=1;}
	else {D4=0;} // As saidas est�o conectadas como open-collector. Dessa forma, a l�gica � invertida.
	if(cmd&BIT5){D5=1;}
	else {D5=0;}
	if(cmd&BIT6){D6=1;}
	else {D6=0;}
	if(cmd&BIT7){D7=1;}
	else {D7=0;}

	//Pulso no pino de Enable para habilitar leitura do LCD na borda de descida
	E_PIN = 0; // As saidas est�o conectadas como open-collector. Dessa forma, a l�gica � invertida.
        DELAY_SINC();
	E_PIN = 1;
	DELAY_SINC();

	//Seleciona pinos de Dados: Nibble-Low
	if(cmd&BIT0){D4=1;}
	else {D4=0;}
	if(cmd&BIT1){D5=1;}
	else {D5=0;}
	if(cmd&BIT2){D6=1;}
	else {D6=0;}
	if(cmd&BIT3){D7=1;}
	else {D7=0;}    

	//Pulso no pino de Enable para habilitar leitura do LCD na borda de descida
	E_PIN = 0;
        DELAY_SINC();
	E_PIN = 1;
	DELAY_SINC();
	
	//Como os comandos Clear Display(0b00000001) e Cursor Home(0b0000001x),
	//demoram mais tempo para serem executados,cerca de cerca de 1,5ms.
	//Garante-se um atraso maior caso os mesmos sejam efetuados
	if (cmd==0x01 || cmd==0x02 || cmd==0x03)
	{ DELAY_SINC(); }
        RS_PIN =1;
}


/******************************************************************************
 * Funcao:		void LCD_EnderecoCursor(unsigned char endereco)
 * Entrada:		endereco: endere�o da memoria de dados do display que se 
 *				colocar o cursor
 * Sa�da:		Nenhuma (void)
 * Descri��o:	(Set the Display Data RAM Address)
 *	Essa rotina posiciona o cursor no endere�o desejado da memoria RAM do 
 *	display LCD. Os comandos de escrita seguintes v�o come�ar a partir da
 *	posi��o escolhida por esta fun��o
 *****************************************************************************/
void LCD_EnderecoCursor(unsigned char endereco)
{
	//Seleciona pinos de Controle para escrita do comando
	//n�o usado // 	RW_PIN = 0;	//escrita=0, leitura=1
	RS_PIN = 0;		// comando=0, dados=1
	E_PIN =  1; 	// desabilita=0, habilita=1

	//Seleciona pinos de Dados: Nibble-High
	D7 = 1;	
	if(endereco&BIT4){D4=1;}
	else {D4=0;}
	if(endereco&BIT5){D5=1;}
	else {D5=0;}
	if(endereco&BIT6){D6=1;}
	else {D6=0;}
	
	//Pulso no pino de Enable para habilitar leitura do LCD na borda de descida
	E_PIN = 0;
        DELAY_SINC();
	E_PIN = 1;
	DELAY_SINC();

	//Seleciona pinos de Dados: Nibble-Low
	if(endereco&BIT0){D4=1;}
	else {D4=0;}
	if(endereco&BIT1){D5=1;}
	else {D5=0;}
	if(endereco&BIT2){D6=1;}
	else {D6=0;}
	if(endereco&BIT3){D7=1;}
	else {D7=0;}

	//Pulso no pino de Enable para habilitar leitura do LCD na borda de descida
	E_PIN = 0;
        DELAY_SINC();
	E_PIN = 1;
	DELAY_SINC();
        
        RS_PIN =1;
}


/******************************************************************************
 * Funcao:		void LCD_PosicaoCursor(unsigned char linha, unsigned char coluna)
 * Entrada:		linha: n�mero de 1 � 4 da linha em que se deseja deixar o cursor
 *
 *				coluna: n�mero de 1 � 16 coluna linha em que se deseja deixar o cursor
 *
 * Sa�da:		Nenhuma (void)
 * Descri��o:	Essa rotina posiciona o cursor na tela do display, na linha 
 *	e coluna desejados. Os comandos de escrita seguintes v�o come�ar a partir
 *  da posi��o escolhida por esta fun��o
 *****************************************************************************/
void LCD_PosicaoCursor(unsigned char linha, unsigned char coluna)
{
	//endere�o de inicio de cada linha
	#define linha1	0x00	
	#define	linha2	0x40
	#define linha3	0x14
	#define	linha4	0x54
	
	unsigned char endereco = 0x00;
	
	coluna--;
	
	if (coluna >= 0)
	{
		//Calcula o endere�o da mem�ria RAM do display que deve ser usado
		//para posicionar o cursor na linha e coluna desejados
		switch (linha)
		{
			case 1:
				endereco = linha1 + coluna;
				LCD_EnderecoCursor(endereco);		
			break;
		
			case 2:
				endereco = linha2 + coluna;
				LCD_EnderecoCursor(endereco);
			break;
			
			case 3:
				endereco = linha3 + coluna;
				LCD_EnderecoCursor(endereco);		
			break;
		
			case 4:
				endereco = linha4 + coluna;
				LCD_EnderecoCursor(endereco);
			break;
			
			default:
				endereco = linha1;
			break;
		}
	}
	else
	{
		endereco = linha1;
	}
}


/******************************************************************************
 * Funcao:		void LCD_EscreveCaractere(unsigned char data)
 * Entrada:		data: Byte de dados para ser escrito na memoria RAM do LCD
 * Sa�da:		Nenhuma (void)
 * Descri��o:	(Write a Data byte to the LCD)
 *	Essa rotina escreve o byte de caracteres no display a partir da posi��o
 *	de memoria atual do cursor ou da posi��o de mem�ria	escolhida com as
 *	fun��es LCD_PosicaoCursor() ou LCD_EnderecoCursor().
 *****************************************************************************/
void LCD_EscreveCaractere(char cmd)
{
	//Seleciona pinos de Controle para escrita do comando
	//n�o usado // 	RW_PIN = 0;	//escrita=0, leitura=1
	RS_PIN = 1;		// comando=0, dados=1
	E_PIN =  1; 	// desabilita=0, habilita=1

	//Seleciona pinos de Dados: Nibble-High
	if(cmd&BIT4){D4=1;}
	else {D4=0;} // As saidas est�o conectadas como open-collector. Dessa forma, a l�gica � invertida.
	if(cmd&BIT5){D5=1;}
	else {D5=0;}
	if(cmd&BIT6){D6=1;}
	else {D6=0;}
	if(cmd&BIT7){D7=1;}
	else {D7=0;}

	//Pulso no pino de Enable para habilitar leitura do LCD na borda de descida
	E_PIN = 0; // As saidas est�o conectadas como open-collector. Dessa forma, a l�gica � invertida.
        DELAY_SINC();
	E_PIN = 1;
	DELAY_SINC();

	//Seleciona pinos de Dados: Nibble-Low
	if(cmd&BIT0){D4=1;}
	else {D4=0;}
	if(cmd&BIT1){D5=1;}
	else {D5=0;}
	if(cmd&BIT2){D6=1;}
	else {D6=0;}
	if(cmd&BIT3){D7=1;}
	else {D7=0;}    

	//Pulso no pino de Enable para habilitar leitura do LCD na borda de descida
	E_PIN = 0;
    DELAY_SINC();
	E_PIN = 1;
	DELAY_SINC();
}


/******************************************************************************
 * Funcao:		void LCD_EscreveInteiro(int valor)
 * Entrada:		valor: numero inteiro com sinal (de -32768 a 32767) a ser escrito no
 *				display.
 *
 * Sa�da:		Nenhuma (void)
 * Descri��o:	Essa rotina escreve o numero inteiro no display a partir da posi��o
 *	de memoria atual do cursor ou da posi��o de mem�ria escolhida com as
 *	fun��es LCD_PosicaoCursor() ou LCD_EnderecoCursor().
 *****************************************************************************/
void LCD_EscreveInteiro(int valor)
{
	char texto[7] = " ";
	myitoa(valor, texto);
	LCD_EscreveFrase(texto);
}


/******************************************************************************
 * Funcao:		void LCD_EscreveFloat(float valor, char formatacao)
 * Entrada:		valor: numero float com sinal (de -9999.9999 a 9999.9999) que
 *				ser� escrito no display
 *
 *				formatacao: numero de casas decimais desejadas ap�s a 
 *				virgula (de 0 � 4 casas decimais).
 *
 * Sa�da:	Nenhuma (void)
 * Descri��o:	Essa rotina escreve o numero float no display a partir da posi��o
 *	atual do cursor com quantas casas decimais forem definidas no
 *  par�metro formatacao.
 *****************************************************************************/
void LCD_EscreveFloat(float valor, char formatacao)
{
	int numInt = 0;		// Guarda parte inteira
	float numFloat = 0; // Guarda parte fracion�ria
	char n;				// contador do comando for
	
	// Escreve parte inteira
	numInt = (int)valor;
	LCD_EscreveInteiro(numInt);
	
	// Escreve as casas decimais indicada na formata��o
	if( formatacao > 0 )
	{
		LCD_EscreveCaractere('.');
		// Separa parte fracion�ria
		numFloat = (valor - numInt);
		// Se NumFloat � negativo, corrige o sinal
		numFloat = fabs(numFloat);
		
		// Garante que so escreve ate 5 casas decimais
		if(formatacao>4)
			{formatacao=4;}
		
		// Calcula as casas decimais
		for (n = formatacao; n>0; n--)
		{
			numFloat *= 10;
			// Escreve zeros ap�s a virgula
			if( (numFloat<1) && (n>1))
			{
				LCD_EscreveCaractere('0');
			}
		}
		// Escreve parte fracion�ria
		LCD_EscreveInteiro((int)numFloat);

	}
}


/******************************************************************************
 * Funcao:		void LCD_EscreveFrase(char *buffer)
 * Entrada:		*buffer: apontador para string na memoria de dados(RAM)
 * Sa�da:		Nenhuma (void)
 * Descri��o:	Essa rotina copia uma string terminada com caracter nulo
 *	da memoria de dados do microcontrolador para o display de LCD.
 *	A string apontada por *buffer � copiada no display a partir da posi��o
 *  de memoria atual do cursor ou da posi��o de mem�ria
 *  escolhida com as fun��es LCD_PosicaoCursor() ou LCD_EnderecoCursor().
 *****************************************************************************/
void LCD_EscreveFrase(char *buffer)
{
	while(*buffer)		// escreve dados no LCD ate achar caracter nulo
	{
		LCD_EscreveCaractere(*buffer); // escreve caracter no LCD
		buffer++;		// incrementa apontador
	}
}


/******************************************************************************
 * Funcao:		void LCD_CriaCaractere(unsigned char endereco, const char *buffer)
 * Entrada:		endereco: Numero de 0 � 7 que indica a posi��o da CGRAM que o 
 *				caractere ser� criado.
 *
 *				*buffer: apontador para o vetor de char com tamanho 8 que 
 *				guarda cada linha de desenho do caractere que ser� criado.
 *
 * Sa�da:		Nenhuma (void)
 * Descri��o:   Essa rotina cria um caractere de usu�rio em uma das 7 posi��es
 *	disponiveis na CGRAM do display. O caractere criado tem tamanho 8x5 pixel
 *  (8 linhas e 5 colunas), portanto cada valor do vetor buffer representa
 *  uma linha, onde os 3 bits mais significativos de cada valor � desprezado.
 *****************************************************************************/
void LCD_CriaCaractere(unsigned char endereco, const char *buffer)
{
	unsigned char cmd;		//guarda comando a ser enviado para LCD
	unsigned char n = 0;	//contador do comando for

	//Dependendo do valor de endereco, seleciona o comando correto
	//Garante que enderco s� tenha valores de 0 � 7
	endereco = endereco & (BIT0|BIT1|BIT2);
	
	switch (endereco)
	{	case 0: cmd = BIT6;	break;
		case 1: cmd = BIT6|BIT3;	break;
		case 2: cmd = BIT6|BIT4;	break;
		case 3: cmd = BIT6|BIT4|BIT3;	break;
		case 4: cmd = BIT6|BIT5;	break;
		case 5: cmd = BIT6|BIT5|BIT3;	break;
		case 6: cmd = BIT6|BIT5|BIT4;	break;
		case 7: cmd =  BIT6|BIT5|BIT4|BIT3;	break;
		default:cmd = BIT7;	break;
	}	
	//Comando de escrita na CGRAM (cria��o de caratere personalizado)
	LCD_EscreveComando(cmd);
	
	//Escreve cada linha do caractere com a sequencia de valores do buffer
	for(n=0; n<8; n++)
	{
		LCD_EscreveCaractere(*buffer);	// escreve caracter no LCD
	    buffer++;	// incrementa apontador
	}
	
	//Comando para sair do modo cria��o de caratere e posicionar cursor
	//na linha 1, coluna 1 da tela.
	LCD_EnderecoCursor(0x00);
	
}

/* A utility function to reverse a string  */
void reverse(char s[])
 {
     int i, j;
     char c;
 
     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
 }
 

// Implementation of itoa()
void myitoa(int n, char s[])
{
     int i, sign;
 
     if ((sign = n) < 0)  /* record sign */
         n = -n;          /* make n positive */
     i = 0;
     do {       /* generate digits in reverse order */
         s[i++] = n % 10 + '0';   /* get next digit */
     } while ((n /= 10) > 0);     /* delete it */
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);
 }

