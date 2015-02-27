
// Free-RTOS includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "portmacro.h"
#include "osi.h"


// Driverlib includes
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "hw_types.h"
#include "hw_ints.h"
#include "interrupt.h"
#include "rom.h"
#include "rom_map.h"
#include "uart.h"
#include "prcm.h"
#include "utils.h"
#include "hw_uart.h"

// Common interface includes
#include "uart_if.h"
#include "udma_if.h"

#include "pin_mux.h"

#include "HC-05driver.h"
#include "pin.h"
#include "ButtonDriver.h"
#include "led.h"

// LED Driver includes
#include "LEDBoardGPIO.h"
#include "DisplayDriver.h"
#include "GenerateImage.h"

//*****************************************************************************
//                          MACROS
//*****************************************************************************
#define APPLICATION_VERSION     "0.2.0"
#define APP_NAME                "Foton_FW"
#define SYSTICKS_PER_SECOND     100
#define BUFF_SIZE               20
#define SYSTICK_RELOAD_VALUE    0x0000C3500
#define UART_PRINT              Report
//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************
volatile int g_iCounter = 0;

extern void (* const g_pfnVectors[])(void);

//*****************************************************************************
//                 GLOBAL VARIABLES -- End
//*****************************************************************************


//*****************************************************************************
//                      LOCAL DEFINITION
//*****************************************************************************


//*****************************************************************************
//
//! Board Initialization & Configuration
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
static void
BoardInit(void)
{
/* In case of TI-RTOS vector table is initialize by OS itself */

  MAP_IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);

  //
  // Enable Processor
  //
  MAP_IntMasterEnable();
  MAP_IntEnable(FAULT_SYSTICK);

  PRCMCC3200MCUInit();
}


TaskHandle_t       DEBOUNCE_TSK_HNDLE;
TaskHandle_t       BLUETOOTH_READ_HNDLE;
TaskHandle_t       BLUETOOTH_CMD_READ_HNDLE;
TaskHandle_t       DISP_IMG_HNDLE;
ButtonDriver * Button1_PTR;
ButtonDriver * Button2_PTR;

static ButtonDriver button1(13);
static ButtonDriver button2(22);
/*
 * Bluetooth (HC-05)
 * Board Pin	Function	GPIO Pin Alias	Pin Mode Config	Peripheral Pin	Foton Alias
 * PIN_18	 When pulled high, powers bluetooth module		    GPIO_28	0	LED	BLUETOOTH_READ_ENABLE
 * PIN_05	 Changes HC-05 between configure and transmit mode.	GPIO_03	0	KEY	BLUETOOTH_STATE_ENABLE
 * PIN_55	 Data line for Bluetooth Tx							GPIO_01	5	RXD	BLUETOOTH_TX
 * PIN_57	 Data line for Bluetooth Rx							GPIO_02	5	TXD	BLUETOOTH_RX
 *
 */
#ifdef USING_SERIAL_FOR_BLUETOOTH
static HC_05Bluetooth  bluetooth(PIN_57,PIN_MODE_3,PIN_55,PIN_MODE_3,28,PIN_MODE_0,14,PIN_MODE_0);
#else
static HC_05Bluetooth  bluetooth(PIN_02,PIN_MODE_7,PIN_01,PIN_MODE_7,28,PIN_MODE_0,14,PIN_MODE_0);
#endif
char RXDATABUFF[MAX_COMMAND_INDEX];
HC_05Bluetooth * FOTON_BLUETOOTH;
static FOTON_LIVE_MESSAGE local_current_message;
FOTON_LIVE_MESSAGE * CURRENT_MESSAGE;
BUTTON_DEBOUNCE_CTRL Button1_Debounce;
BUTTON_DEBOUNCE_CTRL Button2_Debounce;


// LED Board Display Stuff
static DisplayDriver leddisplay;
DisplayDriver * FOTON_LED_BOARD;



//*****************************************************************************
// FreeRTOS User Hook Functions enabled in FreeRTOSConfig.h
//*****************************************************************************

