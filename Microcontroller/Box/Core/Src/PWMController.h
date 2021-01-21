#define MAX_PINS 4
#define HCLK_MHz 32

// Enable Timer 2 in RCC
#define Timer2Enable 1
// Bits to indicate Interupt
#define TimerUpdateInterupt 1
#define TimerCompare1Interupt 1 << 1
#define TimerCompare2Interupt 1 << 2
#define TimerCompare3Interupt 1 << 3
#define TimerCompare4Interupt 1 << 4
#define TimerRun 1

#ifndef PWM_PWMCONTROLLER_H_
#define PWM_PWMCONTROLLER_H_
#include "stm32l1xx.h"

#define GpioModeOutput 0b01
#define GpioTypePushPull 0b0
#define GpioSpeedLow 0b00
#define GpioPullUpDownNone 0b00

class PWMPin {
private:
	GPIO_TypeDef *port;
	uint8_t pinNumber;
public:
	volatile uint32_t *dutyCycle;
	PWMPin(GPIO_TypeDef *_port, uint8_t _pinNumber,
			volatile uint32_t *_dutyCycle) :
			port(_port), pinNumber(_pinNumber), dutyCycle(_dutyCycle) {
		// Pins als Output setzen
		port->MODER |= GpioModeOutput << pinNumber * 2;

		// Output-Type Push-Pull setzen
		port->OTYPER |= GpioTypePushPull << pinNumber;

		// Output-Speed auf LOW setzen
		port->OSPEEDR |= GpioSpeedLow << pinNumber * 2;

		// kein Pull-Up/-Down
		port->PUPDR |= GpioPullUpDownNone << pinNumber * 2;
	}

	void toggleOff(bool isManuallyControlled = false) {
		if (isManuallyControlled || *dutyCycle < 1023) {
			port->ODR &= 0xFFFF - (1 << pinNumber);
		}
	}
	void toggleOn(bool isManuallyControlled = false) {
		if (isManuallyControlled || *dutyCycle > 5) {
			port->ODR |= 1 << pinNumber;
		}
	}

	~PWMPin() {
		toggleOff();
	}
}
;

class PWMController {
private:
	static PWMController *instance; //Singleton

	uint8_t currentPinCount;
	PWMPin *pins[MAX_PINS];

	PWMController() :
			currentPinCount(0), pins { nullptr } {
	}

	void initTimer(bool debug) {
		if (debug) {
			// Timer Stop bei Debugger-Stop
			DBGMCU->CR |= 0b110;
			DBGMCU->APB1FZ |= 1;
		}

		// Clock für Timer 2 aktivieren
		RCC->APB1ENR |= Timer2Enable;

		//Prescaler fuer Frequenz 1MHz setzen
		TIM2->PSC = HCLK_MHz - 1;

		// 10-Bit Aufloesung fuer PWM einstellen
		TIM2->ARR = 1024 - 1;

		// Capture fuer Pins aktivieren
		TIM2->EGR |= TimerUpdateInterupt | TimerCompare1Interupt
				| TimerCompare2Interupt | TimerCompare3Interupt;
		TIM2->CCR1 = 0;
		TIM2->CCR2 = 0;
		TIM2->CCR3 = 0;
		TIM2->CCR4 = 0;

		// Interupt bei Overflow und Caktivieren
		TIM2->DIER |= TimerUpdateInterupt | TimerCompare1Interupt
				| TimerCompare2Interupt | TimerCompare3Interupt;
	}

public:
	static PWMController* getInstance(bool debug = false) {
		if (PWMController::instance == nullptr) {
			PWMController::instance = new PWMController;
			PWMController::instance->initTimer(debug);
		}
		return PWMController::instance;
	}

	PWMPin* addPin(GPIO_TypeDef *port, uint8_t pinNumber) {
		volatile uint32_t *dutyCycle;
		switch (currentPinCount) {
		case 0:
			dutyCycle = &TIM2->CCR1;
			break;
		case 1:
			dutyCycle = &TIM2->CCR2;
			break;
		case 2:
			dutyCycle = &TIM2->CCR3;
			break;
		case 3:
			dutyCycle = &TIM2->CCR4;
			break;
		default:
			dutyCycle = nullptr;
		}
		PWMPin *pin = new PWMPin(port, pinNumber, dutyCycle);
		pins[currentPinCount] = pin;
		currentPinCount++;
		return pin;
	}

	void handleInterupt(uint32_t status) {
		if (status & TimerUpdateInterupt) {
			for (int i = 0; i < currentPinCount; ++i) {
				pins[i]->toggleOn();
			}
		}

		if (status & TimerCompare1Interupt && currentPinCount > 0) {
			pins[0]->toggleOff();
		}

		if (status & TimerCompare2Interupt && currentPinCount > 1) {
			pins[1]->toggleOff();
		}

		if (status & TimerCompare3Interupt && currentPinCount > 2) {
			pins[2]->toggleOff();
		}

		if (status & TimerCompare4Interupt && currentPinCount > 3) {
			pins[3]->toggleOff();
		}
	}

	void startTimer() {
		// Timer-Interupt bei NVIC aktivieren
		NVIC_EnableIRQ(TIM2_IRQn);
		// Timer Starten
		TIM2->CR1 |= TimerRun;
	}

	void stopTimer() {
		// Timer-Interupt bei NVIC aktivieren
		NVIC_DisableIRQ(TIM2_IRQn);
		// Timer Starten
		CLEAR_BIT(TIM2->CR1, TimerRun);

		for (int i = 0; i < currentPinCount; i++) {
			pins[i]->toggleOff(false);
		}
	}
};

PWMController *PWMController::instance = nullptr;

extern "C" void TIM2_IRQHandler() {
	uint32_t status = TIM2->SR;
	TIM2->SR = 0;
	PWMController::getInstance(true)->handleInterupt(status);
}

#endif /* PWM_PWMCONTROLLER_H_ */
