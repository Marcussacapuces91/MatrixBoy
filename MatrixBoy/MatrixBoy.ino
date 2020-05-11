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
#include "snake.h"

Matrix matrix;
Snake snake(matrix);

void setup() {  
  Serial.begin(115200);
  while (!Serial) ;

  matrix.begin();

  const auto a = matrix.alim();
  
  matrix.showBattery(a);
  delay(2000);
  matrix.clear();
  
  while (matrix.button() & Matrix::B) {
    matrix.println( String(F("Bat: ")) + String(a) + String(F("v   ")) );
  }
}

void loop() {
  const auto t = millis();  
  while(!matrix.pressA( String(F("[A] to play Snake...  ")) ) && (millis() - t < 60000U)) ;
  if (millis() - t >= 60000U) {
    matrix.clear();
    matrix.deepSleep();
  }
  
  snake.setup();
  while (snake.loop()) ;
}
