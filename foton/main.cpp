
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

//*****************************************************************************
//                          MACROS
//*****************************************************************************
#define APPLICATION_VERSION     "0.1.0"
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

static HC_05Bluetooth  bluetooth(PIN_02,PIN_MODE_7,PIN_01,PIN_MODE_7,28,PIN_MODE_0,14,PIN_MODE_0);
HC_05Bluetooth * FOTON_BLUETOOTH;
static FOTON_LED_MESSAGE local_current_message;
FOTON_LED_MESSAGE * CURRENT_MESSAGE;
BUTTON_DEBOUNCE_CTRL Button1_Debounce;
BUTTON_DEBOUNCE_CTRL Button2_Debounce;




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




static void button_func(const ButtonSTATUS & button_data, const bool &button_state)
{
	//Report("GOT THE BUTTON SIGNAL FROM FREE RTOS TASK!!!! Button 1 state is %u",  button_state);
	if(button_state)
		FOTON_BLUETOOTH->sendMessage("Button one pressed.",18);
	else
		FOTON_BLUETOOTH->sendMessage("Button one released.",18);

	//Toggle_LED(GREEN_LED);
}


static void button_func2(const ButtonSTATUS & button_data, const bool &button_state)
{
	//Report("GOT THE INTERRUPT!!!! Button 2 state is %u", button_state);
	if(button_state)
		FOTON_BLUETOOTH->sendMessage("Button two pressed.",18);
	else
		FOTON_BLUETOOTH->sendMessage("Button two released.",18);
	//Toggle_LED(YELLOW_LED);
}








void main()
{
    //
    // Initailizing the board
    //
    BoardInit();
	UDMAInit();


    //
    // Initialize uDMA
    //


    //
    // Muxing for Enabling UART_TX and UART_RX.
    //
    PinMuxConfig();
    //

    InitTerm();

   // bluetooth.enterTransferMode();

    //bluetooth.enableDMA();
    bluetooth.setPowerOn(true);
    bluetooth.setLiveMode();
    bluetooth.enable();

    // Clear terminal
    //
    ClearTerm();

    CURRENT_MESSAGE = &local_current_message;
    FOTON_BLUETOOTH = &bluetooth;



   // Report("Initializing Bluetooth Device");



   // InitializeLEDs(); //Initialize LEDS

    // Configure Button one
    Button1_PTR = &button1;
    button1.configureInterrupt(&BUTTON_ISR,ButtonDriver::BOTH_EDGES);
    BUTTON_DEBOUNCE_CTRL  debounce;
    debounce.FIRE_MODE = BUTTON_ON_PRESSED_X;
    debounce.CTRL_DATA = 3;
    button1.registerButtonFunc(button_func);
    ButtonDriver::configureDebounce(1, debounce);
    button1.enableInterrupt();

    // Configure Button two
    Button2_PTR = &button2;
    button2.configureInterrupt(&BUTTON_ISR,ButtonDriver::BOTH_EDGES);
    button2.registerButtonFunc(button_func2);
    debounce.FIRE_MODE = BUTTON_ON_PULSE_AT_X;
    debounce.CTRL_DATA = 1;
    ButtonDriver::configureDebounce(2, debounce);
    button2.enableInterrupt();

    xTaskCreate( BUTTON_DEBOUNCE_TASK, "B-Deb",OSI_STACK_SIZE, NULL, 2, &DEBOUNCE_TSK_HNDLE);
    xTaskCreate( BluetoothReadTask, "BLE",OSI_STACK_SIZE, NULL, 2, &BLUETOOTH_READ_HNDLE);

    // attempt to use bluetooth
//    bluetooth.enterTransferMode();
    //bluetooth.setReadMode();
    bluetooth.sendMessage("at\r\n",4);
   // bluetooth.setWriteMode();

    vTaskStartScheduler();
    return;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
