/*
 * GccApplication2.c
 *
 * Created: 23.11.2019 17:37:23
 * Author : mikov
 */


#define F_CPU 8000000UL

// MCU AT90CAN128


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


typedef struct{
	float x;
	float y;
	float z;
} COORDINATES;

//Declare structs

COORDINATES P1, *p_P1 = &P1;
COORDINATES P2, *p_P2 = &P2;
COORDINATES P3, *p_P3 = &P3;
COORDINATES P4, *p_P4 = &P4;
COORDINATES P5, *p_P5 = &P5;
COORDINATES P6, *p_P6 = &P6;

COORDINATES A1, *p_A1 = &A1;
COORDINATES A2, *p_A2 = &A2;
COORDINATES A3, *p_A3 = &A3;
COORDINATES A4, *p_A4 = &A4;
COORDINATES A5, *p_A5 = &A5;
COORDINATES A6, *p_A6 = &A6;

COORDINATES B1, *p_B1 = &B1;
COORDINATES B2, *p_B2 = &B2;
COORDINATES B3, *p_B3 = &B3;
COORDINATES B4, *p_B4 = &B4;
COORDINATES B5, *p_B5 = &B5;
COORDINATES B6, *p_B6 = &B6;

COORDINATES q1, *p_q1 = &q1;
COORDINATES q2, *p_q2 = &q2;
COORDINATES q3, *p_q3 = &q3;
COORDINATES q4, *p_q4 = &q4;
COORDINATES q5, *p_q5 = &q5;
COORDINATES q6, *p_q6 = &q6;

COORDINATES L1, *p_L1 = &L1;
COORDINATES L2, *p_L2 = &L2;
COORDINATES L3, *p_L3 = &L3;
COORDINATES L4, *p_L4 = &L4;
COORDINATES L5, *p_L5 = &L5;
COORDINATES L6, *p_L6 = &L6;

typedef float Matrix3x3[3][3];

Matrix3x3 RollMatrix, PitchMatrix, YawMatrix, RollPitchMatrix, RollPitchYawMatrix;

typedef float Vector[3];

Vector rq1, rq2, rq3, rq4, rq5, rq6;

double dutyCycle = 0;
double dutyCycle1 = 0;

int cliFlag = 0;

//Fixed leg length

int s = 115;

//Fixed servo arm length

int a = 15;

//Servo angle relative to the X axis

float pi= 3.14159265359;
int beta[6] = {330, 150, 90, 270, 210, 30};
float betar[6];

// Translation vector connecting the middle of the base with the middle of the platform
// SET VALUES xi, yi, zi

double xi= 0; //max 36 min -25
double yi= 0; //max 36 min -36
double zi= 0; //max 126 min 100

// SET VALUES roll, pitch a yaw

double roll = 0;
double pitch = 0;
double yaw = 0;

// Values roll, pitch a yaw in radians

float rollr;
float pitchr;
float yawr;

// Values for timers

float pwm_servo1;
float pwm_servo2;
float pwm_servo3;
float pwm_servo4;
float pwm_servo5;
float pwm_servo6;

// Required leg length

float l1;
float l2;
float l3;
float l4;
float l5;
float l6;

//Support calculation variables

float M[6], N[6], S[6];

//Servo arm angle

float alfa[6];
float alfad[6];

