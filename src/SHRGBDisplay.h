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
static const int ROWS = 13;
static const int COLS = 11;
static const int titlefont = 2;
static const int itemfont = 2;
static const int CELL_WIDTH = 72; // SCREEN_WIDTH / COLS;
static const int HALF_CELL_WIDTH = CELL_WIDTH / 2;
static const int CELL_HEIGHT = 36; // SCREEN_HEIGHT / ROWS;
static const int HALF_CELL_HEIGHT = CELL_HEIGHT / 2;
static const int COL[] = {0, CELL_WIDTH, CELL_WIDTH * 2, CELL_WIDTH * 3, CELL_WIDTH * 4, CELL_WIDTH * 5, CELL_WIDTH * 6, CELL_WIDTH * 7, CELL_WIDTH * 8};
static const int ROW[] = {0, CELL_HEIGHT, CELL_HEIGHT * 2, CELL_HEIGHT * 3, CELL_HEIGHT * 4, CELL_HEIGHT * 5, CELL_HEIGHT * 6, CELL_HEIGHT * 7, CELL_HEIGHT * 8};

#include <GFXHelpers.h>

std::map<String, String> liveData;
std::map<String, String> dashData;

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
		terminalPrintln("INIT Dash", gfx);
		//	init_past();
		// gfx->begin();

#ifdef TFT_BL
		pinMode(TFT_BL, OUTPUT);
		digitalWrite(TFT_BL, HIGH);

