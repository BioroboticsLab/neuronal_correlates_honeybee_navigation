#include "mcp3903_adc.h"
#include "main.h" // TODO think about removing this to make it more general

// Reads a 24 bit register of the ADC
uint32_t readRegister_adc_MCP3903e_continuously(SPI_HandleTypeDef hspi_adc, uint8_t reg){
	uint8_t spiRxBuff[3];
	spiRxBuff[0] = 0;
	spiRxBuff[1] = 0;
	spiRxBuff[2] = 0;

	uint8_t Byte_Nastr=0;	
	HAL_SPI_TransmitReceive(&hspi_adc, &Byte_Nastr, spiRxBuff, 3, 50); // Receive 3 bytes (24 Bit from  ADC register)
	
	uint8_t spiRxBuff_b0 = spiRxBuff[0];
	uint8_t spiRxBuff_b1 = spiRxBuff[1];
	uint8_t spiRxBuff_b2 = spiRxBuff[2];
	
	uint32_t registerValue = spiRxBuff_b0 << 16;// + spiRxBuff_b1 << 8 + spiRxBuff_b0;
	registerValue += spiRxBuff_b1 << 8;
	registerValue += spiRxBuff_b2;
	return registerValue;
}

void init_adc_continuousReading_config(SPI_HandleTypeDef hspi_adc){
	init_adc_MCP3903e(hspi_adc, OSR_64); // OSR_256: 24 bit OSR_64: 20 bit // Todo Put resolution in config
	//setGain(hspi_adc, 0, GAIN_1, 0);
	for (int channel_i = 0; channel_i<6; channel_i++){
		setGain(hspi_adc, channel_i, GAIN_1, 1);
	}
	uint8_t reg = 0;
	uint8_t cmdByte = DEVICE_ADDR | reg <<1 | 1;
	//__HAL_SPI_ENABLE(&hspi_adc);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); // CS for SPI1 GPIO ON 0
	HAL_SPI_Transmit(&hspi_adc,&cmdByte,1,50);	
}

// Readout all 6 Channels of the ADC and returns the values
// The rsult is a 18 byte long array (3 bytes per 24 bit value for one channel)
int readoutADC_continuously(SPI_HandleTypeDef hspi_adc, uint8_t *adc_values){
		for (int ch=0; ch<6; ch++){ // iterate throw registers (0-5 are channels)
			uint32_t reg_val;
			//DWT_Delay_us(1000);
			reg_val = readRegister_adc_MCP3903e_continuously(hspi_adc, ch);
			

			//print_uart2("..........................................................................................................................");
			// TODO Give the ADC time to meaure
			/*for (volatile int  noops = 0; noops >240000000; noops++){
				noops = noops;
			}		*/
			uint8_t reg_val_0 = (reg_val >> 16)  & 0xFF;
			uint8_t reg_val_1 = (reg_val >> 8)  & 0xFF;
			uint8_t reg_val_2 = reg_val & 0xFF; // Last 8 bits		
			adc_values[ch*3] = reg_val_0;
			adc_values[ch*3+1] = reg_val_1;
			adc_values[ch*3+2] = reg_val_2; // Last 8 bits
		}
		return 1;
}

// test function
void testADC(SPI_HandleTypeDef hspi_adc){
	init_adc_MCP3903e(hspi_adc, OSR_256); // OSR_256: 24 bit
	for (int channel_i = 0; channel_i<6; channel_i++){
		setGain(hspi_adc, channel_i, GAIN_1, 1);
	}
	uint8_t reg = 0;
	uint8_t cmdByte = DEVICE_ADDR | reg <<1 | 1;
	//__HAL_SPI_ENABLE(&hspi_adc);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); // CS for SPI1 GPIO ON 0
	HAL_SPI_Transmit(&hspi_adc,&cmdByte,1,50);	
	for (int k=0; k<10000000; k++){
		for (int ch=0; ch<6; ch++){ // iterate throw registers (0-5 are channels)
			char str_for_print[15];
			sprintf(str_for_print, "reg: %d", ch);
			print_uart2(str_for_print);
			//writeRegister_adc_MCP3903e(i, 1234567891);
			//print_uart2("ADC write done\r\n");
			
			/*uint8_t cmdByte = DEVICE_ADDR | reg <<1 | 1;
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); // CS for SPI1 GPIO ON 0
			HAL_SPI_Transmit(&hspi1,&cmdByte,1,50); //HAL_SPI_Transmit(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size, uint32_t Timeout)
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); // CS for SPI1 GPIO ON 1*/
			
			uint32_t reg_val;			
			reg_val = readRegister_adc_MCP3903e(hspi_adc, ch);	
			char str_for_test_print[10];
			sprintf(str_for_test_print, "%u", reg_val);
			print_uart2(" val: ");
			print_uart2(str_for_test_print);
			print_uart2("    ");
		}
		print_uart2("\r\n");
	}
}

