#include "ECE_Buzzy.h"

void ECE_Buzzy::init(sf::Texture* tex, sf::Vector2f startPos) {
    setTexture(*tex);
    setOrigin(getLocalBounds().width/2.f, getLocalBounds().height/2.f);
    setPosition(startPos);
}

void ECE_Buzzy::updateFromInput(float dt, float speed, const sf::RenderWindow& window) {
    float dir = 0.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  dir -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) dir += 1.f;

    move(dir * speed * dt, 0.f);

    // clamp to window width
    float half = getGlobalBounds().width / 2.f;
    auto sz = window.getSize();
    auto p  = getPosition();
    if (p.x < half) p.x = half;
    if (p.x > sz.x - half) p.x = sz.x - half;
    setPosition(p);
}