void SetUpPlatformCoordinates(void){

	//Definition of the anchor points of the platform
	//Referential point lays in the middle of the platform

	p_P1->x = 44.7;
	p_P1->y = 5;
	p_P1->z = 0;

	p_P2->x = -20.29;
	p_P2->y = 40.15;
	p_P2->z = 0;

	p_P3->x = -24.62;
	p_P3->y = 37.64;
	p_P3->z = 0;

	p_P4->x = -24.62;
	p_P4->y = -37.64;
	p_P4->z = 0;

	p_P5->x = -20.29;
	p_P5->y = -40.15;
	p_P5->z = 0;

	p_P6->x = 44.7;
	p_P6->y = -5;
	p_P6->z = 0;

	//Anchor points of leg with servo arm
	//Referential point lays in the middle of the base

	p_A1->x = 52.8;
	p_A1->y = 22.45;
	p_A1->z = 0;

	p_A2->x = -6.96;
	p_A2->y = 56.95;
	p_A2->z = 0;

	p_A3->x = -24.62;
	p_A3->y = 37.64;
	p_A3->z = 0;

	p_A4->x = -45.84;
	p_A4->y = -34.5;
	p_A4->z = 0;

	p_A5->x = -6.96;
	p_A5->y = -56.95;
	p_A5->z = 0;

	p_A6->x = 52.8;
	p_A6->y = -22.45;
	p_A6->z = 0;

	//Anchor points of servo arm with servo shaft

	p_B1->x = 39.81;
	p_B1->y = 29.95;
	p_B1->z = 0;

	p_B2->x = 6.03;
	p_B2->y = 49.45;
	p_B2->z = 0;

	p_B3->x = -45.84;
	p_B3->y = 19.5;
	p_B3->z = 0;

	p_B4->x = -45.84;
	p_B4->y = -19.5;
	p_B4->z = 0;

	p_B5->x = 6.03;
	p_B5->y = -49.45;
	p_B5->z = 0;

	p_B6->x = 39.81;
	p_B6->y = -29.948;
	p_B6->z = 0;

	//Servo angle relative to the X axis from degrees to radians

	betar[0] = beta[0] * (pi/180);
	betar[1] = beta[1] * (pi/180);
	betar[2] = beta[2] * (pi/180);
	betar[3] = beta[3] * (pi/180);
	betar[4] = beta[4] * (pi/180);
	betar[5] = beta[5] * (pi/180);

}

void SetupMatrixes(void){


	RollMatrix[0][0] = 1;
	RollMatrix[1][0] = 0;
	RollMatrix[2][0] = 0;
	RollMatrix[0][1] = 0;
	RollMatrix[1][1] = cos(rollr);
	RollMatrix[2][1] = sin(rollr);
	RollMatrix[0][2] = 0;
	RollMatrix[1][2] = -sin(rollr);
	RollMatrix[2][2] = cos(rollr);

	PitchMatrix[0][0] = cos(pitchr);
	PitchMatrix[1][0] = 0;
	PitchMatrix[2][0] = -sin(pitchr);
	PitchMatrix[0][1] = 0;
	PitchMatrix[1][1] = 1;
	PitchMatrix[2][1] = 0;
	PitchMatrix[0][2] = sin(pitchr);
	PitchMatrix[1][2] = 0;
	PitchMatrix[2][2] = cos(pitchr);

	YawMatrix[0][0] = cos(yawr);
	YawMatrix[1][0] = sin(yawr);
	YawMatrix[2][0] = 0;
	YawMatrix[0][1] = -sin(yawr);
	YawMatrix[1][1] = cos(yawr);
	YawMatrix[2][1] = 0;
	YawMatrix[0][2] = 0;
	YawMatrix[1][2] = 0;
	YawMatrix[2][2] = 1;

}

int main(void)

