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

#include "Matrix.h"

class SpaceInvaders {
public:
  SpaceInvaders(Matrix& aMatrix);

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
    return false;
  }  

private:
  Matrix& matrix;  
};

SpaceInvaders::SpaceInvaders(Matrix& aMatrix) :
  matrix(aMatrix)
{}
