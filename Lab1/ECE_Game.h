#pragma once
#include <SFML/Graphics.hpp>
#include <list>
#include <vector>
#include <random>                 // RNG for enemy firing
#include "ECE_Buzzy.h"
#include "ECE_LaserBlast.h"
#include "ECE_Enemy.h"

class ECE_Game {
public:
    ECE_Game();
    void run();

private:
    enum class State { Start, Playing };

    void processEvents();
    void update(float dt);
    void render();

    void resetRound();
    void spawnEnemies();
    void handleEnemyMovement(float dt);
    void handleCollisions();
    void enemyFireRandom();       // enemies shoot using same LaserBlast
    void makeLaserTexture();

private:
    sf::RenderWindow m_window;
    State m_state{State::Start};

    // Assets
    sf::Texture m_texBuzzy;
    sf::Texture m_texEnemyA;
    sf::Texture m_texEnemyB;
    sf::Texture m_texLaser;
    sf::Texture m_texStart;
    bool        m_hasStart{false};

    sf::Font m_font;
    sf::Text m_text;

    // World
    ECE_Buzzy m_buzzy;
    std::vector<ECE_Enemy> m_enemies;
    std::list<ECE_LaserBlast> m_blasts;   // both player & enemy blasts

    // Params
    float m_enemyDir   = 1.f;     // +1 right, -1 left
    float m_playerSpeed= 350.f;
    float m_enemySpeed = 60.f;

    // Enemy firing control
    std::mt19937  m_rng;
    sf::Clock     m_enemyFireClock;
    float         m_enemyFireMin = 1.0f;  // seconds
    float         m_enemyFireMax = 2.5f;  // seconds
    float         m_nextEnemyFire = 1.2f; // seconds
};
