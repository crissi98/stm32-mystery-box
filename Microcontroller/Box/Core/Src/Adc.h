/*
 * AdcController.h
 *
 *  Created on: 7 Jan 2021
 *      Author: ap7u5
 */

#ifndef SRC_ADC_H_
#define SRC_ADC_H_

#include "stm32l1xx_hal.h"

#define HCLK_MHZ 64

#define RCC_EnableGPIOA 1
#define GPIO_AnalogInput 0b11

#define RCC_EnableADC 1 << 9
#define ADC_PowerOn 1
#define ADC_StatusOn 1 << 6
#define ADC_StatusStarted 1 << 3
#define ADC_Ready 1 << 8
#define ADC_Res10Bit 0b01 << 24
#define ADC_SequenceFinish 1 << 1
#define ADC_ConversionStart 1 << 30
#define ADC_SampleTimeLong 0b111
#define ADC_LengthOffset 20

class Adc {
private:
	int numberOfChannels;
	uint16_t results[4];
public:
	Adc(): numberOfChannels(0) {}
	void init() {
		// GPIO-Port und ADC aktivieren
		RCC->AHBENR |= RCC_EnableGPIOA;
		RCC->APB2ENR |= RCC_EnableADC;

		// lange Sample-Zeit für geanuere Messungen
		ADC1->SMPR3 |= ADC_SampleTimeLong;

		// ADC anschalten
		if (!ADC1->CR2 & ADC_PowerOn) {
			ADC1->CR2 |= ADC_PowerOn;
		}

}

	uint16_t getMeasurement(int channel) {

		ADC1->SQR5 = channel;

		while (ADC1->SR & ADC_Ready) {
			// Warten, bis bereit zum Lesen
		}


		// Konvertierung starten
		ADC1->CR2 |= ADC_ConversionStart;

		// Auf Ergebnis warten
		while ((ADC1->SR & ADC_SequenceFinish) == 0) {
			// you just wait sunshine...
		}

		// Ergebnis bestätigen
		ADC1->SR |= ADC_ConversionStart | ADC_StatusStarted;

		// Ergebnis auf 10-Bit normalisieren
		return (uint16_t)((ADC1->DR & 0b111111111111) >> 2);

	}

	virtual ~Adc() {
		// ADC Clock anhalten
		CLEAR_BIT(RCC->APB2ENR, RCC_EnableADC);
	};
};


#endif /* SRC_ADC_H_ */
