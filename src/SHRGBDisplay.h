/*
#include <esp_mac.h>
#include "esp_system.h"


// Fake an Arduino Mega
#define SIGNATURE_0 0x1E
#define SIGNATURE_1 0x98
#define SIGNATURE_2 0x01

// Configure FASTLED with proper pin order
#define FASTLED_ESP8266_NODEMCU_PIN_ORDER

// A unique identifier for the device.
//  in the future we could use the bytes to generate some
//  other format (ala UUID), but now it's just a unique
//  string tied to the device.


String getMacAddress() {
	uint8_t baseMac[6];
	// Get MAC address for WiFi station
	//esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
	char baseMacChr[18] = {0};
	sprintf(baseMacChr, "%02X:%02X:%02X:%02X:%02X:%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
	return String(baseMacChr);
}

String getUniqueId() {

   // byte mac[6];
	uint8_t mac[8];

	esp_efuse_mac_get_default(mac);
   mac = esp_read_mac(ESP_MAC_EFUSE_CUSTOM);
 return getMacAddress();
	return WiFi.macAddress();
}
//Call function to get custom Mac address


*/

#define PIXEL_WIDTH 800
#define PIXEL_HEIGHT 480
#define SCREEN_WIDTH_MM 95 // only the screen area with pixels, in mm. Can be approximate, used to calculate density
#define PIXEL_PER_MM (PIXEL_WIDTH / SCREEN_WIDTH_MM)

#define TFT_BL 2 // backlight pin
#define RGB565_DDGREY RGB565(16, 16, 16)
#define RGB565_vDGREY RGB565(8, 8, 8)

#ifndef __SHRGBDisplay_H__
#define __SHRGBDisplay_H__

#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <map>
#include "icons.h"
#include "logos.h"

#define DEVICE_NAME "ESP-SimHubDisplay 1"

// https://github.com/eCrowneEng/ESP-SimHub-ESP32S3-SCREEN/issues/1
// 8048S043 - 800x480, capacitive touch
Arduino_ESP32RGBPanel *rgbpanel = new Arduino_ESP32RGBPanel(
	40 /* DE */, 41 /* VSYNC */, 39 /* HSYNC */, 42 /* PCLK */,
	45 /* R0 */, 48 /* R1 */, 47 /* R2 */, 21 /* R3 */, 14 /* R4 */,
	5 /* G0 */, 6 /* G1 */, 7 /* G2 */, 15 /* G3 */, 16 /* G4 */, 4 /* G5 */,
	8 /* B0 */, 3 /* B1 */, 46 /* B2 */, 9 /* B3 */, 1 /* B4 */,
	0 /* hsync_polarity */, 8 /* hsync_front_porch */, 4 /* hsync_pulse_width */, 16 /* hsync_back_porch */,
	0 /* vsync_polarity */, 4 /* vsync_front_porch */, 4 /* vsync_pulse_width */, 4 /* vsync_back_porch */,
	1 /* pclk_active_neg */, 16000000 /* prefer_speed */);

Arduino_RGB_Display *gfx = new Arduino_RGB_Display(
	PIXEL_WIDTH /* width */, PIXEL_HEIGHT /* height */, rgbpanel, 0 /* rotation */, true /* auto_flush */);

static const int SCREEN_WIDTH = PIXEL_WIDTH;
static const int SCREEN_HEIGHT = PIXEL_HEIGHT;
static const int X_CENTER = SCREEN_WIDTH / 2;
static const int Y_CENTER = SCREEN_HEIGHT / 2;

static const int titlefont = 2;
static const int itemfont = 2;
static const int CELL_WIDTH = 40; // SCREEN_WIDTH / COLS;
static const int HALF_CELL_WIDTH = CELL_WIDTH / 2;
static const int CELL_HEIGHT = 30; // SCREEN_HEIGHT / ROWS;
static const int HALF_CELL_HEIGHT = CELL_HEIGHT / 2;
static const int ROWS = round(SCREEN_WIDTH / CELL_WIDTH);
static const int COLS = round(SCREEN_HEIGHT / CELL_HEIGHT);

static const int COL[] = {0, CELL_WIDTH, CELL_WIDTH * 2, CELL_WIDTH * 3, CELL_WIDTH * 4, CELL_WIDTH * 5, CELL_WIDTH * 6, CELL_WIDTH * 7, CELL_WIDTH * 8};
static const int ROW[] = {0, CELL_HEIGHT, CELL_HEIGHT * 2, CELL_HEIGHT * 3, CELL_HEIGHT * 4, CELL_HEIGHT * 5, CELL_HEIGHT * 6, CELL_HEIGHT * 7, CELL_HEIGHT * 8};

#include <GFXHelpers.h>

std::map<String, String> liveData;
std::map<String, String> dashData;
std::map<String, String> lastData;

class SHRGBDisplay
{
private:
	// Global variables

