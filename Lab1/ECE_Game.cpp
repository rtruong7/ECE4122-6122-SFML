#include "ECE_Game.h"
#include <iostream>

ECE_Game::ECE_Game()
: m_window(sf::VideoMode(800, 600), "Buzzy Defender") {
    m_window.setFramerateLimit(120);

    // Load textures (exact filenames in Lab1/graphics/)
    if (!m_texBuzzy.loadFromFile("graphics/Buzzy_blue.png"))
        std::cerr << "Missing graphics/Buzzy_blue.png\n";
    if (!m_texEnemyA.loadFromFile("graphics/bulldog.png"))
        std::cerr << "Missing graphics/bulldog.png\n";
    if (!m_texEnemyB.loadFromFile("graphics/clemson_tigers.png"))
        std::cerr << "Missing graphics/clemson_tigers.png\n";
    m_hasStart = m_texStart.loadFromFile("graphics/Start_Screen.png");

    makeLaserTexture();

    // Optional HUD font; game runs without it
    m_font.loadFromFile("graphics/DejaVuSans.ttf");
    if (!m_font.getInfo().family.empty()) {
        m_text.setFont(m_font);
        m_text.setCharacterSize(22);
        m_text.setFillColor(sf::Color::White);
    }

    // Seed RNG for enemy firing
    m_rng.seed(std::random_device{}());
    m_nextEnemyFire = 1.2f;
    m_enemyFireClock.restart();

    resetRound();
}

void ECE_Game::makeLaserTexture() {
    // simple 6x16 white rectangle as the laser sprite
    sf::Image img;
    img.create(6, 16, sf::Color::White);
    m_texLaser.loadFromImage(img);
}

void ECE_Game::resetRound() {
    m_enemies.clear();
    m_blasts.clear();
    m_enemyDir = 1.f;

    // Buzzy starts in the UPPER middle per spec
    m_buzzy.init(&m_texBuzzy, { m_window.getSize().x * 0.5f, m_window.getSize().y * 0.18f });

    spawnEnemies();
}

void ECE_Game::spawnEnemies() {
    // Enemies start lower and MARCH UP when they hit a side
    const int rows = 5;
    const int cols = 8;
    const float spacingX = 90.f;
    const float spacingY = 70.f;
    const sf::Vector2f origin(90.f, 520.f);

    m_enemies.clear();
    m_enemies.reserve(rows * cols);
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            ECE_Enemy e;
            auto* tex = (r % 2 == 0) ? &m_texEnemyA : &m_texEnemyB;
            e.init(tex, { origin.x + c * spacingX, origin.y - r * spacingY });

            // scale down if large art
            const float targetW = 64.f;
            float scale = targetW / e.getLocalBounds().width;
            e.setScale(scale, scale);

            m_enemies.push_back(e);
        }
    }
}

void ECE_Game::run() {
    const float dt = 1.f / 120.f;
    sf::Clock clock;
    float acc = 0.f;

    while (m_window.isOpen()) {
        processEvents();
        acc += clock.restart().asSeconds();
        while (acc >= dt) {
            update(dt);
            acc -= dt;
        }
        render();
    }
}

void ECE_Game::processEvents() {
    sf::Event e;
    while (m_window.pollEvent(e)) {
        if (e.type == sf::Event::Closed) m_window.close();
    }
}

