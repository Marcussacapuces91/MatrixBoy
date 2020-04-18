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


class Matrix {
public:

/**
 * Etats retournés par la méthode flash.
 */
  enum button_t {
    UP = 5,
    DOWN = 0,
    LEFT = 3,
    RIGHT = 6,
    A = 4,
    B = 2,
    OFF = -1
  };

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

    for (byte c = 0; c < 8; ++c) {
      leds[c] = 0xFF << c;
    }

    noInterrupts();
    OCR1A = 10;
    TCCR1A = _BV(WGM10);           
    TCCR1B = _BV(WGM12) | _BV(CS11) | _BV(CS10);   // clk/64
    TIMSK1 |= _BV(OCIE1A) | _BV(TOIE1);
    interrupts();

    cCol = 0;
    
    pinMode(13, OUTPUT);
    
  }

/**
 * Éclaire une rangée pendant un temps donnée ne dépassant pas le paramètre t.
 * @param t : durée maximum de l'itération.
 * @return Le code du bouton pressé. Attention le code du bouton dépend
 */
  button_t flash(const unsigned &t) const {
    static uint8_t c = 0;

    const uint8_t l = leds[c];

// Set right rows
    PORTC = (PORTC & ~B00010100) | (l & 0x10) | (l & 0x40) >> 4;
    PORTD = (PORTD & ~B11111100) | (l & 0x03) << 5 | (l & 0x04) << 2 | (l & 0x08) << 4 | (l & 0x20) >> 2 | (l & 0x80) >> 5;
        
    const uint8_t col = cols[c];
    
// Switch col ON
    pinMode(col, OUTPUT);
    
// Count bits (Brian Kernighan’s Algorithm)
    byte count = 0;
    for (byte n = l; n > 0; n &= (n - 1)) ++count;

// Adapt lumi.
    delayMicroseconds(10 + count * t / 8);

// Test bouton
    const button_t ret = analogRead(A6) < 1000 ? static_cast<button_t>(c) : OFF;

// Next col.    
    c = (c + 1) % 8;

// Switch col OFF (Hi state, without pullup)
    pinMode(col, INPUT);

    return ret;
  }

/**
 * Méthode appelée par l'interruption au début de l'éclairage.
 */
  inline static void overflowInt() {
    const uint8_t l = leds[cCol];

// Set right rows
    PORTC = (PORTC & ~B00010100) | (l & 0x10) | (l & 0x40) >> 4;
    PORTD = (PORTD & ~B11111100) | (l & 0x03) << 5 | (l & 0x04) << 2 | (l & 0x08) << 4 | (l & 0x20) >> 2 | (l & 0x80) >> 5;

// Switch col ON
    pinMode(cols[cCol], OUTPUT);

  }

  inline static void comparatorInt() {
// Test bouton
    const button_t ret = analogRead(A6) < 1000 ? static_cast<button_t>(cCol) : OFF;
    Serial.println(ret);

// Switch off
    pinMode(cols[cCol], INPUT);

// Next col    
    ++Matrix::cCol;
    Matrix::cCol %= 8;

// Count bits (Brian Kernighan’s Algorithm)
    const uint8_t l = leds[cCol];
    byte count = 0;
    for (byte n = l; n > 0; n &= (n - 1)) ++count;
    
// Set timing on
    OCR1A = 10 + count * 16;
  }

protected:

private:

/// leds values (bits).
  static volatile uint8_t leds[8];

/// current col.  
  static byte cCol;

/// Colones order.
  static const uint8_t cols[8];

/// Rows order.
  static const uint8_t rows[8];
  
};

ISR(TIMER1_OVF_vect) {    // ROW ON
  interrupts();
  Matrix::overflowInt();
}

ISR(TIMER1_COMPA_vect) {  // ROW OFF
  interrupts();
  Matrix::comparatorInt();
}


volatile uint8_t Matrix::leds[8];

byte Matrix::cCol;

const uint8_t Matrix::cols[] = { A0, A1, A5,  9,  8, 10, 12, A3};
const uint8_t Matrix::rows[] = {  5,  6,  4,  7, A4,  3, A2,  2};