	int cellTitleHeight = 0;
	bool hasReceivedData = false;
	bool drawbackground = false;

public:
	void setup()
	{
		gfx->begin();
		gfx->setFont(u8g2_font_spleen12x24_mn);
		splash();
		terminalPrintln("INIT Dash", gfx);
		//	init_past();
		// gfx->begin();

#ifdef TFT_BL
		pinMode(TFT_BL, OUTPUT);
		digitalWrite(TFT_BL, HIGH);

#endif

	}
	void splash() {
	
		gfx->setTextColor(RGB565(0xff, 0x66, 0x00));

		// drawCentreCentreString(DEVICE_NAME, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 4, gfx);
		gfx->setCursor(0, 0);
		gfx->setTextColor(WHITE);
		gfx->setTextSize(1);
		gfx->fillScreen(BLACK);
		//	render_dash();
		//
		gfx->fillScreen(BLACK);

		// gfx->drawBitmap(SCREEN_WIDTH / 2 - 200, CELL_HEIGHT * 6 - 10, LOGO_bitmap_gamehub_icon_small_text_1, 400, 80, BLACK, WHITE);
		gfx->drawBitmap(1, CELL_HEIGHT * 6 - 30, LOGO_bitmap_gamehub_icon_small_text_1, 400, 80, BLACK, WHITE);

		gfx->setTextColor(RGB565(0xff, 0x66, 0x00));
	//	delayMicroseconds(10000);
		// render_dash();

		// gfx->fillRoundRect(0, SCREEN_HEIGHT/3, SCREEN_WIDTH, SCREEN_HEIGHT/3 + 20, 5, WHITE);
		/*
				gfx->setTextColor(RGB565(0xff, 0x66, 0x00));
				drawString(DEVICE_NAME, 20, SCREEN_HEIGHT - 60, 2, gfx);
				gfx->setTextColor(YELLOW, BLACK);
				drawString(getUniqueId(), 20, SCREEN_HEIGHT - 40, 2, gfx);
		*/
		//
		/*

		*/
		//
		// read("bob:bob");
		// if (!serialdata != NULL && !hasReceivedData)
		{
		}
		//
		//read();
		if (hasReceivedData == true)
			drawbackground = true;
			//if(dashData["CurrentDateTime"]!=NULL)
			{
		//gfx->fillScreen(RGB565_DDGREY);
	}
	}
	String set_value(String data = "")
	{

		//	String data = FlowSerialReadStringUntil(';');
		int split = data.indexOf(":");
		String value = data.substring(split + 1);
		String key = data.substring(0, split);
		if (key > "")
		{
			//	terminalPrintln(key,gfx);
			// if (value != lastData[key])
			dashData[key] = value;
			// LiveData[name] = "0";
			lastData[key] = value;
			return (dashData[key]);
		}
		return data;
	}

	void clear_data()
	{

		dashData["FlashingLights"] == NULL;
		dashData["LapInvalidated"] == NULL;
		dashData["Gear"] == NULL;
		dashData["SpeedKmh"] == NULL;
		dashData["Rpms"] == NULL;
		dashData["RPMPercent"] == NULL;
		dashData["mDeltaBest"] == NULL;
		dashData["SessionBestLiveDeltaProgressSeconds"] == NULL;
		dashData["ERSPercent"] == NULL;
		dashData["IsInPitLane"] == NULL;
		dashData["PitLimiterOn"] == NULL;
		dashData["Flag_Yellow"] == NULL;
		dashData["FuelPercent"] == NULL;
		dashData["ERSPercent"] == NULL;
		dashData["CompletedLaps"] == NULL;
		dashData["Position"] == NULL;
		dashData["Geara"] == NULL;
		dashData["LastLapTime"] == NULL;
		dashData["BestLapTime"] == NULL;
		dashData["TCLevel"] == NULL;
		dashData["ABSLevel"] == NULL;
		dashData["BrakeBias"] == NULL;
		dashData["Fuel"] == NULL;
		dashData["EngineMap"] == NULL;
		dashData["Fuel_RemainingLaps"] == NULL;
		dashData["Fuel_CurrentLapConsumption"] == NULL;
		dashData["TyrePressureFrontLeft"] == NULL;
		dashData["TyrePressureFrontRight"] == NULL;
		dashData["TyrePressureRearLeft"] == NULL;
		dashData["TyrePressureRearRight"] == NULL;
		dashData["BrakeTemperatureFrontLeft"] == NULL;
		dashData["BrakeTemperatureFrontRight"] == NULL;
		dashData["BrakeTemperatureRearLeft"] == NULL;
		dashData["BrakeTemperatureRearRight"] == NULL;
		dashData["TyreTemperatureFrontLeft"] == NULL;
		dashData["TyreTemperatureFrontRight"] == NULL;
		dashData["TyreTemperatureRearLeft"] == NULL;
		dashData["TyreTemperatureRearRight"] == NULL;
		dashData["CarSettings_RPMShiftLight2"] == NULL;
		dashData["CarSettings_RPMShiftLight1"] == NULL;
		dashData["Flag_Checkered"] == NULL;
		dashData["Flag_Black"] == NULL;
		dashData["Flag_Blue"] == NULL;
		dashData["Flag_White"] == NULL;
		dashData["EstimatedDelta"] == NULL;
		dashData["CompactDelta"] == NULL;
		dashData["SplitDelta"] == NULL;
		dashData["CarSettings_CurrentDisplayedRPMPercent"] == NULL;
		dashData["CurrentLapTime"] == NULL;
		dashData["ABSActive"] == NULL;
		dashData["TCActive"] == NULL;
		dashData["CurrentDateTime"] == NULL;
		dashData["TCLevel"] == NULL;

		dashData["Fuel_RemainingLaps"] == NULL;
	}

