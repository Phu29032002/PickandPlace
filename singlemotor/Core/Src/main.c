/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  *///NEW siglemotor
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"
#include "stdio.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usbd_cdc_if.h"
#include "string.h "
#include "math.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */
char *data = "done\n";
char *data1 = "top\n";
char *data2 = "LoadData";
char *data3 = "Completed\n";
char *bot = "bottom";
char *detect = "cambottomdetected";
char *undetect = "cambottomundetected";
char *feeder = "feederok";
uint8_t buffer[128];
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define MOTORy_STEP_PIN GPIO_PIN_1
#define MOTORy_STEP_PORT GPIOA
#define MOTORy_DIR_PIN GPIO_PIN_2
#define MOTORy_DIR_PORT GPIOA

#define MOTORx_STEP_PIN GPIO_PIN_3
#define MOTORx_STEP_PORT GPIOA
#define MOTORx_DIR_PIN GPIO_PIN_4
#define MOTORx_DIR_PORT GPIOA
//
#define MOTORz_STEP_PIN GPIO_PIN_5
#define MOTORz_STEP_PORT GPIOA
#define MOTORz_DIR_PIN GPIO_PIN_6
#define MOTORz_DIR_PORT GPIOA

#define MOTORr_STEP_PIN GPIO_PIN_13
#define MOTORr_STEP_PORT GPIOB
#define MOTORr_DIR_PIN GPIO_PIN_8
#define MOTORr_DIR_PORT GPIOA

#define PUMPER_PIN GPIO_PIN_12
#define PUMPER_PORT GPIOB
//Markhead
uint16_t home = 0;
uint16_t test;
float Xtest=0;
float Ytest =0;
float Rtest=0;
char *bufftest;
/* Define motor directions */
#define MOTORy_FORWARD GPIO_PIN_SET
#define MOTORy_BACKWARD GPIO_PIN_RESET
#define MOTORx_LEFT GPIO_PIN_RESET
#define MOTORx_RIGHT GPIO_PIN_SET
#define MOTORz_UP GPIO_PIN_SET
#define MOTORz_DOWN GPIO_PIN_RESET
#define MOTORr_clockwise GPIO_PIN_SET

#define PUMP_OFF GPIO_PIN_RESET
#define PUMP_ON GPIO_PIN_SET
//#define LIMIT_X_PIN GPIO_PIN_10
//#define LIMIT_X_PORT GPIOB
#define LIMIT_Y_PIN GPIO_PIN_10
#define LIMIT_Y_PORT GPIOB
#define LIMIT_X_PIN GPIO_PIN_11
#define LIMIT_X_PORT GPIOB
#define LIMIT_Z_PIN GPIO_PIN_1
#define LIMIT_Z_PORT GPIOB

#define axis_X 1
#define axis_Y 2
#define axis_Z 3
#define axis_r 4
//Set
uint16_t max_pos_y = 160;
uint16_t max_pul_y = 32000;
uint16_t max_pos_x = 200;
uint16_t max_pul_x = 40000;
uint16_t max_pos_z = 400;
uint16_t max_pul_z = 8000;
uint16_t max_pos_r = 360;
uint16_t max_pul_r = 6400;
float moveX=0,moveY=0;
void delay_1ms(void)
{
 __HAL_TIM_SetCounter(&htim1, 0);
 while (__HAL_TIM_GetCounter(&htim1)<1);//fix 20 to delay 1ms 10 is a half = 0.5ms
}

void delay_ms(int time)
{
 int i = 0;
 for(i = 0; i < time; i++)
 {
   delay_1ms();
 }
}


