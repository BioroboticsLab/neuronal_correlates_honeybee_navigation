// fdc1004_adc.c file

#include "fdc1004_adc.h"

// USER VARIABLES
uint8_t i2c_tx_buffer[5] = {0,0,0,0,0};  // I2C tx buffer
uint8_t i2c_rx_buffer[5] = {0,0,0,0,0};  // I2C rx buffer
uint16_t dummy_RegVal_uint16;            // dummy uint16 value for 2 byte read frame
uint32_t dummy_Val_uint32;							   // dummy uint32 value for 4 byte raw cap value 
int32_t dummy_Val_int32;							   // dummy int32 value for 4 byte raw cap value 


// GENERAL REG WRITE
/**
  * @brief  Writes any register for FDC1004 via I2C interface. 
  * @param  i2c_handle Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
	* @param  regAddress [uint8] configuration register address - no check for valid address!.
	* @param  regVal [uint16] configuration register value.
  * @retval HAL_Status enum [HAL_StatusTypeDef].
  */
HAL_StatusTypeDef fdc1004_write_register(I2C_HandleTypeDef i2c_handle, uint8_t regAddress, uint16_t regVal){
	HAL_StatusTypeDef status = HAL_OK;
	// TO-DO - regAddress check
	i2c_tx_buffer[0] = regAddress; // pointer value
	i2c_tx_buffer[1] = (regVal >> 8);// MSB byte
	i2c_tx_buffer[2] = (regVal & 0xFF);// LSB byte
	status = HAL_I2C_Master_Transmit(&i2c_handle,FDC1004_DevAddress<<1,i2c_tx_buffer,3,FDC1004_I2C_TIMEOUT);  
	return status;
}

// GENERAL REG READ
/**
  * @brief  Reads configuration register for channel 1 of FDC1004
  *                via I2C interface. 
  * @param  i2c_handle Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
	* @param  regAddress [uint8] configuration register address - no check for valid address!.
  * @retval Register Value [uint16] of configuration register value.
  */
uint16_t fdc1004_read_register(I2C_HandleTypeDef i2c_handle, uint8_t regAddress){
	// TO-DO - regAddress check
	i2c_tx_buffer[0] = regAddress; // set pointer value
	// MAYBE needed: HAL_Delay(4); 
	HAL_I2C_Master_Transmit(&i2c_handle,FDC1004_DevAddress<<1,i2c_tx_buffer,1,FDC1004_I2C_TIMEOUT);
	i2c_rx_buffer[0] = 0;// MSB byte - reset
	i2c_rx_buffer[1] = 0;// LSB byte - reset
	HAL_I2C_Master_Receive(&i2c_handle,FDC1004_DevAddress<<1,i2c_rx_buffer,2,FDC1004_I2C_TIMEOUT);  
  dummy_RegVal_uint16 = (i2c_rx_buffer[0]<<8 | i2c_rx_buffer[1]);
	return dummy_RegVal_uint16;
}

// WRITE CONFIG REGS
/**
  * @brief  Sets configuration register for channel 1 of FDC1004
  *                via I2C interface. 
  * @param  i2c_handle Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
	* @param  regVal [uint16] configuration register value.
  * @retval HAL_Status enum [HAL_StatusTypeDef].
  */
HAL_StatusTypeDef fdc1004_write_ConfigChannel_1(I2C_HandleTypeDef i2c_handle, uint16_t regVal){
	HAL_StatusTypeDef status = HAL_OK;
	status = fdc1004_write_register(i2c_handle, FDC1004_ADDR_CONF_MEAS1, regVal);  
	return status;
}

/**
  * @brief  Sets configuration register for channel 2 of FDC1004
  *                via I2C interface. 
  * @param  i2c_handle Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
	* @param  regVal [uint16] configuration register value.
  * @retval HAL_Status enum [HAL_StatusTypeDef].
  */
HAL_StatusTypeDef fdc1004_write_ConfigChannel_2(I2C_HandleTypeDef i2c_handle, uint16_t regVal){
	HAL_StatusTypeDef status = HAL_OK;
	status = fdc1004_write_register(i2c_handle, FDC1004_ADDR_CONF_MEAS2, regVal);   
	return status;
}

/**
  * @brief  Sets configuration register for channel 3 of FDC1004
  *                via I2C interface. 
  * @param  i2c_handle Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
	* @param  regVal [uint16] configuration register value.
  * @retval HAL_Status enum [HAL_StatusTypeDef].
  */
HAL_StatusTypeDef fdc1004_write_ConfigChannel_3(I2C_HandleTypeDef i2c_handle, uint16_t regVal){
	HAL_StatusTypeDef status = HAL_OK;
	status = fdc1004_write_register(i2c_handle, FDC1004_ADDR_CONF_MEAS3, regVal);   
	return status;
}

/**
  * @brief  Sets configuration register for channel 4 of FDC1004
  *                via I2C interface. 
  * @param  i2c_handle Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
	* @param  regVal [uint16] configuration register value.
  * @retval HAL_Status enum [HAL_StatusTypeDef].
  */
HAL_StatusTypeDef fdc1004_write_ConfigChannel_4(I2C_HandleTypeDef i2c_handle, uint16_t regVal){
	HAL_StatusTypeDef status = HAL_OK;
  status = fdc1004_write_register(i2c_handle, FDC1004_ADDR_CONF_MEAS4, regVal);    
	return status;
}

// READ CONFIG REG.
/**
  * @brief  Reads configuration register for channel 1 of FDC1004
  *                via I2C interface. 
  * @param  i2c_handle Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval Register Value [uint16] of configuration register value.
  */
uint16_t fdc1004_read_ConfigChannel_1(I2C_HandleTypeDef i2c_handle){
  dummy_RegVal_uint16 = fdc1004_read_register(i2c_handle, FDC1004_ADDR_CONF_MEAS1);
	return dummy_RegVal_uint16;
}

/**
  * @brief  Reads configuration register for channel 2 of FDC1004
  *                via I2C interface. 
  * @param  i2c_handle Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval Register Value [uint16] of configuration register value.
  */
