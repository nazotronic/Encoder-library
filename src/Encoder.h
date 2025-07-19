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

/*
Turn right:
CLK: ──┐     ┌─────
       └─────┘     
DT : ─────┐     ┌──
          └─────┘

States sequence:
CLK/DT 11 → 01 → 00 → 10 → 11


Turn left:
CLK: ─────┐     ┌──
          └─────┘     
DT : ──┐     ┌─────
       └─────┘

States sequence:
CLK/DT 11 → 10 → 00 → 01 → 11
*/

#ifndef _ENCODER_H
#define _ENCODER_H

#include <Arduino.h>

/* --- user settings --- */
#define ENC_TYPE_STEP4 0
#define ENC_TYPE_STEP2 1

#define ENC_PORT_INPUT INPUT
#define ENC_PORT_INPUT_PULLUP INPUT_PULLUP

#define BUTTON_PORT_INPUT INPUT
#define BUTTON_PORT_INPUT_PULLUP INPUT_PULLUP
/* --- user settings --- */

/* --- library settings --- */
#define TURNS_BUFFER_TYPE uint32_t
#define TURN_INFO_BITS    2 // 2 because we have: 1.turn direction   2.if turn was with a button holding

#define MAX_TURNS_COUNT   sizeof(TURNS_BUFFER_TYPE) * (8 / TURN_INFO_BITS) // (8 / SAVE_INFO) = how many "full info" turns we can write in 1 byte

#define CLICK_MIN_TIME    30
#define CLICK_MAX_TIME    400
#define HOLD_MIN_TIME     CLICK_MAX_TIME + 1
#define CLICKS_MAX_TIME   300
/* --- library settings --- */

/* --- macro functions --- */
#define IS_LEFT(val)      ((val & 0x03) == 0x00)
#define IS_RIGHT(val)     ((val & 0x03) == 0x01)

#define IS_LEFT_HOLDED(val)  ((val & 0x03) == 0x02)
#define IS_RIGHT_HOLDED(val) ((val & 0x03) == 0x03)

#define MIN(A, B)         ((A < B) ? A : B)

#define PRESSED_FLAG_POS  0x01
#define CLICK_FLAG_POS    0x02
#define CLICKS_FLAG_POS   0x04
#define HOLDED_FLAG_POS   0x08

#define SET_PRESSED_FLAG(val)  (val |= PRESSED_FLAG_POS)
#define SET_CLICK_FLAG(val)    (val |= CLICK_FLAG_POS)
#define SET_CLICKS_FLAG(val)   (val |= CLICKS_FLAG_POS)
#define SET_HOLDED_FLAG(val)   (val |= HOLDED_FLAG_POS)
#define SET_CLICKS(val, count)  val = (val & 0x0F) | ((uint8_t)count << 4)

#define CLEAR_PRESSED_FLAG(val) (val &= ~PRESSED_FLAG_POS)
#define CLEAR_CLICK_FLAG(val)   (val &= ~CLICK_FLAG_POS)
#define CLEAR_CLICKS_FLAG(val)  (val &= ~CLICKS_FLAG_POS)
#define CLEAR_HOLDED_FLAG(val)  (val &= ~HOLDED_FLAG_POS)
#define CLEAR_CLICKS(val)       (val &= 0x0F)

#define IS_PRESSED_FLAG(val) (val & PRESSED_FLAG_POS)
#define IS_CLICK_FLAG(val)   (val & CLICK_FLAG_POS)
#define IS_CLICKS_FLAG(val)  (val & CLICKS_FLAG_POS)
#define IS_HOLDED_FLAG(val)  (val & HOLDED_FLAG_POS)
#define GET_CLICKS(val)      (val >> 4)
/* --- macro functions --- */

/* --- main class --- */
class Encoder {
public:
	/* --- constructors --- */
	Encoder(uint16_t clkPort, uint16_t dtPort, uint16_t butPort);
	/* --- constructors --- */

	/* --- main functions --- */
	void tick();

	/* encoder */
	bool isTurn();
	bool isLeft(bool saveFlag = false);
	bool isRight(bool saveFlag = false);
	bool isLeftH(bool saveFlag = false);
	bool isRightH(bool saveFlag = false);
	bool getTurn(bool saveFlag = false);

	void deleteTurns();

	/* button */
	bool isPressed();
	bool isClick(bool saveFlag = false);
	bool isHolded(bool saveFlag = false);

	uint8_t getClicks(bool saveFlag = false);
	bool isSingle(bool saveFlag = false);
	bool isDouble(bool saveFlag = false);
	bool isTriple(bool saveFlag = false);

	bool isActive();
	bool isHolding();

	void clearButFlags();
	/* --- main functions --- */

	/* --- setters --- */
	/* encoder */
	void setEncType(uint8_t encType);
	void setEncPortMode(uint8_t encPortMode);
	void setEncInvert(bool encInvert);

	void setMaxTurnsCount(uint8_t maxTurnsCount);

	/* button */
	void setButPortMode(uint8_t butPortMode);
	void setButInvert(bool butInvert);
	/* --- setters --- */

private:
	/* --- private --- */
	int8_t readEnc();
	void deleteTurn();

	void readBut();
	/* --- private --- */

	/* --- settings --- */
	/* encoder */
	uint16_t clkPort, dtPort = 0;

	uint8_t encType = ENC_TYPE_STEP4;
	uint8_t encPortMode = ENC_PORT_INPUT;
	bool encInvert = false;

	uint8_t maxTurnsCount = MAX_TURNS_COUNT;

	/* button */
	uint16_t butPort = 0;

	uint8_t butPortMode = BUTTON_PORT_INPUT;
	bool butInvert = false;
	/* --- settings --- */

	/* --- variables --- */
	/* encoder */
	uint8_t lastClk, lastDt = 0;
	int8_t pulsesCounter = 0;

	bool pressWhileTurnFlag = false;
	uint8_t turnsCounter = 0;
	TURNS_BUFFER_TYPE encFlagsBuffer = 0;

	/* button */
	bool lastBut = false;	
	bool holdedReadFlag = false;
	
	uint32_t lastActionTime = 0;
	uint32_t pressTime = 0;
	uint32_t releaseTime = 0;

	uint8_t butFlags = 0;
	/* --- variables --- */
};

#endif