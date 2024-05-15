/******************************************************************************
 * Nome do Arquivo:	teclado_debounce.h
 * Dependencias:  Veja seção INCLUDES
 * Processador:   MSP430F1611 | MSP430F249 | MSP430F149
 * Opção de Clock:  CPU: 1MHz
 * Compilador:    IAR 5.6
 * Empresa:     CEFET-MG
 *
 * Plataforma:		Kit MSP430F1611/ MSP430F149
 *
 * Autor            Renato Zanetti
 *
 * Data:			      v1 - 15/Mai/2016 - Renato Zanetti
 *                  v2 - 22/Mai/2017 - Renato Zanetti
 *                  v3 - 18/Ago/2023 - Renato Zanetti
 * 
 * Descrição:       Permite a utilização de um teclado 4 linhas por 3 colunas.
 *                  A varredura do teclado é feita a cada 5ms (temporização 
 *                  feita por timer), sendo implementada uma técnica de debounce
 *                  descrita no documento "A guide to debounce" de Jack Ganssle.
 *  
 * Uso:             Para utilização da biblioteca, deve-se executar a função
 *                  "Teclado_Init()" para se configurar os periféricos e utlizar 
 *                  a função "Teclado_Status()" frequentemente (pois não há buffer)
 *                  para se verificar se existe alguma leitura de teclas feita,
 *                  obtendo-se a tecla lida via função  "Teclado_Leitura()".
 * 
 * Nota:            1) Essa biblioteca utiliza a TAIFG do TIMER_A do MSP430, de forma que
 *                  esta fonte de interrupção fica impossibilitada de uso por outras aplicações.
 * 
 *                  2) O firmware considera que as linhas serão usadas para varrer a 
 *                  matrix de teclas (saídas) e as colunas serão usadas como entradas
 *                  para se determinar qual tecla foi apertada.  
 ****************************************************************************/


#ifndef __TECLADO_H
#define __TECLADO_H

#include "io430.h"       
        
//  Define os pinos onde esta conectado o teclado. 
//  Define os pinos para o teclado. 
#if defined (__MSP430F1611__) || defined (__MSP430F249__) //kit_F1611 ou simulador
  #define CONFIG_LINHAS()   P3DIR &= ~0x0f //mantem as linhas como entrada para evitar curto-circuito
  #define CONFIG_LINHA_1      P3DIR_bit.P0
  #define CONFIG_LINHA_2      P3DIR_bit.P1
  #define CONFIG_LINHA_3      P3DIR_bit.P2
  #define CONFIG_LINHA_4      P3DIR_bit.P3
   
  #define LINHA_1     P3OUT_bit.P0
  #define LINHA_2     P3OUT_bit.P1
  #define LINHA_3     P3OUT_bit.P2
  #define LINHA_4     P3OUT_bit.P3
   
   
#elif defined (__MSP430F149__) //kit_F149
  #define CONFIG_LINHAS()     P6DIR &= ~0xf0
  #define CONFIG_LINHA_1      P6DIR_bit.P4
  #define CONFIG_LINHA_2      P6DIR_bit.P5
  #define CONFIG_LINHA_3      P6DIR_bit.P6
  #define CONFIG_LINHA_4      P6DIR_bit.P7
   
  #define LINHA_1     P6OUT_bit.P4
  #define LINHA_2     P6OUT_bit.P5
  #define LINHA_3     P6OUT_bit.P6
  #define LINHA_4     P6OUT_bit.P7

#endif
 

#define COLUNAS              (P2IN & 0x07)
#define CONFIG_COLUNAS()     P2DIR &= ~0x07 //colunas ficam como entradas para evitar queimar
#define CONFIG_COLUNA_1      P2DIR_bit.P2
#define CONFIG_COLUNA_2      P2DIR_bit.P1
#define CONFIG_COLUNA_3      P2DIR_bit.P0


/******************************************************************************
 * Funcao:        void Teclado_Init(void)
 * Entrada:       Nenhuma (void)
 * Saida:         Nenhuma (void)
 * Descricao:     Configura pinos de I/O necessarios. Configura todos os pinos de 
 *				  I/O como entradas digitais. Deve ser executada antes de uma possivel
 *				  configuracao de pinos analogicos.
 *                Tambem configura o TMR_A para gerar interrupcoes a cada 5ms. 
 *                Para que o teclado funcione corretamente, deve-se habilitar a 
 *                interrupcao globalmente (GIE).
 *****************************************************************************/
 void Teclado_Init(void);
 
 /******************************************************************************
 * Funcao:      void Teclado_Varredura_debounced (void)
 * Entrada:     Nenhuma (void)
 * Saida:       Nenhuma (void)
 * Descricao:   Funcao executada por Teclado_ISR() a cada interrupcao do TMR_A.
 *              Ideia de funcionamento do debounce:
 *                 first true read after reset:     1111 1110 0000 0001 return 0
 *                 second true read:                1111 1110 0000 0011 return 0
 *                 after 9 true reads:              1111 1111 1111 1111 return 0
 *                 first false read:                1111 1111 1111 1110 return 0
 *                 after 8 false reads:             1111 1111 0000 0000 return 1
 *                 after many false reads:          1111 1110 0000 0000 return 0
 *
 *****************************************************************************/
 void Teclado_Varredura_debounced (void);
 
/******************************************************************************
 * Funcao:      void Teclado_Status (void)
 * Entrada:     Nenhuma (void)
 * Saida:       Retorna o status de leitura - 0, caso nao houve tecla pressionada
 *                                            1, caso tenha tecla pressionada
 * Descricao:   Trata-se de uma flag de controle que pode ser utilizada em uma 
 *              rotina de polling. A flag e atualizada toda vez que Teclado_Varredura_debounced()
 *              encontra uma tecla pressionada.
 *****************************************************************************/
char  Teclado_Status(void);

/******************************************************************************
 * Funcao:      void Teclado_Overwrite(void)
 * Entrada:     Nenhuma (void)
 * Saida:       0 -  indica que nao houve perda de dados
 *              1 - indica que uma nova tecla foi pressionada antes da leitura 
 *                  do dado anterior por Teclado_Leitura()
 * Descricao:   Trata-se de uma flag de controle que pode ser utilizada para  
 *              indicar perda de dados durante a execucao do programa. A flag 
 *              e atualizada toda vez que Teclado_Varredura_debounced() encontra
 *              uma tecla pressionada.
 *****************************************************************************/
char  Teclado_Overwrite(void);
 
 /******************************************************************************
 * Funcao:      void Teclado_Leitura(void)
 * Entrada:     Nenhuma (void)
 * Saida:       Tecla lida, em formato ASCII - '0' a '9', '#' e '*'. Retorna 0
 *              se houver erro de leitura de tecla salva.
 * Descricao:   Verifica se houve alguma tecla pressioda, retornando um valor em 
 *              acordo com "const char teclas[][]". Retorna 0 caso não haja 
 *              nenhuma tecla salva.
 *****************************************************************************/
 unsigned char Teclado_Leitura(void);
 
#endif// final da biblioteca Teclado
