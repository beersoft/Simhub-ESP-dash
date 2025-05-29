#define MESSAGE_HEADER 0x03

void Command_Hello() {
	FlowSerialTimedRead();
	delay(10);
	FlowSerialPrint(VERSION);
	FlowSerialFlush();
}

void Command_SetBaudrate() {
	SetBaudrate();
}

void Command_ButtonsCount() {
	FlowSerialWrite(0);
	FlowSerialFlush();
}

void Command_TM1638Count() {
	FlowSerialWrite((byte)(0));
	FlowSerialFlush();
}

void Command_SimpleModulesCount() {
	FlowSerialWrite(0);
	FlowSerialFlush();
}

void Command_Acq() {
	FlowSerialWrite(0x03);
	FlowSerialFlush();
}

void Command_DeviceName() {
	FlowSerialPrint(DEVICE_NAME);
	FlowSerialPrint("\n");
	FlowSerialFlush();
}

void Command_UniqueId() {
	auto id = getUniqueId();
	FlowSerialPrint(id);
	FlowSerialPrint("\n");
	FlowSerialFlush();
}

void Command_MCUType() {
	FlowSerialPrint(SIGNATURE_0);
	FlowSerialPrint(SIGNATURE_1);
	FlowSerialPrint(SIGNATURE_2);
	FlowSerialFlush();
}
void Command_RGBLEDSCount()
{
	FlowSerialWrite(0);
	FlowSerialFlush();
}

void Command_EncodersCount() {
#ifdef INCLUDE_ENCODERS
	FlowSerialWrite(ENABLED_ENCODERS_COUNT);
#endif
}

void Command_SpeedoData() {
#ifdef INCLUDE_SPEEDOGAUGE
	speedoTonePin.readFromString();
#endif
}

void Command_TachData() {
#ifdef INCLUDE_TACHOMETER
	rpmTonePin.readFromString();
#endif
}

void Command_BoostData() {
#ifdef INCLUDE_BOOSTGAUGE
	shBOOSTPIN.readFromString();
#endif
}

void Command_TempData() {
#ifdef INCLUDE_TEMPGAUGE
	shTEMPPIN.readFromString();
#endif
}

void Command_ConsData() {
#ifdef INCLUDE_CONSGAUGE
	shCONSPIN.readFromString();
#endif
}

void Command_FuelData() {
#ifdef INCLUDE_FUELGAUGE
	shFUELPIN.readFromString();
#endif
}

void Command_GLCDData() {
#ifdef INCLUDE_OLED
	shGLCD.read();
#endif 
#ifdef INCLUDE_NOKIALCD
	shNOKIA.read();
#endif 
}

void Command_ExpandedCommandsList() {
#ifdef INCLUDE_SPEEDOGAUGE
	FlowSerialPrintLn("speedo");
#endif
#ifdef INCLUDE_TACHOMETER
	FlowSerialPrintLn("tachometer");
#endif
#ifdef INCLUDE_BOOSTGAUGE
	FlowSerialPrintLn("boostgauge");
#endif
#ifdef INCLUDE_TEMPGAUGE
	FlowSerialPrintLn("tempgauge");
#endif
#ifdef INCLUDE_FUELGAUGE
	FlowSerialPrintLn("fuelgauge");
#endif
#ifdef INCLUDE_CONSGAUGE
	FlowSerialPrintLn("consumptiongauge");
#endif
#ifdef INCLUDE_DM163_MATRIX
	FlowSerialPrintLn("dm163rgb");
#endif
#ifdef INCLUDE_ENCODERS
	FlowSerialPrintLn("encoders");
#endif
	FlowSerialPrintLn("mcutype");
	FlowSerialPrintLn("keepalive");
	FlowSerialPrintLn();
	FlowSerialFlush();
}

void Command_TM1638Data() {
#ifdef INCLUDE_TM1638
	// TM1638
	for (int j = 0; j < TM1638_ENABLEDMODULES; j++) {
		// Wait for display data
		int newIntensity = FlowSerialTimedRead();
		if (newIntensity != TM1638_screens[j]->Intensity) {
			TM1638_screens[j]->Screen->setupDisplay(true, newIntensity);
			TM1638_screens[j]->Intensity = newIntensity;
		}

		TM1638_SetDisplayFromSerial(TM1638_screens[j]);
	}
#endif
}

void Command_Features() {
	delay(10);


	// Gear
	FlowSerialPrint("G");

	// Name
	FlowSerialPrint("N");

	// UniqueID
	FlowSerialPrint("I");

	// Additional buttons
	FlowSerialPrint("J");

	// Custom Protocol Support
	FlowSerialPrint("P");

	// Xpanded support
	FlowSerialPrint("X");

	// RGB MATRIX
#if defined(INCLUDE_WS2812B_MATRIX) || defined(INCLUDE_DM163_MATRIX) || defined(INCLUDE_SUNFOUNDERSH104P_MATRIX)
	if (WS2812B_MATRIX_ENABLED > 0 || DM163_MATRIX_ENABLED > 0 || SUNFOUNDERSH104P_MATRIX_ENABLED >0) {
		FlowSerialPrint("R");
	}
#endif

#if defined(INCLUDE_SHAKEIDUALVNH5019) || defined(INCLUDE_SHAKEITADASHIELD) || defined(INCLUDE_SHAKEITDKSHIELD) || defined(INCLUDE_SHAKEITL298N) || defined(INCLUDE_SHAKEITMOTOMONSTER) || defined(INCLUDE_SHAKEITPWM)  || defined(INCLUDE_SHAKEITPWMFANS)
	FlowSerialPrint("V");
#endif 

	FlowSerialPrint("\n");
	FlowSerialFlush();
}

void Command_Shutdown() {
#if	defined(INCLUDE_DM163_MATRIX)
	if (DM163_MATRIX_ENABLED > 0) {
		shRGBLedsDM163.clear();
	}
#endif
#if	defined(INCLUDE_SUNFOUNDERSH104P_MATRIX)
	if (SUNFOUNDERSH104P_MATRIX_ENABLED > 0) {
		shRGBMatrixSunfounderSH104P.clear();
	}
#endif
#if defined(INCLUDE_WS2812B_MATRIX) 
	if (WS2812B_MATRIX_ENABLED > 0) {
		shRGBMatrixWS2812B.clear();
	}
#endif
}







void Command_GearData() {
	char gear = FlowSerialTimedRead();
	SHRGBDisplay.read("Gear"+gear);
#ifdef INCLUDE_74HC595_GEAR_DISPLAY
	if (ENABLE_74HC595_GEAR_DISPLAY == 1) {
		RS_74HC595_SetChar(gear);
	}
#endif

#ifdef INCLUDE_6c595_GEAR_DISPLAY
	if (ENABLE_6C595_GEAR_DISPLAY == 1) {
		RS_6c595_SetChar(gear);
	}
#endif
}

void Command_I2CLCDData() {
#ifdef INCLUDE_I2CLCD
	shI2CLcd.read();
#endif
}
void Command_SHRGBDisplay() {
	SHRGBDisplay.read();
	FlowSerialWrite(0x15);
}
void Command_CustomProtocolData() {
	// if(SHCustomProtocol) {SHCustomProtocol.read();
	// FlowSerialWrite(0x15);}
}