void ECE_Game::update(float dt) {
    if (m_state == State::Start) {
        // Press Enter to start a fresh round
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
            resetRound();
            m_nextEnemyFire = 1.2f;
            m_enemyFireClock.restart();
            m_state = State::Playing;
        }
        return;
    }

    // == Playing ==
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) { m_window.close(); return; }

    // Player movement + fire (player fires DOWN toward enemies)
    m_buzzy.updateFromInput(dt, m_playerSpeed, m_window);

    static bool prevSpace = false;
    bool spaceNow = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
    if (spaceNow && !prevSpace) {
        sf::Vector2f p = m_buzzy.getPosition();
        p.y += m_buzzy.getGlobalBounds().height * 0.5f;
        m_blasts.emplace_back(&m_texLaser, p, sf::Vector2f(0.f, 400.f), ECE_LaserBlast::Owner::Player);
    }
    prevSpace = spaceNow;

    // Enemy random firing (enemy fires UP toward Buzzy)
    if (m_enemyFireClock.getElapsedTime().asSeconds() >= m_nextEnemyFire) {
        enemyFireRandom();
        std::uniform_real_distribution<float> dist(m_enemyFireMin, m_enemyFireMax);
        m_nextEnemyFire = dist(m_rng);
        m_enemyFireClock.restart();
    }

    // Update blasts and prune offscreen
    for (auto& b : m_blasts) b.update(dt);
    for (auto it = m_blasts.begin(); it != m_blasts.end(); ) {
        if (it->isOffscreen(m_window)) it = m_blasts.erase(it); else ++it;
    }

    handleEnemyMovement(dt);
    handleCollisions();

    // Win/lose checks — immediately return to Start screen
    bool anyAlive = false;
    for (auto& e : m_enemies) if (e.alive) { anyAlive = true; break; }
    if (!anyAlive) {
        m_state = State::Start;        // auto back to Start
        return;
    }

    // Lose if an enemy overlaps Buzzy or climbs into Buzzy's row
    for (auto& e : m_enemies) {
        if (!e.alive) continue;
        if (e.collidesWith(m_buzzy.getGlobalBounds()) ||
            e.getPosition().y <= m_buzzy.getPosition().y + 30.f) {
            m_state = State::Start;    // auto back to Start
            return;
        }
    }
}

void ECE_Game::handleEnemyMovement(float dt) {
    bool hitEdge = false;
    for (auto& e : m_enemies)
        e.updateMarch(dt, m_enemyDir, hitEdge, m_window, m_enemySpeed);

    if (hitEdge) {
        m_enemyDir *= -1.f;
        for (auto& e : m_enemies) e.shiftUp(20.f);  // move UP each time a side is hit
    }
}

void ECE_Game::handleCollisions() {
    for (auto it = m_blasts.begin(); it != m_blasts.end(); ) {
        bool removed = false;

        if (it->owner() == ECE_LaserBlast::Owner::Player) {
            // Player blasts vs enemies
            for (auto& e : m_enemies) {
                if (!e.alive) continue;
                if (it->collidesWith(e.getGlobalBounds())) {
                    e.alive = false;               // enemy disappears
                    it = m_blasts.erase(it);
                    removed = true;
                    break;
                }
            }
        } else {
            // Enemy blasts vs Buzzy
            if (it->collidesWith(m_buzzy.getGlobalBounds())) {
                m_state = State::Start;            // hit → back to Start screen
                it = m_blasts.erase(it);
                removed = true;
            }
        }

        if (!removed) ++it;
    }
}

void ECE_Game::enemyFireRandom() {
    // collect indices of alive enemies
    std::vector<size_t> alive;
    alive.reserve(m_enemies.size());
    for (size_t i = 0; i < m_enemies.size(); ++i)
        if (m_enemies[i].alive) alive.push_back(i);
    if (alive.empty()) return;

    // pick one at random
    std::uniform_int_distribution<size_t> pick(0, alive.size() - 1);
    const ECE_Enemy& shooter = m_enemies[alive[pick(m_rng)]];

    sf::Vector2f p = shooter.getPosition();
    p.y -= shooter.getGlobalBounds().height * 0.5f;   // shoot up toward Buzzy
    m_blasts.emplace_back(&m_texLaser, p, sf::Vector2f(0.f, -300.f), ECE_LaserBlast::Owner::Enemy);
}

void ECE_Game::render() {
    m_window.clear(sf::Color(106,133,165));

    if (m_state == State::Start) {
        if (m_hasStart) {
            sf::Sprite splash(m_texStart);
            auto win = m_window.getSize();
            auto b = splash.getGlobalBounds();
            splash.setPosition((win.x - b.width)/2.f, (win.y - b.height)/2.f);
            m_window.draw(splash);
        }
        if (!m_font.getInfo().family.empty()) {
            m_text.setString("Press Enter to Start");
            m_text.setPosition(20.f, 20.f);
            m_window.draw(m_text);
        }
        m_window.display();
        return;
    }

    // == Playing ==
    for (auto& e : m_enemies) if (e.alive) m_window.draw(e);
    m_window.draw(m_buzzy);
    for (auto& b : m_blasts) m_window.draw(b);

    m_window.display();
}
