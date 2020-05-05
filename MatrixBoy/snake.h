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

#include "matrix.h"

class Snake {
public:  
  Snake(Matrix& aMatrix) 
  : matrix(aMatrix) 
  {}

/**
 * Méthode exécutée à l'initialisation de l'instance.
 * @return un booléen qui indique le bon déroulement de la méthode.
 */
  boolean setup() {
    Serial.println(__FUNCTION__);
    Serial.println(sizeof(serpent));

    serpent.last = 20;

    return true;
  }

/**
 * Méthode exécutée en boucle par l'instance.
 * @return un booléen qui indique le bon déroulement de la méthode. 
 *         En cas de retour FALSE, il ne faut plus appeler cette méthode (fin du jeu par exemple).
 */
  boolean loop() {
    static int8_t dx = -1;
    static int8_t dy = 0;
    static auto t = millis();

    const auto b = matrix.button();
    if ((b & Matrix::UP) && !(b & Matrix::DOWN)) { dx = 0; dy = +1; }
    else if (!(b & Matrix::UP) && (b & Matrix::DOWN)) { dx = 0; dy = -1; };
    if ((b & Matrix::LEFT) && !(b & Matrix::RIGHT)) { dx = -1; dy = 0; }
    else if (!(b & Matrix::LEFT) && (b & Matrix::RIGHT)) { dx = +1; dy = 0; };
    
    const auto m = millis();
    if (m  - t > 500) {
      t = m;
      auto x = serpent.X();
      auto y = serpent.Y();
      matrix.unSet(x, y);
      for (auto i = 0; i < serpent.len(); ++i) {
        serpent.diff(i, x, y);
        matrix.unSet(x, y);
      }
      
      serpent.move(dx, dy);
      
      x = serpent.X();
      y = serpent.Y();
      matrix.set(x, y);
      for (auto i = 0; i < serpent.len(); ++i) {
        serpent.diff(i, x, y);
        matrix.set(x, y);
      }
    }
    
    return true;
  }

private:
/// Une référence vers la matrice de Leds
  Matrix& matrix;

  struct {
    byte headX:3;
    byte headY:3;
    byte last;
    uint32_t data[4];
  
    byte len() const {
      return last;
    }

    byte X() const {
      return headX;
    }

    byte Y() const {
      return headY;
    }

    void diff(const size_t pos, byte& x, byte& y) const {
      const auto d = (data[pos / 16] >> (pos % 16) * 2) & 0x03;
      switch (d) {
        case 0 : x = (x == 7 ? 0 : x+1); break;
        case 1 : y = (y == 7 ? 0 : y+1); break;
        case 2 : x = (x) ? x - 1 : 7; break;
        case 3 : y = (y) ? y - 1 : 7; break;
      }
    }

    void move(const int8_t dx, const int8_t dy) {
      headX += dx;
      headY += dy;

      data[3] = (data[3] << 2) | (data[2] >> 30) ;
      data[2] = (data[2] << 2) | (data[1] >> 30) ;
      data[1] = (data[1] << 2) | (data[0] >> 30) ;
      data[0] = data[0] << 2;

      if (dx == 1) data[0] |= 0x2;
      else if (dy == -1) data[0] |= 0x1;
      else if (dy == 1) data[0] |= 0x3;
      // else if (dx == -1) data[0] |= 0x0;
    }
    
  } serpent = { 4, 4, 0, { 0, 0, 0, 0 } };

};
