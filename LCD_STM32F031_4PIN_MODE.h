/*
 * LCD_STM32F031_4PIN_MODE.h
 *
 *  Created on: 5. 6. 2020
 *      Author: mikov
 */

#ifndef INC_LCD_STM32F031_4PIN_MODE_H_
#define INC_LCD_STM32F031_4PIN_MODE_H_


/*
 *
 * Takto pojmenuj porty, RS, E, D4, D5, D6, D7
#define RS_Pin GPIO_PIN_0
#define RS_GPIO_Port GPIOF
#define E_Pin GPIO_PIN_1
#define E_GPIO_Port GPIOF
#define D4_Pin GPIO_PIN_8
#define D4_GPIO_Port GPIOA
#define D5_Pin GPIO_PIN_11
#define D5_GPIO_Port GPIOA
#define D7_Pin GPIO_PIN_4
#define D7_GPIO_Port GPIOB
#define D6_Pin GPIO_PIN_5
#define D6_GPIO_Port GPIOB
*/

//***** List of COMMANDS *****//
#define LCD_CLEARDISPLAY 		0x01
#define LCD_RETURNHOME 			0x02
#define LCD_ENTRYMODESET 		0x04
#define LCD_DISPLAYCONTROL	 	0x08
#define LCD_CURSORSHIFT 		0x10
#define LCD_FUNCTIONSET 		0x20
#define LCD_SETCGRAMADDR 		0x40
#define LCD_SETDDRAMADDR 		0x80
#define TimeDelayBeforeEnable 	800
#define TimeDelayBeforeDisable	 1600
//***** List of commands Bitfields *****//
//1) Entry mode Bitfields
#define LCD_ENTRY_SH		 	0x01
#define LCD_ENTRY_ID 			0x02
//2) Entry mode Bitfields
#define LCD_ENTRY_SH		 	0x01
#define LCD_ENTRY_ID 			0x02
//3) Display control
#define LCD_DISPLAY_B			0x01
#define LCD_DISPLAY_C			0x02
#define LCD_DISPLAY_D			0x04
//4) Shift control
#define LCD_SHIFT_RL			0x04
#define LCD_SHIFT_SC			0x08
//5) Function set control
#define LCD_FUNCTION_F			0x04
#define LCD_FUNCTION_N			0x08
#define LCD_FUNCTION_DL			0x10

static uint8_t DisplayControl = 0x0F;


// 8MHz delay function
void nonExactTimeDelay(int timeDelay){

	volatile int i;
for(i=0; i<=timeDelay; i++){

}
}

// Enable LCD registers
void EnableLCD(void){

		nonExactTimeDelay(TimeDelayBeforeEnable);
		HAL_GPIO_WritePin(E_GPIO_Port, E_Pin,GPIO_PIN_SET);
		nonExactTimeDelay(TimeDelayBeforeDisable);
		HAL_GPIO_WritePin(E_GPIO_Port , E_Pin, GPIO_PIN_RESET );
}

// Register select
void RegisterSelect(unsigned short bitstate){
	if(bitstate == 1){
		HAL_GPIO_WritePin(RS_GPIO_Port, RS_Pin, GPIO_PIN_SET);
	}
	else{
		HAL_GPIO_WritePin(RS_GPIO_Port, RS_Pin, GPIO_PIN_RESET);
	}
}

// Write functions
static void LCDwrite(char byte){

	char LowerInstruction = byte&0xF;
	char UpperInstruction = (byte>>4)&0xF;

	//4 bits mode

		//write data to output pins
		//MSB data
		//LSB data
	HAL_GPIO_WritePin(D4_GPIO_Port, D4_Pin, (GPIO_PinState)(UpperInstruction  & 0x1));
	HAL_GPIO_WritePin(D5_GPIO_Port, D5_Pin, (GPIO_PinState)(UpperInstruction  & 0x2));
	HAL_GPIO_WritePin(D6_GPIO_Port, D6_Pin, (GPIO_PinState)(UpperInstruction  & 0x4));
	HAL_GPIO_WritePin(D7_GPIO_Port, D7_Pin, (GPIO_PinState)(UpperInstruction  & 0x8));

		//Write the Enable pulse
	EnableLCD();

		//LSB data
		//LSB data

	HAL_GPIO_WritePin(D4_GPIO_Port, D4_Pin, (GPIO_PinState)(LowerInstruction & 0x1));
	HAL_GPIO_WritePin(D5_GPIO_Port, D5_Pin, (GPIO_PinState)(LowerInstruction & 0x2));
	HAL_GPIO_WritePin(D6_GPIO_Port, D6_Pin, (GPIO_PinState)(LowerInstruction & 0x4));
	HAL_GPIO_WritePin(D7_GPIO_Port, D7_Pin, (GPIO_PinState)(LowerInstruction & 0x8));

	//Write the Enable pulse
	EnableLCD();

}

void SendACharacter(char character){
	RegisterSelect(1);
	LCDwrite(character);
}

