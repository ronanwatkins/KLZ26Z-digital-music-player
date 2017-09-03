// Standard C Included Files
#include <stdio.h>
// SDK Included Files
#include "board.h"
#include "fsl_os_abstraction.h"
#include "fsl_tsi_driver.h"
#include "fsl_debug_console.h"
#include "flash.h"
#include "fsl_pit_driver.h"
#include "fsl_i2c_master_driver.h"
#include "fsl_i2c_shared_function.h"
#include "fsl_clock_manager.h"
#include "melodys.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////
#define TSI_THRESHOLD_SAMPLING      (100u)
#define TSI_INSTANCE                0
#define ESC 0x1B
#define clearLine() PRINTF("%c[2K", ESC) //clears line
#define clearScreen() PRINTF("%c[2J %c[H", ESC, ESC) //clears screen and resets terminal to start
#define resetTerminal() PRINTF("%cc", ESC)
#define highlight() PRINTF("%c[4;31;43m", ESC)
#define redBackground() PRINTF("%c[41m%c[30m", ESC, ESC)
#define yellowBackground() PRINTF("%c[43m%c[30m", ESC, ESC)
#define blackBackground() PRINTF("%c[40m%c[37m", ESC, ESC)
#define whiteBackground() PRINTF("%c[47m%c[30m", ESC, ESC)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
extern void TSI_DRV_IRQHandler(uint32_t instance);	//TSI interrupt handler in SDK
int tsi_config(uint32_t * avg_TSI_untouch);			//Configures TSI module
int tsi_measure(uint32_t * left, uint32_t * right);	//Takes a measurement from the TSI module
void program_all();
void read_from_flash();
void read_value(int length, int ADDRESS, int * array);
void sing(int song);
void buzz(uint32_t targetPin, long frequency, long length);
void config_i2c_pins(void);
int get_delay_value();

//TSI interrupt handler
void TSI0_IRQHandler(void)
{
    TSI_DRV_IRQHandler(0);
}

void I2C0_IRQHandler(void)
{
    I2C_DRV_IRQHandler(I2C0_IDX);
}

int sliderLevel = 1;
volatile unsigned int songNum = 0;

bool playing = false;
void PORTC_PORTD_IRQHandler()
{
	GPIO_DRV_ClearPinIntFlag(SW1);
	songNum = sliderLevel;

	if(playing){ //If switch is pressed while a song is playing stop the song
		playing = false;
		songNum = 0;
	}
}

//Global Variables
uint8_t tsiChn[BOARD_TSI_ELECTRODE_CNT];	//Array of TSI channels
tsi_state_t myTsiState;						//Driver data storage
uint32_t melodyPin = D3;

int melody_result[78];
int tempo_result[78];
int underworld_melody_result[56];
int underworld_tempo_result[56];
int axel_melody_result[23];
int axel_tempo_result[23];
int sailormoon_melody_result[38];
int sailormoon_tempo_result[38];
int steveuniverse_melody_result[45];
int steveuniverse_tempo_result[45];

