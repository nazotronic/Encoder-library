/*
 * Library: Encoder
 *
 * Description: Short library to work with encoder.
 * 
 * Author: Vereshchynskyi Nazar
 * Email: verechnazar12@gmail.com
 * Version: 1.0.0
 * Date: 02.07.2025
 */

#include <Arduino.h>
#include <Encoder.h>

#define CLK_PORT D5
#define DT_PORT D6
#define SW_PORT D7

Encoder enc(CLK_PORT, DT_PORT, SW_PORT);

IRAM_ATTR void tickInterrupt() {
	enc.tick();
}

void setup() {
	Serial.begin(9600);

	enc.setEncPortMode(ENC_PORT_INPUT_PULLUP);
	enc.setButPortMode(BUTTON_PORT_INPUT_PULLUP);
	enc.setButInvert(true);

	attachInterrupt(CLK_PORT, tickInterrupt, CHANGE);
	attachInterrupt(DT_PORT, tickInterrupt, CHANGE);
	attachInterrupt(SW_PORT, tickInterrupt, CHANGE);

	/*
	uncomment to see how the buffer works
	*/

	// enc.setMaxTurnsCount(2);
	// Serial.println("Writing...");
	// delay(8000);
}

void loop() {
	if (enc.isPressed() || enc.isTurn()) {
		Serial.println("\n\n\\/ --- NEW ACTION --- \\/");
		/* uncomment to learn this functions */
		// enc.clearButFlags();
		// enc.deleteTurns();
	}

	if (enc.isTurn()) {
		Serial.println("- Is turn");
	}


	if (enc.isLeft()) {
		Serial.println("- Left");
	}

	if (enc.isRight()) {
		Serial.println("- Right");
	}

	if (enc.isLeftH()) {
		Serial.println("- Left holded");
	}

	if (enc.isRightH()) {
		Serial.println("- Right holded");
	}


	if (enc.isClick()) {
		Serial.println("- Click");
	}

	if (enc.isHolded()) {
		Serial.println("- Holded");
	}


	if (enc.isSingle()) {
		Serial.println("- Single");
	}

	if (enc.isDouble()) {
		Serial.println("- Double");
	}

	if (enc.isTriple()) {
		Serial.println("- Triple");
	}


	/*
	* not recommended for use with multi-click functions (isSingle, isDouble, isTriple)
	* as it clears the multi-click flag
	* to see this function work with multi-click functions, please give the true parameter to multi-click functions above
	*/
	
	// if (enc.getClicks(true)) {
	// 	Serial.println(String("- Clicked - ") + enc.getClicks() + " times");
	// }


	/*
	* Commented to avoid cluttering the output.
	* Uncomment below, and comment above to explore these functions
	*/
	
	// if(but.isActive()) {
	// 	Serial.println("- Active");
	// }

	// if(but.isHolding()) {
	// 	Serial.println("- Holding");
	// }

}