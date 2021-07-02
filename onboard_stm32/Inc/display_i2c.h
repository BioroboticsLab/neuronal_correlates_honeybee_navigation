#ifndef LCM1602_DISPLAY_H_
#define LCM1602_DISPLAY_H_

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"
#include "utility_umweltspaeher.h"

/* I2C Settings */
#define LC_I2Cx									I2C3
#define LC_I2C_MASTER_ADDRESS		0x00
#define LC_I2C_SLAVE_ADDRESS		0x27<<1
#define LC_I2C_SPEED						I2C_CLOCK_STANDARD

#define LC_PWRUP_DELAY					400000
#define LC_4ms_DELAY						5000
#define LC_100us_DELAY					100
#define LC_37us_DELAY						50
#define LC_DISPLAY_ROW					2
#define LC_DISPLAY_COL					16
#define LC_DISPLAY_LENGTH				LC_DISPLAY_ROW * LC_DISPLAY_COL


/* Connection Pin PCF8574T -> HD44780
 * P0  ->  R/S
 * P1  ->  RW
 * P2  ->  E
 * P3  ->  BackLight

 * P4-P7 -> D4-D7
 */
#define LC_BL_BIT							0x08
#define LC_EN_BIT							0x04
#define LC_RW_BIT							0x02
#define LC_RS_BIT							0x01

/* Display Instructions */
#define LC_ISTR_CLEAR					0x01
#define LC_ISTR_RETURN_HOME		0x02

/* Entry Mode Set */
#define LC_EMS								0x04
#define LC_EMS_ID							0x02		// 1 -> increment        0 -> decrement
#define LC_EMS_S							0x01		// 1 -> display shift with cursor

/* Cursor or Display Shift */
#define LC_CDS								0x10
#define LC_CDS_SC							0x08		// 1 -> select display   0 -> select cursor
#define LC_CDS_RL							0x04		// 1 -> shift right		 0 -> shift left

/* Function Set */
#define LC_FS									0x20
#define LC_FS_DL							0x10		// 1 -> 8 bit interface  0 -> 4 bit interface
#define LC_FS_N								0x08		// 1 -> 2 lines			 0 -> 1 line
#define LC_FS_F								0x04		// 1 -> 5x10 dots        0 -> 5x8 dots
#define LC_FS_DEFAULT					LC_FS | LC_FS_N

/* Display Control */
#define LC_DC									0x08
#define LC_DC_D								0x04		// display on
#define LC_DC_C								0x02		// cursor on
#define LC_DC_B								0x01		// cursor blink


/* Entry Mode Set Typedef */
typedef enum {
  INCREMENT = 1,
  DECREMENT = 0
} IncrementMode_TypeDef;

typedef enum {
  SHIFT = 1,
  NO_SHIFT = 0
} DisplayShiftMode_TypeDef;

/* Cursor or Display Shift Typedef */
typedef enum {
  CURSOR = 1,
  DISPLAY = 0
} CursorDisplay_TypeDef;

typedef enum {
  RIGHT = 1,
  LEFT = 0
} ShiftDirection_TypeDef;

/* Display Control Typedef */
typedef enum {
  DISPLAY_ON = 1,
  DISPLAY_OFF = 0
} DisplayMode_TypeDef;

typedef enum {
  CURSOR_ON = 1,
  CURSOR_OFF = 0
} CursorMode_TypeDef;

typedef enum {
  BLINK_ON = 1,
  BLINK_OFF = 0
} BlinkingCursor_TypeDef;

/* Write Typedef */
typedef enum {
  DATA = 1,
  COMMAND = 0
} WriteMode_TypeDef;

typedef enum {
  DONE = 1,
  LENGTH_ERROR = 0
} WriteResult_TypeDef;

/* Set Cursor Position Typedef */
typedef enum {
  POSITION_SET = 0,
  ROW_ERROR = 1,
  COL_ERROR = 2
} CursorPositionResult_TypeDef;


/* Initialize I2C for communication with PCF8574T using:
 * 		SDA -> PB9
 * 		SCK -> PB8
 * By default, the display lights up, enables the cursor and blink.
 */
void LCM1602_Display_Init(void);

/* Cleans the display, returns the cursor and the display (if they have been shifted) to the position
 * initial and reset the address counter of the DDRAM.
 *
 * NOTE: this function forces the cursor to shift to the right.
 */
void LCM1602_Display_ClearDisplay(void);

/* Return the cursor and the display to their original position. Leave the address unaltered
 * counter of the DDRAM.
 *
 * NOTE: The SOVRASCRIVE display! If you return to the original position and write when they are present
 * gi? of the characters, the latter will be lost. If you do not want to lose data already? writings are convenient
 * shift the whole display of how many characters you want to write. The shift of the entire display can? to be
 * done using the LCM1602_Display_CursorDisplayShift function.
 */
void LCM1602_Display_ReturnHome(void);

/* Enable display, cursor and blinking.
 * Parameters:
 * - DisplayMode_TypeDef display_mode: enable or disable the display.
 * 		-> possible values: DISPLAY_ON, DISPLAY_OFF
 * 			- DISPLAY_ON: lights up the display
 * 			- DISPLAY_OFF: turn off the display
 *
 * - CursorMode_TypeDef cursor_mode: enable or disable the cursor display
 * 		-> possible values: CURSOR_ON, CURSOR_OFF
 * 			- CURSOR_ON: enables cursor display
 * 			- CURSOR_OFF: disables the display of the cursor
 *
 * - BlinkingCursor_TypeDef blink_mode: enable or disable cursor blinking
 * 		-> possible values: CURSOR_ON, CURSOR_OFF
 * 			- BLINK_ON: enable blinking
 * 			- BLINK_OFF: disable blinking
 */