uint16_t fdc1004_read_ConfigChannel_2(I2C_HandleTypeDef i2c_handle){
  dummy_RegVal_uint16 = fdc1004_read_register(i2c_handle, FDC1004_ADDR_CONF_MEAS2);
	return dummy_RegVal_uint16;
}

/**
  * @brief  Reads configuration register for channel 3 of FDC1004
  *                via I2C interface. 
  * @param  i2c_handle Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval Register Value [uint16] of configuration register value.
  */
uint16_t fdc1004_read_ConfigChannel_3(I2C_HandleTypeDef i2c_handle){
  dummy_RegVal_uint16 = fdc1004_read_register(i2c_handle, FDC1004_ADDR_CONF_MEAS3);
	return dummy_RegVal_uint16;
}

/**
  * @brief  Reads configuration register for channel 4 of FDC1004
  *                via I2C interface. 
  * @param  i2c_handle Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval Register Value [uint16] of configuration register value.
  */
uint16_t fdc1004_read_ConfigChannel_4(I2C_HandleTypeDef i2c_handle){
  dummy_RegVal_uint16 = fdc1004_read_register(i2c_handle, FDC1004_ADDR_CONF_MEAS4);
	return dummy_RegVal_uint16;
}

// GENERATE MEAS_CONFIG REG VALUE
/**
  * @brief  Generates CONF_MEASx register value for Single-ended measurement
  *                of FDC1004 for specified settings. 
  * @param  CHA_val [uint8] Channel select, any FDC1004_CHANNELID_CHx. 
  * @param  CAPDAC_val [uint8] Cap. DAC offset value within 0, FDC1004_CAPDAC_MAX (0x1F).
  * @param  CAPDAC_connected [uint8] CAPDAC connected [1] or disconnected [0].
  * @retval CONF_MEASx register Value [uint16].
  */
uint16_t fdc1004_gen_SE_CONF_MEASx_value(uint8_t CHA_val, uint8_t CAPDAC_val, uint8_t CAPDAC_connected){
	uint16_t CONF_MEASx_val = 0;
	uint8_t CHA_val_tmp = CHA_val;
	uint8_t CAPDAC_val_tmp = CAPDAC_val;
	uint8_t CAPDAC_connected_tmp = CAPDAC_connected;
	
	// check CHA value within boundaries
	if (CHA_val_tmp > FDC1004_CHANNELID_CH4) {
		CHA_val_tmp = FDC1004_CHANNELID_CH1;
		//print_uart2("Error: FDC1004 - CHA value out of bound, replaced with Ch. 1\r\n");
	}
	// check CAPDAC_connected value
	if (CAPDAC_connected_tmp > 0x01) {
		CAPDAC_connected_tmp = 0x01;
		//print_uart2("Error: FDC1004 - CAPDAC_connected value invalid, replaced with CAPDAC connected.\r\n");
	}
	// check CAPDAC value
	if (CAPDAC_val_tmp > FDC1004_CAPDAC_MAX) {
		CAPDAC_val_tmp = FDC1004_CAPDAC_MAX;
		//print_uart2("Error: FDC1004 - CAPDAC value out of bound, replaced with FDC1004_CAPDAC_MAX\r\n");
	}
	// creat CONF_MEASx value
	CONF_MEASx_val = ((CHA_val_tmp << 13)|(CAPDAC_val_tmp << 5));
	
	if (CAPDAC_connected_tmp == 0x01) {
		// CAPDAC connected
		CONF_MEASx_val = CONF_MEASx_val | (FDC1004_CHANNELID_TO_CAPDAC << 10);
	} else {
		// CAPDAC disconnected
		CONF_MEASx_val = CONF_MEASx_val | (FDC1004_CHANNELID_DISABLED << 10);
	}
	return CONF_MEASx_val; 
}

/**
  * @brief  Generates CONF_MEASx register value for Differential measurement
  *                of FDC1004 for specified settings. 
  * @param  CHA_val [uint8] Channel select, any FDC1004_CHANNELID_CHx. 
  * @param  CHB_val [uint8] Channel select, any FDC1004_CHANNELID_CHx. 
  *                Restrictions: CHB_val < CHA_val; CHA_val != CHB_val.
  * @retval CONF_MEASx register Value [uint16].
  */
uint16_t fdc1004_gen_DIF_CONF_MEASx_value(uint8_t CHA_val, uint8_t CHB_val){
	uint16_t CONF_MEASx_val = 0;
	uint8_t CHA_val_tmp = CHA_val;
	uint8_t CHB_val_tmp = CHB_val;
	uint8_t dummy_CHx_tmp = 0;
	
	// check CHA value within boundaries
	if (CHA_val_tmp > FDC1004_CHANNELID_CH4) {
		CHA_val_tmp = FDC1004_CHANNELID_CH1;
		//print_uart2("Error: FDC1004 - CHA value out of bound, replaced with Ch. 1\r\n");
	}
	// check CHB value within boundaries
	if (CHB_val_tmp > FDC1004_CHANNELID_CH4) {
		CHB_val_tmp = FDC1004_CHANNELID_CH2;
		//print_uart2("Error: FDC1004 - CHB value out of bound, replaced with Ch. 2\r\n");
	}
	// check CHA value > CHB value.
	if (CHA_val_tmp > CHB_val_tmp) {
		dummy_CHx_tmp = CHB_val_tmp;
		CHB_val_tmp = CHA_val_tmp;
		CHA_val_tmp = dummy_CHx_tmp;
		//print_uart2("Error: FDC1004 - CHB value smaller than CHA value. Channels swapped.\r\n");
	}

	// create CONF_MEASx value
	CONF_MEASx_val = ((CHA_val_tmp << 13)|(CHB_val_tmp << 10));
	return CONF_MEASx_val;
}


// SOFT-RESET WRITE
/**
  * @brief  Generates CONF_MEASx register value for Differential measurement
  *                of FDC1004 for specified settings. 
  * @retval HAL_Status enum [HAL_StatusTypeDef].
  */
