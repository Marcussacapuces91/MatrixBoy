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
#include "pitches.h"

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
    matrix.println(F("Snake (c) 2020, MSoft"));
    delay(1000);
    for (byte i = 3; i > 0; --i) {
      matrix.clear(true);
      delay(50);
      matrix.clear();
      matrix.println('0' + i);
      delay(1000);
    }
    matrix.clear(true);
    delay(50);
    matrix.clear();
    
    return true;
  }

/**
 * Méthode exécutée en boucle par l'instance.
 * @return un booléen qui indique le bon déroulement de la méthode. 
 *         En cas de retour FALSE, il ne faut plus appeler cette méthode (fin du jeu par exemple).
 */
  boolean loop() {
    static unsigned pt = 0;
    static int8_t dx = -1;
    static int8_t dy = 0;
    static auto t = millis();
    static auto p = millis();
    static byte comptePomme = 0;
    static unsigned long delayTone = 0; // OFF

    const auto b = matrix.button();
    if ((b & Matrix::UP) && !(b & Matrix::DOWN)) { dx = 0; dy = +1; }
    else if (!(b & Matrix::UP) && (b & Matrix::DOWN)) { dx = 0; dy = -1; };
    if ((b & Matrix::LEFT) && !(b & Matrix::RIGHT)) { dx = -1; dy = 0; }
    else if (!(b & Matrix::LEFT) && (b & Matrix::RIGHT)) { dx = +1; dy = 0; };
    
    const auto m = millis();

    if ( delayTone && ((m - delayTone) > 50) ) {
      noTone(Matrix::TONE_PIN);
      delayTone = 0;
    }

    if ((m - p > 75) && pomme.set) {
      p = m;
      static bool light = false;

      if (light) {
        matrix.set(pomme.x, pomme.y);
      } else {
        matrix.unSet(pomme.x, pomme.y);
      }
      light = !light;
      
    }
    
    if (m - t > 500) { // Every 500 ms
      t = m;

// Effacer serpent
      auto x = serpent.headX;
      auto y = serpent.headY;
      matrix.unSet(x, y);
      for (auto i = 0; i < serpent.len(); ++i) {
        serpent.diff(i, x, y);
        matrix.unSet(x, y);
      }
      
// Déplacer serpent
      serpent.move(dx, dy);

// Afficher serpent      
      x = serpent.headX;
      y = serpent.headY;
      matrix.set(x, y);
      for (auto i = 0; i < serpent.len(); ++i) {
        serpent.diff(i, x, y);
        matrix.set(x, y);
        if (serpent.headX == x && serpent.headY == y) { // croisement
          tone(Matrix::TONE_PIN, NOTE_C4, 250);
          delay(350);
          tone(Matrix::TONE_PIN, NOTE_B3, 250);
          delay(350);
          tone(Matrix::TONE_PIN, NOTE_A3, 500);

          String s(pt);
          s += String(F(" pts"));
          matrix.println(s);
          delay(3000);
          matrix.clear(true);
          delay(50);
          matrix.clear();
    
          serpent.last = 0;
          serpent.headX = 4;
          serpent.headY = 4;
          dx = -1;
          dy = 0;
          
          comptePomme = 0;
          pomme.set = false;
          
        }
      }

// Tête serpent sur la pomme
      if ( pomme.set && (serpent.headX == pomme.x) && (serpent.headY == pomme.y) ) {
        tone(Matrix::TONE_PIN, NOTE_B0);
        delayTone = m;
        comptePomme = 0;  // reset;
        pomme.set = false;
        ++serpent.last;
        ++pt;
      }
      
      ++comptePomme;
      if (!(comptePomme % 2) && !pomme.set) { // ne pas faire réaparaitre la pomme immédiatement
        pomme.set = true;
        do {
          pomme.x = rand() % 8;
          pomme.y = rand() % 8;
        } while (matrix.test(pomme.x, pomme.y));
//        matrix.set(pomme.x, pomme.y);
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

  struct {
    byte x:3;
    byte y:3;
    bool set:1;
  } pomme = {0, 0, false};
  
};