int main(void)
{
    uint32_t avgUntouch = 0; 	//Average TSI value of untouched state
    uint32_t avgMeasure, i;
    tsi_status_t result;
    int32_t left_electrode, right_electrode;
    int MEMORY_ADDRESS = 0x1FC00;
    int diff = 0, lastLevel = 1;

    // Initialize hardware
    hardware_init();

    //Initialise the GPIO driver
    GPIO_DRV_Init(switchPins, ledPins);

    // Initialize the OS abstraction layer
    OSA_Init();

    //Configure TSI and measure when untouched
    tsi_config(&avgUntouch);

    pit_user_config_t pitChannel0 = {
		.isInterruptEnabled = false,// Disable timer interrupt.
		.periodUs = 500000U // Set timer period to 0.5 sec.
	};
	// Initialize PIT instance 0. Timers will stop running in debug mode.
	PIT_DRV_Init(0, 0);
	PIT_DRV_InitChannel(0, 0, &pitChannel0);

	//Initialise the FRDM-KL26Z Board
	config_i2c_pins();

	// Init I2C module
	i2c_master_state_t master;
	I2C_DRV_MasterInit(0, &master);

	DISABLE_FLASH_DATA_CACHE;
	ENABLE_FLASH_STALL;
	__disable_irq();

	flash_erase_sector(MEMORY_ADDRESS);

	//Program tunes and tempos to flash memory
	program_mario_melody();
	program_mario_tempo();

	program_underworld_melody();
	program_underworld_tempo();

	program_axel_melody();
	program_axel_tempo();

	program_sailormoon_melody();
	program_sailormoon_tempo();

	program_steveuniverse_melody();
	program_steveuniverse_tempo();

	//read tunes and tempos from flash
	read_from_flash();

	DISABLE_FLASH_STALL;
	ENABLE_FLASH_DATA_CACHE;
	__enable_irq();

    while(1)
    {
    	tsi_measure(&left_electrode, &right_electrode);
    	avgMeasure = (left_electrode + right_electrode)/2;
    	diff = left_electrode - right_electrode; //Get difference between left and right electrode

    	if(avgMeasure > (avgUntouch+20)){ //If the touch sensor is pressed
    		if(diff > 80){ //Left side touched
				sliderLevel = 5;
			}
			else if(diff > 45){
				sliderLevel = 4;
			}
			else if(diff < 45 && diff > -45){ //Middle touched
				sliderLevel = 3;
			}
			else if(diff < -45 && diff > -90){
				sliderLevel = 2;
			}
			else if(diff < -90){ //Right side touched
				sliderLevel = 1;
			}
    	}

        // Measures each 100ms.
        OSA_TimeDelay(100u);

        clearScreen();

        //Display song list
        if(sliderLevel == 1) //Highlight songs in list when the slider is in the correct position
        	whiteBackground();
        else
        	blackBackground();
		PRINTF("1. Super Mario Theme\n\r");
		if(sliderLevel == 2)
			whiteBackground();
		else
			blackBackground();
		PRINTF("2. Underworld Theme\n\r");
		if(sliderLevel == 3)
			whiteBackground();
		else
			blackBackground();
		PRINTF("3. Axel F\n\r");
		if(sliderLevel == 4)
			whiteBackground();
		else
			blackBackground();
		PRINTF("4. Sailor Moon\n\r");
		if(sliderLevel == 5)
			whiteBackground();
		else
			blackBackground();
		PRINTF("5. Steven Universe\n\r");
		blackBackground();

        if(songNum != 0){
        	playing = true;
        	sing(songNum);
        }
    }
}

void read_from_flash() {
	//Addresses of tunes and tempos in flash
	int MARIO_MELODY_ADDRESS = 0x1FC00;
	int MARIO_TEMPO_ADDRESS = 0x1FCC0;
	int UNDERWORLD_MELODY_ADDRESS = 0x1FD80;
	int UNDERWORLD_TEMPO_ADDRESS = 0x1FE10;
	int AXEL_MELODY_ADDRESS = 0x1FE80;
	int AXEL_TEMPO_ADDRESS = 0x1FF20;
	int SAILORMOON_MELODY_ADDRESS = 0x1F44C;
	int SAILORMOON_TEMPO_ADDRESS = 0x1F4D0;
	int STEVEUNIVERSE_MELODY_ADDRESS = 0x1F44C;
	int STEVEUNIVERSE_TEMPO_ADDRESS = 0x1F570;

	//Read tunes and tempos from flash
	read_value(78, MARIO_MELODY_ADDRESS, melody_result);
	read_value(78, MARIO_TEMPO_ADDRESS, tempo_result);
	read_value(56, UNDERWORLD_MELODY_ADDRESS, underworld_melody_result);
	read_value(56, UNDERWORLD_TEMPO_ADDRESS, underworld_tempo_result);
	read_value(23, AXEL_MELODY_ADDRESS, axel_melody_result);
	read_value(23, AXEL_TEMPO_ADDRESS, axel_tempo_result);
	read_value(38, SAILORMOON_MELODY_ADDRESS, sailormoon_melody_result);
	read_value(38, SAILORMOON_TEMPO_ADDRESS, sailormoon_tempo_result);
	read_value(45, STEVEUNIVERSE_MELODY_ADDRESS, steveuniverse_melody_result);
	read_value(45, STEVEUNIVERSE_TEMPO_ADDRESS, steveuniverse_tempo_result);
}

