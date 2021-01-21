#include "main.h"
#include "i2c_master.h"
#include "ssd1306.h"
#include "Adc.h"
#include "PWMController.h"
#include "apds9960.h"
#include "UartReceiver.h"

#define RCC_GPIOA_Enable 1
#define RCC_GPIOC_Enable 1 << 2

#define CONFIG_LENGTH 25
#define COLOR_TEXT_LENGTH 11
#define COLOR_VALUE_LENGTH 12
#define MISC_CONFIG_LENGTH 2

void SystemClock_Config(void);

// Display
const uint8_t DISPLAY_ADDR = 0x3D;
const uint8_t WIDTH = 128;
const uint8_t HEIGHT = 64;
const uint8_t PIXEL_PER_ROW = 8;

// Color Myserty
const uint8_t ADC_CHANNEL_RED = 0;
const uint8_t ADC_CHANNEL_GREEN = 1;
const uint8_t ADC_CHANNEL_BLUE = 4;
const uint8_t PWM_PIN_RED = 0;
const uint8_t PWM_PIN_GREEN = 1;
const uint8_t PWM_PIN_BLUE = 2;

uint16_t redMin = 800;
uint16_t redMax = 1024;
uint16_t greenMin = 0;
uint16_t greenMax = 50;
uint16_t blueMin = 20;
uint16_t blueMax = 300;
uint8_t targetProximity = 150;
uint8_t targetBrightness = 100;

std::string color = "pinkes     ";

// Sensor
const uint8_t SENSOR_ADDR = 0x39;

void configureValues();
void wakeupMystery();
void colorMystery();
void sleepMystery();

I2C_Master *i2c = nullptr;

// I2C Lese- und Schreibefunktionen, angepasst für jeweilige Komponente
int displayWrite(const char *buf, uint32_t len) {
	i2c->write(DISPLAY_ADDR, (uint8_t*) buf, len);
	return 0;
}

int sensorWrite(uint8_t *buf, uint32_t len) {
	i2c->write(SENSOR_ADDR, buf, len);
	return 0;
}

int sensorRead(uint8_t *buf, uint32_t len) {
	i2c->read(SENSOR_ADDR, buf, len);
	return 0;
}

uint16_t measureLed(Adc *adc, int channel) {
	int sum = 0;
	for (int i = 0; i < 10; i++) {
		sum += adc->getMeasurement(channel);
	}
	return (uint16_t) (sum / 10);
}

//verwendete Komponenten
SSD1306<WIDTH, HEIGHT, PIXEL_PER_ROW, displayWrite> *display = nullptr;
APDS9960<sensorWrite, sensorRead> *sensor = nullptr;
Adc *adc = nullptr;
PWMController *pwm = nullptr;
PWMPin *pinRed = nullptr;
PWMPin *pinGreen = nullptr;
PWMPin *pinBlue = nullptr;
UartReceiver *rcv = nullptr;

int main(void) {

	// Initialisierung aller Komponenten
	HAL_Init();

	SystemClock_Config();

	RCC->AHBENR |= RCC_GPIOA_Enable | RCC_GPIOC_Enable;

	i2c = new I2C_Master();
	i2c->init();

	sensor = new APDS9960<sensorWrite, sensorRead>;
	sensor->init();
	sensor->powerOn();
	sensor->proximityOn();
	sensor->alsOn();

	display = new SSD1306<WIDTH, HEIGHT, PIXEL_PER_ROW, displayWrite>;
	display->init();
	display->clearDisplay();

	adc = new Adc();
	adc->init();

	pwm = PWMController::getInstance(true);
	pinRed = pwm->addPin(GPIOC, PWM_PIN_RED);
	pinGreen = pwm->addPin(GPIOC, PWM_PIN_GREEN);
	pinBlue = pwm->addPin(GPIOC, PWM_PIN_BLUE);

	pinRed->toggleOff(true);
	pinBlue->toggleOff(true);
	pinGreen->toggleOff(true);

	rcv = UartReceiver::getInstance();
	rcv->init();

	while (1) {
		// In Konfigurationsroutine springen, wenn UserButton auf uC gedürckt
		if ((GPIOC->IDR & (1 << 13)) == 0) {
			configureValues();
		}
		// Abfolge der Rätsel
		wakeupMystery();
		colorMystery();
		sleepMystery();
	}

}

// 16 Bit Wert (Little Endian) aus Konfiguration (buffer) in Variable speicheren
uint8_t set16BitValue(uint16_t *variableToSet, uint8_t *buffer, uint8_t idx) {
	uint8_t value = *(buffer + idx);
	*variableToSet = value << 8;
	idx++;
	value = *(buffer + idx);
	*variableToSet += value;
	idx++;
	return idx;
}