HAL_StatusTypeDef fdc1004_softReset(I2C_HandleTypeDef i2c_handle){
	// generate 
	uint16_t regVal = 0;
	regVal = regVal | (FDC1004_RST << 15);
	HAL_StatusTypeDef status = HAL_OK;
  status = fdc1004_write_register(i2c_handle, FDC1004_ADDR_FDC_CONF, regVal);  
	return status;
}


// SOFT-RESET COMPLETION CHECK
/**
  * @brief  Checks FDC1004 soft-reset completion. 
  * @param  i2c_handle Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval Soft-reset complete flag, complete [1].
  */
uint8_t fdc1004_softresetComplete(I2C_HandleTypeDef i2c_handle){
	uint8_t status_softreset_complete = 0xFF; // error handling value.
  uint16_t FDCreg_val = fdc1004_read_FDCreg(i2c_handle);
	
	FDCreg_val = FDCreg_val >> 15; //only MSB bit relevant, RST.
	
	// Check if soft-reset is complete
	if (FDCreg_val == 1){
		status_softreset_complete = 1; // soft-reset completed
	}	else{
		status_softreset_complete = 0; // soft-reset completed
	}
	
	return status_softreset_complete;
}

// GENERATE FDC REG VALUE
/**
  * @brief  Generates FDC register value of FDC1004 for specified settings. 
  * @param  CHANNEL_TRIG_val [uint8] Channel measurement trigger select,
  *            4-bit value, each bit encodes channel; MSB: Channel 1, LSB: Channel 4;
  *            To enable set high '1' e.g. 0b1100, 0x0C triggers Channels 1 and 2.
  *            + For REPEAT MODE: up to four (4) channels can be triggered in one operation. Only latest trigger is repeated.
  *            + For NO REPEAT MODE: only one (1) channel can be triggered in one operation.
  * @retval FDC register Value [uint16].
  */
uint16_t fdc1004_gen_FDC_value(uint8_t CHANNEL_TRIG_val, uint8_t GLOBAL_RATE_val, uint8_t REPEAT_val){
	uint16_t FDCreg_val = 0;
	uint8_t CHANNEL_TRIG_val_tmp = CHANNEL_TRIG_val;
	uint8_t GLOBAL_RATE_val_tmp = GLOBAL_RATE_val;
	uint8_t REPEAT_val_tmp = REPEAT_val;
	
	// check CHANNEL_TRIG_val value within boundaries
	if (CHANNEL_TRIG_val_tmp > 0x0F) {
		CHANNEL_TRIG_val_tmp = 0x00;
		print_uart2("Error: FDC1004 - MEAS_x value invalid, replaced with all ch. disabled.\r\n");
	}
	// check GLOBAL_RATE_val value within boundaries
	if (GLOBAL_RATE_val_tmp > 0x03 || GLOBAL_RATE_val_tmp == 0x00) {
		CHANNEL_TRIG_val_tmp = 0x01;
		//print_uart2("Error: FDC1004 - RATE value invalid, replaced with 100S/s (0x01).\r\n");
	}
	// check REPEAT_val value within boundaries
	if (REPEAT_val_tmp > 0x01) {
		REPEAT_val_tmp = 0x00;
		//print_uart2("Error: FDC1004 - REPEAT value invalid, replaced with single meas. (0x00).\r\n");
	}
	
	// creat FDC value
	FDCreg_val = ((GLOBAL_RATE_val_tmp << 10)|(REPEAT_val_tmp << 8)|(CHANNEL_TRIG_val_tmp << 4));
	return FDCreg_val; 
}


// WRITE FDC REG 
/**
  * @brief  Sets FDC register of FDC1004 via I2C interface. 
  * @param  i2c_handle Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
	* @param  regVal [uint16] FDC register value.
  * @retval HAL_Status enum [HAL_StatusTypeDef].
  */
HAL_StatusTypeDef fdc1004_write_FDCreg(I2C_HandleTypeDef i2c_handle, uint16_t regVal){
	HAL_StatusTypeDef status = HAL_OK;
  status = fdc1004_write_register(i2c_handle, FDC1004_ADDR_FDC_CONF, regVal);    
	return status;
}
// READ FDC REG
/**
  * @brief  Reads FDC register of FDC1004 via I2C interface. 
  * @param  i2c_handle Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval Register Value [uint16] of FDC register value.
  */
uint16_t fdc1004_read_FDCreg(I2C_HandleTypeDef i2c_handle){
	// TO-DO: issue for DONE_n bits, mask for config check.
  dummy_RegVal_uint16 = fdc1004_read_register(i2c_handle, FDC1004_ADDR_FDC_CONF);
	return dummy_RegVal_uint16;
}

// CHECK MEAS COMPLETED
/**
  * @brief  Checks measurement for specfied channels complete of FDC1004 via I2C interface. 
  * @param  i2c_handle Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  relevantChannels_val [uint8_t ].
  *            4-bit value, each bit encodes channel; MSB: Channel 1, LSB: Channel 4;
  *            To enable set high '1' e.g. 0b1100, 0x0C triggers Channels 1 and 2.
  * @retval Register Value [uint16] of FDC register value.
  */
uint8_t fdc1004_checkMeasComplete(I2C_HandleTypeDef i2c_handle, uint8_t relevantChannels_val){
	uint8_t status_channels_complete = 0xFF; // error handling value.
	uint8_t relevantChannels_val_tmp = relevantChannels_val;
  uint16_t FDCreg_val = fdc1004_read_FDCreg(i2c_handle);
	
	// check CHANNEL_TRIG_val value within boundaries
	if ((relevantChannels_val_tmp > 0x0F) || (relevantChannels_val_tmp == 0)) {
		relevantChannels_val_tmp = 0x08;
		status_channels_complete = 0xF0; // error handling value.
		//print_uart2("Error: FDC1004 - relevantChannels_val value invalid, replaced with ch. 1 mask.\r\n");
	}
	
	FDCreg_val = FDCreg_val & 0x0F; //only last four (4) bits matter, DONE_x.
	
	// Check if measurement is complete
	if (FDCreg_val == relevantChannels_val_tmp){
		status_channels_complete = 1; // measurement completed
	}	else{
		status_channels_complete = 0; // measurement completed
	}
	
	return status_channels_complete;
}

