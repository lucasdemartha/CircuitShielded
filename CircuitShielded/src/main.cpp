#include <Arduino.h>

// Definição dos pinos
#define VOLTAGE_SENSOR_PIN BIT4 // Pino do sensor de tensão (ADC)
#define VOLTAGE_SELECT_PIN BIT2 // Pino do botão de seleção de faixa de tensão
#define GREEN_LED_PIN BIT0      // Pino do LED verde
#define RED_LED_PIN BIT1        // Pino do LED vermelho
#define RELAY_PIN BIT3          // Pino do relé
#define ZERO_CROSS_PIN BIT5     // Pino do leitor do zero cross

// Configuração do ADC
void configureADC()
{
  ADCCTL0 &= ~ADCENC;                         // Desabilitar conversor ADC
  ADCCTL1 = ADCSSEL_0 | ADCSHP | ADCCONSEQ_2; // Configurar sinal de clock, amostragem e modo de sequência de canais
  ADCCTL2 = ADCRES;                           // Configurar resolução de 10 bits
  ADCMCTL0 = ADCINCH_4;                       // Selecionar canal do sensor de tensão
  ADCCTL0 |= ADCENC;                          // Habilitar conversor ADC
}

// Configuração dos pinos
void configurePins()
{
  P1DIR |= GREEN_LED_PIN | RED_LED_PIN | RELAY_PIN;    // Configurar pinos como saída
  P1OUT &= ~(GREEN_LED_PIN | RED_LED_PIN | RELAY_PIN); // Desligar os LEDs e o relé
  P1DIR &= ~VOLTAGE_SELECT_PIN;                        // Configurar pino do botão como entrada
  P1REN |= VOLTAGE_SELECT_PIN;                         // Habilitar resistor de pull-up para o botão
  P1OUT |= VOLTAGE_SELECT_PIN;                         // Configurar pull-up para o botão
  P1DIR &= ~ZERO_CROSS_PIN;                            // Configurar pino do leitor do zero cross como entrada
  P1IES &= ~ZERO_CROSS_PIN;                            // Interrupção será gerada na transição de baixo para alto
  P1IE |= ZERO_CROSS_PIN;                              // Habilitar interrupção para o pino do zero cross
}

// Função para verificar a faixa de tensão selecionada
bool isVoltageInRange(float voltage, int range)
{
  if (range == 1)
    return voltage >= 1.1 && voltage <= 1.3;
  else if (range == 2)
    return voltage >= 2.0 && voltage <= 2.3;
  else
    return false; // Faixa inválida
}

int main(void)
{
  WDTCTL = WDTPW | WDTHOLD; // Desabilitar o watchdog timer

  configureADC();
  configurePins();

  __bis_SR_register(GIE); // Habilitar interrupções globais

  for (;;)
  {
    ADCCTL0 |= ADCSC; // Iniciar conversão ADC

    // Aguardar finalização da conversão
    while (ADCCTL0 & ADCBUSY)
      ;

    int voltageRange = 0;

    if (!(P1IN & VOLTAGE_SELECT_PIN))
    {
      // Botão de seleção pressionado, verificar faixa selecionada
      voltageRange = 1;
    }
    else
    {
      // Botão de seleção não pressionado, verificar faixa padrão
      voltageRange = 2;
    }

    float voltage = ADCMEM0 * 3.3 / 1023; // Converter valor ADC para tensão

    if (isVoltageInRange(voltage, voltageRange))
    {
      P1OUT |= GREEN_LED_PIN; // Tensão estável, ligar LED verde

      P1OUT |= RELAY_PIN; // Acionar o relé

      // Aguardar próxima transição de descida do sinal zero cross
      while (!(P1IN & ZERO_CROSS_PIN))
        ;
      while (P1IN & ZERO_CROSS_PIN)
        ;
      P1OUT &= ~RELAY_PIN; // Desacionar o relé
    }
    else
    {
      P1OUT |= RED_LED_PIN; // Tensão não estável, ligar LED vermelho

      P1OUT &= ~RELAY_PIN; // Desligar o relé
    }
  }
}

#pragma vector = PORT1_VECTOR
__interrupt void Port1_ISR(void)
{
  if (P1IFG & ZERO_CROSS_PIN) // Verificar se a interrupção foi gerada pelo pino do zero cross
  {
    // Transição de descida do zero cross ocorreu
    // Não é necessário executar nenhuma ação neste caso

    P1IFG &= ~ZERO_CROSS_PIN; // Limpar a flag de interrupção do pino do zero cross
  }
}

/*
  Neste código, o pino P1.5 (BIT5) é configurado como entrada para detectar o cruzamento por zero.
  Quando ocorre um cruzamento por zero na rede de 60Hz, uma interrupção é gerada e o código dentro da função Port1_ISR é executado.
  Você pode adicionar as ações necessárias dentro dessa função para lidar com a detecção do cruzamento por zero.
*/