	void read(String serialdata_test = "")
	{
		// hasReceivedData = false;
		int refresh = 0;
		// dashData=liveData;
		// if(serialdata_test=="")
		String serialdata = "SpeedKmh:123;Gear:0;CarSettings_CurrentDisplayedRPMPercent:50;FuelPercent:69;CarSettings_RPMRedLineSetting:0;Rpms:1234;LapInvalidated:0;CurrentLapTime:00:00.000;LastLapTime:00:00.000;BestLapTime:00:00.000;CompactDelta:-1.234;SessionBestLiveDeltaProgressSeconds:0.00;CompletedLaps:000;Position:001;TCLevel:1;TCActive:1;ABSLevel:1;ABSActive:0;BrakeBias:50;Fuel:500;EngineMap:1;Fuel_RemainingLaps:-;Fuel_CurrentLapConsumption:-;IsInPitLane:1;PitLimiterOn:1;TyrePressureFrontLeft:00.0;TyrePressureFrontRight:00.0;TyrePressureRearLeft:00.0;TyrePressureRearRight:00.0;BrakeTemperatureFrontLeft:9000;BrakeTemperatureFrontRight:700;BrakeTemperatureRearLeft:600;BrakeTemperatureRearRight:800;TyreTemperatureFrontLeft:70;TyreTemperatureFrontRight:70;TyreTemperatureRearLeft:70;TyreTemperatureRearRight:1000;CarSettings_RPMShiftLight1:0;CarSettings_RPMShiftLight2:0;Flag_Checkered:0;Flag_Black:0;Flag_Blue:0;Flag_White:0;Flag_Yellow:1;mHeadlights:0;CurrentDateTime:******************;";
		// serialdata = serialdata + "SplitDelta:;CarSettings_CurrentDisplayedRPMPercent:39;CurrentLapTime:01:27.610;ABSActive:1;TCActive:0;SpeedKmh:039;Rpms:02828;EstimatedDelta:-4.03;mDeltaBest:;SessionBestLiveDeltaProgressSeconds:-0.00;CompactDelta:-1.234;CurrentDateTime:5/21/2025 4:43:39 PM;";
		//serialdata ="FlashingLights:0;LapInvalidated:0;Gear:0;SpeedKmh:0;Rpms:0;mDeltaBest:0;SessionBestLiveDeltaProgressSeconds:0;IsInPitLane:0;PitLimiterOn:0;Flag_Yellow:0;FuelPercent:0;ERSPercent:0;CompletedLaps:0;Position:0;Geara:0;LastLapTime:0;BestLapTime:0;TCLevel:0;ABSLevel:0;BrakeBias:0;Fuel:0;EngineMap:0;Fuel_RemainingLaps:0;Fuel_CurrentLapConsumption:0;TyrePressureFrontLeft:0;TyrePressureFrontRight:0;TyrePressureRearLeft:0;TyrePressureRearRight:0;BrakeTemperatureFrontLeft:0;BrakeTemperatureFrontRight:0;BrakeTemperatureRearLeft:0;BrakeTemperatureRearRight:0;TyreTemperatureFrontLeft:0;TyreTemperatureFrontRight:0;TyreTemperatureRearLeft:0;TyreTemperatureRearRight:0;CarSettings_RPMShiftLight2:0;CarSettings_RPMShiftLight1:0;Flag_Checkered:0;Flag_Black:0;Flag_Blue:0;Flag_White:0;EstimatedDelta:0;CompactDelta:0;SplitDelta:0;CarSettings_CurrentDisplayedRPMPercent:0;CurrentLapTime:0;ABSActive:0;TCActive:0;TCLevel:0;CurrentDateTime:0;";
		
		int looper = 0;
		String key;
		String value;
		clear_data();
		// String
		serialdata = FlowSerialReadStringUntil('\n');
		//	gfx->setTextColor(YELLOW, BLACK);
		//	terminalPrintln(serialdata,gfx);
		//	drawString(serialdata, 20, SCREEN_HEIGHT - 60, 2, gfx);

		if (serialdata != NULL)
		{
			int index = 1;
			int slen = 50;
			slen = serialdata.lastIndexOf(";");
			int lsplit = 0;
			int i;
			String key;
			String value;
			int split;
			int row = 10;

			for (int i = 0; i < slen - 1; i++)
			{
				row++;
				split = serialdata.indexOf(";", i);
				// if(i != split)
				{
					key = serialdata.substring(i, split);
				}
				if (i < split)
				{
					set_value(key);
					i = split;
				}
			}

			if(dashData["CurrentDateTime"]!=NULL)
			{
				hasReceivedData = true;
				render_dash();
			}
		}
		FlowSerialWrite(0x15);
	}

	// Called once per arduino loop, timing can't be predicted,
	// but it's called between each command sent to the arduino
	void loop()
	{
		/*
		if(dashData["CurrentDateTime"]!=NULL){
		render_dash();
		//	gfx->setTextColor(MAROON, RGB565_DDGREY); // IsInPitLane
		//	drawString(dashData["CurrentDateTime"], CELL_WIDTH * 7.5, ROWS * CELL_HEIGHT - HALF_CELL_HEIGHT, 2, gfx);
		//
		// if(hasReceivedData)
		//();
		gfx->setTextColor(YELLOW, BLACK);

		drawString( "\n " + dashData["CurrentDateTime"], 20, SCREEN_HEIGHT - 20, 1, gfx);
	}
		hasReceivedData = false;
	*/
	}

	void idle()
	{
		//	loop();
	}

