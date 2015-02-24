/*
 * HC-05driver.h
 *
 *  Created on: Nov 21, 2014
 *      Author: Christopher
 */

#ifndef HC_05DRIVER_HP_
#define HC_05DRIVER_HP_


#define TX_PIN  PIN_55 // 54
#define RX_PIN  PIN_57 // 56
#define RTS_PIN PIN_20 // 19
#define STATE_PIN PIN_18 // 17

#define MAX_NAME_LENGTH 90
#define MAX_COMMAND_INDEX 300
#include "hw_types.h"
#include "udma.h"
#include "FreeRTOS.h"
#include "ButtonDriver.h"


typedef struct FOTON_LED_MESSAGE
{
    unsigned char FUNC_MJR : 3;
    unsigned char CTRL_1   : 5;
    unsigned char FUNC_MNR : 3;
    unsigned char CTRL_2   : 5;
    unsigned char RED      : 8;
    unsigned char GREEN    : 8;
    unsigned char BLUE     : 8;
}FOTON_LED_MESSAGE;


enum FUNCTIONS_MAJOR
{
    NONE = 0,
    LED_MJR,
    DRAW_MJR,
    TRANSFER_MJR,
    SYSTEM_MJR
};

enum LED_MINOR
{
    LED_CLEAR_MNR = 0,
    LED_SET_MNR,
    LED_COLOR_MNR,
    LED_READ_MNR
};


enum DRAW_MINOR
{
    LINE = 0,
    SQUARE,
    CIRCLE
};

enum HC_05_PARSE_STATES
{
	START = 0,
	FUNCTION_MJR,
	FUNCTION_MNR,
	COL,
	ROW,
	RED,
	GREEN,
	BLUE
};

//#include "queue.h"






extern void BlueToothInterruptHandler();


enum HC_05_AT_COMMANDS
{
	Test = 0,
	Reset,
	ReqVersion,
	ResetDefaultStatus,
	ReqBTAddress,
	ReqBTName,
	SetBTName,
	GetMasterName,
	GetModuleRole,
	SetModuleRole,
	GetDeviceType,
	SetDeviceType,
	GetAccessCode,
	SetAccessCode,
	GetAMode,
    SetAMode,
    GetPassKey,
    SetPassKey,
    GetBaudRate,
    SetBaudRate,
    GetConnectMode,
    SetConnectMode,
    GetBTAddressBind,
    SetBTAddressBind,
    GetCStatusNotify,
    SetCStatusNotify,
    GetSecurityMode,
    SetSecurityMode,
    DeleteAuthentication,
    ClearAuthenticationList,
    FindAuthenticationFor,
    GetAuthenticationCount,
    GetMostRecentDevice,
    GetCurrentStatus,
    DropCurrentDevice,
    EnterHibernate,
    ExitEnergyMode,
    GetDisconnecType
};


enum HC_05_Errors
{
	AT_COMMAND_ERROR = 0,
	DEFAULT_RESULT = 1,
	PSKEY_WRITE_ERROR = 2,
	NAME_TOO_LONG_ERROR = 3,

};


class HC_05Bluetooth
{

public:

	HC_05Bluetooth(unsigned char RX_pin,unsigned long RX_Mode,
			   unsigned char TX_pin,unsigned long TX_Mode,
			   unsigned char POWER_pin,unsigned long GPIO_power,
			   unsigned char STATE_pin,unsigned long GPIO_state);


	void configureDMATransfers(bool livemode);
	void enableDMA();
	void enable(void);
	void disable(void);
	void enterConfigureMode(void);
	void enterTransferMode(void);
	void sendMessage(const char * message, unsigned int length);
	void setPowerOn(bool power_on = true);
	void processNextByte(char byte);
	// Bluetooth AT Config Commands Specific to the HC-05 bluetooth
	void setATCommand(const char * command){
		WaitingForResponse = true;
	}
	void getATCommand(char * buffer){
		WaitingForResponse = true;
	}

	void getCommand(char * buffer){
		WaitingForResponse = true;
	}

	void setCommand(char * buffer){
		WaitingForResponse = true;
	}

	void setLiveMode();
	void setOtherMode();

	/*bool testAT(){return false;}
	bool resetBluetooth(){return false;}
	bool requestVersion(char[] version_buffer){return false;}
	bool resetToDefaultStatus(){return false;}
	bool requestBluetoothAddress(){return false;}
	bool requestBluetoothName(){return false;}
	bool getBluetoothName(char[] name_buffer){return false;}
	bool setBluetoothName(const char * name){return false;}
    bool getMasterName(char[]  mname_buffer){return false;}
	bool getModuleRole(char[]  module_role){return false;}
	bool setModuleRole(const char * module_role){return false;}
	bool getDeviceType(char[]  device_type_buffer){return false;}
	bool setDeviceType(const char * device_type){return false;}
	bool setAccessCode(const char * access_code){return false;}
    bool getAccessCode(char[] access_code_buffer){return false;}
    bool getAMode(char[] access_mode_buffer){return false;}
	bool setAMode(const char * access_mode){return false;}
	bool getPassKey(char[] pass_key_buffer){return false;}
	bool setPassKey(const char * pass_key){return false;}
	    GetBaudRate,
	    SetBaudRate,
	    GetConnectMode,
	    SetConnectMode,
	    GetBTAddressBind,
	    SetBTAddressBind,
	    GetCStatusNotify,
	    SetCStatusNotify,
	    GetSecurityMode,
	    SetSecurityMode,
	    DeleteAuthentication,
	    ClearAuthenticationList,
	    FindAuthenticationFor,
	    GetAuthenticationCount,
	    GetMostRecentDevice,
	    GetCurrentStatus,
	    DropCurrentDevice,
	    EnterHibernate,
	    ExitEnergyMode,
	    GetDisconnecType*/

	~HC_05Bluetooth();
	unsigned short  FrontIndex;
	unsigned short  BackIndex;
	char            mMessageBuffer[MAX_COMMAND_INDEX];
	unsigned char   mRXDataBuff[8];
private:
	void waitForModeChange(void);

	unsigned short  MessageCount;
	bool            WaitingForResponse;
    bool            mEnabled;
    bool            mPoweredON;
    HC_05_PARSE_STATES mPARSE_STATE;

    // port addresses
	unsigned long mStatePortAddress;
	unsigned long mPowerPortAddress;
	// pin numbers
	unsigned char mStatePinNumber;
	unsigned char mPowerPinNumber;
	// pin addresses
	unsigned char mStatePinAddress;
	unsigned char mPowerPinAddress;


};


extern HC_05Bluetooth *  FOTON_BLUETOOTH;
extern void BluetoothReadTask(void *);
extern FOTON_LED_MESSAGE * CURRENT_MESSAGE;
#endif /* HC_05DRIVER_HPP_ */