void read_value(int length, int ADDRESS, int * array) {
	int digit[2];
	int i=0, j=0;

	//This loop retrieves 2 integers from memory in every increment
	for(i=0, j=0; i<length; i+=2, j+=4) {
		digit[0] = *(unsigned char *)(ADDRESS+j+2); //Get first half of integer
		digit[1] = *(unsigned char *)(ADDRESS+j+3); //Get second half of integer

		//Integers larger than 2 digits are split when placed in flash
		//This code sticks the integer together and places it in array
		array[i] = (digit[0]<<8)|(digit[1]);

		//Same as first half of function but for second integer
		digit[0] = *(unsigned char *)(ADDRESS+j);
		digit[1] = *(unsigned char *)(ADDRESS+j+1);

		array[i+1] = (digit[0]<<8)|(digit[1]);
	}
}

void PIT_delay_usec(unsigned long usec){
	PIT_DRV_SetTimerPeriodByUs(0,0,usec);
	PIT_DRV_StartTimer(0,0);
	while(!PIT_DRV_IsIntPending(0,0))	//Wait while PIT flag is low
	{}
	PIT_DRV_ClearIntFlag(0,0);	//Clear flag
	PIT_DRV_StopTimer(0,0);	//Stop timer
}

void sing(int song) {
  // Iterate over the notes of the melody:
  int i=0, j=0;

  int arraySize;

  //Display current song and define the size of its array
  switch(song) {
  	  case 1:
  		PRINTF("\nPlaying: 'Super Mario Theme'\n\r");
  		arraySize = 78;
  		break;
  	  case 2:
  		PRINTF("\nPlaying: 'Underworld Theme'\n\r");
  		arraySize = 56;
  		break;
  	  case 3:
  		PRINTF("\nPlaying: 'Axel F'\n\r");
  		arraySize = 23;
  		break;
  	  case 4:
  		PRINTF("\nPlaying: 'Sailor Moon'\n\r");
  		arraySize = 38;
  		break;
  	  case 5:
  		PRINTF("\nPlaying: 'Steven Universe'\n\r");
  		arraySize = 45;
  		break;
  	  default:
  		  break;
  }

  int melody[arraySize];
  int tempo[arraySize];

  //Copying the integers from the tune and tempo into arrays
  for(i=0; i<arraySize; i++){
	  switch(song) {
		  case 1:
			  melody[i] = melody_result[i];
			  tempo[i] = tempo_result[i];
			  break;
		  case 2:
			  melody[i] = underworld_melody_result[i];
			  tempo[i] = underworld_tempo_result[i];
			  break;
		  case 3:
			  melody[i] = axel_melody_result[i];
			  tempo[i] = axel_tempo_result[i];
			  break;
		  case 4:
			  melody[i] = sailormoon_melody_result[i];
			  tempo[i] = sailormoon_tempo_result[i];
			  break;
		  case 5:
			  melody[i] = steveuniverse_melody_result[i];
			  tempo[i] = steveuniverse_tempo_result[i];
			  break;
		  default:
		  	  break;
	  }
  }

  int size = sizeof(melody) / sizeof(int);
  for (int thisNote = 0; thisNote < size; thisNote++) {
	  if(!playing) //If switch is pressed while song is playing
		  return;
	// Calculate the note duration
	int noteDuration = 1000 / tempo[thisNote];

	buzz(melodyPin, melody[thisNote], noteDuration);

	// Minumum time between notes
	int pauseBetweenNotes = noteDuration * 1.30;
	// Add value from accelerometer to delayValue to adjust
	// The time between notes and thus the speed of the song
	int delayValue = get_delay_value();
    pauseBetweenNotes += delayValue;

    PIT_delay_usec(pauseBetweenNotes*1000);

	// Stop the tone playing
	buzz(melodyPin, 0, noteDuration);
  }
}