void init_adc_std_config(SPI_HandleTypeDef hspi_adc){
	init_adc_MCP3903e(hspi_adc, OSR_32); // OSR_256: 24 bit // Todo Put resolution in config
	//setGain(hspi_adc, 0, GAIN_1, 0);
	for (int channel_i = 0; channel_i<6; channel_i++){
		setGain(hspi_adc, channel_i, GAIN_1, 1);
	}
}



// Readout all 6 Channels of the ADC and returns the values
// The rsult is a 18 byte long array (3 bytes per 24 bit value for one channel)
int readoutADC(SPI_HandleTypeDef hspi_adc, uint8_t *adc_values){
		for (int ch=0; ch<6; ch++){ // iterate throw registers (0-5 are channels)
			uint32_t reg_val;
			reg_val = readRegister_adc_MCP3903e(hspi_adc, ch);
			//HAL_Delay(10); // Needs interrupts
			/*adc_values[ch*3] = (reg_val >> 16)  & 0xFF;
			adc_values[ch*3+1] = (reg_val >> 8)  & 0xFF;
			adc_values[ch*3+2] = reg_val & 0xFF; // Last 8 bits*/
			
			// TODO IF this is removed array with channels contains just one channel
			// testprint begin
			/*char str_for_print[20];
			sprintf(str_for_print, "send=%u", reg_val);
			//sprintf(str_for_print, "s=%u, %u, %u", reg_val_0, reg_val_1, reg_val_2);
			print_uart2(str_for_print); */
			//print_uart2("  # ");
			// testprint end
			
			// TODO Give the ADC time to meaure
			for (volatile int  noops = 0; noops >24000000; noops++){
				noops = noops;
			}
			
			uint8_t reg_val_0 = (reg_val >> 16)  & 0xFF;
			uint8_t reg_val_1 = (reg_val >> 8)  & 0xFF;
			uint8_t reg_val_2 = reg_val & 0xFF; // Last 8 bits
			
			adc_values[ch*3] = reg_val_0;
			adc_values[ch*3+1] = reg_val_1;
			adc_values[ch*3+2] = reg_val_2; // Last 8 bits
		}
		return 1;
}

// Write 24 bit to the ADC register reg
void writeRegister_adc_MCP3903e(SPI_HandleTypeDef hspi_adc, uint8_t reg, unsigned long data){
	uint8_t cmdByte = DEVICE_ADDR | reg <<1;
	uint8_t b2 = (data & 0xff0000) >> 16;
	uint8_t b1 = (data & 0x00ff00) >> 8;
	uint8_t b0 = data & 0x0000ff;
	
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); // CS for SPI1 GPIO ON 0
	HAL_SPI_Transmit(&hspi_adc,&cmdByte,1,50); //HAL_SPI_Transmit(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size, uint32_t Timeout)
	HAL_SPI_Transmit(&hspi_adc,&b2,1,50); // 24 bit jeweils als 8 bit // TODO put it in one transmission
	HAL_SPI_Transmit(&hspi_adc,&b1,1,50);
	HAL_SPI_Transmit(&hspi_adc,&b0,1,50);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); // CS for SPI1 GPIO ON 1
}

