#include "imgui.h"
#include <SFML/Audio/Music.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/SoundSource.hpp>
#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>

#define MAX_HEIGHT 800
#define MAX_WIDTH 1000
#define GRAIN_SIZE 10
#define MAP_WIDTH MAX_WIDTH / GRAIN_SIZE
#define MAP_HEIGHT MAX_HEIGHT / GRAIN_SIZE

int main() {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    auto window =
        sf::RenderWindow({MAX_WIDTH, MAX_HEIGHT}, "CMake SFML Project",
                         sf::Style::Close, settings);
    window.setFramerateLimit(60);

    sf::SoundBuffer buffer1;
    sf::Sound sound_pop;
    if (!buffer1.loadFromFile("../pop.wav"))
        return -1;
    sound_pop.setBuffer(buffer1);
    sound_pop.setVolume(10);

    float volume_effects = 10, volume_music = 10;
    sf::SoundBuffer buffer;
    sf::Sound sound_grain;
    if (!buffer.loadFromFile("../pan.wav"))
        return -1;
    sound_grain.setBuffer(buffer);
    sound_grain.setVolume(volume_effects);

    sf::Music music;
    if (!music.openFromFile("../music.wav"))
        return -1;
    music.setLoop(true);
    music.setVolume(volume_music);
    music.play();

    ImGui::SFML::Init(window);

    sf::Clock clock;

    bool useMap1 = true;
    int map1[MAP_WIDTH][MAP_HEIGHT] = {0};
    int map2[MAP_WIDTH][MAP_HEIGHT] = {0};

    bool isButtonPressed = false;

    bool useRandomColor = false;
    int red, green, blue = 0;

    std::srand(time(NULL));

    while (window.isOpen()) {
        for (auto event = sf::Event(); window.pollEvent(event);) {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Right) {
                    isButtonPressed = true;
                    if (useRandomColor) {
                        red = std::rand() % 256;
                        green = std::rand() % 256;
                        blue = std::rand() % 256;
                    }
                }
            } else if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Right)
                    isButtonPressed = false;
            }
        }

        sf::Time elapsed = clock.restart();
        ImGui::SFML::Update(window, elapsed);

        ImGui::Begin("Tools");
        if (ImGui::Button("reset")) {
            sound_pop.play();
            red = 0;
            green = 0;
            blue = 0;
            for (int j = MAP_HEIGHT - 1; j >= 0; j--) {
                for (int i = 0; i < MAP_WIDTH; i++) {
                    map1[i][j] = 0;
                    map2[i][j] = 0;
                }
            }
        }
        ImGui::Text("Color");
        ImGui::Text("r: ");
        ImGui::SameLine();
        ImGui::SliderInt("##r", (int *)&red, 0, 255);
        ImGui::Text("g: ");
        ImGui::SameLine();
        ImGui::SliderInt("##g", (int *)&green, 0, 255);
        ImGui::Text("b: ");
        ImGui::SameLine();
        ImGui::SliderInt("##b", (int *)&blue, 0, 255);
        ImGui::Text("rand: ");
        ImGui::SameLine();
        ImGui::Checkbox("##random", &useRandomColor);
        ImGui::Text("Sound");
        ImGui::Text("fx: ");
        ImGui::SameLine();
        if (ImGui::SliderFloat("##effects", &volume_effects, 0, 100))
            sound_grain.setVolume(volume_effects);
        ImGui::Text("music: ");
        ImGui::SameLine();
        if (ImGui::SliderFloat("##music", &volume_music, 0, 100))
            music.setVolume(volume_music);
        ImGui::End();

        // Update here

        if (isButtonPressed) {
            int x = sf::Mouse::getPosition(window).x;
            int y = sf::Mouse::getPosition(window).y;

            if (x < 0)
                x = 0;
            else if (x >= MAX_WIDTH)
                x = MAX_WIDTH - 1;
            x = x % MAX_WIDTH / GRAIN_SIZE;

            if (y < 0)
                y = 0;
            else if (y >= MAX_HEIGHT)
                y = MAX_HEIGHT - 1;
            y = y % MAX_HEIGHT / GRAIN_SIZE;

            if (useMap1) {
                if (map1[x][y] == 0)
                    map1[x][y] = red | (green << 8) | (blue << 16) + 1;
            } else {
                if (map2[x][y] == 0)
                    map2[x][y] = red | (green << 8) | (blue << 16) + 1;
            }
        }

        for (int j = MAP_HEIGHT - 1; j >= 0; j--) {
            for (int i = 0; i < MAP_WIDTH; i++) {
                if (useMap1) {
                    if (map1[i][j]) {
                        if (j == MAP_HEIGHT - 1) {
                            map2[i][j] = map1[i][j];
                        } else if (map2[i][j + 1] == 0)
                            map2[i][j + 1] = map1[i][j];
                        else if (map2[i - 1][j + 1] == 0 && i - 1 > 0) {
                            map2[i - 1][j + 1] = map1[i][j];
                            if (sound_grain.getStatus() !=
                                sf::SoundSource::Playing)
                                sound_grain.play();
                        } else if (map2[i + 1][j + 1] == 0 &&
                                   i + 1 < MAP_WIDTH) {
                            map2[i + 1][j + 1] = map1[i][j];
                            if (sound_grain.getStatus() !=
                                sf::SoundSource::Playing)
                                sound_grain.play();
                        } else
                            map2[i][j] = map1[i][j];
                    }
                } else {
                    if (map2[i][j]) {
                        if (j == MAP_HEIGHT - 1) {
                            map1[i][j] = map2[i][j];
                        } else if (map1[i][j + 1] == 0)
                            map1[i][j + 1] = map2[i][j];
                        else if (map1[i + 1][j + 1] == 0 && i + 1 < MAP_WIDTH)
                            map1[i + 1][j + 1] = map2[i][j];
                        else if (map1[i - 1][j + 1] == 0 && i - 1 > 0)
                            map1[i - 1][j + 1] = map2[i][j];
                        else
                            map1[i][j] = map2[i][j];
                    }
                }
            }
        }
        useMap1 = !useMap1;

        // Draw here
        window.clear();
        for (int i = 0; i < MAP_WIDTH; i++) {
            for (int j = 0; j < MAP_HEIGHT; j++) {
                if (useMap1) {
                    if (map1[i][j]) {
                        sf::RectangleShape grain = sf::RectangleShape(
                            sf::Vector2f(GRAIN_SIZE, GRAIN_SIZE));
                        grain.setPosition(
                            sf::Vector2f(i * GRAIN_SIZE, j * GRAIN_SIZE));
                        grain.setFillColor(sf::Color(
                            map1[i][j] & 0xFF, (map1[i][j] >> 8) & 0xFF,
                            (map1[i][j] >> 16) & 0xFF));
                        window.draw(grain);
                    }
                    map2[i][j] = 0;
                } else {
                    if (map2[i][j]) {
                        sf::RectangleShape grain = sf::RectangleShape(
                            sf::Vector2f(GRAIN_SIZE, GRAIN_SIZE));
                        grain.setPosition(
                            sf::Vector2f(i * GRAIN_SIZE, j * GRAIN_SIZE));
                        grain.setFillColor(sf::Color(
                            map2[i][j] & 0xFF, (map2[i][j] >> 8) & 0xFF,
                            (map2[i][j] >> 16) & 0xFF));
                        window.draw(grain);
                    }
                    map1[i][j] = 0;
                }
            }
        }

        ImGui::SFML::Render(window);

        window.display();
    }

    ImGui::SFML::Shutdown();
}
