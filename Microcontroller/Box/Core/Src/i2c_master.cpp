#include "i2c_master.h"

void I2C_Master::init() {
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN_Msk;

  GPIOB->MODER &= ~GPIO_MODER_MODER8_0;
  GPIOB->MODER |= GPIO_MODER_MODER8_1;
  GPIOB->OTYPER |= GPIO_OTYPER_OT_8;
  GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR8_Msk;
  GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR8_Msk;

  GPIOB->MODER &= ~GPIO_MODER_MODER9_0;
  GPIOB->MODER |= GPIO_MODER_MODER9_1;
  GPIOB->OTYPER |= GPIO_OTYPER_OT_9;
  GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR9_Msk;
  GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR9_Msk;

	GPIOB->AFR[1] = GPIOB->AFR[1] & ~GPIO_AFRH_AFSEL8 | (4UL << GPIO_AFRH_AFSEL8_Pos);
	GPIOB->AFR[1] = GPIOB->AFR[1] & ~GPIO_AFRH_AFSEL9 | (4UL << GPIO_AFRH_AFSEL9_Pos);
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN_Msk;

	I2C1->CR2 |=  I2C_CR2_ITEVTEN_Msk | I2C_CR2_ITERREN_Msk | I2C_CR2_ITBUFEN_Msk | 0x02;
	I2C1->CCR = 0x50;  //I2C Frequenz = 100kHz
	I2C1->TRISE = 0x65; //Berechnung: siehe Datasheet
	I2C1->CR1 |= I2C_CR1_PE_Msk;
}
void I2C_Master::write(uint8_t addr, uint8_t* data, uint32_t len, bool repeated_start) {
	uint32_t sr1 = 0;
	uint32_t sr2 = 0;
	uint32_t i = 0;
	I2C1->CR1 |= I2C_CR1_START_Msk;
	 while(!(I2C1->SR1 & I2C_SR1_SB_Msk));
	I2C1->DR = addr << 1;
	 while(!(I2C1->SR1 & I2C_SR1_ADDR_Msk));
	sr2 = I2C1->SR2;
	while(i++ < len) {
		I2C1->DR = data[i - 1];
		 while(!(I2C1->SR1 & I2C_SR1_TXE_Msk));
		if(I2C1->SR1 & I2C_SR1_BTF_Msk) {
			I2C1->DR;
		}
	}

	if(repeated_start) {
		I2C1->CR1 |= I2C_CR1_START_Msk;
		 while(!(I2C1->SR1 & I2C_SR1_SB_Msk));
	} else {
		I2C1->CR1 |= I2C_CR1_STOP_Msk;
	}
}
void I2C_Master::read(uint8_t addr, uint8_t* data, uint32_t len, bool stop_cond) {
	uint32_t sr1 = 0;
	uint32_t sr2 = 0;
	uint32_t i = 0;
	if(len > 1) {
		I2C1->CR1 |= I2C_CR1_ACK_Msk;
	} else {
		I2C1->CR1 &= ~I2C_CR1_ACK_Msk;
	}

	if(!stop_cond) {
		I2C1->CR1 |= I2C_CR1_START_Msk;
	}

	while(!(I2C1->SR1 & I2C_SR1_SB_Msk));
	I2C1->DR = addr << 1 | 1;
	while(!(I2C1->SR1 & I2C_SR1_ADDR_Msk));
	I2C1->SR1;
	I2C1->SR2;
	while(i++ < len) {
		while(!(I2C1->SR1 & I2C_SR1_RXNE_Msk));
		if(i == len - 1) {
			I2C1->CR1 &= ~I2C_CR1_ACK_Msk;
		}
		data[i - 1] = I2C1->DR;
		if(I2C1->SR1 & I2C_SR1_BTF_Msk) {
			I2C1->DR;
		}
	}

	if(stop_cond) {
		I2C1->CR1 |= I2C_CR1_STOP_Msk;
	}
}