void SendAnInstruction(char character){
	RegisterSelect(0);
	LCDwrite(character);
}

static void LCD1602_write4bitCommand(char Instruction4Bit){
	unsigned short LowerInstruction = Instruction4Bit&0xF;
	//Set RS to 0
	RegisterSelect(0);
	//LSB data

	HAL_GPIO_WritePin(D4_GPIO_Port, D4_Pin, (GPIO_PinState)(LowerInstruction & 0x1));
	HAL_GPIO_WritePin(D5_GPIO_Port, D5_Pin, (GPIO_PinState)(LowerInstruction & 0x2));
	HAL_GPIO_WritePin(D6_GPIO_Port, D6_Pin, (GPIO_PinState)(LowerInstruction & 0x4));
	HAL_GPIO_WritePin(D7_GPIO_Port, D7_Pin, (GPIO_PinState)(LowerInstruction & 0x8));

	//Write the Enable pulse
	EnableLCD();
}


void LCDInitialize(){
//Set the mode to 4 bits
	//Function set variable to 4 bits mode

	//Initialise LCD
	//1. Wait at least 15ms
	nonExactTimeDelay(300);
	//2. Attentions sequence
	LCD1602_write4bitCommand(0x3);
	nonExactTimeDelay(100);
	LCD1602_write4bitCommand(0x3);
	nonExactTimeDelay(10);
	LCD1602_write4bitCommand(0x3);
	nonExactTimeDelay(10);
	LCD1602_write4bitCommand(0x2);  //4 bit mode
	nonExactTimeDelay(10);
	//3. Display control (Display ON, Cursor ON, blink cursor)
	SendAnInstruction(LCD_DISPLAYCONTROL |/* LCD_DISPLAY_B | LCD_DISPLAY_C |*/ LCD_DISPLAY_D);
	nonExactTimeDelay(100);
	//4. Clear LCD and return home
	SendAnInstruction(LCD_CLEARDISPLAY);
	nonExactTimeDelay(100);
	//4. Function set; Enable 2 lines, Data length to 4 bits
	SendAnInstruction(LCD_FUNCTIONSET | LCD_FUNCTION_N);
	nonExactTimeDelay(100);

}

void LCD1602_print(char *string){

	while(*string){
		SendACharacter(*string++);
	}
}

//4) set cursor position
void LCD1602_setCursor(uint8_t row, uint8_t col)
{
	uint8_t maskData;
	maskData = (col-1)&0x0F;
	if(row==1)
	{
		maskData |= (0x80);
		SendAnInstruction(maskData);
	}
	else
	{
		maskData |= (0xc0);
		SendAnInstruction(maskData);
	}
}

//********** Print numbers to LCD **********//
//1. Integer
void LCD1602_PrintInt(int number)
{
	char numStr[16];
	sprintf(numStr,"%d", number);
	LCD1602_print(numStr);
}

void LCDSendAndInteger(int IntegerToBeDisplayed, unsigned short MaxLenghtOfDigits){

	char StringNumber[MaxLenghtOfDigits];
	sprintf(StringNumber, "%d", IntegerToBeDisplayed);
	LCD1602_print(StringNumber);
}
//2. Float
void LCD1602_PrintFloat(float number, int decimalPoints)
{

	char StringNumber[decimalPoints];
	snprintf(StringNumber, decimalPoints +1 , "%f", number);
	LCD1602_print(StringNumber);
}

//6) Cursor ON/OFF
void LCD1602_noCursor(void)
{
	DisplayControl &= ~(0x02);
	SendAnInstruction(DisplayControl);
}
void LCD1602_cursor(void)
{
	DisplayControl |= (0x02);
	SendAnInstruction(DisplayControl);
}
//7) Clear display
void LCD1602_clear(void)
{
	SendAnInstruction(LCD_CLEARDISPLAY);
	//nonExactTimeDelay(50);
}
//8) Blinking cursor
void LCD1602_noBlink(void)
{
	DisplayControl &= ~(0x01);
	SendAnInstruction(DisplayControl);
}
void LCD1602_blink(void)
{
	DisplayControl |= 0x01;
	SendAnInstruction(DisplayControl);
}

//9) Display ON/OFF
void LCD1602_noDisplay(void)
{
	DisplayControl &= ~(0x04);
	SendAnInstruction(DisplayControl);
}
void LCD1602_display(void)
{
	DisplayControl |= (0x04);
	SendAnInstruction(DisplayControl);
}
//10) Shift Display, right or left
void LCD1602_shiftToRight(uint8_t num)
{
	for(uint8_t i=0; i<num;i++)
	{
		SendAnInstruction(0x1c);
	}
}
void LCD1602_shiftToLeft(uint8_t num)
{
	for(uint8_t i=0; i<num;i++)
	{
		SendAnInstruction(0x18);
	}
}


#endif /* INC_LCD_STM32F031_4PIN_MODE_H_ */