// WRITE OFFSET_CALn REG - TO-DO modify from reset (0x0000) values by adding gen. function for register value.
/**
  * @brief  Sets OFFSET_CALn register for channel 1 of FDC1004
  *                via I2C interface. 
  * @param  i2c_handle Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
	* @param  regVal [uint16] configuration register value, reset value 0x0000.
  * @retval HAL_Status enum [HAL_StatusTypeDef].
  */
HAL_StatusTypeDef fdc1004_write_OFFSET_CAL1(I2C_HandleTypeDef i2c_handle, uint16_t regVal){
	HAL_StatusTypeDef status = HAL_OK;
  status = fdc1004_write_register(i2c_handle, FDC1004_ADDR_OFFSET_CAL_CIN1, regVal); 
	return status;
}

/**
  * @brief  Sets OFFSET_CALn register for channel 2 of FDC1004
  *                via I2C interface. 
  * @param  i2c_handle Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
	* @param  regVal [uint16] configuration register value, reset value 0x0000.
  * @retval HAL_Status enum [HAL_StatusTypeDef].
  */
HAL_StatusTypeDef fdc1004_write_OFFSET_CAL2(I2C_HandleTypeDef i2c_handle, uint16_t regVal){
	HAL_StatusTypeDef status = HAL_OK;
  status = fdc1004_write_register(i2c_handle, FDC1004_ADDR_OFFSET_CAL_CIN2, regVal);   
	return status;
}

/**
  * @brief  Sets OFFSET_CALn register for channel 3 of FDC1004
  *                via I2C interface. 
  * @param  i2c_handle Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
	* @param  regVal [uint16] configuration register value, reset value 0x0000.
  * @retval HAL_Status enum [HAL_StatusTypeDef].
  */
HAL_StatusTypeDef fdc1004_write_OFFSET_CAL3(I2C_HandleTypeDef i2c_handle, uint16_t regVal){
	HAL_StatusTypeDef status = HAL_OK;
  status = fdc1004_write_register(i2c_handle, FDC1004_ADDR_OFFSET_CAL_CIN3, regVal); 
	return status;
}

/**
  * @brief  Sets OFFSET_CALn register for channel 4 of FDC1004
  *                via I2C interface. 
  * @param  i2c_handle Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
	* @param  regVal [uint16] configuration register value, reset value 0x0000.
  * @retval HAL_Status enum [HAL_StatusTypeDef].
  */
HAL_StatusTypeDef fdc1004_write_OFFSET_CAL4(I2C_HandleTypeDef i2c_handle, uint16_t regVal){
	HAL_StatusTypeDef status = HAL_OK;
  status = fdc1004_write_register(i2c_handle, FDC1004_ADDR_OFFSET_CAL_CIN4, regVal);   
	return status;
}

// WRITE GAIN_CALn REG - TO-DO modify from reset (0x4000) values by adding gen. function for register value.
/**
  * @brief  Sets OFFSET_CALn register for channel 1 of FDC1004
  *                via I2C interface. 
  * @param  i2c_handle Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
	* @param  regVal [uint16] configuration register value, reset value 0x4000.
  * @retval HAL_Status enum [HAL_StatusTypeDef].
  */
HAL_StatusTypeDef fdc1004_write_GAIN_CAL1(I2C_HandleTypeDef i2c_handle, uint16_t regVal){
	HAL_StatusTypeDef status = HAL_OK;
  status = fdc1004_write_register(i2c_handle, FDC1004_ADDR_GAIN_CAL_CIN1, regVal); 
	return status;
}

/**
  * @brief  Sets OFFSET_CALn register for channel 2 of FDC1004
  *                via I2C interface. 
  * @param  i2c_handle Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
	* @param  regVal [uint16] configuration register value, reset value 0x4000.
  * @retval HAL_Status enum [HAL_StatusTypeDef].
  */
HAL_StatusTypeDef fdc1004_write_GAIN_CAL2(I2C_HandleTypeDef i2c_handle, uint16_t regVal){
	HAL_StatusTypeDef status = HAL_OK;
  status = fdc1004_write_register(i2c_handle, FDC1004_ADDR_GAIN_CAL_CIN2, regVal);  
	return status;
}

/**
  * @brief  Sets OFFSET_CALn register for channel 3 of FDC1004
  *                via I2C interface. 
  * @param  i2c_handle Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
	* @param  regVal [uint16] configuration register value, reset value 0x4000.
  * @retval HAL_Status enum [HAL_StatusTypeDef].
  */
HAL_StatusTypeDef fdc1004_write_GAIN_CAL3(I2C_HandleTypeDef i2c_handle, uint16_t regVal){
	HAL_StatusTypeDef status = HAL_OK;
  status = fdc1004_write_register(i2c_handle, FDC1004_ADDR_GAIN_CAL_CIN3, regVal);
	return status;
}

/**
  * @brief  Sets OFFSET_CALn register for channel 4 of FDC1004
  *                via I2C interface. 
  * @param  i2c_handle Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
	* @param  regVal [uint16] configuration register value, reset value 0x4000.
  * @retval HAL_Status enum [HAL_StatusTypeDef].
  */
HAL_StatusTypeDef fdc1004_write_GAIN_CAL4(I2C_HandleTypeDef i2c_handle, uint16_t regVal){
	HAL_StatusTypeDef status = HAL_OK;
  status = fdc1004_write_register(i2c_handle, FDC1004_ADDR_GAIN_CAL_CIN4, regVal);  
	return status;
}



// READ CHANNEL RAW CAPACITANCE VALUE
/**
  * @brief  Reads raw capacitance value of FDC1004 for specified channel.
  *                via I2C interface. 
  * @param  i2c_handle Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  MSB_pointer_address [uint8_t] register address for MSB_value.
  * @param  LSB_pointer_address [uint8_t] register address for LSB_value.
  */
