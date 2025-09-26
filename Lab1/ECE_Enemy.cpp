#include "ECE_Enemy.h"

void ECE_Enemy::init(sf::Texture* tex, sf::Vector2f startPos) {
    setTexture(*tex);
    setOrigin(getLocalBounds().width/2.f, getLocalBounds().height/2.f);
    setPosition(startPos);
    alive = true;
}

void ECE_Enemy::updateMarch(float dt, float dir, bool& hitEdge, const sf::RenderWindow& window, float speed) {
    if (!alive) return;
    move(dir * speed * dt, 0.f);

    auto b = getGlobalBounds();
    if (b.left < 0.f || b.left + b.width > window.getSize().x) {
        hitEdge = true;
    }
}

void ECE_Enemy::shiftUp(float dy) {
    move(0.f, -dy); // SFML Y-axis: negative is up
}