#endif

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
		delayMicroseconds(10000);
		// render_dash();

		// gfx->fillRoundRect(0, SCREEN_HEIGHT/3, SCREEN_WIDTH, SCREEN_HEIGHT/3 + 20, 5, WHITE);

		gfx->setTextColor(RGB565(0xff, 0x66, 0x00));
		drawString(DEVICE_NAME, 20, SCREEN_HEIGHT -60, 2, gfx);
		gfx->setTextColor(YELLOW, BLACK);
		drawString(getUniqueId(), 20,SCREEN_HEIGHT -40 , 2, gfx);

		//
		/*

		*/
		// read("bob");
		read();
	}
	String set_value(String data = "")
	{

		//	String data = FlowSerialReadStringUntil(';');
		int split = data.indexOf(":");
		String key = data.substring(0, split);
		if (key > "")
		{
			dashData[key] = data.substring(split + 1);
			// LiveData[name] = "0";

			return (dashData[key]);
		}
		return data;
	}

	void read(String stuff = "")
	{
		// dashData=liveData;
		String stuff2 = FlowSerialReadStringUntil('\n');
	//	if (!stuff2.length() && !hasReceivedData)
	//		stuff2 = "SpeedKmh:000;Gear:-;CarSettings_CurrentDisplayedRPMPercent:0;fuelp:69;CarSettings_RPMRedLineSetting:0;Rpms:0;LapInvalidated:0;CurrentLapTime:-;LastLapTime:-;BestLapTime:-;CompactDelta:-;SessionBestLiveDeltaProgressSeconds:0.00;CompletedLaps:0;Position:1;TCLevel:0;TCActive:1;ABSLevel:0;ABSActive:0;BrakeBias:0;Fuel:0;EngineMap:0;Fuel_RemainingLaps:-;Fuel_CurrentLapConsumption:-;IsInPitLane:1;PitLimiterOn:1;TyrePressureFrontLeft:00.0;TyrePressureFrontRight:00.0;TyrePressureRearLeft:00.0;TyrePressureRearRight:00.0;BrakeTemperatureFrontLeft:9000;BrakeTemperatureFrontRight:700;BrakeTemperatureRearLeft:600;BrakeTemperatureRearRight:800;TyreTemperatureFrontLeft:70;TyreTemperatureFrontRight:70;TyreTemperatureRearLeft:70;TyreTemperatureRearRight:1000;CarSettings_RPMShiftLight1:0;CarSettings_RPMShiftLight2:0;Flag_Checkered:0;Flag_Black:0;Flag_Blue:0;Flag_White:0;Flag_Yellow:1;mHeadlights:0;CurrentDateTime:******************;";
		int index = 1;
		int slen = 50;
		slen = stuff2.lastIndexOf(";");
		int lsplit = 0;
		int i;
		String key;
		String value;
		int split;
		int row = 10;

		for (int i = 0; i < slen - 1; i++)
		{
			row++;
			split = stuff2.indexOf(";", i);
			// if(i != split)
			{
				key = stuff2.substring(i, split);
			}
			if (i < split)
			{
				set_value(key);
				i = split;
			}
		}

		if (dashData["CurrentDateTime"] > "")
		{
			hasReceivedData = true;
			render_dash();
		}
	}

	// Called once per arduino loop, timing can't be predicted,
	// but it's called between each command sent to the arduino
	void loop()
	{

		//	gfx->setTextColor(MAROON, RGB565_DDGREY); // IsInPitLane
		//	drawString(dashData["CurrentDateTime"], CELL_WIDTH * 7.5, ROWS * CELL_HEIGHT - HALF_CELL_HEIGHT, 2, gfx);
		//	render_dash();
	}

	void idle()
	{
		//	loop();
	}

	void render_dash()
	{
		// do_string("bob");
		int nextpos = 0;

		gfx->setTextColor(MAROON, RGB565_DDGREY);
		//
		if (dashData["gear"] != "0")

		{
			gfx->setTextColor(DARKGREEN, RGB565_DDGREY); // IsInPitLane
			if (dashData["CarSettings_RPMShiftLight2"].toInt() >= 1)
				gfx->setTextColor(MAROON, RGB565_DDGREY); // IsInPitLane

			//
			if (!drawbackground)
			{
				gfx->fillRect(0, CELL_HEIGHT * 3, CELL_WIDTH * 103, CELL_HEIGHT * 7, RGB565_DDGREY);
				drawbackground = true;
				gfx->fillScreen(RGB565_DDGREY);
			}
			gfx->fillRect(CELL_WIDTH * 4, CELL_HEIGHT * 9, CELL_WIDTH * 3, CELL_HEIGHT * 2, flag());
			//	gfx->fillRect(CELL_WIDTH * 7, CELL_HEIGHT * 3, HALF_CELL_WIDTH * 1, CELL_HEIGHT * 7, flag());
			gfx->drawRect(CELL_WIDTH * 4 + 1, CELL_HEIGHT * 3, CELL_WIDTH * 3, CELL_HEIGHT * 6, BLUE);
			drawCentreCentreString(dashData["Gear"], SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 20, gfx); // Data
			// if (prev_gear != dashData["Gear"])
			{

				// gfx->fillRect(CELL_WIDTH * 4, CELL_HEIGHT * 3, CELL_WIDTH * 4, CELL_HEIGHT * 7, RGB565_DDGREY);
				//	gfx->drawRect(CELL_WIDTH *4 , CELL_HEIGHT*3,  CELL_WIDTH * 4 ,CELL_HEIGHT * 7, DARKGREY);
				//	gfx->fillRect(CELL_WIDTH * 3 - 5, CELL_HEIGHT * 6, 6, CELL_HEIGHT * 2, BLACK);
			}
		}

		// if (dashData["Rpms"].toInt()>0)
		{
			gfx->fillRect((CELL_WIDTH * COLS / 100) * dashData["CarSettings_CurrentDisplayedRPMPercent"].toInt(), 1, CELL_WIDTH * COLS, CELL_HEIGHT - 2, DARKGREY);
			// gfx->fillRect((SCREEN_WIDTH / 100) * CarSettings_RPMRedLineSetting.toInt() - 5, 1, SCREEN_WIDTH, CELL_HEIGHT - 2, RED);
			gfx->fillRect(1, 1, (CELL_WIDTH * COLS / 100) * dashData["CarSettings_CurrentDisplayedRPMPercent"].toInt(), CELL_HEIGHT - 2, DARKGREEN);
			//	(dashData["gCurrentDisplayedRPMPercentear"] = "0");
			/*if(dashData["CarSettings_RPMShiftLight2"].toInt())
			{			gfx->drawCircle(CELL_WIDTH * 6 + HALF_CELL_WIDTH, CELL_HEIGHT * 2, 31, DARKGREY);
						gfx->drawCircle(CELL_WIDTH * 4 + HALF_CELL_WIDTH, CELL_HEIGHT * 2, 31, DARKGREY);
						gfx->drawCircle(CELL_WIDTH * 6 + HALF_CELL_WIDTH, CELL_HEIGHT * 2, 31, DARKGREY);
						gfx->fillCircle(CELL_WIDTH * 6 + HALF_CELL_WIDTH, CELL_HEIGHT * 2, 30, round(dashData["CarSettings_RPMShiftLight2"].toInt()) ? MAGENTA : RGB565_DDGREY);
						gfx->fillCircle(CELL_WIDTH * 4 + HALF_CELL_WIDTH, CELL_HEIGHT * 2, 30, round(dashData["CarSettings_RPMShiftLight1"].toInt()) ? MAGENTA : RGB565_DDGREY);

						gfx->fillCircle(CELL_WIDTH * 7 + HALF_CELL_WIDTH, CELL_HEIGHT * 2, 30, round(dashData["CarSettings_RPMShiftLight1"].toInt()) ? MAGENTA : RGB565_DDGREY);
						gfx->fillCircle(CELL_WIDTH * 4 + HALF_CELL_WIDTH, CELL_HEIGHT * 2, 30, round(dashData["CarSettings_RPMShiftLight2"].toInt()) ? MAGENTA : RGB565_DDGREY);

			}*/
			element(0, 9, 4, 2, dashData["Rpms"], "Rpms", LIGHTGREY, 0, 0, "RPM");

			// if (dashData["SpeedKmh"].toInt() >0)
			element(0, 5, 4, 4, dashData["SpeedKmh"], "SpeedKmh", WHITE, 0, 0, "KPH");
		}

		if (dashData["CompactDelta"].length())
		{
			element(COLS - 2, 1, 2, 2, dashData["CompactDelta"], "CompactDelta", dashData["CompactDelta"].indexOf('-') >= 0 ? GREEN : RED, 3, 0, "Delta");
		}

		if (dashData["CurrentLapTime"].length())
			element(COLS - 4, 3, 4, 2, dashData["CurrentLapTime"], "CurrentLapTime", dashData["LapInvalidated"] == "True" ? RED : WHITE, 0, 0, "Current");

		if (dashData["LastLapTime"].length())
		{
			element(COLS - 4, 5, 4, 2, dashData["LastLapTime"], "LastLapTime", YELLOW, 0, 0, "Last");
			element(COLS - 4, 7, 4, 2, dashData["BestLapTime"], "BestLapTime", MAGENTA, 0, 0, "Best");
		}
		// drawCell(COL[0], ROW[0], , "sessionBestLiveDeltaSeconds", "Delta", "double", sessionBestLiveDeltaSeconds.indexOf('-') >= 0 ? GREEN : RED);

		//	std::string str = sessionBestLiveDeltaProgressSeconds;

		// if (LiveData["sessionBestLiveDeltaProgressSeconds"] == "1")
		//	element(COLS - 2, 8, 2, 1, sessionBestLiveDeltaProgressSeconds, "Delta Pogress", sessionBestLiveDeltaProgressSeconds.indexOf('-') >= 0 ? GREEN : RED, 0);
		if (dashData["Position"].length())
			element(0, 3, 2, 2, dashData["Position"], "Position", WHITE, 0, 0, "Pos");
		if (dashData["CompletedLaps"].length())
			element(2, 3, 2, 2, dashData["CompletedLaps"], "CompletedLaps", WHITE, 0, 0, "Lap", "");

		// fuel
		// if( dashData["fuelp"].toInt())
		{
			int hpos = HALF_CELL_WIDTH;
			hpos = (COLS - 6) * (CELL_WIDTH +HALF_CELL_WIDTH);
			int vpos = (ROWS - 4) * (CELL_HEIGHT  )+ HALF_CELL_HEIGHT;;
			double fperc = (dashData["fuelp"].toFloat() / 100) * (HALF_CELL_WIDTH*3);
			gfx->drawRect(hpos - HALF_CELL_WIDTH, vpos - HALF_CELL_HEIGHT, CELL_WIDTH * 2, CELL_HEIGHT * 4 +5, WHITE);
			//drawString("Fuel", hpos - 30, vpos - 15, 2, gfx);						   // Data
			drawString("Fuel: "+dashData["Fuel"] + "L", hpos -10, vpos + HALF_CELL_WIDTH - 20, 2, gfx); // Data
			//hpos = hpos + 72;
			 vpos = (ROWS - 2) * CELL_HEIGHT + HALF_CELL_HEIGHT;
			gfx->fillRect(hpos -HALF_CELL_HEIGHT, vpos, (HALF_CELL_WIDTH*3), CELL_HEIGHT, DARKGREY);
			gfx->fillRect(hpos -HALF_CELL_HEIGHT, vpos, fperc, CELL_HEIGHT, DARKGREEN);
			hpos = hpos + 144;
			hpos = hpos + HALF_CELL_WIDTH;
			// drawString( dashData["fuelp"] +"%", hpos, vpos +HALF_CELL_HEIGHT , 2, gfx); // Data
		}

		
		int tix;
		int tiy;
		tix = (COLS - 2) * CELL_WIDTH;
		tiy = (ROWS - 4) * CELL_HEIGHT + HALF_CELL_HEIGHT;
		gfx->drawRect(tix, tiy - HALF_CELL_HEIGHT, CELL_WIDTH * 2, (CELL_HEIGHT * 4) + 5, WHITE);
		tix = (tix + HALF_CELL_WIDTH) - 5;
		tiy = tiy + HALF_CELL_HEIGHT;
		drawCentreCentreString(dashData["TyrePressureFrontLeft"], tix + 15, tiy - HALF_CELL_HEIGHT, 2, gfx);
		gfx->fillRect(tix + 0, tiy, 15, CELL_HEIGHT, temp(dashData["TyreTemperatureFrontLeft"]));
		gfx->fillRect(tix + 15, tiy + (HALF_CELL_HEIGHT / 2), 10, HALF_CELL_HEIGHT, temp(dashData["BrakeTemperatureFrontLeft"]));

		tiy = tiy + CELL_HEIGHT + CELL_HEIGHT;
		drawCentreCentreString(dashData["TyrePressureRearLeft"], tix + 15, tiy - HALF_CELL_HEIGHT, 2, gfx);
		gfx->fillRect(tix + 0, tiy, 15, CELL_HEIGHT, temp(dashData["TyreTemperatureRearLeft"]));
		gfx->fillRect(tix + 15, tiy + (HALF_CELL_HEIGHT / 2), 10, HALF_CELL_HEIGHT, temp(dashData["BrakeTemperatureRearLeft"]));

		tix = (COLS - 1) * CELL_WIDTH;
		tiy = ((ROWS - 4) * CELL_HEIGHT) + HALF_CELL_HEIGHT;
		tix = tix + HALF_CELL_HEIGHT;
		tiy = tiy + HALF_CELL_HEIGHT;

		drawCentreCentreString(dashData["TyrePressureFrontRight"], tix + 15, tiy - HALF_CELL_HEIGHT, 2, gfx);
		gfx->fillRect(tix - 0, tiy + (HALF_CELL_HEIGHT / 2), 10, HALF_CELL_HEIGHT, temp(dashData["BrakeTemperatureFrontRight"]));
		gfx->fillRect(tix + 10, tiy, 15, CELL_HEIGHT, temp(dashData["TyreTemperatureFrontRight"]));

		tiy = tiy + CELL_HEIGHT + CELL_HEIGHT;
		drawCentreCentreString(dashData["TyrePressureRearRight"], tix + 15, tiy - HALF_CELL_HEIGHT, 2, gfx);
		gfx->fillRect(tix, tiy + (HALF_CELL_HEIGHT / 2), 10, HALF_CELL_HEIGHT, temp(dashData["BrakeTemperatureRearRight"]));

		gfx->fillRect(tix + 10, tiy, 15, CELL_HEIGHT, temp(dashData["TyreTemperatureRearRight"]));


		uint16_t colour;
		nextpos = 0;
		if (dashData["ABSLevel"])
		{
			colour = setColor(ORANGE, dashData["ABSLevel"], RGB565_DDGREY);
			gfx->setTextColor(colour, BLACK);
			gfx->drawBitmap(nextpos, CELL_HEIGHT, iconABS_plus, 72, 72, colour, BLACK);
			drawCentreCentreString(dashData["ABSLevel"], nextpos + HALF_CELL_WIDTH, CELL_HEIGHT + CELL_HEIGHT + HALF_CELL_HEIGHT, 2, gfx); // Data
			nextpos = nextpos + CELL_WIDTH;
		}

		if (dashData["BrakeBias"])
		{
			colour = setColor(RED, dashData["BrakeBias"], RGB565_DDGREY);
			gfx->setTextColor(colour, BLACK);
			gfx->drawBitmap(nextpos, CELL_HEIGHT, iconBrake_Balance_Front_plus, 71, 72, colour, BLACK);
			drawCentreCentreString(dashData["BrakeBias"], nextpos + HALF_CELL_WIDTH, CELL_HEIGHT + CELL_HEIGHT + HALF_CELL_HEIGHT, 2, gfx); // Data
			nextpos = nextpos + CELL_WIDTH;
		}
		if (dashData["TCLevel"])
		{
			colour = setColor(ORANGE, dashData["TCLevel"], RGB565_DDGREY);
			gfx->setTextColor(colour, BLACK);
			gfx->drawBitmap(nextpos, CELL_HEIGHT, iconTracion_Control, 71, 72, colour, BLACK);
			drawCentreCentreString(dashData["TCLevel"], nextpos + HALF_CELL_WIDTH, CELL_HEIGHT + CELL_HEIGHT + HALF_CELL_HEIGHT, 2, gfx); // Data
			nextpos = nextpos + CELL_WIDTH;
		}

		if (dashData["tcTcCut"].length())
		{
			colour = setColor(ORANGE, dashData["tcTcCut"], RGB565_DDGREY);
			//	gfx->drawBitmap(nextpos, (ROWS - 3) * CELL_HEIGHT,iconToggle_Wiper,72, 72,RED, BLACK);nextpos = nextpos + 74;
			gfx->setTextColor(colour); // IsInPitLane
			gfx->drawBitmap(nextpos, CELL_HEIGHT, iconTracion_Control, 71, 72, colour, BLACK);
			drawCentreCentreString(dashData["tcTcCut"], nextpos + HALF_CELL_WIDTH, CELL_HEIGHT + CELL_HEIGHT + HALF_CELL_HEIGHT, 2, gfx); // Data
			nextpos = nextpos + CELL_WIDTH;
		}
		if (dashData["EngineMap"])
		{
			colour = setColor(BLUE, dashData["EngineMap"], RGB565_DDGREY);
			gfx->setTextColor(colour); // IsInPitLane
			gfx->drawBitmap(nextpos, CELL_HEIGHT, iconEngine_Map_plus, 71, 72, colour, BLACK);
			drawCentreCentreString(dashData["EngineMap"], nextpos + HALF_CELL_WIDTH, CELL_HEIGHT + CELL_HEIGHT + HALF_CELL_HEIGHT, 2, gfx); // Data
			nextpos = nextpos + CELL_WIDTH;
		}

		// nextpos = nextpos + 72;
		// nextpos = nextpos + 72;
		// nextpos = nextpos + 72;

		// if (LiveData["gear"] == "1")
		{
			// nextpos = 0;

			//	if (dashData["TCActive"])
			{

				gfx->drawBitmap(nextpos, CELL_HEIGHT, epd_bitmap_Stability_Control, 72, 72, setColor(ORANGE, dashData["TCActive"], RGB565_DDGREY, 1), setColor(ORANGE, dashData["TCActive"], RGB565_DDGREY));
				nextpos = nextpos + CELL_WIDTH;
			}

			if (dashData["ABSActive"])
			{
				gfx->drawBitmap(nextpos, CELL_HEIGHT, epd_bitmap_ABS, 72, 72, setColor(YELLOW, dashData["ABSActive"], RGB565_DDGREY, 1), setColor(YELLOW, dashData["ABSActive"], RGB565_DDGREY));
				nextpos = nextpos + CELL_WIDTH;
			}

			// TCActive.toInt() == 1 ? RED : RGB565_DDGREY);
			if (dashData["brake"].length())
			{
				gfx->drawBitmap(nextpos, CELL_HEIGHT, epd_bitmap_Hand_Brake, 72, 72, setColor(ORANGE, dashData["brake"], RGB565_DDGREY));
				nextpos = nextpos + CELL_WIDTH;
			}

			if (dashData["PitLimiterOn"].length())
			{
				gfx->drawBitmap(nextpos, CELL_HEIGHT, epd_bitmap_Pit_Limiter, 72, 72,
								setColor(PURPLE, dashData["PitLimiterOn"], RGB565_DDGREY, 1), setColor(PURPLE, dashData["PitLimiterOn"], RGB565_DDGREY));
				nextpos = nextpos + CELL_WIDTH;
			}

			// if (dashData["lights"].length())
			{
				gfx->drawBitmap(nextpos, CELL_HEIGHT, epd_bitmap_Lights, 72, 72, setColor(PURPLE, dashData["lights"], RGB565_DDGREY));
				nextpos = nextpos + CELL_WIDTH;
			}
			// if (dashData["wipers"].length())
			{
				gfx->drawBitmap(nextpos, CELL_HEIGHT, epd_bitmap_Toggle_Wiper, 72, 72, setColor(GREEN, dashData["wipers"], RGB565_DDGREY));
				// gfx->drawBitmap(144,1,epd_bitmap_Ignition,72,72,ignition == "1" ? RED : DARKGREY);
				nextpos = nextpos + CELL_WIDTH;
			}

			// drawBitmap(1,1,epd_bitmap_Toggle_Wiper,72,72,RED,BLACK);
			/*
					element(1,1,1,1,absLevel,"ABS",RED,0);
					element(4,1,3,2,absLevel ,"HAT",GREEN,1);
			*/
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
		if (temp.toInt() < 70)
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
		if (temp.toInt() < 600)
			return (GREEN);
		if (temp.toInt() < 800)
			return (YELLOW);
		if (temp.toInt() < 800)
			return (ORANGE);
		if (temp.toInt() > 1100)
			return (RED);
	}
	int32_t flag()
	{

		int32_t flag = RGB565_DDGREY;
		if (dashData["Flag_Checkered"].toInt())
			flag = DARKGREEN;
		if (dashData["Flag_Black"].toInt())
			flag = RED;
		if (dashData["Flag_Blue"].toInt())
			flag = BLUE;
		if (dashData["Flag_White"].toInt())
			flag = WHITE;
		if (dashData["Flag_Yellow"].toInt())
			flag = YELLOW;
		if (dashData["IsInPitLane"].toInt())
			flag = PURPLE;
		return (flag);
	}
	void element(int x, int y, int ewidth, int eheight, String data, String name = "Data", int32_t color = WHITE, int fill = 0, int oversize = 0, String pre = "", String post = "")
	{

		if (data.length())
		{
			int xPadding = 10;
			const static int yPadding = 0; // 5;

			// const bool dataChanged = (prevData[name] != data);
			// const bool colorChanged = (prevColor[name] != color);
			//  x+=10;
			if (dashData[name])
				data = dashData[name];
			gfx->setTextColor(color, BLACK);
			if (fill == 1)
			{
				gfx->setTextColor(BLACK, color);
			}

			x = CELL_WIDTH * x;	 //-1;
			y = CELL_HEIGHT * y; //-1;
								 // if(x ==0)

			int x2 = CELL_WIDTH * ewidth;
			int y2 = CELL_HEIGHT * eheight; //- 1;
			int multiplier = 1;
			int xcenter = ceil(x2) / 2; //-xPadding ;
			int ycenter = ceil(y2) / 2;

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
						gfx->fillRect(x + 1, y + 1, x2 - 1, y2 - 1, color);
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
						gfx->fillRect(x + 1, y + 1, x2 - 1, y2 - 1, color);
					} // Rectangle
				}
				if (fill == 0)
				{
					// if (colorChanged)
					{
						//	gfx->fillRect(x + 1, y + 1, x2, y2 - 4, BLACK);
					}
					//
					gfx->drawRect(x + 1, y + 1, x2 - 1, y2 - 1, color); // Rectangle
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
				drawString(pre, x + 5, y + 5, 2, gfx);												// Data
				drawCentreCentreString(data + post, x + (xcenter), y + ycenter + 5, fontSize, gfx); // Data
																									// drawCentreString(data, x + (x - x2) , y + ycenter, fontSize, gfx); // Data
			}
		}
	}
};

#endif