uint32_t fdc1004_read_RawCap_Value(I2C_HandleTypeDef i2c_handle, uint8_t MSB_pointer_address, uint8_t LSB_pointer_address){
  uint16_t MSB_regVal_rawCap = 0;
	uint16_t LSB_regVal_rawCap = 0;
	dummy_Val_uint32 = 4294967295; // error handling init value
  
	// check MSB_pointer address valid.
	if((MSB_pointer_address == FDC1004_ADDR_MEAS1_MSB) || (MSB_pointer_address == FDC1004_ADDR_MEAS2_MSB) 
		    || (MSB_pointer_address == FDC1004_ADDR_MEAS3_MSB) || (MSB_pointer_address == FDC1004_ADDR_MEAS4_MSB)) {
			// check that correct LSB_pointer address is used.
			if ((MSB_pointer_address + 1) == LSB_pointer_address) {
				MSB_regVal_rawCap	= fdc1004_read_register(i2c_handle, MSB_pointer_address); 
				// important to read FIRST MSB register and then LSB register to reset FDC measurement DONE_n reg.
				LSB_regVal_rawCap	= fdc1004_read_register(i2c_handle, LSB_pointer_address);
				// build 24-bit cap raw value - still in two complement form
				dummy_Val_uint32 = (MSB_regVal_rawCap << 8) | (LSB_regVal_rawCap >> 8);				
			}
			else {print_uart2("FDC1004 - Error - Wrong LSB_pointer address used in fdc1004_read_RawCap_Value().\r\n");}
	} else {print_uart2("FDC1004 - Error - Wrong MSB_pointer address used in fdc1004_read_RawCap_Value().\r\n");}
  return dummy_Val_uint32;
}

// READ RAW CAPACITANCE VALUE - CHANNEL 1
/**
  * @brief  Reads raw capacitance value of FDC1004 for channel 1.
  *                via I2C interface. 
  * @param  i2c_handle Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  */
uint32_t fdc1004_read_RawCapValue_Channel_1(I2C_HandleTypeDef i2c_handle){
  dummy_Val_uint32 = fdc1004_read_RawCap_Value(i2c_handle, FDC1004_ADDR_MEAS1_MSB, FDC1004_ADDR_MEAS1_LSB);
  return dummy_Val_uint32;
}
// READ RAW CAPACITANCE VALUE - CHANNEL 2
/**
  * @brief  Reads raw capacitance value of FDC1004 for channel 2.
  *                via I2C interface. 
  * @param  i2c_handle Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  */
uint32_t fdc1004_read_RawCapValue_Channel_2(I2C_HandleTypeDef i2c_handle){
  dummy_Val_uint32 = fdc1004_read_RawCap_Value(i2c_handle, FDC1004_ADDR_MEAS2_MSB, FDC1004_ADDR_MEAS2_LSB);
  return dummy_Val_uint32;
}
// READ RAW CAPACITANCE VALUE - CHANNEL 3
/**
  * @brief  Reads raw capacitance value of FDC1004 for channel 3.
  *                via I2C interface. 
  * @param  i2c_handle Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  */
uint32_t fdc1004_read_RawCapValue_Channel_3(I2C_HandleTypeDef i2c_handle){
  dummy_Val_uint32 = fdc1004_read_RawCap_Value(i2c_handle, FDC1004_ADDR_MEAS3_MSB, FDC1004_ADDR_MEAS3_LSB);
  return dummy_Val_uint32;
}
// READ RAW CAPACITANCE VALUE - CHANNEL 4
/**
  * @brief  Reads raw capacitance value of FDC1004 for channel 4.
  *                via I2C interface. 
  * @param  i2c_handle Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  */
uint32_t fdc1004_read_RawCapValue_Channel_4(I2C_HandleTypeDef i2c_handle){
  dummy_Val_uint32 = fdc1004_read_RawCap_Value(i2c_handle, FDC1004_ADDR_MEAS4_MSB, FDC1004_ADDR_MEAS4_LSB);
  return dummy_Val_uint32;
}
// Convert 24-bit Two  complement to int32 value
/**
  * @brief  Convert 24-bit two complement value to int32. 
  * @param  24 bit two complement [uint32].
  * @retval Converted Int32 value [int32]; max: 8388607, min: -8388608 for valid output.
  *               Error handling value: 0x7FFFFFFF.
  */
int32_t fdc1004_calc_24bitTwoComplement2Int32(uint32_t raw_TwoCompVal){
	int32_t raw_Val_int32;
	
	// check input value is valid 24-bit number.
	if(raw_TwoCompVal > 0xFFFFFF){
		//print_uart2("FDC1004 - Error - Two complement - invalid input value.\r\n");
		return 0x7FFFFFFF; // error handling
	}
	
	//raw_Val_int32 = (raw_TwoCompVal & 0x7FFFFF); // first 23 bits only, bit 24 is twocompl. indicators
	if ((raw_TwoCompVal >> 23) & 1){  
		raw_Val_int32 = (raw_TwoCompVal & 0x7FFFFF) - (1 << 23);
	} else {
		raw_Val_int32 = raw_TwoCompVal;
	}		
	return raw_Val_int32;
}


/**
  * @brief  Calculates physical capacitance value [pF, float] 
  *                from raw int32 two complements of 24-bit FDC1004 value and CAPDAC.
  *                Note: Using float sufficient to capture full sensor resolution (i.e. double not needed).
  * @param  raw_CapVal [int32] raw capacitance value obtained from measurement in 24-bit (!) two complements form.
	* @param  CAPDAC_phys_val [float] physical value of CAPDAC used.
  * @retval Channel capacitance physical value [pF, float].
  */
float fdc1004_calc_phys_CapValue_float(uint32_t raw_TwoCompCapVal, float CAPDAC_phys_val){
	// TO-DO use double instead to capture full range?
	// TO-DO check CAPDAC value
	float CAPDAC_phys_val_tmp = (float) CAPDAC_phys_val;
	float phys_CapVal = 0.0F;
	int32_t raw_CapVal = fdc1004_calc_24bitTwoComplement2Int32(raw_TwoCompCapVal);
	// TO-DO
	phys_CapVal = (((float) raw_CapVal) / (float) 0x80000); // 0x80000 = 2**19
	// "two complement operation" - TO-DO verify
	phys_CapVal = phys_CapVal + CAPDAC_phys_val_tmp; // add CAPDAC physical value
	return phys_CapVal;
}

