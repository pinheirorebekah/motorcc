/******************************************************************************
 * Nome do Arquivo:	displayLCD.h
 * Dependencias:	Veja seção INCLUDES
 * Processador:		MSP430F1611 | MSP430F249 | MSP430F149
 * Opção de Clock:	CPU: 1MHz
 * Compilador:		IAR 5.6
 * Empresa:			CEFET-MG
 *
 * Plataforma:		Placa Rock V2.0
 *
 * Autor:			Henrique Resende Martins
 *					Mauricio Ferrari Santos Corrêa
 *					Bruno Silveira Avelar
 *					James Hamilton Oliveira Junior
 *                  Renato Zanetti
 *
 * Data:			v1 - 15/Fev/2009 - M.Sc. Henrique Resende Martins
 *					v2 - 28/Abr/2009 - Mauricio Ferrari Santos Corrêa
 *					v3 - 16/Jun/2009 - Mauricio Ferrari Santos Corrêa
 *					v4 - 18/Nov/2009 - Mauricio Ferrari Santos Corrêa
 *					v5 - 05/Dez/2009 - Mauricio Ferrari Santos Corrêa
 *					v6 - 28/fev/2010 - Bruno Silveira Avelar
 *					v7 - 10/Mar/2010 - Mauricio Ferrari Santos Corrêa
 *					v8 - 15/Out/2010 - Mauricio Ferrari Santos Corrêa
 *					v9 - 15/Dez/2010 - Mauricio Ferrari Santos Corrêa
 *					v10- 17/Jan/2011 - Mauricio Ferrari Santos Corrêa
 *					v11- 19/Jan/2011 - James Hamilton Oliveira Junior
 *					v12- 02/Mai/2011 - Mauricio Ferrari Santos Corrêa
 *					v13- 05/Out/2011 - Mauricio Ferrari Santos Corrêa
 *					v14- 06/Jun/2012 - Mauricio Ferrari Santos Corrêa
 *                  v15- 03/Mai/2016 - Renato Zanetti
 *					v16- 22/Mai/2017 - Renato Zanetti
 * 					v17- 16/Ago/2023 - Renato Zanetti
 *
 * Descrição:   Modificação da biblioteca xlcd.h fornecida com o compilador
 *		C18 da Microchip. Foi modificada para funcionar com o compilador XC8 e 
 *		posteriormente com o XC16.
 *      A biblioteca xlcd.h trata da comunicação com display de LCD por meio 
 *      de 4-bits (length), utilizando o PORTD tanto para os 4 bits de dados 
 *      quanto para os 2 bits de controle (RS e E). Finalmente, foi adaptada
 * 		para funcionar com o MSP430, considerando o compilador IAR 5.6.
 * 
 *	Notas:
 *	- Essa bliblioteca de rotinas foi escrita pensando em um LCD
 *	16x2 FDCC1602E implementado com o controlador SPLC780D, mas pode
 *	ser utilizado com qualquer LCD 16x2 com controladores equivalentes
 *	como, por exemplo, o controlador Hitachi HD44780 LCD controller.
 *
 *	- O usuário deve definir os seguintes itens:
 *		-- O tipo de interface de comunicação do LDC com o uC (4 ou 8-bits):
 *		Se for o modo 4-bit, utilizar o upper nibble (nibble é metade de um Byte)
 *		, ou seja, enviar primeiro os 4 bits mais significativos (upper nibble)
 *		e depois os 4 bits menos sgnificativos (lower nibble).
 *
 *		-- Cursor e/ou Blink:
 *		O usuário poderá definir se quer visualizar cursor, blink
 *		ou nenhum dos dois.
 *
 *		-- O sincronismo da comunicação com o LCD:
 *		As temporizações entre o envio de comandos, não foi utilizado
 *		o bit de Busy do LCD, mas sim, através de funções de atraso (delay).
 *		Os tempos de delay escolhidos estão colocados nos defines:
 *		DELAY_SINC e DELAY_INICIO, que foram ecolhidos bem acima do 
 *		tempo mínimo requerido pelo display de LCD (5ms o de sincronismo e
 *		15ms o inicial).
 * 
 * 		-- para o funcionamento da conversao int to ascii, usou-se a função myitoa,
 * 		cuja a implementação foi encontrada online
 *****************************************************************************/
#ifndef __DISPLAYLCD_H
#define __DISPLAYLCD_H

/** I N C L U D E S **********************************************************/
#include "io430.h"
#include <math.h>
#include <string.h> 
/** D E F I N E S ************************************************************/



// Define dos pinos de DADOS do LCD ligados no uc
#define D4	P5OUT_bit.P4
#define D5	P5OUT_bit.P5
#define D6	P5OUT_bit.P6
#define D7	P5OUT_bit.P7

// Define do CFG dos pinos de DADOS do LCD ligados no uC
#define CFG_D4	        P5DIR_bit.P4
#define CFG_D5	        P5DIR_bit.P5
#define CFG_D6	        P5DIR_bit.P6
#define CFG_D7	        P5DIR_bit.P7


