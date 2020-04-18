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

#include "matrix.h"

Matrix matrix;

void setup() {  
  Serial.begin(115200);
  while (!Serial) ;
  
  matrix.begin();

  tone(11, 297, 250); // Ré
  delay(250);
  tone(11, 371, 250); // Fa#
  delay(250);
  tone(11, 396, 250); // Sol
}

void loop() {
  static unsigned long temps = millis();
  static byte x = 4;
  static byte y = 4;

  const unsigned long m = millis();
  if (m - temps > 250) {
    matrix.unSet(x % 8, y % 8);
    
    const byte b = matrix.button();
    if (b & Matrix::UP) y = (y+1) % 8;
    if (b & Matrix::DOWN) y = (y == 0 ? 7 : y-1);
    if (b & Matrix::LEFT) x = (x == 0 ? 7 : x-1);
    if (b & Matrix::RIGHT) x = (x+1) % 8;

//    if (b & Matrix::A) Serial.print(F("A "));
//    if (b & Matrix::B) Serial.print(F("B "));
//    if (b) Serial.println();

    matrix.set(x % 8, y % 8);
    Serial.print(x);
    Serial.print(',');
    Serial.println(y);

    temps = m;
  }

//  static char dx = 1;
//   static char dy = 1;
  
}
