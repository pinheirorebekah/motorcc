
// Rebekah Pinheiro - ELT3B Prática 13
#include "io430.h"
#include "displayLCD.h"
#include "teclado_debounce.h"

int conferetecla;
int tecla , TC;
int ciclo = 0;
int inicio , cycle ;


// EXIBE LCD
void exibe_LCD (){
  ciclo = TACCR1/10;
  LCD_PosicaoCursor(1,4);
  LCD_EscreveFrase("CICLO DE");
  LCD_PosicaoCursor(2,3);
  LCD_EscreveFrase("TRABALHO:");
  LCD_PosicaoCursor(2,13);
  LCD_EscreveInteiro(ciclo);
  LCD_PosicaoCursor(2,15);
  LCD_EscreveFrase("%  ");
}


// função teclas
void Teclado (void){
  conferetecla = Teclado_Status();
  if(conferetecla){
    tecla = Teclado_Leitura();
  }
  
  switch(tecla) {
  case '1': 
    // TC--;
    // decrementa anti-horário
    TA0CCR1 -= 10;
    exibe_LCD ();
    tecla = 0;
    break;
    
  case '2':        
    // 50% 
    TA0CCR1 = 500;
    exibe_LCD ();
    tecla = 0;
    break ;
    
  case '3':
    //incrementa horário
    TA0CCR1 += 10;
    exibe_LCD ();
    tecla = 0;
    break;
    
  case '5':
    //stop motor 
    TA0CCR1= 470;
    exibe_LCD ();
    tecla = 0;
    break; 
    
  case '7':
    // horário 10%
    TA0CCR1 = 470+100;
    exibe_LCD ();
    tecla = 0;
    break;
    
  case '8':
    //rampa máxima
    TA0CCR1=470; 
    inicio = 200;
    cycle = 50;
    
    TACCTL0 = CCIE;
    __bis_SR_register(GIE);   	
    
    while( cycle != 0){
      while(1){ 
        if(inicio == 0)
          break;
      }
      TA0CCR1+=10;			
      inicio = 200;
      cycle--;
    }
    TACCTL0_bit.CCIE = 0;
    __bic_SR_register(GIE);   
    exibe_LCD ();
    tecla = 0;
    break;
  }
}

// função do clock
void clock_xt2_init (void){
  
  BCSCTL1 &= ~XT2OFF; // turn on XT2
  __delay_cycles(1000); // espera cristal estabilizar
  
  while (IFG1_bit.OFIFG){
    IFG1_bit.OFIFG =0;   //Limpa flag indica erro
    __delay_cycles(1000); //awaits XT2 stabilization
  }
  
  BCSCTL2 |= (SELS); 
  
}  

// função do timer
void Timer_Init (void){
  
  TACTL |=TACLR; //zera TAR e DIV 
  
  TACTL |= (TASSEL1+ID1); // SMCLK, 1:4
  TACCR0 = 999;
  //  TACCR0 = 476;
  
  TC = 500;
  TA0CCR0=999;     
  TA0CCR1=999/2;
  TACCR1 = TC;
  TACCTL1 = OUTMOD_7;
  
  TACTL |= MC_1; //crescente
}

int main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  
  clock_xt2_init();
  Timer_Init();
  exibe_LCD ();
  LCD_Init ();
  Teclado_Init();
  
  P1DIR |= BIT2;
  P1SEL |= BIT2;
  __enable_interrupt(); // habilita a interrupção 
  
  
  while(1)
  {
    Teclado ();
  }     
}

// Interrupção do TimerA0
#pragma vector=TIMER0_A0_VECTOR
__interrupt void baseTempo()
{
  inicio--; 
}
