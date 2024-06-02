#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <SFML/Audio.hpp>

class Song {
public:
    std::string name;
    std::string filePath;
    Song* next;
    Song* prev;

    Song(const std::string& n, const std::string& f) : name(n), filePath(f), next(nullptr), prev(nullptr) {}
};

class Playlist {
private:
    Song* head;
    Song* tail;
    Song* current;

public:
    Playlist() : head(nullptr), tail(nullptr), current(nullptr) {}
    ~Playlist() {
        Song* temp = head;
        while (temp) {
            Song* next = temp->next;
            delete temp;
            temp = next;
        }
    }

    void addSong(const std::string& name, const std::string& filePath) {
        Song* newSong = new Song(name, filePath);
        if (!head) {
            head = tail = current = newSong;
        }
        else {
            tail->next = newSong;
            newSong->prev = tail;
            tail = newSong;
        }
    }

    void nextSong() {
        if (current && current->next) {
            current = current->next;
        }
    }

    void previousSong() {
        if (current && current->prev) {
            current = current->prev;
        }
    }

    std::string getCurrentSong() const {
        if (current) {
            return current->filePath;
        }
        return "";
    }

    std::string getCurrentSongName() const {
        if (current) {
            return current->name;
        }
        return "";
    }
};

class AudioPlayer {
private:
    sf::Music music;
    float volume;

public:
    AudioPlayer() : volume(100.0f) {}

    bool play(const std::string& filePath) {
        if (music.openFromFile(filePath)) {
            music.setVolume(volume);
            music.play();
            return true;
        }
        return false;
    }

    void stop() {
        music.stop();
    }

    void pause() {
        music.pause();
    }

    void resume() {
        music.play();
    }

    bool isPlaying() const {
        return music.getStatus() == sf::SoundSource::Playing;
    }

    bool isPaused() const {
        return music.getStatus() == sf::SoundSource::Paused;
    }

    float getCompletionPercentage() const {
        if (isPlaying() || isPaused()) {
            return music.getPlayingOffset().asSeconds() / music.getDuration().asSeconds();
        }
        return 0.0f;
    }

    void increaseVolume() {
        if (volume < 100.0f) {
            volume += 10.0f;
            if (volume > 100.0f)
                volume = 100.0f;
            music.setVolume(volume);
        }
    }

    void decreaseVolume() {
        if (volume > 0.0f) {
            volume -= 10.0f;
            if (volume < 0.0f)
                volume = 0.0f;
            music.setVolume(volume);
        }
    }

    float getVolume() const {
        return volume;
    }
};