void LCM1602_Display_DisplayControl(DisplayMode_TypeDef display_mode, CursorMode_TypeDef cursor_mode,
		BlinkingCursor_TypeDef blink_mode);

/* Select the cursor direction and if the display also performs the shift.
 * Parameters:
 * - IncrementMode_TypeDef id_mode: indicates the shift direction of the cursor after writing.
 * 		-> possible values: INCREMENT, DECREMENT
 * 			- INCREMENT: the cursor shifts after writing to the right
 * 			- DECREMENT: the cursor shifts to the left
 *
 * - DisplayShiftMode_TypeDef ds_mode: indicates whether the display should shift together with the cursor after the
 *		writing. If enabled the cursor will appear? always stationary in the initial position.
 * 		-> possible values: SHIFT, NO_SHIFT
 * 			- SHIFT: enabled to shift with the cursor
 * 			- NO_SHIFT: not enabled to shift
 */
void LCM1602_Display_EntryModeSet(IncrementMode_TypeDef id_mode, DisplayShiftMode_TypeDef ds_mode);

/* Select whether to shift the cursor or display and in which direction.
 * Parameters:
 * - CursorDisplay_TypeDef cd: indicates whether to shift the cursor or the display.
 * 		-> possible values: CURSOR, DISPALY
 * 			- CURSOR: select the cursor
 * 			- DISPALY: select the display
 *
 * - ShiftDirection_TypeDef shift_direction: indicates the direction of the shift.
 * 		-> possible values: RIGHT, LEFT
 * 			- RIGHT: the shift will be? done on the right
 * 			- LEFT: the shift will be? made on the left
 *
 * NOTE: do a shift of the display does not result in the loss of data already? displayed on the screen.
 */
void LCM1602_Display_CursorDisplayShift(CursorDisplay_TypeDef cd, ShiftDirection_TypeDef shift_direction);

/* Move the cursor to the indicated position.
 * Parameters:
 * - uint8_t row: indicates the line where you want to place the cursor.
 * 		-> possible values: 1 <= row <= 4, values ??outside this range will have no effect on the
 * 			 display and function will return? the error value ROW_ERROR.
 *
 * - uint8_t col: indicates the column in which you want to position the cursor.
 * 		-> possible values: 1 <= row <= 20, values ??outside this range will have no effect on the
 * 			 display and function will return? the error value COL_ERROR.
 *
 * Return:
 * - POSITION_SET: successful operation and cursor positioned correctly.
 *
 * - ROW_ERROR: return value if the row parameter? greater than LC_DISPLAY_ROW = 4 or less than 0, in this
 * case? an incorrect line value has been selected and the function has no effect on the display.
 *
 * - COL_ERROR: return value if the parameter col? greater than LC_DISPLAY_COL = 20 or less than 0, in this
 * case? An incorrect column value has been selected and the function has no effect on the display.
 */
CursorPositionResult_TypeDef LCM1602_Display_SetCursorPosition(uint8_t row, uint8_t col);

/* Write on the display.
 * Parameters:
 * - uint8_t * pbuff: pointer to the hex buffer that you want to write on the display. See table
 * 	 on page 17-18 of the manual for the correspondence between hex and symbol to be shown on the display.
 *
 * - uint8_t length: length of the buffer. This parameter must not exceed the maximum length
 * 	 allowed equal to LC_DISPLAY_LENGTH = 80, otherwise the function has no effect.
 *
 * Return:
 * - DONE: writing successful.
 *
 * - LENGTH_ERROR: return value if the length parameter? greater than LC_DISPLAY_LENGTH = 80, in
 * 	 in this case the function had no effect on the display.
 */
WriteResult_TypeDef LCM1602_Display_WriteBuffer(uint8_t* pbuff, uint8_t length);

/* Write a string on the display.
 * Parameters:
 * - uint8_t * pbuff: pointer to the string. Not ? need to indicate the length of the string, but if
 * 	 exceeds LC_DISPLAY_LENGTH = 80 the excess characters will not be displayed.
 *
 * Return:
 * - DONE: writing successful.
 *
 * - LENGTH_ERROR: return value if the length parameter? greater than LC_DISPLAY_LENGTH = 80, in
 * 	 in this case the function had no effect on the display.
 */
WriteResult_TypeDef LCM1602_Display_WriteString(char* pbuff);


/*
----------------------------------------------------------------------
---   Private Functions   ---------------------------------------------
----------------------------------------------------------------------
*/
void LCM1602_Display_Delay(__IO uint32_t nTime);
void LCM1602_Display_Write(uint8_t data, WriteMode_TypeDef mode);
void LCM1602_Display_WriteCommand(uint8_t data);
void LCM1602_Display_WriteByte(uint8_t data);
void LCM1602_Display_I2C_Send(uint8_t data);
void LCM1602_Display_Write4Bit(uint8_t data, WriteMode_TypeDef mode);
void LCM1602_Display_PulseEnable(uint8_t data);

#endif /* LCM1602_DISPLAY_H_ */