	void render_dash()
	{
		// do_string("bob");
		int nextpos = 0;

		/*	itembox(0,0,1,1,RED); //gear
			itembox(1,1,1,1,RED); //gear
			itembox(2,2,1,1,RED); //gear
			itembox(3,3,1,1,RED); //gear
			itembox(4,4,1,1,RED); //gear
			itembox(5,5,1,1,RED); //gear
			itembox(6,6,1,1,RED); //gear
			itembox(7,7,1,1,RED); //gear
			itembox(8,8,1,1,RED); //gear
			itembox(9,9,1,1,RED); //gear
			itembox(10,10,1,1,RED); //gear
			itembox(11,11,1,1,RED); //gear
			itembox(12,12,1,1,RED); //gear
			itembox(13,13,1,1,RED); //gear
			itembox(14,14,1,1,RED); //gear
			itembox(15,15,1,1,RED); //gear
			itembox(16,16,1,1,RED); //gear
			itembox(17,17,1,1,RED); //gear
			itembox(18,18,1,1,RED); //gear
			itembox(19,19,1,1,RED); //gear
			//itembox(1,1,1,1,RED); //gear

	*/

		gfx->setTextColor(MAROON, RGB565_DDGREY);
		//

		gfx->setTextColor(BLACK, RGB565_DDGREY); // IsInPitLane
		gfx->setFont(u8g2_font_spleen32x64_mn);
		// drawCentreCentreString("8", (SCREEN_WIDTH / 2) , (SCREEN_HEIGHT / 2) +128 +64 +5, 4, gfx);
		gfx->setFont(u8g2_font_spleen12x24_mn);
		if (dashData["Gear"] != NULL)
		{
			gfx->setTextColor(DARKGREEN, DARKGREY); // IsInPitLane
			if (dashData["CarSettings_RPMShiftLight2"].toInt() >= 1)
				gfx->setTextColor(MAROON, RGB565_DDGREY); // IsInPitLane

			//
			if (!drawbackground)
			{
				// gfx->fillRect(0, CELL_HEIGHT * 3, CELL_WIDTH * 103, CELL_HEIGHT * 7, RGB565_DDGREY);
				drawbackground = true;
				gfx->fillScreen(RGB565_DDGREY);
				itembox(3, 5, 3, 2, DARKGREY); // pos
				itembox(0, 5, 3, 2, DARKGREY); // lap

				itembox(0, 7, 6, 3, DARKGREY);	// sp
				itembox(0, 10, 6, 3, DARKGREY); // rpm

				itembox(0, 2, 6, 3, DARKGREY); // delta

				itembox(8, 5, 4, 8, DARKGREY); // gear
				itembox(16, 2, 5, 3, DARKGREY); // flag

				itembox(12, 5, 8, 3, DARKGREY);  // c
				itembox(12, 8, 4, 2, DARKGREY);  // l
				itembox(12, 10, 4, 2, DARKGREY); // b
			}
			//	gfx->fillRect(CELL_WIDTH * 4, CELL_HEIGHT * 9, CELL_WIDTH * 3, CELL_HEIGHT * 2, flag());
			// itembox(7,15,5,2,
			//	gfx->fillRect(7, 15, 5, 2, flag());
			// gfx->drawRect(CELL_WIDTH * 4 + 1, CELL_HEIGHT * 3, CELL_WIDTH * 3, CELL_HEIGHT * 6, BLUE);
			gfx->setFont(u8g2_font_spleen32x64_mn);
			// if(dashData["Gear"] != lastData["Gear"])
			//	gfx->fillRect(CELL_HEIGHT * 8, CELL_HEIGHT * 3, CELL_HEIGHT * 6, CELL_HEIGHT * 6, RGB565_DDGREY);

			{

				drawCentreCentreString(dashData["Gear"], (CELL_WIDTH*10), (CELL_HEIGHT *16 ) , 4, gfx);

				//	drawCentreCentreString("" +  + "", (SCREEN_WIDTH / 2) , (SCREEN_HEIGHT / 2) + 140, 2, gfx);
			}
			// Data
			gfx->setFont(u8g2_font_spleen12x24_mn);
			// if (prev_gear != dashData["Gear"])
			{

				// gfx->fillRect(CELL_WIDTH * 4, CELL_HEIGHT * 3, CELL_WIDTH * 4, CELL_HEIGHT * 7, RGB565_DDGREY);
				//	gfx->drawRect(CELL_WIDTH *4 , CELL_HEIGHT*3,  CELL_WIDTH * 4 ,CELL_HEIGHT * 7, DARKGREY);
				//	gfx->fillRect(CELL_WIDTH * 3 - 5, CELL_HEIGHT * 6, 6, CELL_HEIGHT * 2, BLACK);
			}
		}


		if (dashData["SpeedKmh"] != NULL)
		element2(0, 7, 6, 3, "SpeedKmh", WHITE, 0, 0, "KPH");

		// gfx->drawRect(1, 1, CELL_WIDTH * COLS, CELL_HEIGHT - 2, DARKGREY);
		rpms();
		//

	

		// mDeltaBest
		// EstimatedLapTime_SessionBestBasedSimhub_EstimatedDelta
		//if (dashData["EstimatedDelta"] != NULL)
		{
			//	gfx->setTextColor(WHITE, dashData["EstimatedLapTime_SessionBestBasedSimhub_EstimatedDelta"].indexOf('-') >= 0 ? GREEN : RED);
		//		element2(15, 2, 5, 3, "EstimatedLapTime_SessionBestBasedSimhub_EstimatedDelta", dashData["EstimatedLapTime_SessionBestBasedSimhub_EstimatedDelta"].indexOf('-') >= 0 ? GREEN : RED, 3, 0, "Delta1");
		}
		// else
		if (dashData["CompactDelta"] != NULL)
		{
			// itembox(0,2,5,3,DARKGREY); //delta
		//	gfx->setTextColor(WHITE, dashData["CompactDelta"].indexOf('-') >= 0 ? GREEN : RED);
		element2(0, 2, 6, 3, "CompactDelta", dashData["CompactDelta"].indexOf('-') >= 0 ? GREEN : RED, 3, 0, "Delta");
		}

		if (dashData["mDeltaBest"] != NULL)
		{
			//	gfx->setTextColor(WHITE, dashData["mDeltaBest"].indexOf('-') >= 0 ? GREEN : RED);
			//	element2(COLS - 2, ROWS -1, 3, 2, "mDeltaBest", dashData["mDeltaBest"].indexOf('-') >= 0 ? GREEN : RED, 3, 0, "Delta");
		}

		//
		if (dashData["CurrentLapTime"] != NULL )
			element2(12, 5, 8, 3, "CurrentLapTime", dashData["LapInvalidated"] ? RED : WHITE, 0, 0, "Current");

		if (dashData["LastLapTime"] != NULL )
		{
			element2(12, 8, 4, 2, "LastLapTime", YELLOW, 0, 0, "Last");
		}
		if (dashData["BestLapTime"] != NULL )
		{
			element2(12, 10, 4, 2, "BestLapTime", MAGENTA, 0, 0, "Best");
		}
		// drawCell(COL[0], ROW[0], , "sessionBestLiveDeltaSeconds", "Delta", "double", sessionBestLiveDeltaSeconds.indexOf('-') >= 0 ? GREEN : RED);

		//	std::string str = sessionBestLiveDeltaProgressSeconds;

		// if (LiveData["sessionBestLiveDeltaProgressSeconds"] == "1")
		//	element(COLS - 2, 8, 2, 1, sessionBestLiveDeltaProgressSeconds, "Delta Pogress", sessionBestLiveDeltaProgressSeconds.indexOf('-') >= 0 ? GREEN : RED, 0);
		if (dashData["Position"] != NULL )
			element2(3, 5, 3, 2, "Position", WHITE, 0, 0, "Pos");
		if (dashData["CompletedLaps"] != NULL )
			element2(0, 5, 3, 2, "CompletedLaps", WHITE, 0, 0, "Lap", "");
//element2(15, 2, 5, 3
		gfx->fillRect((CELL_WIDTH * 16) +1, (CELL_HEIGHT * 2)+1, (CELL_WIDTH * 4)-1, (CELL_HEIGHT * 3)-1, flag()); // flag
		// if (hasReceivedData)
		{
			tyres(16,8,0);
			fuel(0,13);
			warninglights(6,2);
			statuslights(12,12);
		}
	}
	void fuel(int x, int y)
	{
		// fuel
		int hpos = HALF_CELL_WIDTH;
		hpos = (x) * (CELL_WIDTH);
		int vpos = (y) * (CELL_HEIGHT);
		;

		if (dashData["FuelPercent"] != NULL )
		{
			gfx->setTextColor(DARKGREY, RGB565_DDGREY);
			gfx->drawRect(hpos, vpos, CELL_WIDTH * 5, CELL_HEIGHT * 2 + 5, DARKGREY);
			drawString("Fuel: ", hpos + HALF_CELL_WIDTH, vpos + CELL_HEIGHT, 1, gfx);
			double fperc = (dashData["FuelPercent"].toFloat() / 100) * (HALF_CELL_WIDTH * 4);

			// drawString("Fuel", hpos - 30, vpos - 15, 2, gfx);						   // Data
			// Data
			drawString("" + dashData["Fuel"] + "L", hpos + HALF_CELL_WIDTH, vpos + CELL_WIDTH, 1, gfx); // Data
			// hpos = (5) * (CELL_WIDTH);
			//  hpos = hpos + 72;
			// vpos = (ROWS - 1) * CELL_HEIGHT - HALF_CELL_HEIGHT;
			gfx->fillRect(hpos + (CELL_WIDTH * 2), vpos + HALF_CELL_HEIGHT, (HALF_CELL_WIDTH * 4), CELL_HEIGHT * 1, DARKGREY);
			gfx->fillRect(hpos + (CELL_WIDTH * 2), vpos + HALF_CELL_HEIGHT, fperc, CELL_HEIGHT * 1, DARKGREEN);
			hpos = hpos + 144;
			hpos = hpos + HALF_CELL_WIDTH;
			// drawString( dashData["FuelPercent"] +"%", hpos, vpos +HALF_CELL_HEIGHT , 2, gfx); // Data
		}
	}

void rpms ()
{
	if (dashData["Rpms"] != NULL)
	{
		gfx->fillRect((CELL_WIDTH * COLS / 100) * dashData["RPMPercent"].toInt(), 1, CELL_WIDTH * COLS, (CELL_HEIGHT * 2) - 2, DARKGREY);
		// gfx->fillRect((SCREEN_WIDTH / 100) * CarSettings_RPMRedLineSetting.toInt() - 5, 1, SCREEN_WIDTH, CELL_HEIGHT - 2, RED);
		gfx->fillRect(1, 1, (CELL_WIDTH * COLS / 100) * dashData["RPMPercent"].toInt(), (CELL_HEIGHT * 2) - 2, DARKGREEN);

		element2(0, 10, 6, 3, "Rpms", LIGHTGREY, 0, 0, "RPM");
	}

}