void MotorY (unsigned long long steps, uint8_t direction)
{
  uint16_t AccMax = 1;
  int x;
  if (direction == 0)
	  HAL_GPIO_WritePin(MOTORy_DIR_PORT, MOTORy_DIR_PIN, MOTORy_BACKWARD);
  else
	  HAL_GPIO_WritePin(MOTORy_DIR_PORT, MOTORy_DIR_PIN, MOTORy_FORWARD);
  for(x=0; x<steps; x=x+1)
  {

	HAL_GPIO_WritePin(MOTORy_STEP_PORT, MOTORy_STEP_PIN, GPIO_PIN_SET);
	delay_ms(AccMax);
	HAL_GPIO_WritePin(MOTORy_STEP_PORT, MOTORy_STEP_PIN, GPIO_PIN_RESET);
	delay_ms(AccMax);
  }
}
void SetHomeY()
{
	  uint16_t AccMax = 1;
		HAL_GPIO_WritePin(MOTORy_DIR_PORT, MOTORy_DIR_PIN, GPIO_PIN_RESET);
		while(HAL_GPIO_ReadPin(LIMIT_Y_PORT,LIMIT_Y_PIN))
		{

		HAL_GPIO_WritePin(MOTORy_STEP_PORT, MOTORy_STEP_PIN, GPIO_PIN_SET);
		delay_ms(AccMax);
		HAL_GPIO_WritePin(MOTORy_STEP_PORT, MOTORy_STEP_PIN, GPIO_PIN_RESET);
		delay_ms(AccMax);
		}


}
void MotorX (uint16_t steps, uint8_t direction)
{
  uint16_t AccMax = 1;

  uint16_t x;
  if (direction == 0)
	  HAL_GPIO_WritePin(MOTORx_DIR_PORT, MOTORx_DIR_PIN, MOTORx_LEFT);
  else
	  HAL_GPIO_WritePin(MOTORx_DIR_PORT, MOTORx_DIR_PIN, MOTORx_RIGHT);
  for(x=0; x<steps; x=x+1)
  {

	HAL_GPIO_WritePin(MOTORx_STEP_PORT, MOTORx_STEP_PIN, GPIO_PIN_SET);
	delay_ms(AccMax);
	HAL_GPIO_WritePin(MOTORx_STEP_PORT, MOTORx_STEP_PIN, GPIO_PIN_RESET);
	delay_ms(AccMax);
  }
}
void SetHomeX()
{
	  uint16_t AccMax = 1;
		HAL_GPIO_WritePin(MOTORx_DIR_PORT, MOTORx_DIR_PIN, MOTORx_LEFT);
		while(HAL_GPIO_ReadPin(LIMIT_X_PORT,LIMIT_X_PIN))
		{

		HAL_GPIO_WritePin(MOTORx_STEP_PORT, MOTORx_STEP_PIN, GPIO_PIN_SET);
		delay_ms(AccMax);
		HAL_GPIO_WritePin(MOTORx_STEP_PORT, MOTORx_STEP_PIN, GPIO_PIN_RESET);
		delay_ms(AccMax);
		}
		//MotorX(600,1);
}
void MotorZ (unsigned long long steps, uint8_t direction)
{
  uint16_t AccMax = 4;
  int x;
  if (direction == 0)
	  HAL_GPIO_WritePin(MOTORz_DIR_PORT, MOTORz_DIR_PIN, MOTORz_DOWN);
  else
	  HAL_GPIO_WritePin(MOTORz_DIR_PORT, MOTORz_DIR_PIN, MOTORz_UP);
  for(x=0; x<steps; x=x+1)
  {
	HAL_GPIO_WritePin(MOTORz_STEP_PORT, MOTORz_STEP_PIN, GPIO_PIN_SET);
	delay_ms(AccMax);
	HAL_GPIO_WritePin(MOTORz_STEP_PORT, MOTORz_STEP_PIN, GPIO_PIN_RESET);
	delay_ms(AccMax);
  }

}
void MotorR (unsigned long long steps, uint8_t direction)
{
  uint16_t AccMax = 1;
  int x;
  if (direction == 0)
	  HAL_GPIO_WritePin(MOTORr_DIR_PORT, MOTORr_DIR_PIN, MOTORr_clockwise);
  else
	  HAL_GPIO_WritePin(MOTORr_DIR_PORT, MOTORr_DIR_PIN, GPIO_PIN_RESET);
  for(x=0; x<steps; x=x+1)
  {
	HAL_GPIO_WritePin(MOTORr_STEP_PORT, MOTORr_STEP_PIN, GPIO_PIN_SET);
	delay_ms(AccMax);
	HAL_GPIO_WritePin(MOTORr_STEP_PORT, MOTORr_STEP_PIN, GPIO_PIN_RESET);
	delay_ms(AccMax);
  }

}
void SetHomeZ()
{
	uint16_t AccMax = 1;
			HAL_GPIO_WritePin(MOTORz_DIR_PORT, MOTORz_DIR_PIN, GPIO_PIN_SET);
			while(HAL_GPIO_ReadPin(LIMIT_Z_PORT,LIMIT_Z_PIN))
			{

			HAL_GPIO_WritePin(MOTORz_STEP_PORT, MOTORz_STEP_PIN, GPIO_PIN_SET);
			delay_ms(AccMax);
			HAL_GPIO_WritePin(MOTORz_STEP_PORT, MOTORz_STEP_PIN, GPIO_PIN_RESET);
			delay_ms(AccMax);
			}
}