int main() {
    Playlist playlist;
    AudioPlayer audioPlayer;

    sf::RenderWindow window(sf::VideoMode(800, 600), "BU Music Player");

    // Define buttons
    sf::RectangleShape addButton(sf::Vector2f(200, 50));
    addButton.setPosition(50, 100);
    addButton.setFillColor(sf::Color::Black);

    sf::RectangleShape nextButton(sf::Vector2f(200, 50));
    nextButton.setPosition(50, 200);
    nextButton.setFillColor(sf::Color::Black);

    sf::RectangleShape prevButton(sf::Vector2f(200, 50));
    prevButton.setPosition(50, 300);
    prevButton.setFillColor(sf::Color::Black);

    sf::RectangleShape exitButton(sf::Vector2f(200, 50));
    exitButton.setPosition(50, 400);
    exitButton.setFillColor(sf::Color::Red);

    sf::RectangleShape pauseButton(sf::Vector2f(200, 50));
    pauseButton.setPosition(300, 100);
    pauseButton.setFillColor(sf::Color::Black);

    sf::RectangleShape playButton(sf::Vector2f(200, 50));
    playButton.setPosition(300, 200);
    playButton.setFillColor(sf::Color::Black);

    // Volume control buttons
    sf::RectangleShape volumeUpButton(sf::Vector2f(85, 50));
    volumeUpButton.setPosition(600, 100);
    volumeUpButton.setFillColor(sf::Color::Black);

    sf::RectangleShape volumeDownButton(sf::Vector2f(85, 50));
    volumeDownButton.setPosition(600, 200);
    volumeDownButton.setFillColor(sf::Color::Black);

    // Progress bar background
    sf::RectangleShape progressBarBackground(sf::Vector2f(700, 20)); // Reduced width to fit within window
    progressBarBackground.setPosition(50, 50); // Positioning below buttons
    progressBarBackground.setOutlineThickness(3); // Set the thickness of the outline
    progressBarBackground.setOutlineColor(sf::Color::Black); // Set the color of the outline
    progressBarBackground.setFillColor(sf::Color::Transparent); // Set the fill color to transparent

    // Progress bar
    sf::RectangleShape progressBar(sf::Vector2f(0, 20)); // Width will be updated dynamically
    progressBar.setPosition(50, 50); // Positioning below buttonss
    progressBar.setFillColor(sf::Color::Black); // Set the fill color

    // Define text for buttons

    sf::Font font;
    if (!font.loadFromFile("DejaVuSans.ttf")) {
        std::cerr << "Font not found!" << std::endl;
        return 1;
    }

    sf::Font font1;
    if (!font1.loadFromFile("FuturaPTBold.otf")) {
        std::cerr << "Font not found!" << std::endl;
        return 1;
    }

    sf::Text titleText("BU Music Player", font1, 22);
    titleText.setFillColor(sf::Color::Black);
    titleText.setPosition(window.getSize().x - titleText.getLocalBounds().width - 10, window.getSize().y - titleText.getLocalBounds().height - 10);
    


    sf::Text addText("Add Song", font, 24);
    addText.setPosition(70, 110);
    addText.setFillColor(sf::Color::White);

    sf::Text nextText("Next Song", font, 24);
    nextText.setPosition(70, 210);
    nextText.setFillColor(sf::Color::White);

    sf::Text prevText("Previous Song", font, 24);
    prevText.setPosition(70, 310);
    prevText.setFillColor(sf::Color::White);

    sf::Text exitText("Exit", font, 24);
    exitText.setPosition(70, 410);
    exitText.setFillColor(sf::Color::White);

    sf::Text pauseText("Pause", font, 24);
    pauseText.setPosition(320, 110);
    pauseText.setFillColor(sf::Color::White);

    sf::Text playText("Play", font, 24);
    playText.setPosition(320, 210);
    playText.setFillColor(sf::Color::White);

    sf::Text volumeUpText("Up", font, 24);
    volumeUpText.setPosition(610, 110);
    volumeUpText.setFillColor(sf::Color::White);

    sf::Text volumeDownText("Down", font, 24);
    volumeDownText.setPosition(610, 210);
    volumeDownText.setFillColor(sf::Color::White);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Button clicks
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                if (addButton.getGlobalBounds().contains(mousePos)) {
                    // Add song logic
                    std::string name, filePath;
                    std::cout << "Enter song name: ";
                    std::cin >> name;
                    std::cout << "Enter song file path: ";
                    std::cin >> filePath;
                    playlist.addSong(name, filePath);
                }
                else if (nextButton.getGlobalBounds().contains(mousePos)) {
                    // Play next song logic
                    if (playlist.getCurrentSong().empty()) {
                        std::cout << "No song to play. Add a song first.\n";
                        continue;
                    }
                    audioPlayer.stop();
                    playlist.nextSong();
                    if (!audioPlayer.play(playlist.getCurrentSong())) {
                        std::cout << "Failed to play song: " << playlist.getCurrentSongName() << "\n";
                    }
                    else {
                        std::cout << "Playing: " << playlist.getCurrentSongName() << "\n";
                    }
                }
                else if (prevButton.getGlobalBounds().contains(mousePos)) {
                    // Play previous song logic
                    if (playlist.getCurrentSong().empty()) {
                        std::cout << "No song to play. Add a song first.\n";
                        continue;
                    }
                    audioPlayer.stop();
                    playlist.previousSong();
                    if (!audioPlayer.play(playlist.getCurrentSong())) {
                        std::cout << "Failed to play song: " << playlist.getCurrentSongName() << "\n";
                    }
                    else {
                        std::cout << "Playing: " << playlist.getCurrentSongName() << "\n";
                    }
                }
                else if (volumeUpButton.getGlobalBounds().contains(mousePos)) {
                    // Volume up logic
                    audioPlayer.increaseVolume();
                    std::cout << "Volume increased to: " << audioPlayer.getVolume() << "\n";
                }
                else if (volumeDownButton.getGlobalBounds().contains(mousePos)) {
                    // Volume down logic
                    audioPlayer.decreaseVolume();
                    std::cout << "Volume decreased to: " << audioPlayer.getVolume() << "\n";
                }
                else if (exitButton.getGlobalBounds().contains(mousePos)) {
                    window.close();
                }
                else if (pauseButton.getGlobalBounds().contains(mousePos)) {
                    // Pause song logic
                    if (audioPlayer.isPlaying()) {
                        audioPlayer.pause();
                        std::cout << "Paused: " << playlist.getCurrentSongName() << "\n";
                    }
                }
                else if (playButton.getGlobalBounds().contains(mousePos)) {
                    // Play/resume song logic
                    if (!audioPlayer.isPlaying() && playlist.getCurrentSong() != "") {
                        if (audioPlayer.isPaused()) {
                            audioPlayer.resume();
                            std::cout << "Resumed: " << playlist.getCurrentSongName() << "\n";
                        }
                        else {
                            if (!audioPlayer.play(playlist.getCurrentSong())) {
                                std::cout << "Failed to play song: " << playlist.getCurrentSongName() << "\n";
                            }
                            else {
                                std::cout << "Playing: " << playlist.getCurrentSongName() << "\n";
                            }
                        }
                    }
                }
            }
        }

        // Update progress bar
        progressBar.setSize(sf::Vector2f(700 * audioPlayer.getCompletionPercentage(), progressBar.getSize().y));

        window.clear(sf::Color::White);
        window.draw(addButton);
        window.draw(addText);
        window.draw(nextButton);
        window.draw(nextText);
        window.draw(prevButton);
        window.draw(prevText);
        window.draw(exitButton);
        window.draw(exitText);
        window.draw(pauseButton);
        window.draw(pauseText);
        window.draw(playButton);
        window.draw(playText);
        window.draw(volumeUpButton);
        window.draw(volumeUpText);
        window.draw(volumeDownButton);
        window.draw(volumeDownText);
        window.draw(progressBarBackground); // Draw the progress bar background first
        window.draw(progressBar);
        window.draw(titleText);
        window.display();
}

    return 0;
}