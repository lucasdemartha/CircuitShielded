#include <Arduino.h>

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

  P1DIR |= 0x01; // Configurar P1.0 como saída
  P1DIR |= 0x02; // Configurar P1.1 como saída

  P1OUT &= ~BIT0; // Desligar o LED inicialmente
  P1OUT &= ~BIT1; // Desligar o LED inicialmente

  for (;;)
  {
    P1OUT |= BIT0;
    P1OUT |= BIT1;
    delay_ms(1000);
    P1OUT &= ~BIT0;
    P1OUT &= ~BIT1;
    delay_ms(1000);
  }
}
