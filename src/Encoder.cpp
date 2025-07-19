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

#include "Encoder.h"

/* --- constructors --- */
Encoder::Encoder(uint16_t clkPort, uint16_t dtPort, uint16_t butPort) {
    this->clkPort = clkPort;
    this->dtPort = dtPort;
	this->butPort = butPort;

	setEncPortMode(encPortMode);
	setButPortMode(butPortMode);

    lastClk = digitalRead(clkPort);
    lastDt = digitalRead(dtPort);
	lastBut = isActive();
}
/* --- constructors --- */

/* --- main functions --- */
void Encoder::tick() {
    int8_t status = readEnc();

	if (!status) { 
		readBut();
		return;
	}

	// encoder \/
	pressWhileTurnFlag = true;

	if (turnsCounter == maxTurnsCount) { // buffer is full
		return;
	}

	/* --- encoder --- */
	uint8_t but = isActive();
	
	if (status == 1) { // is right
		encFlagsBuffer |= 0x01 << turnsCounter * TURN_INFO_BITS; // first bit in turn info bits (2) - direction (1: right 0: left)
	}

	encFlagsBuffer |= but << (turnsCounter * TURN_INFO_BITS + 1); // second bit in turn info bits (2) - if with button holding (1: yes 0: no)
	turnsCounter++;
	/* --- encoder --- */
}

/* encoder */
bool Encoder::isTurn() {
    return turnsCounter > 0;
}

bool Encoder::isLeft(bool saveFlag) {
	if (!isTurn()) {
		return false;
	}

	if (IS_LEFT(encFlagsBuffer)) {
		if (!saveFlag) {
			deleteTurn();
		}

		return true;
	}

	return false;
}

bool Encoder::isRight(bool saveFlag) {
	if (!isTurn()) {
		return false;
	}

	if (IS_RIGHT(encFlagsBuffer)) {
		if (!saveFlag) {
			deleteTurn();
		}

		return true;
	}

	return false;
}

bool Encoder::isLeftH(bool saveFlag) {
	if (!isTurn()) {
		return false;
	}

	if (IS_LEFT_HOLDED(encFlagsBuffer)) {
		if (!saveFlag) {
			deleteTurn();
		}

		return true;
	}

	return false;
}

bool Encoder::isRightH(bool saveFlag) {
	if (!isTurn()) {
		return false;
	}

	if (IS_RIGHT_HOLDED(encFlagsBuffer)) {
		if (!saveFlag) {
			deleteTurn();
		}

		return true;
	}

	return false;
}

bool Encoder::getTurn(bool saveFlag) { // direction (1: right 0: left)
	if (!isTurn()) {
		return false;
	}

	bool turn = isRight(true);

	if (!saveFlag) {
		deleteTurn();
	}

	return turn;
}

void Encoder::deleteTurns() {
    encFlagsBuffer = 0;
    turnsCounter = 0;
}

/* button */
bool Encoder::isPressed() {
	if (IS_PRESSED_FLAG(butFlags)) {
		if (millis() - pressTime < CLICK_MIN_TIME) { // it can be noise
			return false;
		}
		
		CLEAR_PRESSED_FLAG(butFlags);
		return true;
	}

	return false;
}

bool Encoder::isClick(bool saveFlag ) {
	if (IS_CLICK_FLAG(butFlags)) {
		if (!saveFlag) {
			CLEAR_CLICK_FLAG(butFlags);
		}

		return true;
	}

	return false;
}

bool Encoder::isHolded(bool saveFlag ) {
	if (!holdedReadFlag) { // if is holding, and we don't processed (isHolded)
		if (isHolding()) {
			SET_HOLDED_FLAG(butFlags);
			holdedReadFlag = true;
		}
	}

	if (IS_HOLDED_FLAG(butFlags)) {
		if (!saveFlag) {
			CLEAR_HOLDED_FLAG(butFlags);
		}

		return true;
	}

	return false;
}

uint8_t Encoder::getClicks(bool saveFlag ) {
	if (!GET_CLICKS(butFlags)) {
		return false;
	}

	if (!IS_CLICKS_FLAG(butFlags)) {
		if (millis() - releaseTime > CLICKS_MAX_TIME) { // if time between clicks is long
			SET_CLICKS_FLAG(butFlags);
		}
	}

	if (IS_CLICKS_FLAG(butFlags)) {
		uint8_t clicks = GET_CLICKS(butFlags);

		if (!saveFlag) {
			CLEAR_CLICKS_FLAG(butFlags);
			CLEAR_CLICKS(butFlags);
		}
		
		return clicks;
	}

	return 0;
}

bool Encoder::isSingle(bool saveFlag ) {
	if (getClicks(true) == 1) {
		if (!saveFlag) {
			CLEAR_CLICKS_FLAG(butFlags);
			CLEAR_CLICKS(butFlags);
		}

		return true;
	}

	return false;
}