{

	SetUpPlatformCoordinates();

	SetupMatrixes();

	//GPIO Output setup

	CLKPR &= ~ (1 << CLKPCE);
	DDRB = (1 << PB5) | (1 << PB6) | (1 << PB7);
	DDRE = (1 << PE3) | (1 << PE4) | (1 << PE5) | (1 << PE6);
	PORTE = (1 << PE6);
	EIMSK = (1 << INT6);

	//TIMERS PWM setup

	TCCR1A |= (1 << COM1A1) | (1 << COM1B1) | (1 << COM1C1) | (1 << WGM11);
	TCCR1B |= (1 << WGM12) | (1 << WGM13) | (1 << CS11);
	TCCR3A |= (1 << COM3A1) | (1 << COM3B1) | (1 << COM3C1) | (1 << WGM31);
	TCCR3B |= (1 << WGM32) |(1 << WGM33) | (1 << CS31);
	TIMSK1 |= (1 << TOIE1);
	TIMSK3 |= (1 << TOIE3);

	ICR1 = 19992;
	ICR3 = 19992;

	//ADC Setup

	ADMUX |= (1 << REFS0);

	DIDR0 |= (1 << ADC0D) | (1 << ADC1D) | (1 << ADC2D) | (1 << ADC3D) | (1 << ADC4D) | (1 << ADC5D);

	sei();

    while (1) {

		if (cliFlag == 0)
		{
			ADCSRA |= (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1);
			ADCSRA |= (1 << ADSC);
		}

		else
		{
			ADCSRA &= ~(1<<ADEN);
			xi = 0;
			yi = 0;
			zi = 113;
			pitch = 0;
			roll = 0;
			yaw = 0;

		// Showcase program

		for (xi = 0; xi >= -20; xi -= 0.25){
			_delay_ms(5);
		}

		for (xi = -20; xi <= 20; xi += 0.25){
			_delay_ms(5);
		}

		for (xi = 20; xi >= 0; xi -= 0.25){
			_delay_ms(5);
		}

		for (yi = 0; yi >= -20; yi -= 0.25){
			_delay_ms(5);
		}

		for (yi = -20; yi <= 20; yi += 0.25){
			_delay_ms(5);
		}

		for (yi = 20; yi >= 0; yi -= 0.25){
			_delay_ms(5);
		}

		for (zi = 113; zi >= 113 - 10; zi -= 0.25){
			_delay_ms(5);
		}

		for (zi = 113 - 10; zi <= 113 + 10; zi += 0.25){
			_delay_ms(5);
		}

		for (zi = 113 + 10; zi >= 113; zi -= 0.25){
			_delay_ms(5);
		}

		for (pitch = 0; pitch >= -12; pitch -= 0.25){
			_delay_ms(5);
		}

		for (pitch = -12; pitch <= 12; pitch += 0.25){
			_delay_ms(5);
		}

		for (pitch = 12; pitch >= 0; pitch -= 0.25){
			_delay_ms(5);
		}

		for (roll = 0; roll >= -16; roll -= 0.25){
			_delay_ms(5);
		}

		for (roll = -16; roll <= 16; roll += 0.25){
			_delay_ms(5);
		}

		for (roll = 16; roll >= 0; roll -= 0.25){
			_delay_ms(5);
		}

		for (yaw = 0; yaw >= -20; yaw -= 0.25){
			_delay_ms(5);
		}

		for (yaw = -20; yaw < 20; yaw += 0.25){
			_delay_ms(5);
		}

		for (yaw = 20; yaw >= 0; yaw -= 0.25){
			_delay_ms(5);
		}

		for (xi = 0; xi >= -20; xi -= 0.25){
			_delay_ms(5);
		}

		for (xi = -20., yi = 0., roll = 0. ; xi <= 20, yi >= -20, roll <= 10 ; xi += 0.25, yi -= 0.125 ,roll += 0.0625){
			_delay_ms(5);
		}

		for (yi =-20 , roll = 10 ; yi <= 20, roll >= -10 ; yi += 0.25 , roll -= 0.125){
			_delay_ms(5);
		}

		for (yi = 20, xi = 20, roll = -10; yi >= 0, xi >= -20, roll <= 0; xi-= 0.25, yi -= 0.125, roll += 0.0625){
			_delay_ms(5);
		}

		for (xi = -20; xi <= 0; xi += 0.25){
			_delay_ms(5);
		}

		cliFlag=0;
		}
	}
}

ISR(INT6_vect){
	cliFlag = 1;
}

// serva 4,5,6  values increasing clockwise

ISR(TIMER1_OVF_vect){

	OCR1A = 77.7 + 1499.4 - 133.2 - pwm_servo4; //servo 4 + 7 degrees
	OCR1B = 55.5 + 1499.4 - 133.2 - pwm_servo5; // servo 5 + 5 degrees
	OCR1C = 111 + 1499.4 - 133.2 - pwm_servo6; // servo 6 + 11.. degrees

}