// Konfigurationsroutine
void configureValues() {
	display->displayText("Waiting for     config");

	// Farbtext einlesen
	uint8_t buffer[CONFIG_LENGTH] = { 0 };
	rcv->pollForData(buffer, COLOR_TEXT_LENGTH);
	color = std::string((char*) buffer);

	// Farbwerte einlesen
	rcv->pollForData(buffer, COLOR_VALUE_LENGTH);
	uint8_t index = 0;
	index = set16BitValue(&redMin, buffer, index);
	index = set16BitValue(&redMax, buffer, index);
	index = set16BitValue(&greenMin, buffer, index);
	index = set16BitValue(&greenMax, buffer, index);
	index = set16BitValue(&blueMin, buffer, index);
	set16BitValue(&blueMax, buffer, index);

	// Helligkeit und Annäherung einlesen
	rcv->pollForData(buffer, MISC_CONFIG_LENGTH);
	targetProximity = buffer[0];
	targetBrightness = buffer[1];

	display->clearDisplay();
}

// Rätsel 1: Aufwachen
void wakeupMystery() {
	bool sleeping = true;

	// Warten, bis der Sensor abgedeckt ist
	while (sensor->getProximityData() < targetProximity);

	// Solange Senor abgedeckst -> Z aufs Display schreiben
	while (sleeping) {
		for (int i = 0; i < 8 * 16 && sleeping; i++) {
			display->displayCharacter('Z');
			sleeping = sensor->getProximityData() > targetProximity;
			HAL_Delay(200);
		}
		display->clearDisplay();
	}

	// Nach aufdecken des Sensors, Licht anfordern
	display->displayText(
			"Ich will noch   nicht aufstehen Es ist ja noch  dunkel draussen");
	sleeping = true;

	// Auf Licht warten
	while (sleeping) {
		sleeping = sensor->getAlsClearData() < targetBrightness;
	}

	display->clearDisplay();
}

// Rätsel 2: Lieblings-T-Shirt
void colorMystery() {
	display->displayText("Moooooin Meister");
	pwm->startTimer(); // LED anschalten
	HAL_Delay(2000);
	std::string shirtText = "Kannst du mir   mein " + color + "T-Shirt geben";
	display->displayText(shirtText);

	bool colorMatch = false;

	while (!colorMatch) {
		// Status der Potis einlesen und Helligkeit der LEDs entsprechend setzen
		*pinRed->dutyCycle = measureLed(adc, ADC_CHANNEL_RED);
		*pinGreen->dutyCycle = measureLed(adc, ADC_CHANNEL_GREEN);
		*pinBlue->dutyCycle = measureLed(adc, ADC_CHANNEL_BLUE);
		if ((GPIOC->IDR & 0b1000) == 0) {
			// Prüfen, ob gewünschter Farbwert erreicht
			bool redMatch = *pinRed->dutyCycle >= redMin
					&& *pinRed->dutyCycle <= redMax;
			bool blueMatch = *pinBlue->dutyCycle >= blueMin
					&& *pinBlue->dutyCycle <= blueMax;
			bool greenMatch = *pinGreen->dutyCycle >= greenMin
					&& *pinGreen->dutyCycle <= greenMax;
			colorMatch = redMatch && blueMatch && greenMatch;

			if (!colorMatch) {
				// Beschwerde, falls Farbe falsch
				display->clearDisplay();
				display->displayText("Nein das passt  noch nicht ganz ");
				display->displayText(shirtText);
			}
		}
	}
	pwm->stopTimer();
}

// Rätsel 3: Einschlafen
void sleepMystery() {
	display->clearDisplay();
	display->displayText(
			"Vielen Dank     Ich bin aber    schon wieder    muede           Kannst du mich  wieder schlafen legen?");

	// Auf zudecken warten
	while (sensor->getProximityData() < targetProximity);

	// Erfolgreichen Abschluss bestätigen
	display->clearDisplay();
	display->displayText(
			"Herzlichen      Glueckwunsch    Du hast alle    Raetsel geloest");
	for (int i = 0; i < 10; i++) {
		pinGreen->toggleOn(true);
		HAL_Delay(500);
		pinGreen->toggleOff(true);
		HAL_Delay(500);
	}

	display->clearDisplay();
}

void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL4;
	RCC_OscInitStruct.PLL.PLLDIV = RCC_PLL_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
		Error_Handler();
	}
}

void Error_Handler(void) {

	__disable_irq();
	while (1) {
	}

}

#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{

}
#endif /* USE_FULL_ASSERT */
