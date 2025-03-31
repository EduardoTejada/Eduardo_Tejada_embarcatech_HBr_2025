#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/dma.h"
#include "microphone_dma.h"

#define COLETAS 1

// Pino e canal do microfone no ADC.
#define MIC_CHANNEL 2
#define MIC_PIN (26 + MIC_CHANNEL)

// Parâmetros e macros do ADC.
#define ADC_CLOCK_DIV 96.f
#define SAMPLES 200 // Número de amostras que serão feitas do ADC.
#define ADC_ADJUST(x) (x * 3.3f / (1 << 12u) - 1.65f) // Ajuste do valor do ADC para Volts.
#define ADC_MAX 3.3f
#define ADC_STEP (3.3f/5.f) // Intervalos de volume do microfone.
#define V_REF 0.006f // Tensão mínima mensurável (~6mV como referência)


#define abs(x) ((x < 0) ? (-x) : (x))

// Canal e configurações do DMA
uint dma_channel;
dma_channel_config dma_cfg;

float sample_mic();
//float mic_power();
uint8_t get_intensity(float v);
float convert_to_db(float voltage_rms);


// Habilitar o microfone
void microphone_init(){
    adc_gpio_init(MIC_PIN);
    adc_init();
}

// Ler o valor do microfone
int microphone_read(){
  adc_select_input(MIC_CHANNEL);

  adc_fifo_setup(
      true, // Habilitar FIFO
      true, // Habilitar request de dados do DMA
      1, // Threshold para ativar request DMA é 1 leitura do ADC
      false, // Não usar bit de erro
      false // Não fazer downscale das amostras para 8-bits, manter 12-bits.
    );
  
    adc_set_clkdiv(ADC_CLOCK_DIV);
  
    // Tomando posse de canal do DMA.
    dma_channel = dma_claim_unused_channel(true);
  
    // Configurações do DMA.
    dma_cfg = dma_channel_get_default_config(dma_channel);
  
    channel_config_set_transfer_data_size(&dma_cfg, DMA_SIZE_16); // Tamanho da transferência é 16-bits (usamos uint16_t para armazenar valores do ADC)
    channel_config_set_read_increment(&dma_cfg, false); // Desabilita incremento do ponteiro de leitura (lemos de um único registrador)
    channel_config_set_write_increment(&dma_cfg, true); // Habilita incremento do ponteiro de escrita (escrevemos em um array/buffer)
    
    channel_config_set_dreq(&dma_cfg, DREQ_ADC); // Usamos a requisição de dados do ADC

    float soma = 0;
    for(int i = 0; i < COLETAS; i++){
      // Pega a potência média da amostragem do microfone.
      float avg = sample_mic();//mic_power();
      avg = 2.f * abs(ADC_ADJUST(avg)); // Ajusta para intervalo de 0 a 3.3V. (apenas magnitude, sem sinal)

      float decibels = convert_to_db(avg);
      //uint intensity = get_intensity(avg); // Calcula intensidade a ser mostrada na matriz de LEDs.
      soma += decibels;
      // Envia a intensidade e a média das leituras do ADC por serial.
  }
  dma_channel_unclaim(dma_channel); // Soltando a posse do canal DMA
  soma /= COLETAS;
  soma = round(soma); // Faz a média e arredonda
  return (int) soma;
}

/**
 * Realiza as leituras do ADC e armazena os valores no buffer.
 */
float sample_mic() {
  adc_fifo_drain(); // Limpa o FIFO do ADC.
  adc_run(false); // Desliga o ADC (se estiver ligado) para configurar o DMA.

  // Buffer de amostras do ADC.
  uint16_t adc_buffer[SAMPLES];

  dma_channel_configure(dma_channel, &dma_cfg,
    adc_buffer, // Escreve no buffer.
    &(adc_hw->fifo), // Lê do ADC.
    SAMPLES, // Faz SAMPLES amostras.
    true // Liga o DMA.
  );

  // Liga o ADC e espera acabar a leitura.
  adc_run(true);
  dma_channel_wait_for_finish_blocking(dma_channel);
  
  // Acabou a leitura, desliga o ADC de novo.
  adc_run(false);

  float avg = 0.f;

  for (uint i = 0; i < SAMPLES; ++i)
    avg += adc_buffer[i] * adc_buffer[i];
  
  avg /= SAMPLES;
  return sqrt(avg);
}

/**
 * Calcula a intensidade do volume registrado no microfone, de 0 a 4, usando a tensão.
 */
uint8_t get_intensity(float v) {
  uint count = 0;

  while ((v -= ADC_STEP/20) > 0.f)
    ++count;
  
  return count;
}

// Converte de volts pra dB
float convert_to_db(float voltage_rms) {
    if (voltage_rms <= V_REF) {
        return 0.0; // Evita log de zero ou valores negativos irreais
    }
    return 20.0 * log10(voltage_rms / V_REF);
}