// CHECK MANUFACTURER ID - sanity check device OK
/**
  * @brief  Reads manufacturer ID register of FDC1004 and compares with nominal value 0x5449.
  *                via I2C interface. 
  * @param  i2c_handle Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  */
void fdc1004_read_ManuID(I2C_HandleTypeDef i2c_handle){
  dummy_RegVal_uint16 = fdc1004_read_register(i2c_handle, FDC1004_ADDR_MANUFACTURE_ID); 
  if (dummy_RegVal_uint16 == 0x5449){
		char str_to_print[50];
		sprintf(str_to_print, "fdc1004 Manufacturer ID: 0x%02X\r\n", dummy_RegVal_uint16);
	  print_uart2(str_to_print);
		print_uart2("FDC1004 - Manufacturer ID correct.\r\n");
	} else {
		print_uart2("Error - FDC1004 - Manufacturer ID incorrect.\r\n");
	}
}

// CHECK DEVICE ID - sanity check device OK
/**
  * @brief  Reads device ID register of FDC1004 and compares with nominal value 0x1004.
  *                via I2C interface. 
  * @param  i2c_handle Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  */
void fdc1004_read_DevID(I2C_HandleTypeDef i2c_handle){
  dummy_RegVal_uint16 = fdc1004_read_register(i2c_handle, FDC1004_ADDR_DEVICE_ID); 
  if (dummy_RegVal_uint16 == 0x1004){
		print_uart2("FDC1004 - Device ID correct.\r\n");
	} else {
		print_uart2("Error - FDC1004 - Device ID incorrect.\r\n");
	}
}

// CAPDAC offset value
/**
  * @brief  Calculates CAPDAC physical value [pF] from discrete input code [3.125 pF / LSB] of FDC1004. 
	* @param  CAPDAC_val [uint8] configuration register value, reset value 0x4000.
  * @retval CAPDAC physical value [pF].
  */
float fdc1004_calc_CAPDAC_offset(uint8_t CAPDAC_val){
	float CAPDAC_val_tmp = (float) CAPDAC_val;
	if (CAPDAC_val > FDC1004_CAPDAC_MAX){
		CAPDAC_val_tmp = 0.0F;
		//print_uart2("Error - FDC1004 - Entered CAPDAC value out of bound.\r\n");
	}
	return (3.125F * CAPDAC_val_tmp);
}

// INIT FDC1004
/**
* @brief  Initialise FDC1004: Soft-restart, Dev. / Man. ID readback. 
  * @param  i2c_handle Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  */
void fdc1004_init(I2C_HandleTypeDef i2c_handle){
	print_uart2("FDC1004 - Start init.\r\n");
	// Soft-reset device
	fdc1004_softReset(i2c_handle);
	
	HAL_Delay(15);  // 15 ms - TO-DO - set proper delay value
	if (fdc1004_softresetComplete(i2c_handle)){
		print_uart2("FDC1004 - Soft-reset complete.\r\n");
	}	
	// OPTIONAL: sanity check: Dev. Man. ID readback
	fdc1004_read_ManuID(i2c_handle);
	fdc1004_read_DevID(i2c_handle);
	print_uart2("FDC1004 - Finished init.\r\n");
}

// Set up SE meas, Channels 1, 2
void fdc1004_setup_SE_Meas_Ch1(I2C_HandleTypeDef i2c_handle, uint8_t in_CAPDAC_val_ch1, uint8_t CAPDAC_connected_ch1){
	uint16_t regVal; 
	
	// CAPDAC value of 4 LSB -> physical CAPDAC val. = 12.5 pF offset, 
	// allows for largest non-zero range {0, 27.5} 
	// due to +/- 15.0 pF sensor range
	// adjust for input capacitance (3.125 pF/LSB).
	// CAPDAC [LSB] 5-bit value, ideally found through alg.
	// CAPDAc connected? on=1, off=0; SE default: on.

	// Set up measurement channel 1
	regVal = fdc1004_gen_SE_CONF_MEASx_value(FDC1004_CHANNELID_CH1,in_CAPDAC_val_ch1,CAPDAC_connected_ch1); 
	fdc1004_write_ConfigChannel_1(i2c_handle, regVal);
	if (regVal == fdc1004_read_ConfigChannel_1(i2c_handle)){
		//print_uart2("FDC1004 - Channel 1 config success.\r\n");
	}
}

// Set up SE meas, Channels 1, 2
void fdc1004_setup_SE_Meas_Ch2(I2C_HandleTypeDef i2c_handle, uint8_t in_CAPDAC_val_ch2, uint8_t CAPDAC_connected_ch2){
	uint16_t regVal; 
	// CAPDAC value of 4 LSB -> physical CAPDAC val. = 12.5 pF offset, 
	// allows for largest non-zero range {0, 27.5} pF
	// due to +/- 15.0 pF sensor range
	// adjust for input capacitance (3.125 pF/LSB).
	// CAPDAC [LSB] 5-bit value, ideally found through alg.
	// CAPDAc connected? on=1, off=0; SE default: on.

	// Set up channel 2
	regVal = fdc1004_gen_SE_CONF_MEASx_value(FDC1004_CHANNELID_CH2,in_CAPDAC_val_ch2,CAPDAC_connected_ch2); 
	fdc1004_write_ConfigChannel_2(i2c_handle, regVal);
	/*if (regVal == fdc1004_read_ConfigChannel_2(i2c_handle)){
		print_uart2("FDC1004 - Channel 2 config success.\r\n");
	}*/
}

