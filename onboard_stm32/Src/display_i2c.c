#include "display_i2c.h"
#include "main.h"

extern I2C_HandleTypeDef hi2c3;

void LCM1602_Display_Init(){
	
	LCM1602_Display_Delay(LC_PWRUP_DELAY);

	/* Initialization and configuration mode? 4bit, page 46 of the display manual */
	LCM1602_Display_Write(0x03, COMMAND);
	LCM1602_Display_Delay(LC_4ms_DELAY);

	LCM1602_Display_Write(0x03, COMMAND);
	LCM1602_Display_Delay(LC_4ms_DELAY);

	LCM1602_Display_Write(0x03, COMMAND);
	LCM1602_Display_Delay(LC_100us_DELAY);

	LCM1602_Display_Write(0x02, COMMAND);
	LCM1602_Display_Delay(LC_100us_DELAY);

	/* I select 4bit interface, the number of lines and the font size. Function Set must be
	 * set before every instruction and can not be changed thereafter.
	 */
	LCM1602_Display_WriteCommand(LC_FS_DEFAULT);

	LCM1602_Display_DisplayControl(DISPLAY_ON, CURSOR_OFF, BLINK_OFF);

	LCM1602_Display_EntryModeSet(INCREMENT, NO_SHIFT);
}

void LCM1602_Display_ClearDisplay(){
	LCM1602_Display_WriteCommand(LC_ISTR_CLEAR);
	LCM1602_Display_Delay(LC_4ms_DELAY/2);
}

void LCM1602_Display_ReturnHome(){
	LCM1602_Display_WriteCommand(LC_ISTR_RETURN_HOME);
	LCM1602_Display_Delay(LC_4ms_DELAY/2);
}

void LCM1602_Display_DisplayControl(DisplayMode_TypeDef display_mode, CursorMode_TypeDef cursor_mode,
		BlinkingCursor_TypeDef blink_mode){
	uint8_t display_control = LC_DC;

	if(display_mode == DISPLAY_ON){
		display_control |= LC_DC_D;
	}

	if(cursor_mode == CURSOR_ON){
		display_control |= LC_DC_C;
	}

	if(blink_mode == BLINK_ON){
		display_control |= LC_DC_B;
	}

	LCM1602_Display_Write(display_control, COMMAND);
}

void LCM1602_Display_EntryModeSet(IncrementMode_TypeDef id_mode, DisplayShiftMode_TypeDef ds_mode){
	uint8_t entrymode_control = LC_EMS;

	if(id_mode == INCREMENT)
		entrymode_control |= LC_EMS_ID;

	if(ds_mode == SHIFT)
		entrymode_control |= LC_EMS_S;

	LCM1602_Display_WriteCommand(entrymode_control);
}

void LCM1602_Display_CursorDisplayShift(CursorDisplay_TypeDef cd, ShiftDirection_TypeDef shift_direction){
	uint8_t cd_control = LC_CDS;

	if(cd == DISPLAY)
		cd_control |= LC_CDS_SC;

	if(shift_direction == RIGHT)
		cd_control |= LC_CDS_RL;

	LCM1602_Display_WriteCommand(cd_control);
}

CursorPositionResult_TypeDef LCM1602_Display_SetCursorPosition(uint8_t row, uint8_t col){
	CursorPositionResult_TypeDef result = ROW_ERROR;

	if (row > LC_DISPLAY_ROW || row <= 0) {
		result = ROW_ERROR;
	}
	else if (col > LC_DISPLAY_COL || col <= 0) {
		result = COL_ERROR;
	}
	else {
		uint8_t cycles = 0;
		if (row == 2) {
			cycles = LC_DISPLAY_COL * 2;
		}
		else if (row == 3) {
			cycles = LC_DISPLAY_COL;
		}
		else {
			cycles = (row - 1) * LC_DISPLAY_COL;
		}

		cycles += col;
		LCM1602_Display_ReturnHome();

		for (int i = 1; i < cycles; i++) {
			LCM1602_Display_CursorDisplayShift(CURSOR, RIGHT);
		}
		result = POSITION_SET;
	}
	return result;
}

WriteResult_TypeDef LCM1602_Display_WriteBuffer(uint8_t* pbuff, uint8_t length){
	uint8_t i = 0;
	WriteResult_TypeDef result = LENGTH_ERROR;

	//if(length <= LC_DISPLAY_LENGTH){

		for(; i < length; i++){
			LCM1602_Display_Write(pbuff[i], DATA);
		}

		result = DONE;
	//}
	return result;
}

WriteResult_TypeDef LCM1602_Display_WriteString(char* pbuff){
	//uint8_t count = 0;
	WriteResult_TypeDef result = LENGTH_ERROR;

	while(*pbuff /*&& (count <= LC_DISPLAY_LENGTH)*/){
		LCM1602_Display_Write(*pbuff++, DATA);
		//count++;
	}

	/*if(count <= LC_DISPLAY_LENGTH)
		result = DONE;*/

	return result;
}

/* Private Functions -----------------------------------------------*/
void LCM1602_Display_WriteByte(uint8_t data){
	LCM1602_Display_Write(data, DATA);
}

void LCM1602_Display_WriteCommand(uint8_t data){
	LCM1602_Display_Write(data, COMMAND);
	LCM1602_Display_Delay(LC_37us_DELAY);
}

void LCM1602_Display_Write(uint8_t data, WriteMode_TypeDef mode){
	LCM1602_Display_Write4Bit((data & 0xF0) >> 4, mode);
	LCM1602_Display_Write4Bit(data & 0x0F, mode);
}

void LCM1602_Display_Write4Bit(uint8_t data, WriteMode_TypeDef mode){
	uint8_t temp = (data & 0x0F) << 4;

	if(mode == DATA){
		temp |= LC_RS_BIT;
	}

	temp |= LC_BL_BIT;
	LCM1602_Display_PulseEnable(temp);
}

void LCM1602_Display_PulseEnable(uint8_t data){
	LCM1602_Display_I2C_Send(data | LC_EN_BIT);
	LCM1602_Display_Delay(1);
	
	LCM1602_Display_I2C_Send(data & ~LC_EN_BIT);
}

void LCM1602_Display_I2C_Send(uint8_t data){
	HAL_I2C_Master_Transmit(&hi2c3, LC_I2C_SLAVE_ADDRESS, &data, 1, LC_4ms_DELAY);
}

void LCM1602_Display_Delay(__IO uint32_t nTime) {
	uint8_t i;

	while (nTime--) { // delay n us
		i = 25;
		while (i--)
			; // delay 1 us
	}
}
