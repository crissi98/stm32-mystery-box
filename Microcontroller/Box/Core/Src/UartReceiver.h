#ifndef SRC_UARTRECEIVER_H_
#define SRC_UARTRECEIVER_H_

#include <stm32l1xx_hal.h>
#include <string.h>

#define RCC_EnableUSART2 1 << 17
#define USART_Enable 1 << 13
#define USART_EnableReceiver 1 << 2
#define USART_EnableTransmitter 1 << 3
#define USART_EnableRcvInterupt 1 << 5
#define GPIOA_Mode_AlternateFunction 0b10
#define GPIO_AlternateFunction_USART 0x7

#define MAX_RCV_LENGTH 64

class UartReceiver {
private:
	static UartReceiver *instance; // Singleton
	uint8_t rcvCount;
	uint8_t rcvBuffer[MAX_RCV_LENGTH];
	UartReceiver() :
			rcvCount(0), rcvBuffer { 0 } {
	}

public:
	static UartReceiver* getInstance() {
		if (instance == nullptr) {
			instance = new UartReceiver;
		}
		return instance;
	}

	void init() {
		// Clock für UART aktivieren
		RCC->APB1ENR |= RCC_EnableUSART2;
		// USART ausschalten
		CLEAR_BIT(USART2->CR1, USART_Enable);
		// GPIO-Ports als UART konfigurieren
		GPIOA->MODER |= GPIOA_Mode_AlternateFunction << 4 | GPIOA_Mode_AlternateFunction << 6;
		GPIOA->AFR[0] |= GPIO_AlternateFunction_USART << 8 | GPIO_AlternateFunction_USART << 12;
		GPIOA->OSPEEDR |= GPIO_SPEED_HIGH << 4 | GPIO_SPEED_HIGH << 6;

		// Receiver anschalten
		USART2->CR1 |= USART_EnableReceiver | USART_EnableRcvInterupt;
		USART2->BRR = 0xd05; //Baudrate = 9600 Bit/s
		USART2->CR1 |= USART_Enable;


		NVIC_EnableIRQ(USART2_IRQn);
	}

	void handleRcv() {
		// empfangene Daten in Buffer speichern
		rcvBuffer[rcvCount] = (uint8_t) (USART2->DR);
		rcvCount++;
	}


	void pollForData(uint8_t *buffer, uint8_t length) {
		// auf Daten warten
		while (rcvCount < length);
		// Daten in Ergebnis-Buffer kopieren
		memcpy(buffer, rcvBuffer, (char)length);
		// restliche Daten an Anfang des Empfaungs-Buffers schieben
		rcvCount -=length;
		memmove(rcvBuffer, rcvBuffer + length, rcvCount);
	}

};

UartReceiver *UartReceiver::instance = nullptr;

extern "C" void USART2_IRQHandler(void) {
	UartReceiver::getInstance()->handleRcv();
	// Interrupt quittieren
	USART2->DR = 0;
}


#endif /* SRC_UARTRECEIVER_H_ */
