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

  enum button_t {
    UP = 5,
    DOWN = 0,
    LEFT = 3,
    RIGHT = 6,
    A = 4,
    B = 2,
    OFF = -1
  };

  void begin() {
    for (uint8_t i = 0; i < 8; ++i) {
      pinMode(cols[i], INPUT);
      digitalWrite(cols[i], LOW);
      pinMode(rows[i], OUTPUT);
      digitalWrite(rows[i], LOW);
    }

    pinMode(A6, INPUT_PULLUP);

    for (byte c = 0; c < 8; ++c) {
      leds[c] = 0xFF << c;
    }
  }

  button_t flash(const unsigned &t) const {
    static uint8_t c = 0;

    const uint8_t l = leds[c];

// Set right rows
    PORTC = (PORTC & ~B00010100) | (l & 0x10) | (l & 0x40) >> 4;
    PORTD = (PORTD & ~B11111100) | (l & 0x03) << 5 | (l & 0x04) << 2 | (l & 0x08) << 4 | (l & 0x20) >> 2 | (l & 0x80) >> 5;
        
    const uint8_t col = pgm_read_byte_near(cols + c);
    
// Switch col ON
    pinMode(col, OUTPUT);
    
// Count bits (Brian Kernighan’s Algorithm)
    byte count = 0;
    for (byte n = l; n > 0; n &= (n - 1)) ++count;

// Adapt lumi.
    delayMicroseconds(10 + count * t / 8);

// Test bouton
    const button_t ret = analogRead(A6) < 1000 ? c : -1;

// Next col.    
    c = (c + 1) % 8;

// Switch col OFF (Hi state, without pullup)
    pinMode(col, INPUT);

    return ret;
  }

protected:

private:

/// leds values (bits).
  uint8_t leds[8];

/// Colones order.
  static const uint8_t cols[8] PROGMEM;

/// Rows order.
  static const uint8_t rows[8];
  
};

const uint8_t Matrix::cols[] PROGMEM = { A0, A1, A5,  9,  8, 10, 12, A3};
const uint8_t Matrix::rows[] = {  5,  6,  4,  7, A4,  3, A2,  2};
