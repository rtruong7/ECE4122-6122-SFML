#pragma once
#include <SFML/Graphics.hpp>

class ECE_Enemy : public sf::Sprite {
public:
    ECE_Enemy() = default;

    void init(sf::Texture* tex, sf::Vector2f startPos);
    void updateMarch(float dt, float dir, bool& hitEdge, const sf::RenderWindow& window, float speed);
    void shiftUp(float dy);
    bool collidesWith(const sf::FloatRect& other) const { return getGlobalBounds().intersects(other); }

    bool alive{true};
};
