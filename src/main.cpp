/*
 Test the tft.print() viz. the libraries embedded write() function

 This sketch used font 2, 4, 7

 Make sure all the required fonts are loaded by editing the
 User_Setup.h file in the TFT_eSPI library folder.

  #########################################################################
  ###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
  #########################################################################
 */

#include <SPI.h>
#include <TFT_eSPI.h>  // Hardware-specific library

TFT_eSPI tft = TFT_eSPI();	// Invoke custom library

#define TFT_GREY 0x5AEB	 // New colour

void setup(void) {
	tft.init();
	tft.setRotation(2);
}

void loop() {
}
