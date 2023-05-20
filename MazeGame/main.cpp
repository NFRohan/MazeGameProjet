#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <string>
#include <sstream>
#include <fstream>
using namespace sf;
using namespace std;

int main() {
    RenderWindow window(VideoMode(600, 600), "game");
    srand(sin(time(nullptr)) * 1000);
    int frameCount = 0;
    int moveTimer = 1000;

    bool updatePath = true;
    int orderedSet[15 * 15];
    int path[100];
    int pathSize = 0;
    int pathPos = 0;

    Vector2i player = Vector2i(13, 13);
    Texture playerTex;
    playerTex.loadFromFile("self.png");
    Sprite playerSprite;
    playerSprite.setTexture(playerTex);
    playerSprite.setPosition(player.x * 40.f, player.y * 40.f);

    Vector2i opponent = Vector2i(1, 1);
    Texture opponentTex;
    opponentTex.loadFromFile("walk2.png");
    Sprite opponentSprite;
    opponentSprite.setTexture(opponentTex);
    opponentSprite.setPosition(opponent.x * 40.f, opponent.y * 40.f);

    Texture coinTex;
    coinTex.loadFromFile("coin.png");
    Sprite coins[5];
    int numCoins = 0;
    int score = 0;

    int gameMap[15 * 15];
    RectangleShape displayRects[15 * 15];

    Font f;
    f.loadFromFile("DMSans.ttf");
    std::ostringstream oss;
    oss << "SCORE: " << to_string(score);
    std::string var = oss.str();
    Text scoreboard;
    scoreboard.setFont(f);
    scoreboard.setCharacterSize(20);
    scoreboard.setString(var);

    ofstream file;

    for (int i = 0; i < 15; i++) {
        for (int j = 0; j < 15; j++) {
            displayRects[i + j * 15].setPosition(i * 40.f, j * 40.f);
            displayRects[i + j * 15].setSize(Vector2f(40.f, 40.f));
            displayRects[i + j * 15].setOutlineThickness(1.f);
            displayRects[i + j * 15].setOutlineColor(Color(0, 0, 0));

            if (!(i == opponent.x && j == opponent.y) && !(i == player.x && j == player.y)) {
                if (rand() / (float)RAND_MAX < 0.22f || i == 0 || j == 0 || i == 14 || j == 14) {
                    gameMap[i + j * 15] = 1;
                    displayRects[i + j * 15].setFillColor(Color(0, 0, 0));
                }
            }
        }
    }



    
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }
            else if (event.type == Event::KeyPressed) {
                switch (event.key.code) {
                case Keyboard::Up:
                    if (gameMap[player.x + (player.y - 1) * 15] != 1) player.y -= 1;
                    break;
                case Keyboard::Down:
                    if (gameMap[player.x + (player.y + 1) * 15] != 1) player.y += 1;
                    break;
                case Keyboard::Right:
                    if (gameMap[(player.x + 1) + player.y * 15] != 1) player.x += 1;
                    break;
                case Keyboard::Left:
                    if (gameMap[(player.x - 1) + player.y * 15] != 1) player.x -= 1;
                    break;
                }
            }

            updatePath = true;
            pathSize = 0;
            pathPos = 0;
            playerSprite.setPosition(player.x * 40.f, player.y * 40.f);
        }

        while (numCoins < 3) {
            int col = (rand() / (float)RAND_MAX) * 15.f;
            int row = (rand() / (float)RAND_MAX) * 15.f;

            if (gameMap[col + row * 15] == 0) {
                gameMap[col + row * 15] = 2;
                coins[numCoins].setTexture(coinTex);
                coins[numCoins].setPosition(col * 40.f, row * 40.f);
                numCoins += 1;
            }
        }

        window.clear(Color(255, 255, 255));

        for (int i = 0; i < 15 * 15; i++) {
            window.draw(displayRects[i]);
        }

        for (int i = 0; i < numCoins; i++) {
            window.draw(coins[i]);
        }

        if (updatePath == true) {
            int counter = 0;

            int fullSet[2000];
            int fullSetSize = 0;

            int openSet[100];
            int openSetSize = 2;
            openSet[0] = player.x + player.y * 15;
            openSet[1] = counter;

            int currentIndex = player.x + player.y * 15;

            while (currentIndex != opponent.x + opponent.y * 15) {
                currentIndex = openSet[0];
                counter = openSet[1] + 1;
                int neighbors[4];

                neighbors[0] = currentIndex - 1;
                neighbors[1] = currentIndex + 1;
                neighbors[2] = currentIndex - 15;
                neighbors[3] = currentIndex + 15;

                for (int i = 0; i < 8; i += 2) {
                    bool alreadyExists = false;

                    for (int j = 0; j < fullSetSize; j += 2) {
                        if (neighbors[i / 2] == fullSet[j]) {
                            alreadyExists = true;
                            break;
                        }
                    }

                    if (alreadyExists == false) {
                        if (gameMap[neighbors[i / 2]] != 1) {
                            fullSet[fullSetSize] = neighbors[i / 2];
                            fullSet[fullSetSize + 1] = counter;
                            fullSetSize += 2;

                            openSet[openSetSize] = neighbors[i / 2];
                            openSet[openSetSize + 1] = counter;
                            openSetSize += 2;
                        }
                        else {
                            fullSet[fullSetSize] = neighbors[i / 2];
                            fullSet[fullSetSize + 1] = 100000;
                            fullSetSize += 2;
                        }
                    }
                }

                for (int i = 0; i < openSetSize; i++) {
                    openSet[i] = openSet[i + 2];
                }

                openSetSize -= 2;
            }

            for (int i = 0; i < 15 * 15; i++) {
                orderedSet[i] = 100000;
            }

            for (int i = 0; i < fullSetSize; i += 2) {
                orderedSet[fullSet[i]] = fullSet[i + 1];
            }

            orderedSet[player.x + player.y * 15] = 0;

            int pathIndex = opponent.x + opponent.y * 15;

            while (pathIndex != player.x + player.y * 15) {
                int neighbors[4];

                neighbors[0] = pathIndex - 1;
                neighbors[1] = pathIndex + 1;
                neighbors[2] = pathIndex - 15;
                neighbors[3] = pathIndex + 15;

                int lowest[2]{ 0, 100000 };
                for (int i = 0; i < 4; i++) {
                    if (orderedSet[neighbors[i]] < lowest[1]) {
                        lowest[0] = i;
                        lowest[1] = orderedSet[neighbors[i]];
                    }
                }

                pathIndex = neighbors[lowest[0]];

                path[pathSize] = pathIndex;

                pathSize += 1;
            }

            updatePath = false;
        }

        if (frameCount % moveTimer == 0) {
            opponent.x = path[pathPos] % 15;
            opponent.y = floor(path[pathPos] / 15);
            opponentSprite.setPosition(opponent.x * 40.f, opponent.y * 40.f);
            pathPos += 1;

            if (moveTimer >= 250) {
                moveTimer = 1000 - floor(frameCount / 1000) * 4;
            }
        }

        window.draw(playerSprite);
        window.draw(opponentSprite);
        
        
        std::ostringstream oss;
        oss.str("");

        oss << "SCORE: " << to_string(score);
        std::string var = oss.str();;
        scoreboard.setString(var);
        window.draw(scoreboard);
        window.display();

        if (gameMap[player.x + player.y * 15] == 2) {
            gameMap[player.x + player.y * 15] = 0;

            int coinIndex = -1;
            for (int i = 0; i < numCoins; i++) {
                if (coins[i].getPosition().x / 40 == player.x && coins[i].getPosition().y / 40 == player.y) {
                    coinIndex = i;
                }
            }

            if (coinIndex != numCoins - 1) {
                coins[coinIndex] = coins[numCoins - 1];
            }

            score += 1;
            numCoins -= 1;
        }

        frameCount += 1;

        if (player == opponent) {
            cout << "Final Score: " << score << endl;
            
            file.open("filePath.txt", ios::app);
            file << score << " ";
            file.close();
            return 0;
        }
    }
    return 0;
}