// serva 1,2,3  values increasing counter-clockwise

ISR(TIMER3_OVF_vect){

	OCR3A = 1499.4 - 22.2 + 133.2 + pwm_servo1; // servo 1 - 2 degrees
	OCR3B = 1499.4 - 33.3 + 133.2 + pwm_servo2; // servo 2 - 2 degrees
	OCR3C = 1499.4 - 188.7 + 133.2 + pwm_servo3; // servo 3 - 18 degrees
}

ISR(ADC_vect){

	switch(ADMUX){
		case 0x40:// read roll value +-10
			roll = (ADC*266.4/1023)+1366.2; //1499.4-11.1*36/2
			roll = roll-1499.4;
			roll = -roll/11.1;

			ADMUX = 0x41;
			break;

		case 0x41:// read pitch value +-10
			pitch = (ADC*199.8/1023)+1399.5;
			pitch = pitch-1499.4;
			pitch = pitch/11.1;

			ADMUX = 0x42;
			break;

		case 0x42:
			xi = (ADC*266.4/1023)+1366.2;
			xi = xi-1499.4;
			xi = xi/11.1;

			ADMUX = 0x43;
			break;

		case 0x43:
			yi = (ADC*266.4/1023)+1366.2;
			yi = yi-1499.4;
			yi = -yi/11.1;

			ADMUX = 0x44;
			break;

		case 0x44:
			zi = (ADC*199.8/1023)+1399.5;
			zi = zi-1499.4;
			zi = 113+zi/11.1;

			ADMUX = 0x45;
			break;

		case 0x45:
			yaw = (ADC*266.4/1023)+1366.2;
			yaw = yaw-1499.4;
			yaw = yaw/11.1;

			ADMUX = 0x40;
			break;
	}
	if ((pitch >= 8.9) && (xi < 0)){
		pitch = 9;
		xi= 0;
	}

	rollr= roll * (pi/180);
	pitchr = pitch * (pi/180);
	yawr = yaw * (pi/180);

	// Rotation Matrix Calculation

	RollPitchMatrix[0][0] = RollMatrix[0][0] * PitchMatrix[0][0] + RollMatrix[0][1] * PitchMatrix[1][0] + RollMatrix[0][2] * PitchMatrix[2][0];
	RollPitchMatrix[0][1] = RollMatrix[0][0] * PitchMatrix[0][1] + RollMatrix[0][1] * PitchMatrix[1][1] + RollMatrix[0][2] * PitchMatrix[2][1];
	RollPitchMatrix[0][2] = RollMatrix[0][0] * PitchMatrix[0][2] + RollMatrix[0][1] * PitchMatrix[1][2] + RollMatrix[0][2] * PitchMatrix[2][2];
	RollPitchMatrix[1][0] = RollMatrix[1][0] * PitchMatrix[0][0] + RollMatrix[1][1] * PitchMatrix[1][0] + RollMatrix[1][2] * PitchMatrix[2][0];
	RollPitchMatrix[1][1] = RollMatrix[1][0] * PitchMatrix[0][1] + RollMatrix[1][1] * PitchMatrix[1][1] + RollMatrix[1][2] * PitchMatrix[2][1];
	RollPitchMatrix[1][2] = RollMatrix[1][0] * PitchMatrix[0][2] + RollMatrix[1][1] * PitchMatrix[1][2] + RollMatrix[1][2] * PitchMatrix[2][2];
	RollPitchMatrix[2][0] = RollMatrix[2][0] * PitchMatrix[0][0] + RollMatrix[2][1] * PitchMatrix[1][0] + RollMatrix[2][2] * PitchMatrix[2][0];
	RollPitchMatrix[2][1] = RollMatrix[2][0] * PitchMatrix[0][1] + RollMatrix[2][1] * PitchMatrix[1][1] + RollMatrix[2][2] * PitchMatrix[2][1];
	RollPitchMatrix[2][2] = RollMatrix[2][0] * PitchMatrix[0][2] + RollMatrix[2][1] * PitchMatrix[1][2] + RollMatrix[2][2] * PitchMatrix[2][2];

	RollPitchYawMatrix[0][0] = RollPitchMatrix[0][0] * YawMatrix[0][0] + RollPitchMatrix[0][1] * YawMatrix[1][0] + RollPitchMatrix[0][2] * YawMatrix[2][0];
	RollPitchYawMatrix[0][1] = RollPitchMatrix[0][0] * YawMatrix[0][1] + RollPitchMatrix[0][1] * YawMatrix[1][1] + RollPitchMatrix[0][2] * YawMatrix[2][1];
	RollPitchYawMatrix[0][2] = RollPitchMatrix[0][0] * YawMatrix[0][2] + RollPitchMatrix[0][1] * YawMatrix[1][2] + RollPitchMatrix[0][2] * YawMatrix[2][2];
	RollPitchYawMatrix[1][0] = RollPitchMatrix[1][0] * YawMatrix[0][0] + RollPitchMatrix[1][1] * YawMatrix[1][0] + RollPitchMatrix[1][2] * YawMatrix[2][0];
	RollPitchYawMatrix[1][1] = RollPitchMatrix[1][0] * YawMatrix[0][1] + RollPitchMatrix[1][1] * YawMatrix[1][1] + RollPitchMatrix[1][2] * YawMatrix[2][1];
	RollPitchYawMatrix[1][2] = RollPitchMatrix[1][0] * YawMatrix[0][2] + RollPitchMatrix[1][1] * YawMatrix[1][2] + RollPitchMatrix[1][2] * YawMatrix[2][2];
	RollPitchYawMatrix[2][0] = RollPitchMatrix[2][0] * YawMatrix[0][0] + RollPitchMatrix[2][1] * YawMatrix[1][0] + RollPitchMatrix[2][2] * YawMatrix[2][0];
	RollPitchYawMatrix[2][1] = RollPitchMatrix[2][0] * YawMatrix[0][1] + RollPitchMatrix[2][1] * YawMatrix[1][1] + RollPitchMatrix[2][2] * YawMatrix[2][1];
	RollPitchYawMatrix[2][2] = RollPitchMatrix[2][0] * YawMatrix[0][2] + RollPitchMatrix[2][1] * YawMatrix[1][2] + RollPitchMatrix[2][2] * YawMatrix[2][2];

	// Vectors of point Px in relation with the base

	rq1[0] = RollPitchYawMatrix[0][0] * p_P1->x + RollPitchYawMatrix[0][1] * p_P1->y + RollPitchYawMatrix[0][2] * p_P1->z;
	rq1[1] = RollPitchYawMatrix[1][0] * p_P1->x + RollPitchYawMatrix[1][1] * p_P1->y + RollPitchYawMatrix[1][2] * p_P1->z;
	rq1[2] = RollPitchYawMatrix[2][0] * p_P1->x + RollPitchYawMatrix[2][1] * p_P1->y + RollPitchYawMatrix[2][2] * p_P1->z;
	p_q1->x = rq1[0] + xi;
	p_q1->y = rq1[1] + yi;
	p_q1->z = rq1[2] + zi;

	rq2[0] = RollPitchYawMatrix[0][0] * p_P2->x + RollPitchYawMatrix[0][1] * p_P2->y + RollPitchYawMatrix[0][2] * p_P2->z;
	rq2[1] = RollPitchYawMatrix[1][0] * p_P2->x + RollPitchYawMatrix[1][1] * p_P2->y + RollPitchYawMatrix[1][2] * p_P2->z;
	rq2[2] = RollPitchYawMatrix[2][0] * p_P2->x + RollPitchYawMatrix[2][1] * p_P2->y + RollPitchYawMatrix[2][2] * p_P2->z;
	p_q2->x = rq2[0] + xi;
	p_q2->y = rq2[1] + yi;
	p_q2->z = rq2[2] + zi;

	rq3[0] = RollPitchYawMatrix[0][0] * p_P3->x + RollPitchYawMatrix[0][1] * p_P3->y + RollPitchYawMatrix[0][2] * p_P3->z;
	rq3[1] = RollPitchYawMatrix[1][0] * p_P3->x + RollPitchYawMatrix[1][1] * p_P3->y + RollPitchYawMatrix[1][2] * p_P3->z;
	rq3[2] = RollPitchYawMatrix[2][0] * p_P3->x + RollPitchYawMatrix[2][1] * p_P3->y + RollPitchYawMatrix[2][2] * p_P3->z;
	p_q3->x = rq3[0] + xi;
	p_q3->y = rq3[1] + yi;
	p_q3->z = rq3[2] + zi;

	rq4[0] = RollPitchYawMatrix[0][0] * p_P4->x + RollPitchYawMatrix[0][1] * p_P4->y + RollPitchYawMatrix[0][2] * p_P4->z;
	rq4[1] = RollPitchYawMatrix[1][0] * p_P4->x + RollPitchYawMatrix[1][1] * p_P4->y + RollPitchYawMatrix[1][2] * p_P4->z;
	rq4[2] = RollPitchYawMatrix[2][0] * p_P4->x + RollPitchYawMatrix[2][1] * p_P4->y + RollPitchYawMatrix[2][2] * p_P4->z;
	p_q4->x = rq4[0] + xi;
	p_q4->y = rq4[1] + yi;
	p_q4->z = rq4[2] + zi;

	rq5[0] = RollPitchYawMatrix[0][0] * p_P5->x + RollPitchYawMatrix[0][1] * p_P5->y + RollPitchYawMatrix[0][2] * p_P5->z;
	rq5[1] = RollPitchYawMatrix[1][0] * p_P5->x + RollPitchYawMatrix[1][1] * p_P5->y + RollPitchYawMatrix[1][2] * p_P5->z;
	rq5[2] = RollPitchYawMatrix[2][0] * p_P5->x + RollPitchYawMatrix[2][1] * p_P5->y + RollPitchYawMatrix[2][2] * p_P5->z;
	p_q5->x = rq5[0] + xi;
	p_q5->y = rq5[1] + yi;
	p_q5->z = rq5[2] + zi;

	rq6[0] = RollPitchYawMatrix[0][0] * p_P6->x + RollPitchYawMatrix[0][1] * p_P6->y + RollPitchYawMatrix[0][2] * p_P6->z;
	rq6[1] = RollPitchYawMatrix[1][0] * p_P6->x + RollPitchYawMatrix[1][1] * p_P6->y + RollPitchYawMatrix[1][2] * p_P6->z;
	rq6[2] = RollPitchYawMatrix[2][0] * p_P6->x + RollPitchYawMatrix[2][1] * p_P6->y + RollPitchYawMatrix[2][2] * p_P6->z;
	p_q6->x = rq6[0] + xi;
	p_q6->y = rq6[1] + yi;
	p_q6->z = rq6[2] + zi;

	// Required leg length

	p_L1->x = p_q1->x - p_B1->x;
	p_L1->y = p_q1->y - p_B1->y;
	p_L1->z = p_q1->z - p_B1->z;
	l1 = sqrt(p_L1->x * p_L1->x + p_L1->y * p_L1->y + p_L1->z * p_L1->z);

	p_L2->x = p_q2->x - p_B2->x;
	p_L2->y = p_q2->y - p_B2->y;
	p_L2->z = p_q2->z - p_B2->z;
	l2 = sqrt(p_L2->x * p_L2->x + p_L2->y * p_L2->y + p_L2->z * p_L2->z);

	p_L3->x = p_q3->x - p_B3->x;
	p_L3->y = p_q3->y - p_B3->y;
	p_L3->z = p_q3->z - p_B3->z;
	l3 = sqrt(p_L3->x * p_L3->x + p_L3->y * p_L3->y + p_L3->z * p_L3->z);

	p_L4->x = p_q4->x - p_B4->x;
	p_L4->y = p_q4->y - p_B4->y;
	p_L4->z = p_q4->z - p_B4->z;
	l4 = sqrt(p_L4->x * p_L4->x + p_L4->y * p_L4->y + p_L4->z * p_L4->z);

	p_L5->x = p_q5->x - p_B5->x;
	p_L5->y = p_q5->y - p_B5->y;
	p_L5->z = p_q5->z - p_B5->z;
	l5 = sqrt(p_L5->x * p_L5->x + p_L5->y * p_L5->y + p_L5->z * p_L5->z);

	p_L6->x = p_q6->x - p_B6->x;
	p_L6->y = p_q6->y - p_B6->y;
	p_L6->z = p_q6->z - p_B6->z;
	l6 = sqrt(p_L6->x * p_L6->x + p_L6->y * p_L6->y + p_L6->z * p_L6->z);

	// Required servo arm angle

	// SERVO 4(1)

	M[0] = 2 * a * (p_L1->z);
	N[0] = 2 * a * (cos(betar[0]) * p_L1->x + sin(betar[0]) * p_L1->y);
	S[0] = l1 * l1 - (s * s - a * a);
	alfa[3] = asin(S[0] / (sqrt(M[0] * M[0] + N[0] * N[0]))) - atan(N[0] / M[0]);
	alfad[3] = alfa[3] * (180 / pi);

	//SERVO 1(2)

	M[1] = 2 * a * (p_L2->z);
	N[1] = 2 * a * (cos(betar[1]) * p_L2->x + sin(betar[1]) * p_L2->y);
	S[1] = l2 * l2 - (s * s - a * a);
	alfa[0] = asin(S[1] / (sqrt(M[1] * M[1] + N[1] * N[1]))) - atan(N[1] / M[1]);
	alfad[0] = alfa[0] * (180 / pi);

	// SERVO 6(3)

	M[2] = 2 * a * (p_L3->z);
	N[2] = 2 * a * (cos(betar[2]) * p_L3->x + sin(betar[2]) * p_L3->y);
	S[2] = l3 * l3 - (s * s - a * a);
	alfa[5] = asin(S[2] / (sqrt(M[2] * M[2] + N[2] * N[2]))) - atan(N[2] / M[2]);
	alfad[5] = alfa[5] * (180 / pi);

	//SERVO 3(4)

	M[3] = 2 * a * (p_L4->z);
	N[3] = 2 * a * (cos(betar[3]) * p_L4->x + sin(betar[3]) * p_L4->y);
	S[3] = l4 * l4 - (s * s - a * a);
	alfa[2] = asin(S[3] / (sqrt(M[3] * M[3] + N[3] * N[3]))) - atan(N[3] / M[3]);
	alfad[2] = alfa[2] * (180 / pi);

	// SERVO 5(5)

	M[4] = 2 * a * (p_L5->z);
	N[4] = 2 * a * (cos(betar[4]) * p_L5->x + sin(betar[4]) * p_L5->y);
	S[4] = l5 * l5 - (s * s - a * a);
	alfa[4] = asin(S[4] / (sqrt(M[4] * M[4] + N[4] * N[4]))) - atan(N[4] / M[4]);
	alfad[4] = alfa[4] * (180 / pi);

	//SERVO 2(6)

	M[5] = 2 * a * (p_L6->z);
	N[5] = 2 * a * (cos(betar[5]) * p_L6->x + sin(betar[5]) * p_L6->y);
	S[5] = l6 * l6 - (s * s - a * a);
	alfa[1] = asin(S[5] / (sqrt(M[5] * M[5] + N[5] * N[5]))) - atan(N[5] / M[5]);
	alfad[1] = alfa[1] * (180 / pi);

	pwm_servo1 = alfad[0] * 11.1;
	pwm_servo2 = alfad[1] * 11.1;
	pwm_servo3 = alfad[2] * 11.1;
	pwm_servo4 = alfad[3] * 11.1;
	pwm_servo5 = alfad[4] * 11.1;
	pwm_servo6 = alfad[5] * 11.1;

	ADCSRA |= 1<<ADSC;
}
