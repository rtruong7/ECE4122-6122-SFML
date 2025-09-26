#include "ECE_LaserBlast.h"

ECE_LaserBlast::ECE_LaserBlast(sf::Texture* tex, sf::Vector2f startPos, sf::Vector2f velocity, Owner who)
: m_vel(velocity), m_owner(who) {
    setTexture(*tex);
    setOrigin(getLocalBounds().width/2.f, getLocalBounds().height/2.f);
    setPosition(startPos);
    setScale(0.7f, 0.7f);
}

void ECE_LaserBlast::update(float dt) {
    move(m_vel * dt);
}

bool ECE_LaserBlast::isOffscreen(const sf::RenderWindow& window) const {
    auto b = getGlobalBounds();
    return (b.top + b.height < 0) || (b.top > window.getSize().y);
}
