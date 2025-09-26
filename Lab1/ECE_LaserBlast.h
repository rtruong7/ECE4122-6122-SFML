#pragma once
#include <SFML/Graphics.hpp>

class ECE_LaserBlast : public sf::Sprite {
public:
    enum class Owner { Player, Enemy };

    ECE_LaserBlast() = default;
    ECE_LaserBlast(sf::Texture* tex, sf::Vector2f startPos, sf::Vector2f velocity, Owner who);

    void update(float dt);
    bool isOffscreen(const sf::RenderWindow& window) const;
    bool collidesWith(const sf::FloatRect& other) const {
        return getGlobalBounds().intersects(other);
    }

    Owner owner() const { return m_owner; }

private:
    sf::Vector2f m_vel{0.f, -400.f};
    Owner m_owner{Owner::Player};
};
