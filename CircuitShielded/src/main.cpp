#include <Arduino.h>

#define ZC_PIN BIT5 // Pino de entrada para a detecção do cruzamento por zero

void delay_ms(unsigned int milliseconds)
{
  unsigned int i;
  for (i = 0; i < milliseconds; i++)
  {
    __delay_cycles(1000); // Atraso de 1000 ciclos de clock (aproximadamente 1 ms em um MSP430 de 1 MHz)
  }
}

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD; // Desabilitar Watchdog Timer
  // Disable the GPIO power-on default high-impedance mode to activate
  // previously configured port settings
  PM5CTL0 &= ~LOCKLPM5;
  P1DIR &= ~ZC_PIN; // Configurar o pino ZC_PIN como entrada
  P1IES &= ~ZC_PIN; // Interrupção será gerada na transição de baixo para alto
  P1IE |= ZC_PIN;   // Habilitar a interrupção para o pino ZC_PIN

  __bis_SR_register(GIE); // Habilitar interrupções globais

  P1DIR |= 0x01; // Configurar P1.0 como saída
  P1DIR |= 0x02; // Configurar P1.1 como saída

  P1OUT &= ~BIT0; // Desligar o LED inicialmente
  P1OUT &= ~BIT1; // Desligar o LED inicialmente

  for (;;){
    P1OUT |= BIT0;
    P1OUT |= BIT1;
    delay_ms(1000);
    P1OUT &= ~BIT0;
    P1OUT &= ~BIT1;
    delay_ms(1000);
  }
}

#pragma vector = PORT1_VECTOR
__interrupt void Port1_ISR(void)
{
    if (P1IFG & ZC_PIN) // Verificar se a interrupção foi gerada pelo pino ZC_PIN
    {
        // Detecção do cruzamento por zero ocorreu

        // Execute as ações necessárias neste ponto

        P1IFG &= ~ZC_PIN; // Limpar a flag de interrupção do pino ZC_PIN
    }

  /*
    Neste código, o pino P1.5 (BIT5) é configurado como entrada para detectar o cruzamento por zero.
    Quando ocorre um cruzamento por zero na rede de 60Hz, uma interrupção é gerada e o código dentro da função Port1_ISR é executado.
    Você pode adicionar as ações necessárias dentro dessa função para lidar com a detecção do cruzamento por zero.
  */

}