uint16_t convert(float pos, int axis)
{
    float max_pos = -1;
    float max_pul =  -1;
	switch(axis)
	{
	case axis_X:
		max_pos = (float)max_pos_x;
		max_pul = (float)max_pul_x;
		break;
	case axis_Y:
		max_pos = (float)max_pos_y;
		max_pul = (float)max_pul_y;
		break;
	case axis_r:
		max_pos = (float)max_pos_r;
		max_pul = (float)max_pul_r;
		break;
	}
	if(max_pul != -1 && max_pos!=-1)
	{
		float PosPerPul = max_pos/max_pul;
		uint16_t reqpul = (uint16_t)ceil(fabs(pos) / PosPerPul);
		test = reqpul;
		if(pos>0)
		{
			switch(axis)
			{
			case axis_X:
				MotorX(reqpul,1);
				break;
			case axis_Y:
				MotorY(reqpul,1);
				break;
			case axis_r:
				MotorR(reqpul,1);
				break;
			}
		}
		else
		{
			switch(axis)
			{
			case axis_X:
				MotorX(reqpul,0);
				break;
			case axis_Y:
				MotorY(reqpul,0);
				break;
			case axis_r:
				MotorR(reqpul,0);
				break;
			}
		}
	}
}
void reversepcb()
{
	delay_ms(500);
	MotorX(23400,0);
	delay_ms(1500);
}

void clearBuffer() {
    memset(buffer, 0, sizeof(buffer));
}

void Feeder()
{
	MotorX(1000,1);
	MotorY(1000,1);
}

void SetHomePNP()
{
	  SetHomeZ();
	  delay_ms(500);
	  SetHomeX();
	  delay_ms(500);
	  MotorX(8000,1);
	  delay_ms(500);
	  SetHomeY();
	  delay_ms(500);
	  MotorY(8000,1);

}
void SetHome2()
{
	  SetHomeZ();
	  delay_ms(1000);
	  SetHomeX();
	  delay_ms(1000);
	 // MotorX(5400,1);//-100
	  convert(54.2,axis_X); //R3 54
	  delay_ms(1000);
	  SetHomeY();
	  delay_ms(1000);
	  //MotorY(4300,1);
	  convert(43.9,axis_Y);//43.1
	  delay_ms(1500);
}

void checkpointcambot()
{
	  delay_ms(500);
	  MotorX(23400,1);
	  delay_ms(1500);
	 // MotorY(20,0);

}

void checkpointfeeder()
{
	  delay_ms(500);
	  MotorY(21000,1);
}
void Checkbottom()
{

	MotorY(21000,0);
	delay_ms(2500);
	CDC_Transmit_FS(bot, strlen((char*)bot));
	ListenBot("cambottomdetected");

}
void listen(char* message)
{

    while (strcmp((char*)buffer, message) != 0)
    {


        delay_ms(4000);
    }


    //CDC_Transmit_FS(data, strlen((char*)data));
}
void ListenBot(char* message)
{
	int startTime = HAL_GetTick();
	clearBuffer();
	  while (strcmp((char*)buffer, message) != 0)
	    {
		          if (HAL_GetTick() - startTime >= 10000)
		          {
		        	  checkpointfeeder();
		              return;//NVIC_SystemReset(); // Reset the STM32
		          }

	        delay_ms(4000);
	    }
	  return;
//	  	  delay_ms(5000);
//	  	  SetHome2();
//	  	  convert(90,axis_r);
//	  	  delay_ms(1500);
//	  	  convert( -0.5222,axis_X);
//	  	  delay_ms(1500);
//	  	  convert( -2.6112,axis_Y);
//	  	  delay_ms(1500);
//	  	  MotorZ(7400,0);
//	  	  delay_ms(3000);
//	  	  HAL_GPIO_WritePin(PUMPER_PORT, PUMPER_PIN, PUMP_OFF);
//	  	  delay_ms(15000);
//	  	  SetHomeZ();
//	  	  clearBuffer();
}
char numStr[21];
void ull_to_string(char* str, unsigned long long a) {
    char convert[21]; // Buffer to hold the number plus a null-terminator
    int i = 0;

    if (a == 0) {
        convert[i++] = '0';
    }

    // Convert number to string in reverse order
    while (a != 0) {
        unsigned long long last_digit = a % 10;
        convert[i++] = '0' + last_digit;
        a /= 10;
    }

    convert[i] = '\0'; // Null-terminate the string

    // Reverse the string
    int j;
    for (j = 0; j < i / 2; j++) {
        char tmp = convert[j];
        convert[j] = convert[i - 1 - j];
        convert[i - 1 - j] = tmp;
    }

    strcpy(str, convert); // Copy the reversed string to the output
}
void float_to_string(char* str, float value) {
    sprintf(str,"%.3f", value); // Convert float to string with 6 decimal places
}