// Define dos pinos de CONTROLE do LCD ligados no uC
#define RS_PIN	        P3OUT_bit.P7
#define E_PIN	        P3OUT_bit.P6
//não usado // #define RW_PIN   LATBbits.LATB2

// Define os CFG dos pinos de CONTROLE do LCD ligados no uC
#define CFG_RS         P3DIR_bit.P7
#define CFG_E          P3DIR_bit.P6
//não usado // #define CFG_RW  CFGBbits.CFGB2



// Define o valor dos delays baseado no clock da CPU de 1MHz
#define DELAY_SINC()	__delay_cycles(1000)		// Delay de 1ms para 
											// Geralmente 1ms é suficiente, mas
											// depende do controlador do display
											// Aumente conforme a necessidade!
											
#define DELAY_5ms()		__delay_cycles(5000)		// Delay de 5ms para 


/*** D E F I N E S   D E   C O N F I G U R A C A O ***************************/

/******************************************************************************
 * Define:		LCD_LimpaDisplay()
 * Descrição: 	Remove todos os caracteres do display e retorna a posição do
 *				cursor para a linha 1 e coluna 1
 *****************************************************************************/
#define LCD_LimpaDisplay()  	LCD_EscreveComando(0x01)


/******************************************************************************
 * Define:		LCD_CursorHome()
 * Descrição: 	Retorna a posição do cursor para a linha 1 e coluna 1, mas
 *				sem remover todos os caracteres do display.
 *****************************************************************************/
#define LCD_CursorHome()  	LCD_EscreveComando(0x02)


/******************************************************************************
 * Define:		LCD_LigaDisplay()
 * Descrição: 	Liga a tela do display e desliga o cursor do visor.
 *****************************************************************************/
#define LCD_LigaDisplay()  		LCD_EscreveComando(0x0c)


/******************************************************************************
 * Define:		LCD_DesligaDisplay()
 * Descrição: 	Desliga a tela do display, mas não remove os caracteres da  
 *				memória, nem apaga o backlight.
 *****************************************************************************/
#define LCD_DesligaDisplay()  	LCD_EscreveComando(0x08)


/******************************************************************************
 * Define:		LCD_LigaCursor()
 * Descrição: 	Faz com que o cursor apareça na tela, e se a mesma estiver
 *				desligada, será ligada.
 *****************************************************************************/
#define LCD_LigaCursor()		LCD_EscreveComando(0x0e)


/******************************************************************************
 * Define:		LCD_DesligaCursor()
 * Descrição: 	Faz com o cursor suma da tela e se a mesma estiver
 *				desligada, será ligada.
 *****************************************************************************/
#define LCD_DesligaCursor()		LCD_EscreveComando(0x0c)


/******************************************************************************
 * Define:		LCD_PiscaCursor()
 * Descrição: 	Faz com que o cursor apareça no LCD e fique piscando. Se a 
 *				tela estiver desligada, será ligada.
 *****************************************************************************/
#define LCD_PiscaCursor()		LCD_EscreveComando(0x0f)


/******************************************************************************
 * Define:		LCD_DeslocaCursorDir()
 * Descrição: 	Faz com que o cursor desloque na tela uma posição à direita
 *****************************************************************************/
#define LCD_DeslocaCursorDir()		LCD_EscreveComando(0x14)


/******************************************************************************
 * Define:		LCD_DeslocaCursorEsq()
 * Descrição: 	Faz com que o cursor desloque na tela uma posição à esquerda
 *****************************************************************************/
#define LCD_DeslocaCursorEsq()		LCD_EscreveComando(0x10)


/******************************************************************************
 * Define:		LCD_DeslocaDisplayDir()	
 * Descrição: 	Faz com que a tela desloque uma posição à direita
 *****************************************************************************/
#define LCD_DeslocaDisplayDir()		LCD_EscreveComando(0x1c)


/******************************************************************************
 * Define:		LCD_DeslocaDisplayEsq()	
 * Descrição: 	Faz com que a tela desloque uma posição à esquerda
 *
 *****************************************************************************/
#define LCD_DeslocaDisplayEsq()		LCD_EscreveComando(0x18)


/**  P R O T O T I P O  D E  F U N C O E S  **********************************/

/******************************************************************************
 * Funcao:		void LCD_Init(void)
 * Entrada:		Nenhuma (void)
 * Saída:		Nenhuma (void)
 * Descrição:	Configura portas do uC que estão conectadas ao 
 *	display LCD como saida. Essa rotina é baseada no controlador de LCD 
 *  Hitachi HD44780. Configura os parametros de comunicação com o display,
 *  tais como:
 *	- modo de operação (4 ou 8 data bits)
 *	- tipo de cursor
 *	- tamanho da fonte
 *	Ao final da configuração limpa o display.
 *****************************************************************************/
void LCD_Init(void);