//*****************************************************************************
//
//! \brief Application defined hook (or callback) function - assert
//!
//! \param[in]  pcFile - Pointer to the File Name
//! \param[in]  ulLine - Line Number
//!
//! \return none
//!
//*****************************************************************************
extern "C" void vAssertCalled( const char *pcFile, unsigned long ulLine )
{
    //Handle Assert here
    while(1)
    {
    }
}

//*****************************************************************************
//
//! \brief Application defined idle task hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
extern "C" void vApplicationIdleHook( void)
{
    //Handle Idle Hook for Profiling, Power Management etc
}

//*****************************************************************************
//
//! \brief Application defined malloc failed hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
extern "C" void vApplicationMallocFailedHook()
{
    //Handle Memory Allocation Errors
    while(1)
    {
    }
}

//*****************************************************************************
//
//! \brief Application defined stack overflow hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
extern "C" void vApplicationStackOverflowHook( OsiTaskHandle *pxTask,
                                   signed char *pcTaskName)
{
    //Handle FreeRTOS Stack Overflow
    while(1)
    {
    }
}



// TOGGLES Bluetooth state from transfer to configure
static void button_func(const ButtonSTATUS & button_data, const bool &button_state)
{
	if(FOTON_BLUETOOTH->isTransfering())
		FOTON_BLUETOOTH->enterConfigureMode();
	else
		FOTON_BLUETOOTH->enterTransferMode();
}

// Sends a test message (great for testing Bluetooth Enable/Disable with #define in ulityfunctions.h)
static void button_func2(const ButtonSTATUS & button_data, const bool &button_state)
{
    bluetooth.sendMessage("Hello World?\r\n",14);
}







void main()
{
    //
    // Initailizing the board
    //
    BoardInit();
	UDMAInit();
	// TODO Add configure calls back after BCM Driver works a bit.
	// TODO Configure Display Driver and pins < make displaydriver static and global
	ConfigureDisplayDriver(&leddisplay);
	FillVary(&leddisplay);
	ConfigLEDPins();


	// initi the clock the first time.
	//PRCMPeripheralClkEnable(PRCM_UARTA0, PRCM_RUN_MODE_CLK);


    InitTerm();

    bluetooth.setLiveMode();
    bluetooth.enableDMA();
    bluetooth.enterTransferMode();


    FOTON_LED_BOARD = &leddisplay;
    // Clear terminal
    ClearTerm();

    CURRENT_MESSAGE = &local_current_message;
    FOTON_BLUETOOTH = &bluetooth;



    // InitializeLEDs(); //Initialize LEDS (Cannot use LEDs until moved RX and TX Uart 1 from pin_01 and pin_02

    // Configure Button one
    Button1_PTR = &button1;
    button1.configureInterrupt(&BUTTON_ISR,ButtonDriver::BOTH_EDGES);
    BUTTON_DEBOUNCE_CTRL  debounce;
    debounce.FIRE_MODE = BUTTON_ON_RELEASED;
    debounce.CTRL_DATA = 1;
    button1.registerButtonFunc(button_func);
    ButtonDriver::configureDebounce(1, debounce);
    button1.enableInterrupt();

    // Configure Button two
    Button2_PTR = &button2;
    button2.configureInterrupt(&BUTTON_ISR,ButtonDriver::BOTH_EDGES);
    button2.registerButtonFunc(button_func2);
    debounce.FIRE_MODE = BUTTON_ON_RELEASED;
    debounce.CTRL_DATA = 1;
    ButtonDriver::configureDebounce(2, debounce);
    button2.enableInterrupt();

    //xTaskCreate( BUTTON_DEBOUNCE_TASK, "B-Deb",OSI_STACK_SIZE, NULL, 2, &DEBOUNCE_TSK_HNDLE);

    xTaskCreate( DisplayCurrentImageRGB, "DispCurImg",OSI_STACK_SIZE, NULL, 1, &DISP_IMG_HNDLE);

    // attempt to use bluetooth
  //  bluetooth.sendMessage("AT+UART?\r\n",22);

    vTaskStartScheduler();
    return;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
