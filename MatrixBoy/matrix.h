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

/*
    for (byte c = 0; c < 8; ++c) {
      leds[c] = 0xFF << c;
    }
*/
//    leds = { 0, 0, 0, 0, 0, 0, 0, 0 };

// Réglage Timer1
    noInterrupts();
    OCR1A = 10;
    TCCR1A = _BV(WGM10);           
    TCCR1B = _BV(WGM12) | _BV(CS11) | _BV(CS10);   // clk/64
    TIMSK1 |= _BV(OCIE1A) | _BV(TOIE1);
    interrupts();

// Réglage Analog-to-Digital Converter on A6
    noInterrupts();
    ADMUX = _BV(REFS0) | _BV(ADLAR) | 0x06; // ref = AVcc & 8 bits & MUX = ADC6
    ADCSRA = _BV(ADEN) | 0x02;    // ADC Prescaler div. 4
    ADCSRB = 0;
    interrupts();

    cCol = 0;
  }

  void set(const byte x, const byte y) {
    leds[y] |= 1 << x; 
  }

  void unSet(const byte x, const byte y) {
    leds[y] &= ~(1 << x);  
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

// Start ADV Conv.
    ADCSRA |= _BV(ADSC);

// Test bouton
    while (!(ADCSRA & _BV(ADIF))) ;  // test int flag.
    ADCSRA |= _BV(ADIF);  // clear int flag.
    buttons = (ADCH < 100) ? buttons | (1 << cCol) : buttons & ~(1 << cCol);
//    Serial.println(ADCH);

// Switch col. off
    pinMode(cols[cCol], INPUT);

// Next col    
    ++Matrix::cCol;
    Matrix::cCol %= 8;

// Count bits (Brian Kernighan’s Algorithm)
    const uint8_t l = leds[cCol];
    byte count = 0;
    for (auto n = l; n > 0; n &= (n - 1)) ++count;
    
// Set timing on
    OCR1A = 10 + (count * 20) / 10;
  }

protected:

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