bool Encoder::isDouble(bool saveFlag ) {
	if (getClicks(true) == 2) {
		if (!saveFlag) {
			CLEAR_CLICKS_FLAG(butFlags);
			CLEAR_CLICKS(butFlags);
		}

		return true;
	}

	return false;
}

bool Encoder::isTriple(bool saveFlag ) {
	if (getClicks(true) == 3) {
		if (!saveFlag) {
			CLEAR_CLICKS_FLAG(butFlags);
			CLEAR_CLICKS(butFlags);
		}

		return true;
	}

	return false;
}

bool Encoder::isActive() {
	return digitalRead(butPort) ^ butInvert;
}

bool Encoder::isHolding() {
	if (lastBut) {
		if (pressWhileTurnFlag) { // if we make turn
			return false;
		}

		if (millis() - pressTime >= HOLD_MIN_TIME) {
			return true;
		}
	}
	
	return false;
}

void Encoder::clearButFlags() {
	butFlags = 0x00;
	lastBut = false;
}
/* --- main functions --- */

/* --- setters --- */
/* encoder */
void Encoder::setEncType(uint8_t encType) {
    this->encType = encType;
}

void Encoder::setEncPortMode(uint8_t encPortMode) {
    this->encPortMode = encPortMode;

    pinMode(clkPort, encPortMode);
    pinMode(dtPort, encPortMode);
}

void Encoder::setEncInvert(bool encInvert) {
    this->encInvert = encInvert;
}

void Encoder::setMaxTurnsCount(uint8_t maxTurnsCount) {
    this->maxTurnsCount = MIN(maxTurnsCount, MAX_TURNS_COUNT);
}

/* button */
void Encoder::setButPortMode(uint8_t butPortMode) {
	this->butPortMode = butPortMode;

    pinMode(butPort, butPortMode);
}

void Encoder::setButInvert(bool butInvert) {
	if (butInvert != this->butInvert) {
		lastBut = !lastBut;
	}

	this->butInvert = butInvert;
}
/* --- setters --- */

/* --- private --- */
int8_t Encoder::readEnc() {
	uint8_t clk = digitalRead(clkPort);
	uint8_t dt = digitalRead(dtPort);

	if (lastClk ^ lastDt ^ clk ^ dt) { // if we have some change
		(clk ^ lastDt) ? pulsesCounter++ : pulsesCounter--; // if new clk != previous dt
		lastClk = clk;
		lastDt = dt;

		if (!pulsesCounter) {
			return 0;
		}

		switch (encType) {
		case ENC_TYPE_STEP4:
			if (!(clk & dt)) { // if not 11, skip.  skip: 01, 10, 00
				return 0;
			}

			break;
		case ENC_TYPE_STEP2:
			if (clk ^ dt) { // if not 00 or 11, skip.  skip: 10, 01
				return 0;
			}

			break;
		}

		int8_t dir = ((pulsesCounter > 0) ^ encInvert) ? 1 : -1; // 1: right  2: left
		pulsesCounter = 0;

		return dir;
	}

	return 0;
}

void Encoder::deleteTurn() {
    encFlagsBuffer >>= TURN_INFO_BITS;
    turnsCounter--;
}

void Encoder::readBut() {
	bool state = isActive();
	
	if (state ^ lastBut) { // if we have change
		lastBut = state;

		if (millis() - lastActionTime < CLICK_MIN_TIME) {
			return;
		}

		lastActionTime = millis();

		if (state) { // if is pressed
			pressTime = millis();
			holdedReadFlag = false;
			pressWhileTurnFlag = false;
			
			SET_PRESSED_FLAG(butFlags);
		}
		else { // is released
			if (pressWhileTurnFlag) {
				pressWhileTurnFlag = false;
				return;
			}

			bool clicksFlag = (pressTime - releaseTime) <= CLICKS_MAX_TIME; // if multi clicks
			
			releaseTime = millis();
			uint32_t delta = releaseTime - pressTime;

			if (delta < CLICK_MIN_TIME) { // noise
				CLEAR_PRESSED_FLAG(butFlags);
				return;
			}

			if (delta >= CLICK_MIN_TIME && delta <= CLICK_MAX_TIME) { // click
				SET_CLICK_FLAG(butFlags);

				if (!clicksFlag) { // reset clicks -> start counting
					SET_CLICKS(butFlags, 1);
					CLEAR_CLICKS_FLAG(butFlags);
				}
				else {
					SET_CLICKS(butFlags, GET_CLICKS(butFlags) + 1); // add click
				}

				return;
			}

			if (delta >= HOLD_MIN_TIME && !holdedReadFlag) { // was holded and we don't processed (isHolded) when was holding
				SET_HOLDED_FLAG(butFlags);
				holdedReadFlag = true;
			}
		}
	}
}
/* --- private --- */