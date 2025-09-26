#include "ECE_Buzzy.h"

void ECE_Buzzy::init(sf::Texture* tex, sf::Vector2f startPos) {
    setTexture(*tex);

    // Center origin using unscaled local bounds
    auto b = getLocalBounds();
    setOrigin(b.width / 2.f, b.height / 2.f);

    // Scale Buzzy so he isn't enormous (target ~64 px width)
    const float targetW = 64.f;
    const float scale   = targetW / b.width;
    setScale(scale, scale);

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
