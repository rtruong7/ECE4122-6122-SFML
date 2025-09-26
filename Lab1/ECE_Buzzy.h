#pragma once
#include <SFML/Graphics.hpp>

class ECE_Buzzy : public sf::Sprite {
public:
    ECE_Buzzy() = default;

    void init(sf::Texture* tex, sf::Vector2f startPos);
    void updateFromInput(float dt, float speed, const sf::RenderWindow& window);

    bool collidesWith(const sf::FloatRect& other) const {
        return getGlobalBounds().intersects(other);
    }
};
