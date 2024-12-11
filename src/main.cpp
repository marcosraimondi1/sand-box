#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>

#define MAX_HEIGHT 800
#define MAX_WIDTH 1000
#define GRAIN_SIZE 5
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

    int map1[MAP_WIDTH][MAP_HEIGHT] = {0};

    bool isButtonPressed = false;
    bool isRestartPressed = false;
    bool gravityEnabled = true;

    bool useRandomColor = false;
    int red, green, blue = 0;

    int bucketSize = 1;

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
        if (ImGui::Button("flush")) {
            if (sound_pop.getStatus() != sf::SoundSource::Playing)
                sound_pop.play();
            isRestartPressed = !isRestartPressed;
        }
        ImGui::SameLine();
        ImGui::Text("gravity:");
        ImGui::SameLine();
        ImGui::Checkbox("##grav", &gravityEnabled);
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
        ImGui::Text("Bucket Size");
        ImGui::SliderInt("##s", &bucketSize, 1, 5);
        ImGui::Text("Sound");
        ImGui::Text("fx: ");
        ImGui::SameLine();
        if (ImGui::SliderFloat("##effects", &volume_effects, 0, 100)) {
            sound_grain.setVolume(volume_effects);
            sound_pop.setVolume(volume_effects);
        }
        ImGui::Text("music: ");
        ImGui::SameLine();
        if (ImGui::SliderFloat("##music", &volume_music, 0, 100)) {
            music.setVolume(volume_music);
        }
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

            for (int i = -bucketSize + 1; i < bucketSize; i++) {
                for (int j = -bucketSize + 1; j < bucketSize; j++) {
                    if (x + i * 2 < 0 || x + i * 2 >= MAX_WIDTH ||
                        y + j * 2 < 0 || y + j * 2 >= MAX_HEIGHT)
                        continue;
                    if (map1[x + i * 2][y + j * 2] == 0) {
                        int mod = (i == 0 && j == 0) ? 0 : (bucketSize + i) * 4;
                        int red_c = red + mod;
                        int green_c = green + mod;
                        int blue_c = blue + mod;
                        int mods = (gravityEnabled ? 0 : 1) << 24;

                        map1[x + i * 2][y + j * 2] =
                            red_c | (green_c << 8) | (blue_c << 16) | mods + 1;
                    }
                }
            }
        }

        for (int j = MAP_HEIGHT - 1; j >= 0; j--) {
            for (int i = 0; i < MAP_WIDTH; i++) {
                if (map1[i][j]) {
                    if (map1[i][j] & (1 << 24) && !isRestartPressed) {
                        // gravity disabled
                        continue;
                    }
                    if (j == MAP_HEIGHT - 1) {
                        map1[i][j] = isRestartPressed ? 0 : map1[i][j];
                    } else if (map1[i][j + 1] == 0) {
                        map1[i][j + 1] = map1[i][j];
                        map1[i][j] = 0;

                    } else if (map1[i - 1][j + 1] == 0 && i - 1 > 0) {
                        map1[i - 1][j + 1] = map1[i][j];
                        map1[i][j] = 0;
                        if (sound_grain.getStatus() != sf::SoundSource::Playing)
                            sound_grain.play();
                    } else if (map1[i + 1][j + 1] == 0 && i + 1 < MAP_WIDTH) {
                        map1[i + 1][j + 1] = map1[i][j];
                        map1[i][j] = 0;
                        if (sound_grain.getStatus() != sf::SoundSource::Playing)
                            sound_grain.play();
                    } else
                        map1[i][j] = isRestartPressed ? 0 : map1[i][j];
                }
            }
        }

        // Draw here
        window.clear();
        for (int i = 0; i < MAP_WIDTH; i++) {
            for (int j = 0; j < MAP_HEIGHT; j++) {
                if (map1[i][j]) {
                    sf::RectangleShape grain = sf::RectangleShape(
                        sf::Vector2f(GRAIN_SIZE, GRAIN_SIZE));
                    grain.setPosition(
                        sf::Vector2f(i * GRAIN_SIZE, j * GRAIN_SIZE));
                    grain.setFillColor(sf::Color(map1[i][j] & 0xFF,
                                                 (map1[i][j] >> 8) & 0xFF,
                                                 (map1[i][j] >> 16) & 0xFF));
                    window.draw(grain);
                }
            }
        }

        ImGui::SFML::Render(window);

        window.display();
    }

    ImGui::SFML::Shutdown();
}

void updateMap(int map[MAP_WIDTH][MAP_HEIGHT]) {}