	void tyres(int x,int y, int text=0)
	{
		int tix = ( (x) * CELL_WIDTH);;
		int tiy = (y) * CELL_HEIGHT ;
		int h;
		int v;

		h = ( (x) * CELL_WIDTH);
		v = ((y) * CELL_HEIGHT + HALF_CELL_HEIGHT) +8;


		int hfl = h;
		int vfl = v;

		int hfr = h +(CELL_WIDTH *2 )+HALF_CELL_WIDTH ;
		int vfr = v ;//+(CELL_HEIGHT *3);

		int hrl = h ;
		int vrl = (v) + (CELL_HEIGHT *2) ;

		int hrr =  h +(CELL_WIDTH *2 )+HALF_CELL_WIDTH ;
		int vrr = (v) + (CELL_HEIGHT *2) ;

		//tix = h;
		//tiy = v;
		gfx->setFont(u8g2_font_crox3cb_mn);
		if (dashData["CurrentDateTime"]!= NULL)	gfx->drawRect(tix, tiy , CELL_WIDTH * 4, (CELL_HEIGHT * 4), DARKGREY);

		if (dashData["TyrePressureFrontLeft"] || dashData["TyreTemperatureFrontLeft"] || dashData["BrakeTemperatureFrontLeft"])
		{
			if(text){
			drawString(dashData["TyrePressureFrontLeft"], hfl, vfl, 1, gfx);
			drawString(dashData["TyreTemperatureFrontLeft"], hfl, vfl + 20, 1, gfx);
			drawString(dashData["BrakeTemperatureFrontLeft"], hfl, vfl + 40, 1, gfx);
			}
			// drawString(  dashData["TyrePressureFrontLeft"]+"\n"+dashData["TyreTemperatureFrontLeft"]+"\n"+dashData["BrakeTemperatureFrontLeft"], hfl, vfl,  1, gfx);

			hfl = hfl + 50;
			gfx->fillRect(hfl + 5, vfl - HALF_CELL_HEIGHT, 15, CELL_HEIGHT + HALF_CELL_HEIGHT, temp(dashData["TyreTemperatureFrontLeft"]));
			gfx->fillRect(hfl + 20, vfl - (HALF_CELL_HEIGHT / 2), 15, CELL_HEIGHT, temp(dashData["BrakeTemperatureFrontLeft"]));
		}

		if (dashData["TyrePressureFrontLeft"] || dashData["TyreTemperatureFrontLeft"] || dashData["BrakeTemperatureFrontLeft"])
		{
			gfx->fillRect(hfr + 5, vfr - HALF_CELL_HEIGHT, 15, CELL_HEIGHT + HALF_CELL_HEIGHT, temp(dashData["TyreTemperatureFrontRight"]));
			gfx->fillRect(hfr - 10, vfr - (HALF_CELL_HEIGHT / 2), 15, CELL_HEIGHT, temp(dashData["BrakeTemperatureFrontRight"]));
			hfr = hfr + 30;
			// tix = (COLS - 4) * CELL_WIDTH;
			if(text){
			drawString(dashData["TyrePressureFrontRight"], hfr, vfr, 1, gfx);
			drawString(dashData["TyreTemperatureFrontRight"], hfr, vfr + 20, 1, gfx);
			drawString(dashData["BrakeTemperatureFrontRight"], hfr, vfr + 40, 1, gfx);}
		}
		/**** */

		if (dashData["TyrePressureFrontLeft"] || dashData["TyreTemperatureFrontLeft"] || dashData["BrakeTemperatureFrontLeft"])
		{
			if(text){
			drawString(dashData["TyrePressureRearLeft"], hrl, vrl, 1, gfx);
			drawString(dashData["TyreTemperatureRearLeft"], hrl, vrl + 20, 1, gfx);
			drawString(dashData["BrakeTemperatureRearLeft"], hrl, vrl + 40, 1, gfx);}
			hrl = hrl + 50;
			gfx->fillRect(hrl + 5, vrl - HALF_CELL_HEIGHT, 15, CELL_HEIGHT + HALF_CELL_HEIGHT, temp(dashData["TyreTemperatureRearLeft"]));
			gfx->fillRect(hrl + 20, vrl - (HALF_CELL_HEIGHT / 2), 15, CELL_HEIGHT, temp(dashData["BrakeTemperatureRearLeft"]));
		}
		if (dashData["TyrePressureRearRight"] || dashData["TyreTemperatureRearRight"] || dashData["BrakeTemperatureRearRight"])
		{
			gfx->fillRect(hrr + 5, vrr - HALF_CELL_HEIGHT, 15, CELL_HEIGHT + HALF_CELL_HEIGHT, temp(dashData["TyreTemperatureRearRight"]));
			gfx->fillRect(hrr - 10, vrr - (HALF_CELL_HEIGHT / 2), 15, CELL_HEIGHT, temp(dashData["BrakeTemperatureRearRight"]));
			hrr = hrr + 30;
			if(text){drawString(dashData["TyrePressureRearRight"], hrr, vrr, 1, gfx);
			drawString(dashData["TyreTemperatureRearRight"], hrr, vrr + 20, 1, gfx);
			drawString(dashData["BrakeTemperatureRearRight"], hrr, vrr + 40, 1, gfx);}
		}
	}

