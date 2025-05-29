#include <Arduino.h>
#include <EspSimHub.h>
#include <FlowSerialRead.h>

#include <Arduino_GFX_Library.h>

#define DEVICE_NAME "ESP-SimHubDisplay 1"

#define PIXEL_WIDTH 800
#define PIXEL_HEIGHT 480
#define SCREEN_WIDTH_MM 95 // only the screen area with pixels, in mm. Can be approximate, used to calculate density
#define PIXEL_PER_MM (PIXEL_WIDTH / SCREEN_WIDTH_MM)

#define INCLUDE_WIFI FALSE
// Less secure if you plan to commit or share your files, but saves a bunch of memory. 
//  If you hardcode credentials the device will only work in your network
#define USE_HARDCODED_CREDENTIALS TRUE

#if INCLUDE_WIFI
#if USE_HARDCODED_CREDENTIALS
#define WIFI_SSID "dash"
#define WIFI_PASSWORD "dash"
#endif

#define BRIDGE_PORT 10001 // Perle TruePort uses port 10,001 for the first serial routed to the client
#define DEBUG_TCP_BRIDGE  // emits extra events to Serial that show network communication, set to false to save memory and make faster

#include <TcpSerialBridge2.h>
#include <ECrowneWifi.h>
#include <FullLoopbackStream.h>

FullLoopbackStream outgoingStream;
FullLoopbackStream incomingStream;

#endif // INCLUDE_WIFI


char loop_opt;
char xactionc;
unsigned long lastSerialActivity = 0;

void idle(bool critical);


// Don't change this
#define VERSION 'j'




//#include "SHCustomProtocol.h"
//SHCustomProtocol shCustomProtocol;


#include "SHRGBDisplay.h"
SHRGBDisplay SHRGBDisplay;



#include <SHCommands_RGBLED.h>

void setup(void)
{
//	Serial.begin(115200);
  FlowSerialBegin(19200);
  

  arqserial.setIdleFunction(idle);
  SHRGBDisplay.setup();
#if INCLUDE_WIFI
#if DEBUG_TCP_BRIDGE
//	Serial.begin(115200);
#endif
#endif

#if INCLUDE_WIFI
	ECrowneWifi::setup(&outgoingStream, &incomingStream, gfx);
#endif
/*
FlowSerialPrint("hello");
	FlowSerialPrint("\n");
	FlowSerialFlush();
	String serialdata = FlowSerialReadStringUntil('\n');
		//	gfx->setTextColor(YELLOW, BLACK);
			terminalPrintln(serialdata,gfx);
			FlowSerialPrint(serialdata);
			FlowSerialPrint("\n");
	FlowSerialFlush();
	FlowSerialPrint("G");
	FlowSerialFlush();
*/

}

void loop()
{
#if INCLUDE_WIFI
	ECrowneWifi::loop();
#endif

  //SHRGBDisplay.loop();

	if (FlowSerialAvailable() > 0) {
		if (auto r = FlowSerialTimedRead() == MESSAGE_HEADER)
		{
			lastSerialActivity = millis();
			// Read command
			loop_opt = FlowSerialTimedRead();
			switch(loop_opt) {
				case '1': Command_Hello(); break;
				case '0': Command_Features(); break;
				case '4': Command_RGBLEDSCount(); break;
				case 'X': {
					String xaction = FlowSerialReadStringUntil(' ', '\n');
					if (xaction == F("list")) Command_ExpandedCommandsList();
					else if (xaction == F("mcutype")) Command_MCUType();
				}
				break;
				case 'N': Command_DeviceName(); break;
				case 'I': Command_UniqueId(); break;
				case '8': Command_SetBaudrate(); break;
				case 'J': Command_ButtonsCount(); break;
				case '2': Command_TM1638Count(); break;
				case 'B': Command_SimpleModulesCount(); break;
				case 'A': Command_Acq(); break;
				case 'G': Command_GearData(); break;
				case 'P': Command_CustomProtocolData(); break;
				default:
					break;
			}
		}
	}
}

void idle(bool critical) {
#if INCLUDE_WIFI
	yield();
	ECrowneWifi::flush();
#endif

SHRGBDisplay.idle(); 
}