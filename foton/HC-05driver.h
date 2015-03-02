/*
 * HC-05driver.h
 *
 *  Created on: Nov 21, 2014
 *      Author: Christopher
 */

#ifndef HC_05DRIVER_HP_
#define HC_05DRIVER_HP_




#define MAX_NAME_LENGTH 90
#define MAX_COMMAND_INDEX 300
#include "hw_types.h"
#include "udma.h"
#include <FreeRTOS.h>
#include <queue.h>
#include "ButtonDriver.h"


typedef struct FOTON_LIVE_MESSAGE
{
    unsigned char FUNC_CTRL;
    unsigned char DATA1;
    unsigned char DATA2;
    unsigned char DATA3;
}FOTON_LIVE_MESSAGE;






enum FUNCTIONS_MAJOR
{
    NONE = 0,
    LED_CLEAR,
    LED_SET_COLOR,
    LED_SET_AT,
    LED_READ
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
	LIVE_FUNCTION_DECODE,
	LIVE_DATA1,
	LIVE_DATA2,
	LIVE_DONE,
};

//#include "queue.h"






extern void BlueToothInterruptHandler();


enum HC_05_AT_COMMAND
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
    GetDisconnectType
};

typedef struct BLUETOOTH_AT_REQUEST
{
	unsigned short   CallerID;
	HC_05_AT_COMMAND CommandID;
	BLUETOOTH_AT_REQUEST(unsigned short callerid, HC_05_AT_COMMAND commandid)
		: CallerID(callerid), CommandID(commandid)
	{}

}BLUETOOTH_AT_REQUEST;

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
	void processATCommandResponse(char command[], int last_index);
	void setPowerOn(bool power_on = true);
	void powerCycle();
	// Bluetooth AT Config Commands Specific to the HC-05 bluetooth
	void setATCommand(const char * command){
		WaitingForResponse = true;
	}
	void getATCommand(char * buffer){
		WaitingForResponse = true;
	}

	void getCommand(HC_05_AT_COMMAND command_type, unsigned short caller_id);

	void setCommand(char * buffer){
		WaitingForResponse = true;
	}

	void setLiveMode();
	void setOtherMode();

	void waitForModeChange(void);

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
	bool isTransfering() {return mTransferModeEnabled;}
private:

	unsigned short  MessageCount;
	bool            WaitingForResponse;
    bool            mEnabled;
    bool            mPoweredON;
    bool            mTransferModeEnabled;
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

extern char RXDATABUFF[MAX_COMMAND_INDEX];
extern HC_05Bluetooth *  FOTON_BLUETOOTH;
extern const char *      COMMAND_STR_TABLE[GetDisconnectType];
extern void BluetoothReadTask(void *);
extern void BluetoothProcessATTask(void*);
extern TaskHandle_t BLUETOOTH_READ_HNDLE;
extern TaskHandle_t BLUETOOTH_CMD_READ_HNDLE;
extern FOTON_LIVE_MESSAGE * CURRENT_MESSAGE;
extern QueueHandle_t        AT_COMMAND_QUEUE;
#endif /* HC_05DRIVER_HPP_ */
