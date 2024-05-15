#include "teclado_debounce.h"

#define CICLOS  5000        //5ms @ Fclk = 1MHz

const char teclas[4][4]={   {'1','2','3', 0}, //0s zeros a direita servem para erros de leitura
                            {'4','5','6', 0}, 
                            {'7','8','9', 0}, 
                            {'*','0','#', 0},
                                              };

typedef struct{
  unsigned char linha; //armazena a linha da tecla apertada
  unsigned char coluna; //armazena a coluna da tecla apertada
  unsigned char status; // contem 1 quando alguma tecla foi apertada
  unsigned char overwrite; //indica se o usuario perdeu a leitura de uma tecla
} teclado_t; //cria um novo tipo de dados

teclado_t teclado; //cria uma variavel do tipo teclado_t




char  Teclado_Status()	{ return	teclado.status; }
char  Teclado_Overwrite()	{	return teclado.overwrite;}

void Timer_Setup(void)
{
  // Configuração do Timer_A
  // Núcleo do Timer
  // Define valor inicial do TAR
  TBR=0;                       //Reseta contador 
  TBCTL = TASSEL_2 +            // Fonte do clock: SMCLK
      ID_1;                 // Divisão por 1
  // Bloco Capture/Compare
  TBCCR0=CICLOS;                //interrupcao a cada numero de CICLOS de clocks
  TBCCTL0_bit.CCIFG=0; //limpa a flag
  TBCCTL0_bit.CCIE=1;
  TBCTL |= MC_1;
  
  
  return;
}

void Teclado_Init(void){
  
  CONFIG_LINHAS();
  CONFIG_COLUNAS();
  
  Timer_Setup();
  
  teclado.coluna=0;
  teclado.linha=0;
  teclado.overwrite=0;
  teclado.status=0;
  
  return;
  
}

void Teclado_Varredura_debounced (void){
  static unsigned int state=0;
  
  CONFIG_LINHA_1=1; //somente 1 coluna como saida por vez
  LINHA_1 = 1;// nivel logico em 1 na primeira coluna
  
  __delay_cycles(5); //espera a mudanca de nivel de tensao na coluna
  
  state = (state << 1)| COLUNAS | 0xFE00; // debounce (10 a 16 vezes)
  // pino em nivel alto caso um botao seja apertado
  if (COLUNAS) { //necessario sempre guardar linha e coluna para gerar a saida da leitura
    teclado.linha = 0;
    teclado.coluna = COLUNAS;// se for linha 1, LINHAS=0b0000 0001
  }
  LINHA_1 = 0;
  CONFIG_LINHA_1=0; //coluna 1 volta a ser cfg como entrada
  
  CONFIG_LINHA_2=1;
  LINHA_2 = 1;
  __delay_cycles(5); //espera a mudanca de nivel de tensao na coluna
  
  state = state | COLUNAS | 0xFE00; //nao e necessario deslocar state novamente
  if (COLUNAS) { //necessario sempre guardar linha e coluna para gerar a saida da leitura
    teclado.linha = 1;
    teclado.coluna = COLUNAS;// se for linha 1, LINHAS=0b0000 0001
  }
  
  LINHA_2 = 0;
  CONFIG_LINHA_2=0; //coluna 1 volta a ser cfg como entrada
  
  CONFIG_LINHA_3=1;
  LINHA_3 = 1;
  __delay_cycles(5); //espera a mudanca de nivel de tensao na coluna
  
  state = state | COLUNAS | 0xFE00; //nao e necessario deslocar state novamente
  if (COLUNAS) { //necessario sempre guardar linha e coluna para gerar a saida da leitura
    teclado.linha = 2;
    teclado.coluna = COLUNAS;// se for linha 1, LINHAS=0b0000 0001
  }
  
  LINHA_3 = 0;
  CONFIG_LINHA_3=0; //coluna 1 volta a ser cfg como entrada
  
  CONFIG_LINHA_4=1;
  LINHA_4 = 1;
  __delay_cycles(5); //espera a mudanca de nivel de tensao na coluna
  
  state = state | COLUNAS | 0xFE00; //nao e necessario deslocar state novamente
  if (COLUNAS) { //necessario sempre guardar linha e coluna para gerar a saida da leitura
    teclado.linha = 3;
    teclado.coluna = COLUNAS;// se for linha 1, LINHAS=0b0000 0001
  }
  
  LINHA_4 = 0;
  CONFIG_LINHA_4=0; //coluna 1 volta a ser cfg como entrada
  
  if(state == 0xFF00){ //houve uma tecla apertada e o usuario ja a soltou
    if (teclado.status) //ja tinha uma leitura armazenada
      teclado.overwrite = 1;
    else{
      teclado.status = 1; //indica que houve uma leitura de tecla
      
      switch (teclado.coluna) { //covercao da leitura binaria na linha do teclado
      case 1: 
        teclado.coluna = 2;
        break;
      case 2:
        teclado.coluna = 1;
        break;
      case 4:
        teclado.coluna = 0;
        break;
      default:
        teclado.coluna = 3;
        break;
        
      }
    }
  }
  
  
  
}
unsigned char Teclado_Leitura(void) {
  
  if (teclado.status) {
    teclado.status = 0;
    return teclas[teclado.linha][teclado.coluna];
  } else
    return 0;
  
}


// Interrupção do Timer0
#pragma vector=TIMERB0_VECTOR
__interrupt void Teclado_ISR()
{  
    TBCCTL0_bit.CCIFG=0; //limpa a flag
    Teclado_Varredura_debounced(); 

}