struct PartData {
    char part_id; // Store only the first character of the Part ID
    float rotation;
    float x;
    float y;
};
//markcomp
#define MAX_PARTS 100
struct PartData parts[MAX_PARTS];

int part_count = 0;
uint8_t newdata[64];
// Function to parse incoming data string
struct PartData parseDataString(char *data) {
    char *token = strtok(data, ",");
    struct PartData partData;
    // Store part_id
    if (token != NULL)
        partData.part_id = token[0]; // Store only the first character of the Part ID

    // Move to the next token and store rotation
    token = strtok(NULL, ",");
    if (token != NULL)
        partData.rotation = atof(token); // Convert string to float

    // Move to the next token and store x
    token = strtok(NULL, ",");
    if (token != NULL)
        partData.x = atof(token); // Convert string to float

    // Move to the next token and store y
    token = strtok(NULL, ",");
    if (token != NULL)
        partData.y = atof(token); // Convert string to float
    return partData;
}


void SendCheckY()
{
	char numStr[32];

	    for (int i = 0; i < part_count; i++) {
	        float_to_string(numStr, parts[i].y);
	        delay_ms(500);
	        CDC_Transmit_FS((uint8_t *)numStr, strlen(numStr));
	        delay_ms(2000);
	    }
}

void SendCheck() {
    for (int i = 0; i < part_count; i++) {
        char message[2]; // Buffer to hold the part_id and a null terminator
        message[0] = parts[i].part_id; // Assign the part ID to the first character
        message[1] = '\0'; // Null-terminate the string

        CDC_Transmit_FS((uint8_t*)message, 1); // Send only the part_id character
        delay_ms(4000); // Adjust the delay as needed
    }
}
void Component()
{
	CDC_Transmit_FS(data2, strlen((char*)data2));
	clearBuffer();
	char *end = "end";
	 while (strcmp((char*)buffer, end) != 0)
	  {
		 CDC_Transmit_FS(buffer, strlen((char*)buffer));
		 struct PartData partData = parseDataString(buffer);
		 // Only add parts with non-zero x and y values
		         if (fabs(partData.x) > 0.0001 && fabs(partData.y) > 0.0001)
		         {
		             parts[part_count++] = partData;
		         }
		 clearBuffer();
		 	delay_ms(6000);
	  }
	 CDC_Transmit_FS(data3, strlen((char*)data3));
	 delay_ms(2000);
}
void PickandPlace()
{

}