	void warninglights(int x = 0, int y = 1)
	{
		int nextpos;
		int startpos =x * CELL_WIDTH;
		int ypos = y * (CELL_HEIGHT );
		int wl = CELL_HEIGHT ;
		int lightwidth = 72;
		uint16_t colour;
		ypos=ypos+4;
		startpos= startpos+4;
		nextpos = startpos ;
		gfx->setTextColor(RGB565_DDGREY, BLACK);
		colour = RGB565_DDGREY;

		// nextpos = nextpos + 72;
		// nextpos = nextpos + 72;
		// nextpos = nextpos + 72;

		//nextpos = startpos + 16;
		if (dashData["TCActive"] != NULL)
		{

			gfx->drawBitmap(nextpos, ypos, epd_bitmap_Stability_Control, 72, 72, setColor(ORANGE, dashData["TCActive"], RGB565_DDGREY, 1), setColor(ORANGE, dashData["TCActive"], RGB565_DDGREY));
			nextpos = nextpos + (CELL_WIDTH*2 );
		}

		if (dashData["ABSActive"] != NULL)
		{
			gfx->drawBitmap(nextpos, ypos, epd_bitmap_ABS, 72, 72, setColor(YELLOW, dashData["ABSActive"], RGB565_DDGREY, 1), setColor(YELLOW, dashData["ABSActive"], RGB565_DDGREY));
			nextpos = nextpos + (CELL_WIDTH*2 );
		}

		// TCActive.toInt() == 1 ? RED : RGB565_DDGREY);
		if (dashData["brake"] != NULL)
		{
			gfx->drawBitmap(nextpos, ypos, epd_bitmap_Hand_Brake, 72, 72, setColor(ORANGE, dashData["brake"], RGB565_DDGREY));
			nextpos = nextpos + (CELL_WIDTH*2 );
		}

		if (dashData["PitLimiterOn"] != NULL)
		{
			gfx->drawBitmap(nextpos, ypos, epd_bitmap_Pit_Limiter, 72, 72,
							setColor(PURPLE, dashData["PitLimiterOn"], RGB565_DDGREY, 1),
							setColor(PURPLE, dashData["PitLimiterOn"], RGB565_DDGREY));
							nextpos = nextpos + (CELL_WIDTH*2 );
		}

		if (dashData["lights"] != NULL)
		{
			gfx->drawBitmap(nextpos, ypos, epd_bitmap_Lights, 72, 72, setColor(BLUE, dashData["FlashingLightw"], RGB565_DDGREY, 1), setColor(BLUE, dashData["FlashingLightw"], RGB565_DDGREY));
			nextpos = nextpos + (CELL_WIDTH*2 );
		}
		if (dashData["LapInvalidated"] != NULL)
		{
			gfx->drawBitmap(nextpos, ypos, iconHand_Brake, 72, 72, setColor(RED, dashData["LapInvalidated"], RGB565_DDGREY, 1), setColor(RED, dashData["LapInvalidated"], RGB565_DDGREY));
			nextpos = nextpos + (CELL_WIDTH*2 );
		}
		if (dashData["wipers"] != NULL)
		{
			gfx->drawBitmap(nextpos, ypos, epd_bitmap_Toggle_Wiper, 72, 72, setColor(GREEN, dashData["wipers"], RGB565_DDGREY, 1), setColor(PURPLE, dashData["wipers"], RGB565_DDGREY));
			// gfx->drawBitmap(144,1,epd_bitmap_Ignition,72,72,ignition == "1" ? RED : DARKGREY);
			nextpos = nextpos + (CELL_WIDTH*2 );
		}

		// drawBitmap(1,1,epd_bitmap_Toggle_Wiper,72,72,RED,BLACK);
		/*
				element(1,1,1,1,absLevel,"ABS",RED,0);
				element(4,1,3,2,absLevel ,"HAT",GREEN,1);
		*/
	}