// Trigger Measurement (Repeat, No Repeat Mode)
void fdc1004_trigger_Meas(I2C_HandleTypeDef i2c_handle, uint8_t sample_rate, uint8_t channel_trg_ID, uint8_t repeatMode_On){
	// + For REPEAT MODE: up to four (4) channels can be triggered in one operation. Only latest trigger is repeated.
  // + For NO REPEAT MODE: only one (1) channel can be triggered in one operation.
	// repeated measurement, use: repeatMode_On = {FDC1004_REPEAT, FDC1004_NO_REPEAT}.
	// channel_ID = {FDC1004_TRIGGER_CHANNELID_CHn, n = 1,2,3,4};
	// Set up FDC register 
	uint16_t regVal = fdc1004_gen_FDC_value(channel_trg_ID, sample_rate, repeatMode_On);
	fdc1004_write_FDCreg(i2c_handle, regVal);
	// Optional readback
	//if (regVal == fdc1004_read_FDCreg(i2c_handle)){
	//print_uart2("FDC1004 - FDC config success.\r\n");
	//}
}	
	
// read measured value Ch. 1
float fdc1004_readMeasVal_Ch1(I2C_HandleTypeDef i2c_handle, uint8_t in_CAPDAC_val){
	int32_t rawCapVal;
	float physCapVal;
	float phys_CAPDAC_val;
 
	// OPTIONAL - string
	//char str_for_print[50];
	
	rawCapVal = fdc1004_read_RawCapValue_Channel_1(i2c_handle);
	// convert to physical value
	phys_CAPDAC_val = fdc1004_calc_CAPDAC_offset(in_CAPDAC_val);
	physCapVal = fdc1004_calc_phys_CapValue_float(rawCapVal, phys_CAPDAC_val);
	
	// OPTIONAL print physical value
	//
	/*char str_for_print[100];
	sprintf(str_for_print, "Ch. 1: rawCapVal: %d     phys_CAPDAC_val: %f    %18.15f pF.", rawCapVal, phys_CAPDAC_val, physCapVal); // number of digits do not represent noise [much less].
	print_uart2(str_for_print);
	print_uart2("\r\n");*/
	// 
	return physCapVal;
}

// read measured value Ch. 2
float fdc1004_readMeasVal_Ch2(I2C_HandleTypeDef i2c_handle, uint8_t in_CAPDAC_val){
	int32_t rawCapVal;
	float physCapVal;
	float phys_CAPDAC_val;
 
	// OPTIONAL - string
	//char str_for_print[50];
	
	rawCapVal = fdc1004_read_RawCapValue_Channel_2(i2c_handle);
	// convert to physical value
	phys_CAPDAC_val = fdc1004_calc_CAPDAC_offset(in_CAPDAC_val);
	physCapVal = fdc1004_calc_phys_CapValue_float(rawCapVal, phys_CAPDAC_val);
	
	// OPTIONAL print physical value
	///*
	/*jsprintf(str_for_print, "Ch. 2: %18.15f pF.", physCapVal); // number of digits do not represent noise [much less].
	print_uart2(str_for_print);
	print_uart2("\r\n");*/
	//*/
	return physCapVal;
}

// TO-DO Check Measurement complete
void fdc1004_doneRequest_SE_Meas(I2C_HandleTypeDef i2c_handle, uint8_t channel_trg_ID){
	// channel_ID = {FDC1004_TRIGGER_CHANNELID_CHn, n = 1,2,3,4};
	if (fdc1004_checkMeasComplete(i2c_handle, channel_trg_ID) == 1){
		print_uart2("FDC1004 - Measurement ready.\r\n");
	}
}

// Redout FDC including binary search for capdac (in_CAPDAC_val) - by Julian
// NEEDS at least 4ms delay between each readout!!!
// TODO: Make the channel selection nice
float fdc1004_readMeasVal_channel_autoCapdac(I2C_HandleTypeDef i2c_handle, int channel){
	if (channel==2){
		return fdc1004_readMeasVal_channel_2_autoCapdac(i2c_handle);
	}
	static int in_CAPDAC_val = 15;
	static int val_to_change = 1;
	static int in_CAPDAC_val_was_inced = 0;
	float FDC_1004_physCapVal = 0;
	FDC_1004_physCapVal = fdc1004_readMeasVal_Ch1(i2c_handle, in_CAPDAC_val);
	if ((FDC_1004_physCapVal > in_CAPDAC_val*3.125+15)){	
		if (in_CAPDAC_val_was_inced && (val_to_change < 5)){
			val_to_change *= 2;
		}
		else if (val_to_change>1){
			val_to_change /= 2;
		}
		in_CAPDAC_val += val_to_change;
		if (in_CAPDAC_val > 31){
			in_CAPDAC_val = 31;
			val_to_change = 1;
		}
		in_CAPDAC_val_was_inced = 1;
		FDC_1004_physCapVal = 0;
	}
	else if((FDC_1004_physCapVal < in_CAPDAC_val*3.125-15) && (in_CAPDAC_val > 0)){ //(FDC_1004_physCapVal > 14) && 
		if((in_CAPDAC_val_was_inced == 0) && (val_to_change < 5)){
			val_to_change *= 2;
		}
		else if (val_to_change>1){
			val_to_change /= 2;
		}
		in_CAPDAC_val -= val_to_change;
		if (in_CAPDAC_val < 0){
			in_CAPDAC_val = 0;
			val_to_change = 1;
		}
		in_CAPDAC_val_was_inced = 0;
		FDC_1004_physCapVal = 0;
	}
	fdc1004_setup_SE_Meas_Ch1(i2c_handle, in_CAPDAC_val, FDC1004_CAPDAC_CONNECTED);
	//char strToPrint[100];
	//sprintf(strToPrint, "in_CAPDAC_val: %d    val_to_change: %d    in_CAPDAC_val_was_inced: %d      ", in_CAPDAC_val, val_to_change, in_CAPDAC_val_was_inced);
	//print_uart2(strToPrint);
	return FDC_1004_physCapVal;
}

