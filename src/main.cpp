#include "util/cl.hpp"
#include "util/part.hpp"
#include <SFML/Graphics.hpp>
#include <time.h>

#define SIZE 10000
#define MASS 5e12

int main() {

  sf::RenderWindow window(sf::VideoMode(1920, 1080), "Particle Sim");

  Particle p(SIZE, MASS);

  CLUtil cl;

  while (window.isOpen()) {
    sf::Event event;

    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    window.clear(sf::Color::Black);

    // Calculate the next frame and update particle pos/vel

    cl.runner(p);

    // Draw the particles

    sf::CircleShape particle;
    particle.setRadius(1.0);


    for (size_t i = 0; i < SIZE; i++) {
      particle.setFillColor(sf::Color(i % 256, i*2 % 256, i*i % 256));
      particle.setPosition(p.xpos[i], p.ypos[i]);
      window.draw(particle);
    }

    window.display();
  }

  return 0;
}
