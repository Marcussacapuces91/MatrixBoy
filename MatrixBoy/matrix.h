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

#if 1
#include "zxpix_font.h"
#define FONT_WIDTH 6
#else
#include "homespun_font.h"
#define FONT_WIDTH 7
#endif

#include <avr/sleep.h>

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

  enum {
    TONE_PIN = 11  // Pin 11
  };

  uint8_t button() const {
    return Matrix::buttons;
  }

/**
 * Initialise l'état de l'instance.
 */
  void begin() {
    for (uint8_t i = 0; i < 8; ++i) {
      pinMode(cols[i], INPUT);
      digitalWrite(cols[i], LOW);
      pinMode(rows[i], OUTPUT);
      digitalWrite(rows[i], LOW);
    }

    pinMode(A6, INPUT);
    setComparator(0x06);
    setTimer1();

    cCol = 0;
  }

/**
 * Mesure power voltage.
 * @return Power Voltage in Volts.
 */
  float alim() const {
    setTimer1(false); // unset
    setADC2BG();
    delay(2); // wait for Vref to settle

    ADCSRA |= _BV(ADSC);  // start conv.
    
    while ( ADCSRA & _BV(ADSC) ) yield();  // wait until end conv.
    const unsigned adc = ADCL + ( ADCH * 256U );

    setComparator(0x06);  // comp. BG vs. A6
    setTimer1();
    return (1023 * 1.1) / adc;
  }

/**
 * Set deep sleep mode (need reset to restart).
 */
  void deepSleep() const {
    ADCSRA = 0;         // Unused ADC (mandatory)
    ADCSRB = 0;         // Unused Comparator
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    sleep_mode();
  }

/**
 * Scrolls one line message and return button status.
 * @param s String to print.
 * @return true if A button was pressed, false if not.
 */
  bool pressA(const String& s) {
    for (unsigned i = 0 ; i < s.length() ; ++i) {
      print( s.charAt(i) );
      if ( button() & Matrix::A ) {
        clear(true);
        delay(75);
        clear();
        return true;
      }
    }
    return false;
  }

  void print(const char c) {
    println(String(c));
  }

  void println(const char c) {
    println(String(c));
  }

  void println(const String& str) {
    for (unsigned l = 0; l < str.length(); ++l) {
      const auto c = str.charAt(l);

      byte m = FONT_WIDTH;
      for (byte i = FONT_WIDTH - 1; i > 1; --i) { //  minimum 2 dots
        if (!pgm_read_byte(&(font[c - 0x20][i]))) {
          m = i;
        }
      }

      for (byte i = 0; i < m; ++i) {
        const byte b = pgm_read_byte(&(font[c - 0x20][i]));
        for (byte j = 0; j < 8; ++j) {
          leds[j] = (leds[j] >> 1) + ( b & (0x80 >> j) ? 0x80 : 0x00 );
        }
        delay(100);
      }
// 1 separator
      for (byte j = 0; j < 8; ++j) {
        leds[j] >>= 1;
      }
      delay(100);
    }
    yield();
  }

/**  
 * Show battery cap.
 * @param v mesured voltage [0..4.2]
 */
  void showBattery(const float& v) {
    const byte i = (v >= 4.1) ? 0b00111100 :
                   (v >= 3.9) ? 0b00011100 :
                   (v >= 3.7) ? 0b00001100 :
                   (v >= 3.5) ? 0b00000100 : 
                                0b00000000 ;
    noInterrupts();
    leds[7] = 0b01111111;
    leds[6] = 0b11000001;
    leds[5] = 0b10000001 | i;
    leds[4] = 0b10000001 | i;
    leds[3] = 0b11000001;
    leds[2] = 0b01111111;
    leds[1] = 0b00000000;
    leds[0] = 0b00000000;
    interrupts();
  }

/**
 * Tests the defined LED (ON/OFF ?).
 * @param x x position.
 * @param y y position.
 * @return State of the defined LED.
 */
  bool test(const byte x, const byte y) const {
    return (leds[y] & (1 << x));
  }

/**
 * Sets the defined LED (ON).
 * @param x x position.
 * @param y y position.
 */
  void set(const byte x, const byte y) {
    leds[y] |= 1 << x; 
  }

/**
 * Unsets the defined LED (OFF).
 * @param x x position.
 * @param y y position.
 */
  void unSet(const byte x, const byte y) {
    leds[y] &= ~(1 << x);  
  }