	void statuslights(int x = 0, int y = 1)
	{
		int nextpos;
		int startpos =x * CELL_WIDTH;
		int ypos = y * (CELL_HEIGHT );
		int wl = CELL_HEIGHT ;
		int lightwidth = 72;
		uint16_t colour;
		startpos= startpos+4;
		ypos = ypos + 4;

		nextpos = startpos ;
		gfx->setTextColor(RGB565_DDGREY, BLACK);
		colour = RGB565_DDGREY;
		if (dashData["ABSLevel"] != NULL)
		{
			colour = setColor(ORANGE, dashData["ABSLevel"], RGB565_DDGREY);
			gfx->setTextColor(colour, RGB565_DDGREY);
			
			gfx->drawBitmap(nextpos, ypos, iconABS_plus, 72, 72, colour, BLACK);
			drawCentreCentreString(dashData["ABSLevel"], nextpos + CELL_WIDTH, ypos + CELL_HEIGHT  + CELL_HEIGHT +5, 1, gfx); // Data
		}
		nextpos = nextpos + (CELL_WIDTH*2 );
		gfx->setTextColor(RGB565_DDGREY, RGB565_DDGREY);
		colour = RGB565_DDGREY;
		if (dashData["BrakeBias"] != NULL)
		{
			colour = setColor(RED, dashData["BrakeBias"], RGB565_DDGREY);
			gfx->setTextColor(colour, RGB565_DDGREY);
			gfx->drawBitmap(nextpos, ypos, iconBrake_Balance_Front_plus, 71, 72, colour, BLACK);
			drawCentreCentreString(dashData["BrakeBias"], nextpos + CELL_WIDTH, ypos + CELL_HEIGHT  + CELL_HEIGHT +5 , 1, gfx); // Data
		}
		nextpos = nextpos + (CELL_WIDTH*2 );
		//	gfx->setTextColor(RGB565_DDGREY, RGB565_DDGREY);
		colour = RGB565_DDGREY;
		if (dashData["TCLevel"] != NULL)
		{
			colour = setColor(ORANGE, dashData["TCLevel"], RGB565_DDGREY);
			gfx->setTextColor(colour, RGB565_DDGREY);

			gfx->drawBitmap(nextpos, ypos, iconTracion_Control, 71, 72, colour, BLACK);
			drawString(dashData["TCLevel"], nextpos + CELL_WIDTH, ypos + CELL_HEIGHT  + CELL_HEIGHT +5 , 1, gfx); // Data
		}
		nextpos = nextpos + (CELL_WIDTH*2 );
		gfx->setTextColor(RGB565_DDGREY, RGB565_DDGREY);
		colour = RGB565_DDGREY;
		/*	if (dashData["tcTcCut"] != NULL)
				{
					colour = setColor(ORANGE, dashData["tcTcCut"], RGB565_DDGREY);
					//	gfx->drawBitmap(nextpos, (ROWS - 3) * CELL_HEIGHT,iconToggle_Wiper,72, 72,RED, BLACK);nextpos = nextpos + 74;
					gfx->setTextColor(colour); // IsInPitLane
				}
				gfx->drawBitmap(nextpos, CELL_HEIGHT, iconTracion_Control, 71, 72, colour, BLACK);
				{
					drawCentreCentreString(dashData["tcTcCut"], nextpos + HALF_CELL_WIDTH, CELL_HEIGHT + CELL_HEIGHT + HALF_CELL_HEIGHT, 2, gfx); // Data
					nextpos = nextpos + CELL_WIDTH;
				}

				*/
		gfx->setTextColor(RGB565_DDGREY, RGB565_DDGREY);
		colour = BLUE;
		if (dashData["EngineMap"] != NULL)
		{
			colour = setColor(BLUE, dashData["EngineMap"], RGB565_DDGREY);
			gfx->setTextColor(colour); // IsInPitLane
			gfx->drawBitmap(nextpos, ypos, iconEngine_Map_plus, 71, 72, colour, BLACK);

			drawCentreCentreString(dashData["EngineMap"], nextpos + HALF_CELL_WIDTH, ypos  + CELL_HEIGHT + CELL_HEIGHT +5, 1, gfx); // Data
			nextpos = nextpos + (CELL_WIDTH * 2);
		}

	}




	uint16_t setColor(uint16_t c, String data, uint16_t b = DARKGREY, int invert = 0)
	{
		// RGB565_DDGREY

		//	dashData[]
		//	return data.toInt() > 0 && dashData[data] ? c : DARKGREY;
		if (invert == 1)
			return data.toInt() != 0 ? b : c;
		return data.toInt() > 0 ? c : b;
	}
	int32_t temp(String temp)
	{
		if (temp.toInt() < 1)
			return (BLACK);
		if (temp.toInt() < 40)
			return (BLUE);
		if (temp.toInt() < 60)
			return (CYAN);
		if (temp.toInt() < 90)
			return (GREEN);
		if (temp.toInt() < 100)
			return (YELLOW);
		if (temp.toInt() < 110)
			return (ORANGE);
		if (temp.toInt() < 210)
			return (RED);
		if (temp.toInt() < 300)
			return (CYAN);
		if (temp.toInt() < 400)
			return (GREEN);
		if (temp.toInt() < 800)
			return (YELLOW);
		if (temp.toInt() < 900)
			return (ORANGE);
		if (temp.toInt() > 1000)
			return (RED);
	}
	int32_t flag()
	{

		int32_t flag = RGB565_DDGREY;

		if (dashData["LapInvalidated"] =1)
		flag = RGB565_vDGREY;

		if (dashData["Flag_Checkered"]!= 0)
			flag = DARKGREEN;
		if (dashData["Flag_Black"]!= 0)
			flag = RED;
		if (dashData["Flag_Blue"]!= 0)
			flag = BLUE;
		if (dashData["Flag_White"]!= 0)
			flag = WHITE;
		if (dashData["Flag_Yellow"]!= 0)
			flag = YELLOW;
		if (dashData["IsInPitLane"]!= 0)
			flag = PURPLE;
		if (dashData["IsInPitLane"]!= 0)
			flag = PURPLE;

		return (flag);
	}

	void element(int x, int y, int ewidth, int eheight, String data, String name = "Data", int32_t color = WHITE, int fill = 0, int oversize = 0, String pre = "", String post = "")
	{
		if (data)
			element2(x, y, ewidth, eheight, name, color, fill, oversize, pre, post);
	}