void buzz(uint32_t targetPin, long frequency, long length) {
  GPIO_DRV_ClearPinOutput(RedLED); //Flash the Red LED every time a notes is played
  long delayValue = 1000000 / frequency / 2; // Calculate the delay value between transitions

  long numCycles = frequency * length / 1000; // Calculate the number of cycles for proper timing

  for (long i = 0; i < numCycles; i++) {
	GPIO_DRV_ClearPinOutput(targetPin); // Turn buzzer on
	PIT_delay_usec(delayValue); // Wait
    GPIO_DRV_SetPinOutput(targetPin); // Turn buzzer off
    PIT_delay_usec(delayValue); // Wait
  }
  GPIO_DRV_SetPinOutput(RedLED);

}

/*********************************************************************************
 * Return readings for the left and right electrode
 */
int tsi_measure(uint32_t * left, uint32_t * right)
{
	tsi_status_t result;
	uint16_t measureResult[BOARD_TSI_ELECTRODE_CNT];
	uint32_t i, avgMeasure = 0;

	result = TSI_DRV_MeasureBlocking(TSI_INSTANCE);
	if(result != kStatus_TSI_Success)
	{
		PRINTF("\r\nThe measure of TSI failed.");
		return -1;
	}

	// Init average measurement.
	avgMeasure = 0;
	for(i = 0; i < BOARD_TSI_ELECTRODE_CNT; i++)
	{
		result = TSI_DRV_GetCounter(TSI_INSTANCE, tsiChn[i], &measureResult[i]);
		if(result != kStatus_TSI_Success)
		{
			PRINTF("\r\nThe read of TSI channel %d failed.", tsiChn[i]);
			return -1;
		}
		avgMeasure += measureResult[i];
	}
	// Calculates average measurement.
	avgMeasure /=BOARD_TSI_ELECTRODE_CNT;
	*left = measureResult[0];
	*right = measureResult[1];

	return(kStatus_TSI_Success);
}

/*************************************************************************************
 * Configure TSI module and return untouched reading
 */