void testPlace()
{
    for(int i = 0; i < part_count; i++)
    {
        // Check and skip if parts[i].x is exactly zero
        if (fabs(parts[i].x) > 0.0001)
        {
            convert(parts[i].x, axis_X);
            delay_ms(1500);
        }

        // Check and skip if parts[i].y is exactly zero
        if (fabs(parts[i].y) > 0.0001)
        {
            convert(parts[i].y, axis_Y);
            delay_ms(1500);
        }

        MotorZ(7000, 0);
        delay_ms(1500);
        SetHomeZ();
        delay_ms(1500);

        // Check and skip if parts[i].x is exactly zero for reverse movement
        if (fabs(parts[i].x) > 0.0001)
        {
            convert(parts[i].x * (-1), axis_X);
            delay_ms(1500);
        }

        // Check and skip if parts[i].y is exactly zero for reverse movement
        if (fabs(parts[i].y) > 0.0001)
        {
            convert(parts[i].y * (-1), axis_Y);
            delay_ms(1500);
        }
    }
}
void Place()
{
    // Check and skip if parts[i].x is exactly zero
    if (fabs(parts[i].x) > 0.0001)
    {
        convert(parts[i].x, axis_X);
        delay_ms(1500);
    }

    // Check and skip if parts[i].y is exactly zero
    if (fabs(parts[i].y) > 0.0001)
    {
        convert(parts[i].y, axis_Y);
        delay_ms(1500);
    }

    MotorZ(7000, 0);
    delay_ms(1500);
    SetHomeZ();
    delay_ms(1500);

    // Check and skip if parts[i].x is exactly zero for reverse movement
    if (fabs(parts[i].x) > 0.0001)
    {
        convert(parts[i].x * (-1), axis_X);
        delay_ms(1500);
    }

    // Check and skip if parts[i].y is exactly zero for reverse movement
    if (fabs(parts[i].y) > 0.0001)
    {
        convert(parts[i].y * (-1), axis_Y);
        delay_ms(1500);
    }
}
void pickIC(int a)
{
	delay_ms(3500);
	convert( -27.8,axis_X);
	delay_ms(3500);
	a = a*(-8);
	convert((45.2-a),axis_Y);
	//convert((45.2),axis_Y);
	delay_ms(3500);
	MotorZ(7200,0);
	delay_ms(1500);
	HAL_GPIO_WritePin(PUMPER_PORT, PUMPER_PIN, PUMP_ON);
	delay_ms(40500);
	SetHomeZ();
	delay_ms(3500);
	convert( 27.8,axis_X);
	delay_ms(3500);
	convert(((-1)*(45.2-a)),axis_Y);
	delay_ms(3500);
}
void pickCAP(int a)
{
	delay_ms(3500);
	convert(-8.5,axis_X);
	delay_ms(3500);
	a = a*(-4);
	convert((44.4-a),axis_Y);
	delay_ms(3500);
	MotorZ(7500,0);
	delay_ms(1500);
	HAL_GPIO_WritePin(PUMPER_PORT, PUMPER_PIN, PUMP_ON);
	delay_ms(40500);
	SetHomeZ();
	delay_ms(3500);
	convert( 8.5,axis_X);
	delay_ms(3500);
	convert(((-1)*(44.4-a)),axis_Y);
	delay_ms(3500);
}
void pickLed(int a)
{
	delay_ms(3500);
	convert(3.5,axis_X);
	delay_ms(3500);
	a = a*(-4);
	convert((44.4-a),axis_Y);
	delay_ms(3500);
	MotorZ(7500,0);
	delay_ms(1500);
	HAL_GPIO_WritePin(PUMPER_PORT, PUMPER_PIN, PUMP_ON);
	delay_ms(40500);
	SetHomeZ();
	delay_ms(3500);
	convert(-3.5,axis_X);
	delay_ms(3500);
	convert(((-1)*(44.4-a)),axis_Y);
	delay_ms(3500);
}
void pickRes(int a)
{
	delay_ms(3500);
	convert(15.5,axis_X);
	delay_ms(3500);
	a = a*(-4);
	convert((44.4-a),axis_Y);
	delay_ms(3500);
	MotorZ(7500,0);
	delay_ms(1500);
	HAL_GPIO_WritePin(PUMPER_PORT, PUMPER_PIN, PUMP_ON);
	delay_ms(40500);
	SetHomeZ();
	delay_ms(3500);
	convert(-15.5,axis_X);
	delay_ms(3500);
	convert(((-1)*(44.4-a)),axis_Y);
	delay_ms(3500);

}
void testserial()
{
	listen("bottom");
}
void Stage1()
{
	listen("start");
	SetHomePNP();
	listen("camtopok");
}
void Stage2()
{
	SetHome2();
	checkpointcambot();
	checkpointfeeder();
}
void Stage3()
{
	listen("LoadData");
	Component();
}

