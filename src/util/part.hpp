#pragma once

#include <cmath>
#include <time.h>


class Particle {
public:
  float mass;
  float *xpos;
  float *ypos;
  float *xvelocity;
  float *yvelocity;
  int size;

  Particle(size_t amount, float masss) {
    mass = masss;
    xpos = new float[amount];
    ypos = new float[amount];
    xvelocity = new float[amount];
    yvelocity = new float[amount];
    size = amount;

    srand(time(NULL));

    float radius = 800;
    
    for(size_t i = 0; i < amount; i++){
      xvelocity[i] = 0.f;
      yvelocity[i] = 0.f;

      float angle = (rand() % 360) * (M_PI / 180); // Convert degrees to radians
      float r = (rand() % static_cast<int>(radius)) + 1.0f; // Random radius within the circle

      // Convert polar coordinates to Cartesian coordinates
      float x = 960 + r * cos(angle);
      float y = 540 + r * sin(angle);

      xpos[i] = x;
      ypos[i] = y;
    }
  }

  ~Particle() {
    delete[] xpos;
    delete[] ypos;
    delete[] xvelocity;
    delete[] yvelocity;
}
};