// Like fdc1004_readMeasVal_channel_autoCapdac for channel 2
float fdc1004_readMeasVal_channel_2_autoCapdac(I2C_HandleTypeDef i2c_handle){
	static int in_CAPDAC_val = 15;
	static int val_to_change = 1;
	static int in_CAPDAC_val_was_inced = 0;
	float FDC_1004_physCapVal = 0;
	FDC_1004_physCapVal = fdc1004_readMeasVal_Ch2(i2c_handle, in_CAPDAC_val);
	if ((FDC_1004_physCapVal > in_CAPDAC_val*3.125+15)){	
		if (in_CAPDAC_val_was_inced && (val_to_change < 5)){
			val_to_change *= 2;
		}
		else if (val_to_change>1){
			val_to_change /= 2;
		}
		in_CAPDAC_val += val_to_change;
		if (in_CAPDAC_val > 31){
			in_CAPDAC_val = 31;
			val_to_change = 1;
		}
		in_CAPDAC_val_was_inced = 1;
		FDC_1004_physCapVal = 0;
	}
	else if((FDC_1004_physCapVal < in_CAPDAC_val*3.125-15) && (in_CAPDAC_val > 0)){ //(FDC_1004_physCapVal > 14) && 
		if((in_CAPDAC_val_was_inced == 0) && (val_to_change < 5)){
			val_to_change *= 2;
		}
		else if (val_to_change>1){
			val_to_change /= 2;
		}
		in_CAPDAC_val -= val_to_change;
		if (in_CAPDAC_val < 0){
			in_CAPDAC_val = 0;
			val_to_change = 1;
		}
		in_CAPDAC_val_was_inced = 0;
		FDC_1004_physCapVal = 0;
	}
	fdc1004_setup_SE_Meas_Ch2(i2c_handle, in_CAPDAC_val, FDC1004_CAPDAC_CONNECTED);
	//char strToPrint[100];
	//sprintf(strToPrint, "in_CAPDAC_val: %d    val_to_change: %d    in_CAPDAC_val_was_inced: %d      ", in_CAPDAC_val, val_to_change, in_CAPDAC_val_was_inced);
	//print_uart2(strToPrint);
	return FDC_1004_physCapVal;
}



// Application in main.c
// TO-DO 1: No check for sensor saturation is implemented. 
// Alg. to find proper CAPDAC val. must be implemented.
// TO-DO 2: Interleaved readout, NO REPEAT, read Ch. 1 after trg. Ch. 2 and vice versa.
// SETUP - repeat mode:
/*
// SETUP - REPEAT
print_uart2("FDC1004 - REPEAT MODE\r\n");
uint8_t FDC_1004_in_CAPDAC_val_ch1 = 4U;    
uint8_t FDC_1004_in_CAPDAC_val_ch2 = 4U;
float FDC_1004_physCapVal_ch1, FDC_1004_physCapVal_ch2;
fdc1004_init(hi2c1);
// Set up Channel 1
fdc1004_setup_SE_Meas_Ch1(hi2c1, FDC_1004_in_CAPDAC_val_ch1, FDC1004_CAPDAC_CONNECTED);
// Set up Channel 2
fdc1004_setup_SE_Meas_Ch2(hi2c1, FDC_1004_in_CAPDAC_val_ch2, FDC1004_CAPDAC_CONNECTED);
// Trigger measurements - REPEAT MODE - both channels in one operation triggered!
fdc1004_trigger_Meas(hi2c1, FDC1004_RATE_400HZ, (FDC1004_TRIGGER_CHANNELID_CH1 + FDC1004_TRIGGER_CHANNELID_CH2), FDC1004_REPEAT);
*/

// LOOP - repeat mode:
// TO-DO: smarter timing, handle skipped reads
/*
// Channel 1
// Delay for new sample new read - 400 S/s - must be set properly to catch at correct rate
HAL_Delay(5);
// read ch. 1 value
FDC_1004_physCapVal_ch1 = fdc1004_readMeasVal_Ch1(hi2c1, FDC_1004_in_CAPDAC_val_ch1);
// Channel 2
// Delay for new sample new read - 400 S/s - must be set properly to catch at correct rate
HAL_Delay(5);
// read ch. 2 value
FDC_1004_physCapVal_ch2 = fdc1004_readMeasVal_Ch2(hi2c1, FDC_1004_in_CAPDAC_val_ch2);
// OPTIONAL Delay	
HAL_Delay(500);
*/

// SETUP - no repeat mode:
/*
print_uart2("FDC1004 - NO REPEAT MODE\r\n");
uint8_t FDC_1004_in_CAPDAC_val_ch1 = 4U;    
uint8_t FDC_1004_in_CAPDAC_val_ch2 = 4U;  
float FDC_1004_physCapVal_ch1, FDC_1004_physCapVal_ch2;
fdc1004_init(hi2c1);
// Set up Channel 1
fdc1004_setup_SE_Meas_Ch1(hi2c1, FDC_1004_in_CAPDAC_val_ch1, FDC1004_CAPDAC_CONNECTED);
// Set up Channel 2
fdc1004_setup_SE_Meas_Ch2(hi2c1, FDC_1004_in_CAPDAC_val_ch2, FDC1004_CAPDAC_CONNECTED);
*/
// LOOP - no repeat mode:
/*
// Channel 1
// Trigger meas ch. 1 - only one channel can be triggered in NO REPEAT MODE.
fdc1004_trigger_Meas(hi2c1, FDC1004_RATE_400HZ, FDC1004_TRIGGER_CHANNELID_CH1, FDC1004_NO_REPEAT);
// check Ch. 1 meas. done
while (fdc1004_checkMeasComplete(hi2c1, FDC1004_TRIGGER_CHANNELID_CH1) != 1){}
// read ch. 1 value
FDC_1004_physCapVal_ch1 = fdc1004_readMeasVal_Ch1(hi2c1, FDC_1004_in_CAPDAC_val_ch1);

// Channel 2
// Trigger meas ch. 2 - only one channel can be triggered in NO REPEAT MODE.
fdc1004_trigger_Meas(hi2c1, FDC1004_RATE_400HZ, FDC1004_TRIGGER_CHANNELID_CH2, FDC1004_NO_REPEAT);
// check Ch. 2 meas. done
while (fdc1004_checkMeasComplete(hi2c1, FDC1004_TRIGGER_CHANNELID_CH2) != 1){}
// read ch. 2 value
FDC_1004_physCapVal_ch2 = fdc1004_readMeasVal_Ch2(hi2c1, FDC_1004_in_CAPDAC_val_ch2);
// OPTIONAL Delay	
//HAL_Delay(500);
*/