	void itembox(int x, int y, int ewidth, int eheight, int32_t color = WHITE)
	{
		x = (CELL_WIDTH)*x;	 //-1;
		y = (CELL_HEIGHT)*y; //-1;
							 // if(x ==0)

		int x2 = CELL_WIDTH * ewidth;
		int y2 = CELL_HEIGHT * eheight; //- 1;
		gfx->drawRect(x, y, x2, y2, color);
	}

void out(String stuff)
{
	
	drawString( "\n " + dashData["CurrentDateTime"], 20, SCREEN_HEIGHT - 20, 1, gfx);

}
	void element2(int x, int y, int ewidth, int eheight, String name = "Data", int32_t color = WHITE, int fill = 0, int oversize = 0, String pre = "", String post = "", int maxlen = 2)
	{
		// if(!dashData[name]) return ;
		// if (dashData[name])
		String data = dashData[name];
		{

			if (dashData[name] == lastData[name])
			{
				// return;
			}

			int xPadding = 10;
			const static int yPadding = 0; // 5;
			x = (CELL_WIDTH)*x;			   //-1;
			y = (CELL_HEIGHT)*y;		   //-1;
										   // if(x ==0)

			int x2 = CELL_WIDTH * (ewidth);
			int y2 = CELL_HEIGHT * (eheight); //- 1;
			int multiplier = 1;
			int xcenter = (ceil(x2)) - CELL_WIDTH; //-xPadding ;
			int ycenter = (ceil(y2)) - CELL_HEIGHT;
			//	gfx->drawRect(x + 1, y + 1, x2 - 1, y2 - 1, color); // Rectangle
			// const bool dataChanged = (prevData[name] != data);
			// const bool colorChanged = (prevColor[name] != color);
			//  x+=10;
			if (fill == 0)
			{
				// if (colorChanged)
				{
					//	gfx->fillRect(x + 1, y + 1, x2, y2 - 4, BLACK);
				}
				//
			}

			if (dashData[name] != lastData[name])
			{
				gfx->setTextColor(color, RGB565_DDGREY);
				gfx->fillRect(x + 1, y + 1, x2 - 1, y2 - 1, RGB565_DDGREY);
			}
			if (fill == 1)
			{
				if (dashData[name] != lastData[name])
					gfx->fillRect(x + 1, y + 1, x2 - 1, y2 - 1, color);
				gfx->setTextColor(RGB565_DDGREY, color);
			}

			if (oversize > 2)
			{

				//	ycenter = ycenter - ((oversize * 2 - yPadding) * 2) - HALF_CELL_HEIGHT;
				//	xcenter = xcenter + (xPadding * 3);
				// int xPadding = 15;
			}
			int fontSize = ceil(titlefont * eheight) + oversize;

			const static int titleAreaHeight = yPadding - cellTitleHeight + (8 * eheight);

			// if (colorChanged )
			// if(fill != 0)
			// if (dataChanged)
			//	if(!tickcount%1)
			{

				// titleAreaHeight= titleAreaHeight*eheight;
				if (fill == 1)
				{
					// if (colorChanged)
					{
						gfx->fillRect(x + 2, y + 2, x2 - 2, y2 - 2, color);
					} // Rectangle
				}
				if (fill == 2)
				{
					// if (colorChanged)
					//{gfx->fillRect(x+1, y+1, x2, y2,  color);	} // Rectangle
				}
				if (fill == 3)
				{
					// if (colorChanged)
					{
						gfx->fillRect(x +2, y +2, x2-2 , y2-2, color);
					} // Rectangle
				}

				// title
				// if(name !="Data") drawString(name, x + xPadding, y + yPadding, titlefont, gfx);			 // Title
				if (fill == 2)
				{
					//	if(name !="Data") drawString(name, x + xPadding, y + yPadding, titlefont, gfx);
				}
				if (fill == 1)
				{
					//	if (name != "Data")		drawString(name, x + xPadding, y + yPadding, titlefont, gfx);
				}
				if (fill == 0) // outline with label
				{
					gfx->setTextColor(color, BLACK);
					// drawCentreString(name, x + (xcenter), y - yPadding, titlefont, gfx); // Title
				}
				// content
			}
			//
			// if (dataChanged)
			{
				gfx->setTextColor(BLACK, BLACK);
				if (fill == 1)
				{
					gfx->setTextColor(color, color);
				}
				// drawCentreString(prevData[name], x + (xcenter), y - yPadding, titlefont, gfx); // Title
				// drawCentreCentreString(prevData[name], x + (xcenter), y + ycenter, fontSize, gfx);
				gfx->setTextColor(color, BLACK);
				if (fill == 1)
				{
					gfx->setTextColor(BLACK, color);
				}
				if (fill == 3)
				{
					gfx->setTextColor(BLACK, color);
				}

				int ypad = HALF_CELL_HEIGHT;
				gfx->setFont(u8g2_font_10x20_mf);
				drawString(pre, x + 10, y + 21, 1, gfx);
				gfx->setFont(u8g2_font_spleen16x32_mn);
				if (eheight > 3)
				{
					gfx->setFont(u8g2_font_spleen16x32_mn);
					ypad = HALF_CELL_HEIGHT;
					// HALF_CELL_WIDTH
					drawRightString(data + post, x + (ewidth * (CELL_WIDTH)) - (4), (y + ((CELL_HEIGHT)*eheight)) - (ypad), 2,gfx); // Data
				}
				else
				{
					// HALF_CELL_WIDTH
					drawRightString(data + post, x + (ewidth * (CELL_WIDTH)) - (8), (y + ((CELL_HEIGHT)*eheight)) - (8), eheight-1 , gfx); // Data
				}
				gfx->setFont(u8g2_font_10x20_mf); // drawCentreString(data, x + (x - x2) , y + ycenter, fontSize, gfx); // Data
			}
			gfx->setTextColor(WHITE);
			lastData[name] = dashData[name];
		}
		gfx->setFont(u8g2_font_10x20_mf);
	}
	
};

#endif