// Reads a 24 bit register of the ADC
uint32_t readRegister_adc_MCP3903e(SPI_HandleTypeDef hspi_adc, uint8_t reg){
	uint8_t cmdByte = DEVICE_ADDR | reg <<1 | 1;
	//uint8_t r = 0;
	
	//__HAL_SPI_ENABLE(&hspi_adc);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); // CS for SPI1 GPIO ON 0
	
	uint8_t spiRxBuff[3];
	spiRxBuff[0] = 0;
	spiRxBuff[1] = 0;
	spiRxBuff[2] = 0;
	
	uint8_t Byte_Nastr=0;
	HAL_SPI_Transmit(&hspi_adc,&cmdByte,1,50);
	
	HAL_SPI_TransmitReceive(&hspi_adc, &Byte_Nastr, spiRxBuff, 3, 50); // Receive 3 bytes (24 Bit from  ADC register)
	/*HAL_SPI_TransmitReceive(&hspi_adc, 0x0, &spiRxBuff_b2, 1, 50); // Receive 1 byte (8 Bit from  ADC register)
	HAL_SPI_TransmitReceive(&hspi_adc, 0x0, &spiRxBuff_b1, 1, 50); // Receive 1 byte (8 Bit from  ADC register)
	HAL_SPI_TransmitReceive(&hspi_adc, 0x0, &spiRxBuff_b0, 1, 50); // Receive 1 byte (8 Bit from  ADC register)*/
	
	// testprints
	/*
	char str_for_print_0[10];
	sprintf( str_for_print_0, "%u", spiRxBuff[0]);
	print_uart2("  |||  spiRxBuff[0]: ");
	print_uart2(str_for_print_0);
	char str_for_print_1[10];
	sprintf( str_for_print_1, "%u", spiRxBuff[1]);
	print_uart2("  spiRxBuff[1]: ");
	print_uart2(str_for_print_1);
	char str_for_print_2[10];
	sprintf( str_for_print_2, "%u", spiRxBuff[2]);
	print_uart2("  spiRxBuff[2]: ");
	print_uart2(str_for_print_2);
	print_uart2("\r\n");*/
	
	uint8_t spiRxBuff_b0 = spiRxBuff[0];
	uint8_t spiRxBuff_b1 = spiRxBuff[1];
	uint8_t spiRxBuff_b2 = spiRxBuff[2];
	
	uint32_t registerValue = spiRxBuff_b0 << 16;// + spiRxBuff_b1 << 8 + spiRxBuff_b0;
	registerValue += spiRxBuff_b1 << 8;
	registerValue += spiRxBuff_b2;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); // CS for SPI1 GPIO ON 1
	return registerValue;
}

// Sets CS to 1 and sets ADC to 16 bit mode
void init_adc_MCP3903e(SPI_HandleTypeDef hspi_adc, uint8_t osr){
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); // CS for SPI1 GPIO ON 1
	
	// Change from 16 to 24 bit mode	
	uint32_t cmd_forStatusReg = 0x9fc000;
	writeRegister_adc_MCP3903e(hspi_adc, REG_STATUS_COMM, cmd_forStatusReg);
	
	uint32_t cmd1 = 0xfc0fd0;
	uint32_t cmd2 = 0x000fc0 | osr << 4;
	
	writeRegister_adc_MCP3903e(hspi_adc, REG_CONFIG, cmd1);
	writeRegister_adc_MCP3903e(hspi_adc,  REG_CONFIG, cmd2);
	

}


void setGain(SPI_HandleTypeDef hspi_adc, uint8_t channel, uint8_t gain, uint8_t boost){
	uint32_t r = readRegister_adc_MCP3903e(hspi_adc, REG_GAIN);
	
	uint8_t idx = channel * 4;
	uint32_t chGain = 0;
	
	if (channel % 2 == 0) //0, 2, 4
	{
		chGain = (boost << 3) | gain;
	}
	else //1, 3, 5 
	{
		chGain = boost | (gain << 1);
	}
	
	r &= ~(0xf << idx);
	r |= chGain << idx;

	writeRegister_adc_MCP3903e(hspi_adc,  REG_GAIN, r);
	
}
