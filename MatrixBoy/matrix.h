/*
 Copyright 2020 Marc SIBERT

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. 
*/

#pragma once

#include "ws2812b.h"

class Matrix {
public:

/**
 * Etats retournés par la méthode flash.
 */
  enum button_t {
    UP    = _BV(5),
    DOWN  = _BV(0),
    LEFT  = _BV(3),
    RIGHT = _BV(6),
    A     = _BV(4),
    B     = _BV(2)
  };

/**
 * Some consts.
 */
  enum {
    TONE_PIN = 11,  // Pin 11
    BANDGAP_VOLTAGE = 1129 // Bandgap voltage @ 30°C (mV)
  };

/**
 * Mesure power voltage.
 * @return Power Voltage in Volts.
 */
  float alim() const;

/**
 * Initialise l'état de l'instance.
 */
  void begin();

/**
 * Retourne l'état des boutons.
 */
  uint8_t button() const;

/**
 * Clear all LEDs.
 * @param invert Let light all LEDs.
 */
  void clear(const bool invert = false);

/**
 * Méthode appelée par l'interruption du comparateur A (fin d'éclairage).
 */
  static void comparatorInt();

/**
 * Set deep sleep mode (need reset to restart).
 */
  void deepSleep() const;

/**
 * Méthode appelée par l'interruption au début de l'éclairage.
 */
  static void overflowInt();

/**
 * Scrolls one line message and return button status.
 * @param s String to print.
 * @return true if A button was pressed, false if not.
 */
  bool pressA(const String& s);

/**  
 *   
 */
  void print(const char c);

/**  
 *   
 */
  void println(const char c);

/**  
 *   
 */
  void println(const String& str);
  
/**
 * Show battery or plug sign and voltage value if B button is pressed.
 */
  void showBatteryAndVoltage();

/**
 * Tests the defined LED (ON/OFF ?).
 * @param x x position.
 * @param y y position.
 * @return State of the defined LED.
 */
  bool test(const byte x, const byte y) const;
  
/**
 * Sets the defined LED (ON).
 * @param x x position.
 * @param y y position.
 */
  void set(const byte x, const byte y);

/**
 * Unsets the defined LED (OFF).
 * @param x x position.
 * @param y y position.
 */
  void unSet(const byte x, const byte y);
  
protected:
/**
 * Set Analog-to-Digital Converter on A6.
 */
  void setADC2A6() const;

/**
 * Set Analog-to-Digital Converter to Bandgap
 */
  void setADC2BG() const;

/**
 * Set Analog Comparator using BG vs. Analog Port (mux)
 * @param mux Multiplexer value [0..7] (ADC0..ADC7)
 */
  void setComparator(const byte mux) const;

/**
 * Set & start/stop Timer1
 * @param state start = true (default) / stop = false
 */
  void setTimer1(const bool state = true) const;

/**
 * Show battery cap.
 * @param v mesured voltage [0..4.2] ; If v > 4.5 show a plug.
 */
  void showBattery(const float& v);
  
private:
/// leds values (bits).
  static volatile uint8_t leds[8];
  
/// current col.  
  static volatile byte cCol;

/// Buttons.
  static volatile uint8_t buttons;

/// Columns order.
  static const uint8_t cols[8];

/// Rows order.
  static const uint8_t rows[8];

/// RGB led
  WS2812b<5, 1, 1> ws; // D13

};
