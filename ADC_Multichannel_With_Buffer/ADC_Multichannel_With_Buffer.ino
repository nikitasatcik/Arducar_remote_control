#include "MedianFilterLib.h"
#include <CircularBuffer.h>

/*
    Multichannel ADC with interrupts. Median filter (Phil Ekstrom’s method) is used for impulse noise reduction. 100 nF ceramic is recommended between Vcc and GND.
    Circular buffer is used to store last N values of ADC from 8 channels.
    Created: Sep 16, 2022, by Kolchiba.
*/

#define ADC_NOISE_VALUE 20

// Define the ADC channels used.  ADLAR will be zero.
#define ADCH0 ((1 << REFS0) | 0)
#define ADCH1 ((1 << REFS0) | 1)
#define ADCH2 ((1 << REFS0) | 2)
#define ADCH3 ((1 << REFS0) | 3)
#define ADCH4 ((1 << REFS0) | 4)
#define ADCH5 ((1 << REFS0) | 5)
#define ADCH6 ((1 << REFS0) | 6)
#define ADCH7 ((1 << REFS0) | 7)

// variable to switch ADC input
volatile int ADC_channel;

// Number of samples for the first chnannel A0.
int ADC_channel_samples;

int adc_output[8];

// buffer_size size for each ADC input
const uint16_t buffer_size = 25;

unsigned long start_time, stop_time;

MedianFilter<int> medianFilter(5);

CircularBuffer<int, buffer_size> ADC0_buffer;
CircularBuffer<int, buffer_size> ADC1_buffer;
CircularBuffer<int, buffer_size> ADC2_buffer;
CircularBuffer<int, buffer_size> ADC3_buffer;
CircularBuffer<int, buffer_size> ADC4_buffer;
CircularBuffer<int, buffer_size> ADC5_buffer;
CircularBuffer<int, buffer_size> ADC6_buffer;
CircularBuffer<int, buffer_size> ADC7_buffer;

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < 8; i++)
  {
    digitalWrite(A0 + i, HIGH); // enable internal pull-up on ADC pins
  }
  ADC_init();
}

void loop() {
  ADC_read();
  ADC_print_data();
}

void ADC_read() {
  if (ADC_channel_samples >= buffer_size)
  {
    using index_t = decltype(ADC0_buffer)::index_t;

    for (index_t i = 0; i < ADC0_buffer.size(); i++) {
      adc_output[0] = medianFilter.AddValue(ADC0_buffer[i]);
    }

    for (index_t i = 0; i < ADC0_buffer.size(); i++) {
      adc_output[1] = medianFilter.AddValue(ADC1_buffer[i]);
    }

    for (index_t i = 0; i < ADC0_buffer.size(); i++) {
      adc_output[2] = medianFilter.AddValue(ADC2_buffer[i]);
    }

    for (index_t i = 0; i < ADC0_buffer.size(); i++) {
      adc_output[3] = medianFilter.AddValue(ADC3_buffer[i]);
    }

    for (index_t i = 0; i < ADC0_buffer.size(); i++) {
      adc_output[4] = medianFilter.AddValue(ADC4_buffer[i]);
    }

    for (index_t i = 0; i < ADC0_buffer.size(); i++) {
      adc_output[5] = medianFilter.AddValue(ADC5_buffer[i]);
    }

    for (index_t i = 0; i < ADC0_buffer.size(); i++) {
      adc_output[6] = medianFilter.AddValue(ADC6_buffer[i]);
    }

    for (index_t i = 0; i < ADC0_buffer.size(); i++) {
      adc_output[7] = medianFilter.AddValue(ADC7_buffer[i]);
    }
    ADC_channel_samples = 0;
  }
}

void ADC_print_data() {
  for (int i = 0; i < 8; i++) {
    Serial.print("ADC_");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(adc_output[i]);
  }
}

void ADC_init() {
  // Set ADEN, prescale, and enable interrupts. Prescale values less than 32 produce inaccurate results when switching MUX.
  ADCSRA |= (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (0 << ADPS0) | (1 << ADEN);  // Prescale 64. Time = 60us  Freq = 19ksps.

  // Clear everything.  No free running because ADATE is clear in ADSRA.
  ADCSRB = 0;

  // Note, enabling ADC takes 12 ADC clocks to execute
  delay(100);

  // Enable global interrupts.
  sei();

  // Set ADSC to start ADC conversion.
  ADCSRA |= (1 << ADSC);
}

// ADC interrupted in single conversion mode
ISR(ADC_vect) {
  // Select all 8 ADC inputs
  ADC_channel = ADMUX & (B00000000 | B00000001 | B00000010 | B00000011 | B00000100 | B00000101 | B00000110 | B00000111 );

  // Select ADC Channel 0.
  ADMUX = ADCH0;

  volatile int raw_val;
  switch (ADC_channel) {
    case 0:
      raw_val = ADCL | (ADCH << 8);
      if (raw_val < ADC_NOISE_VALUE) raw_val = 0;
      ADC0_buffer.push(raw_val);

      // Control ADC samples by the first channel A0, because other channels will have same or higher number of samples.
      ADC_channel_samples++;
      // Select ADC Channel 1.
      ADMUX = ADCH1;
      break;
    case 1:
      raw_val = ADCL | (ADCH << 8);
      if (raw_val < ADC_NOISE_VALUE) raw_val = 0;
      ADC1_buffer.push(raw_val);
      // Select ADC Channel 2.
      ADMUX = ADCH2;
      break;
    case 2:
      raw_val = ADCL | (ADCH << 8);
      if (raw_val < ADC_NOISE_VALUE) raw_val = 0;
      ADC2_buffer.push(raw_val);
      // Select ADC channel 3.
      ADMUX = ADCH3;
      break;

    case 3:
      raw_val = ADCL | (ADCH << 8);
      if (raw_val < ADC_NOISE_VALUE) raw_val = 0;
      ADC3_buffer.push(raw_val);
      // Select ADC channel 4.
      ADMUX = ADCH4;
      break;

    case 4:
      raw_val = ADCL | (ADCH << 8);
      if (raw_val < ADC_NOISE_VALUE) raw_val = 0;
      ADC4_buffer.push(raw_val);
      // Select ADC channel 5.
      ADMUX = ADCH5;
      break;

    case 5:
      raw_val = ADCL | (ADCH << 8);
      if (raw_val < ADC_NOISE_VALUE) raw_val = 0;
      ADC5_buffer.push(raw_val);
      // Select ADC channel 6.
      ADMUX = ADCH6;
      break;
    case 6:
      raw_val = ADCL | (ADCH << 8);
      if (raw_val < ADC_NOISE_VALUE) raw_val = 0;
      ADC6_buffer.push(raw_val);
      // Select ADC channel 7.
      ADMUX = ADCH7;
      break;
    case 7:
      raw_val = ADCL | (ADCH << 8);
      if (raw_val < ADC_NOISE_VALUE) raw_val = 0;
      ADC7_buffer.push(raw_val);
      // Select ADC channel 0.
      ADMUX = ADCH0;
      break;
  }
  // Set ADSC to start the next ADC conversion.
  ADCSRA |= (1 << ADSC);
}
