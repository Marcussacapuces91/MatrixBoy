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
  static unsigned long temps = micros();

  const unsigned long m = micros();
  if (m - temps > 2000) {
    Serial.println(matrix.button(), BIN);
    temps = m;
  }

  static byte x = 4;
  static byte y = 4;
  static char dx = 1;
  static char dy = 1;
  
}