/******************************************************************************
 * Funcao:		void LCD_EscreveComando(unsigned char cmd)
 * Entrada:		cmd: comando que será enviado para o LCD
 * Saída:		Nenhuma (void)
 * Descrição:	(Write a command to the LCD)
 *	Escreve um comando para o display de LCD.
 *****************************************************************************/
void LCD_EscreveComando(unsigned char cmd);


/******************************************************************************
 * Funcao:		void LCD_EnderecoCursor(unsigned char endereco)
 * Entrada:		endereco: endereço da memoria de dados do display que se 
 *				colocar o cursor
 * Saída:		Nenhuma (void)
 * Descrição:	(Set the Display Data RAM Address)
 *	Essa rotina posiciona o cursor no endereço desejado da memoria RAM do 
 *	display LCD. Os comandos de escrita seguintes vão começar a partir da
 *	posição escolhida por esta função
 *****************************************************************************/
void LCD_EnderecoCursor(unsigned char endereco);


/******************************************************************************
 * Funcao:		void LCD_PosicaoCursor(unsigned char linha, unsigned char coluna)
 * Entrada:		linha: número de 1 à 4 da linha em que se deseja deixar o cursor
 *
 *				coluna: número de 1 à 16 coluna linha em que se deseja deixar o cursor
 *
 * Saída:		Nenhuma (void)
 * Descrição:	Essa rotina posiciona o cursor na tela do display, na linha 
 *	e coluna desejados. Os comandos de escrita seguintes vão começar a partir
 *  da posição escolhida por esta função
 *****************************************************************************/
void LCD_PosicaoCursor(unsigned char linha, unsigned char coluna);


/******************************************************************************
 * Funcao:		void LCD_EscreveCaractere(unsigned char data)
 * Entrada:		data: Byte de dados para ser escrito na memoria RAM do LCD
 * Saída:		Nenhuma (void)
 * Descrição:	(Write a Data byte to the LCD)
 *	Essa rotina escreve o byte de caracteres no display a partir da posição
 *	de memoria atual do cursor ou da posição de memória	escolhida com as
 *	funções LCD_PosicaoCursor() ou LCD_EnderecoCursor().
 *****************************************************************************/
void LCD_EscreveCaractere(char data);


/******************************************************************************
 * Funcao:		void LCD_EscreveInteiro(int valor)
 * Entrada:		valor: numero inteiro com sinal (de -32768 a 32767) a ser escrito no
 *				display.
 *
 * Saída:		Nenhuma (void)
 * Descrição:	Essa rotina escreve o numero inteiro no display a partir da posição
 *	de memoria atual do cursor ou da posição de memória escolhida com as
 *	funções LCD_PosicaoCursor() ou LCD_EnderecoCursor().
 *****************************************************************************/
void LCD_EscreveInteiro(int valor);


/******************************************************************************
 * Funcao:		void LCD_EscreveFloat(float valor, char formatacao)
 * Entrada:		valor: numero float com sinal (de -9999.9999 a 9999.9999) que
 *				será escrito no display
 *
 *				formatacao: numero de casas decimais desejadas após a 
 *				virgula (de 0 à 4 casas decimais).
 *
 * Saída:	Nenhuma (void)
 * Descrição:	Essa rotina escreve o numero float no display a partir da posição
 *	atual do cursor com quantas casas decimais forem definidas no
 *  parâmetro formatacao.
 *****************************************************************************/
void LCD_EscreveFloat(float valor, char formatacao);


/******************************************************************************
 * Funcao:		void LCD_EscreveFrase(char *buffer)
 * Entrada:		*buffer: apontador para string na memoria de dados(RAM)
 * Saída:		Nenhuma (void)
 * Descrição:	Essa rotina copia uma string terminada com caracter nulo
 *	da memoria de dados do microcontrolador para o display de LCD.
 *	A string apontada por *buffer é copiada no display a partir da posição
 *  de memoria atual do cursor ou da posição de memória
 *  escolhida com as funções LCD_PosicaoCursor() ou LCD_EnderecoCursor().
 *****************************************************************************/
void LCD_EscreveFrase(char *buffer);


/******************************************************************************
 * Funcao:		void LCD_CriaCaractere(unsigned char endereco, const char *buffer)
 * Entrada:		endereco: Numero de 0 à 7 que indica a posição da CGRAM que o 
 *				caractere será criado.
 *
 *				*buffer: apontador para o vetor de char com tamanho 8 que 
 *				guarda cada linha de desenho do caractere que será criado.
 *
 * Saída:		Nenhuma (void)
 * Descrição:   Essa rotina cria um caractere de usuário em uma das 7 posições
 *	disponiveis na CGRAM do display. O caractere criado tem tamanho 8x5 pixel
 *  (8 linhas e 5 colunas), portanto cada valor do vetor buffer representa
 *  uma linha, onde os 3 bits mais significativos de cada valor é desprezado.
 *****************************************************************************/
void LCD_CriaCaractere(unsigned char endereco, const char *buffer);


#endif