void Stage4()
{
	liseten("feederok");
	delay_ms(1500);
	//automation
	int part_num=0;
	int cap=0,ic=0,res=0,led=0;
	for(int i =0;i<part_count;i++)
	{

		part_num=i;
		moveX = parts[i].x;
		moveY = parts[i].y;
		if(parts[i].part_id=='U')
		{
			PickIC(ic);
			ic++;
		}
		else if(parts[i].part_id=='C')
		{
			PickCap(cap);
			cap++;
		}
		else if(parts[i].part_id=='R')
		{
			PickCap(res);
			res++;

		}
		else if(parts[i].part_id=='D')
		{
			PickCap(led);
			led++;
		}
		Checkbottom();
		reversepcb();
	    if (fabs(parts[i].x) > 0.0001)
	    {
	        convert(parts[i].x, axis_X);
	        delay_ms(1500);
	    }

	    // Check and skip if parts[i].y is exactly zero
	    if (fabs(parts[i].y) > 0.0001)
	    {
	        convert(parts[i].y, axis_Y);
	        delay_ms(1500);
	    }

	    MotorZ(7000, 0);
	    delay_ms(1500);
	    SetHomeZ();
	    delay_ms(1500);

	    // Check and skip if parts[i].x is exactly zero for reverse movement
	    if (fabs(parts[i].x) > 0.0001)
	    {
	        convert(parts[i].x * (-1), axis_X);
	        delay_ms(1500);
	    }

	    // Check and skip if parts[i].y is exactly zero for reverse movement
	    if (fabs(parts[i].y) > 0.0001)
	    {
	        convert(parts[i].y * (-1), axis_Y);
	        delay_ms(1500);
	    }
		checkpointcambot();
		checkpointfeeder();
	}
}
void testPick()
{
	int part_num=0;
	int cap=0,ic=0,res=0,led=0;
	for(int i =0;i<part_count;i++)
	{
		part_num=i;
		moveX = parts[i].x;
		moveY = parts[i].y;
		if(parts[i].part_id=='U')
		{
			PickIC(ic);
			ic++;

		}
		else if(parts[i].part_id=='C')
		{
			PickCap(ic);
			cap++;
		}
		else if(parts[i].part_id=='R')
		{
			PickCap(ic);
			cap++;
		}
		else if(parts[i].part_id=='R')
		{
			PickCap(ic);
			cap++;
		}

	}
}
void test1()
{
	Stage1();
	Stage2();


}

void test2()
{
	SetHomeZ();
	delay_ms(3500);
	SetHome2();
	delay_ms(3500);
	convert(-8.1,axis_X);//C1
	delay_ms(1500);
	convert(0.04,axis_Y);//C1
	delay_ms(1500);
	MotorZ(7400,0);
	delay_ms(14000);
	SetHomeZ();
	delay_ms(3500);
	SetHome2();
	delay_ms(3500);
	convert(3.96679,axis_X);//R5
	delay_ms(1500);
	convert(10,axis_Y);//R5
	delay_ms(1500);
	MotorZ(7400,0);
	delay_ms(14000);
	SetHomeZ();
	delay_ms(3500);
	SetHome2();
	delay_ms(3500);
	convert(-4,axis_X);//R3
	delay_ms(1500);
	convert(10.0182,axis_Y);//R3
	delay_ms(1500);
	MotorZ(7400,0);
	delay_ms(14000);
	SetHomeZ();
	delay_ms(3500);
	SetHome2();
	delay_ms(3500);
	convert(7.03001,axis_X);//D1
	delay_ms(1500);
	convert(-9.98179,axis_Y);//D1
	delay_ms(1500);
	MotorZ(7400,0);
	delay_ms(14000);
	SetHomeZ();
	delay_ms(3500);
	SetHome2();
	delay_ms(3500);
	convert(-0.02,axis_X);//D2
	delay_ms(1500);
	convert(-9.98179,axis_Y);//D2
	delay_ms(1500);
	MotorZ(7400,0);
	delay_ms(14000);


}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start(&htim1);
  /* USER CODE END 2 */
  Stage1();
  Stage2();
  Stage3();
  Stage4();

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  // Transmit fixed message
//	          CDC_Transmit_FS(data, strlen((char *)data));
//	          HAL_Delay(1000);
//	          // Convert num to string and transmit
//	          ull_to_string(numStr, num);
//	          CDC_Transmit_FS((uint8_t *)numStr, strlen(numStr));
	        //  listen();
	          //HAL_Delay(1000);

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 2400;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 2400;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 65535;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4
                          |GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_8, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12|GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA1 PA2 PA3 PA4
                           PA5 PA6 PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4
                          |GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB1 PB10 PB11 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_10|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB12 PB13 */
  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM3 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM3) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
