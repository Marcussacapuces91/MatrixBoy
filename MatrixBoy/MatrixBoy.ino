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
#include "spaceInvaders.h"

Matrix matrix;

void setup() {  
  Serial.begin(115200);
  while (!Serial) ;

  matrix.begin();
  matrix.showBatteryAndVoltage(); 
  
}

void loop() {
  const auto t = millis();
  static byte choice = 0;
  while( millis() - t < 60000U) {
    switch (choice) {
      case 0 : {
        const auto b = matrix.pressButton( String(F("[A] Snake... ")) );
        if (b == Matrix::UP) choice = 1;
        else if (b == Matrix::DOWN) choice = 1;
        else if (b == Matrix::A) {
          Snake snake(matrix);
          snake.setup();
          while (snake.loop()) ;
        }
        else if (b == Matrix::NONE) continue;
        return;
      }
      case 1 : {
        const auto b = matrix.pressButton( String(F("[A] Space Invaders... ")) );
        if (b == Matrix::UP) choice = 0;
        else if (b == Matrix::DOWN) choice = 0;
        else if (b == Matrix::A) {
          SpaceInvaders si(matrix);
          si.setup();
          while (si.loop()) ;
        }
        else if (b == Matrix::NONE) continue;
        return;
      }
    }    
  }
  if (millis() - t >= 60000U) {
    matrix.clear();
    while (matrix.button() == Matrix::NONE) {
      matrix.deepSleep();
    }
  }
  
}
