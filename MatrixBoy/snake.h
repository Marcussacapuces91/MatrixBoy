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

    srand(micros());
    
    return true;
  }

/**
 * Méthode exécutée en boucle par l'instance.
 * @return un booléen qui indique le bon déroulement de la méthode. 
 *         En cas de retour FALSE, il ne faut plus appeler cette méthode (fin du jeu par exemple).
 */
  boolean loop() {
    unsigned tot = 0;

    for (byte p = 3; p > 0; --p) {
      while (!matrix.pressA(String(p) + String(F(" snake")) + ( p > 1 ? String('s') : String() ) + String(F(" - [A] start... ")))) ;

      const auto pts = partie(500);

      tone(Matrix::TONE_PIN, NOTE_C4, 250);
      delay(350);
      tone(Matrix::TONE_PIN, NOTE_B3, 250);
      delay(350);
      tone(Matrix::TONE_PIN, NOTE_A3, 500);
      matrix.clear(true);
      delay(75);
      matrix.clear();
        
      while (!matrix.pressA( String(4-p) + String(F(". ")) + String(pts) + String(F(" pts. [A] play again... ")) )) ;
      tot += pts;
    }
    while (!matrix.pressA( String(F("Total ")) + String(tot) + String(F(" pts. [A] End... ")) )) ;

    return false;
  }

protected:
/**
 * Play a set and return the result.
 * @param speed delay (ms) between each movement.
 * @return points value.
 */
  unsigned partie(const unsigned speed = 500) {
    unsigned pts = 0;
    int8_t dx = -1;
    int8_t dy = 0;

    auto delayPomme = millis();
    auto delayMove = millis();
    unsigned long delayTone = 0; // OFF

    bool light = false;
  
    serpent.last = 0;
    serpent.headX = 4;
    serpent.headY = 4;
    
    pomme.set = false;

    while (true) {

// Test buttons & change direction
      const auto b = matrix.button();
      if ((b & Matrix::UP) && !(b & Matrix::DOWN)) { dx = 0; dy = +1; }
      else if (!(b & Matrix::UP) && (b & Matrix::DOWN)) { dx = 0; dy = -1; };
      if ((b & Matrix::LEFT) && !(b & Matrix::RIGHT)) { dx = -1; dy = 0; }
      else if (!(b & Matrix::LEFT) && (b & Matrix::RIGHT)) { dx = +1; dy = 0; };
    
// Current time
      const auto ct = millis();

// Stop sound after 50ms
      if ( delayTone && ((ct - delayTone) > 50) ) {
        noTone(Matrix::TONE_PIN);
        delayTone = 0;
      }

// Blink Apple, if set all 100 ms
      if ((ct - delayPomme > 100) && pomme.set) {
        delayPomme = ct;
        if (light) {
          matrix.set(pomme.x, pomme.y);
        } else {
          matrix.unSet(pomme.x, pomme.y);
        }
        light = !light;
      }

// Every "speed" ms move Snake    
      if (ct - delayMove > speed) {
        delayMove = ct;

  // Clear existing snake
  ///@todo à priori juste le dernier pixel à supprimer !!!
        byte x = serpent.headX;
        byte y = serpent.headY;
/*
        matrix.unSet(x, y);
        for (byte i = 0; i < serpent.len(); ++i) {
          serpent.diff(i, x, y);
          matrix.unSet(x, y);
        }
*/       
        for (byte i = 0; i < serpent.len(); ++i) {
          serpent.diff(i, x, y);
        }
        matrix.unSet(x, y);
        
  // Move snake
        serpent.move(dx, dy);
  
  // Display snake
  ///@todo rajouter la tête & vérifier s'il faut finalement prolonger la queue
        x = serpent.headX;
        y = serpent.headY;
        matrix.set(x, y);
        for (byte i = 0; i < serpent.len(); ++i) {
          serpent.diff(i, x, y);
          matrix.set(x, y);
          if (serpent.headX == x && serpent.headY == y) { // body crossing
            return pts;
          }
        }
  
        if (pomme.set) {
  // Tête serpent sur la pomme
          if ( (serpent.headX == pomme.x) && (serpent.headY == pomme.y) ) {
            tone(Matrix::TONE_PIN, NOTE_B0);
            delayTone = ct;
            pomme.set = false;
            ++serpent.last;
            ++pts;
          }
        } else {
          pomme.set = true;
          do {
            pomme.x = rand() % 8;
            pomme.y = rand() % 8;
          } while (matrix.test(pomme.x, pomme.y));
        }
      }

      yield();
    }    
  }

private:
/// Une référence vers la matrice de Leds.
  Matrix& matrix;

/// Structure et variable du serpent.
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
    
  } serpent; //  = { 4, 4, 0, { 0, 0, 0, 0 } };

/// Structure et variable de la pomme.
  struct {
    byte x:3;
    byte y:3;
    bool set:1;
  } pomme; // = {0, 0, false};
  
};