int tsi_config(uint32_t * avg_TSI_untouch)
{
    uint16_t measureResult[BOARD_TSI_ELECTRODE_CNT];
    tsi_status_t result;
    uint32_t i, j;
    uint32_t sumUntouch=0;
    uint32_t avgMeasure;

#if (FSL_FEATURE_TSI_VERSION == 2)
	// Set up the HW configuration for normal mode of TSI
	static const tsi_config_t tsiHwConfig =
	{
		.ps       = kTsiElecOscPrescaler_2div,       /*! Prescaler */
		.extchrg  = kTsiExtOscChargeCurrent_10uA,    /*! Electrode charge current */
		.refchrg  = kTsiRefOscChargeCurrent_10uA,    /*! Reference charge current */
		.nscn     = kTsiConsecutiveScansNumber_8time,/*! Number of scans. */
		.lpclks   = kTsiLowPowerInterval_100ms,      /*! Low power clock. */
		.amclks   = kTsiActiveClkSource_BusClock,    /*! Active mode clock source. */
		.ampsc    = kTsiActiveModePrescaler_8div,    /*! Active mode prescaler. */
		.lpscnitv = kTsiLowPowerInterval_100ms,      /*! Low power scan interval. */
		.thresh   = 100u,                            /*! High byte of threshold. */
		.thresl   = 200u,                            /*! Low byte of threshold. */
	};
#elif (FSL_FEATURE_TSI_VERSION == 4)
	// Set up the HW configuration for normal mode of TSI
	static const tsi_config_t tsiHwConfig =
	{
		.ps      = kTsiElecOscPrescaler_2div,        /*! Prescaler */
		.extchrg = kTsiExtOscChargeCurrent_8uA,      /*! Electrode charge current */
		.refchrg = kTsiRefOscChargeCurrent_8uA,      /*! Reference charge current */
		.nscn    = kTsiConsecutiveScansNumber_8time, /*! Number of scans. */
		.mode    = kTsiAnalogModeSel_Capacitive,     /*! TSI analog modes in a TSI instance */
		.dvolt   = kTsiOscVolRails_Dv_103,
		.thresh   = 100,                             /*! High byte of threshold. */
		.thresl   = 200,                             /*! Low byte of threshold. */
	};
#endif


// Set up the configuration of initialization structure
const tsi_user_config_t tsiConfig =
{
    .config        = (tsi_config_t*)&tsiHwConfig,
    .pCallBackFunc = NULL,
    .usrData       = 0,
};



    // Get TSI channel.
    tsiChn[0] = BOARD_TSI_ELECTRODE_1;
	#if (BOARD_TSI_ELECTRODE_CNT > 1)
		tsiChn[1] = BOARD_TSI_ELECTRODE_2;
	#endif
	#if (BOARD_TSI_ELECTRODE_CNT > 2)
		tsiChn[2] = BOARD_TSI_ELECTRODE_3;
	#endif
	#if (BOARD_TSI_ELECTRODE_CNT > 3)
		tsiChn[3] = BOARD_TSI_ELECTRODE_4;
	#endif

	// Driver initialization
	result = TSI_DRV_Init(TSI_INSTANCE, &myTsiState, &tsiConfig);
	if(result != kStatus_TSI_Success)
	{
		PRINTF("\r\nThe initialization of TSI driver failed ");
		return -1;
	}

	// Enable electrodes for normal mode
	for(i = 0; i < BOARD_TSI_ELECTRODE_CNT; i++)
	{
		result = TSI_DRV_EnableElectrode(TSI_INSTANCE, tsiChn[i], true);
		if(result != kStatus_TSI_Success)
		{
			PRINTF("\r\nThe initialization of TSI channel %d failed \r\n", tsiChn[i]);
			return -1;
		}
	}
	// Measures average value of untouched state.
	result = TSI_DRV_MeasureBlocking(TSI_INSTANCE);
	if(result != kStatus_TSI_Success)
	{
		PRINTF("\r\nThe measure of TSI failed. ");
		return -1;
	}

	// Measures average value in untouched mode.
	for(i = 0; i<TSI_THRESHOLD_SAMPLING; i++)
	{
		for(j = 0; j < BOARD_TSI_ELECTRODE_CNT; j++)
		{
			result = TSI_DRV_GetCounter(TSI_INSTANCE, tsiChn[j], &measureResult[j]);
			if(result != kStatus_TSI_Success)
			{
				PRINTF("\r\nThe read of TSI channel %d failed.", tsiChn[j]);
				return -1;
			}
			// Calculates sum of average values.
			sumUntouch += measureResult[j];
		}
	}
	// Calculates average value afer 100 times measurement.
	*avg_TSI_untouch = sumUntouch/(TSI_THRESHOLD_SAMPLING * BOARD_TSI_ELECTRODE_CNT);

	return(kStatus_TSI_Success);
}

/*****************************************************************************
 * Configure PTE24 and PTE25 as I2C clock and data pins with pullup resistors
 */
void config_i2c_pins(void)
{
	PORTE_PCR24 |= (0x5<<8) | 0x3;	//SCL Alt5 + pullup enabled
	PORTE_PCR25 |= (0x5<<8) | 0x3;	//SDA Alt5 + pullup enabled
}

int get_delay_value() {
	int16_t accelData;

	i2c_status_t returnValue;

	//Configuration data for the accelerometer
	i2c_device_t slave =
	{
		.address = 0x1D,
		.baudRate_kbps = 100
	};
	int delayValue = 0;
	char rx_buffer[6];
	unsigned char OUT_X_MSB_reg[1] = {0x01};
	//Read Accelerometer values
	returnValue = I2C_DRV_MasterReceiveDataBlocking(0, &slave, OUT_X_MSB_reg, 1, rx_buffer, sizeof(rx_buffer), 500);
	if (returnValue == kStatus_I2C_Success)
	{
		//Get data from the y of the accelerometer
		//I added 150 onto this data because on a flat surface it was off by -150
		accelData = (((uint16_t)rx_buffer[2]<<6)+(rx_buffer[3]>>2) + 150);
		//The accelerometer goes between ~+2000 / ~-2000
		//I need a value of no more than +/-60 so I multiply by 0.03 to get this
		delayValue = accelData * 0.03;
	}
	else
		PRINTF("I2C Error\n\r");

	return delayValue;
}