/**
 * Clear all LEDs.
 * @param invert Let light all LEDs.
 */
  void clear(const bool invert = false) {
    for (byte i = 0; i < 8; ++i) {
      leds[i] = invert ? 0xFF : 0x00;
    }
  }

/**
 * Méthode appelée par l'interruption au début de l'éclairage.
 */
  inline static void overflowInt() {
    const uint8_t l = leds[cCol];

// Set right rows
    PORTC = (PORTC & ~B00010100) | (l & 0x10) | (l & 0x40) >> 4;
    PORTD = (PORTD & ~B11111100) | (l & 0x03) << 5 | (l & 0x04) << 2 | (l & 0x08) << 4 | (l & 0x20) >> 2 | (l & 0x80) >> 5;

// Switch col. ON
    pinMode(cols[cCol], OUTPUT);
  }

/**
 * Méthode appelée par l'interruption du comparateur A (fin d'éclairage).
 */
  inline static void comparatorInt() {
    
// Unset all rows
    PORTC &= ~B00010100;
    PORTD &= ~B11111100;

// Read comparator bit (Analog Comparator Output) in ACSR
    const byte b = 1 << cCol;
    buttons = ACSR & _BV(ACO) ? buttons | b : buttons & ~b;

// Switch col. off
    pinMode(cols[cCol], INPUT);

// Next col    
    Matrix::cCol = (Matrix::cCol + 1) % 8;

// Count bits (Brian Kernighan’s Algorithm)
    const uint8_t l = leds[cCol];
    byte count = 0;
    for (auto n = l; n > 0; n &= (n - 1)) ++count;
    
// Set timing on
    OCR1A = 10 + (count << 4);
  }

protected:
/**
 * Set Analog Comparator using BG vs. Analog Port (mux)
 * @param mux Multiplexer value [0..7] (ADC0..ADC7)
 */
  void setComparator(const byte mux) const {
    noInterrupts();
    ADMUX  = mux & ( _BV(MUX2) | _BV(MUX1) | _BV(MUX0) );
    ADCSRA = 0;         // Unused ADC (mandatory)
    ADCSRB = _BV(ACME); // Analog Comparator Multiplexer Enable
    ACSR   = _BV(ACBG); // Analog Comparator BandGap
    interrupts();
  }
  
/**
 * Set Analog-to-Digital Converter on A6.
 */
  void setADC2A6() const {
    noInterrupts();
    ADMUX  = _BV(REFS0) | _BV(ADLAR) | 0x06; // ref = AVcc & 8 bits & MUX = ADC6
    ADCSRA = _BV(ADEN) | _BV(ADPS1);    // ADC Prescaler div. 4
    ADCSRB = 0;
    interrupts();
  }

/**
 * Set Analog-to-Digital Converter to Bandgap
 */
  void setADC2BG() const {
    noInterrupts();
    ADMUX  = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);  // ref = AVcc & 10 bits & MUX = V1.1
    ADCSRA = _BV(ADEN) | _BV(ADPS2);  // ADC Prescaler div. 16
    ADCSRB = 0;
    interrupts();
  }

/**
 * Set & start/stop Timer1
 * @param state start = true (default) / stop = false
 */
  void setTimer1(const bool state = true) const {
    noInterrupts();
    OCR1A  = 0;
    TCCR1A = _BV(WGM10);           
    TCCR1B = _BV(WGM12) | _BV(CS11) | _BV(CS10);   // clk/64
    TIMSK1 = state ? _BV(OCIE1A) | _BV(TOIE1) : 0;
    interrupts();
  }

private:

/// leds values (bits).
  static volatile uint8_t leds[8];

/// current col.  
  static volatile byte cCol;

/// Buttons.
  static volatile uint8_t buttons;

/// Colones order.
  static const uint8_t cols[8];

/// Rows order.
  static const uint8_t rows[8];
  
};

ISR(TIMER1_OVF_vect) {    // ROW ON
  Matrix::overflowInt();
}

ISR(TIMER1_COMPA_vect) {  // ROW OFF
  Matrix::comparatorInt();
}


volatile uint8_t Matrix::leds[8];

volatile byte Matrix::cCol;

volatile uint8_t Matrix::buttons;

const uint8_t Matrix::cols[] = { A0, A1, A5,  9,  8, 10, 12, A3};
const uint8_t Matrix::rows[] = {  5,  6,  4,  7, A4,  3, A2,  2};
