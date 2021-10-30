#include <SFML/Graphics.hpp>
#include <cstdint>

int main()
{
    // create the window
    sf::RenderWindow window(sf::VideoMode(800, 600), "My window");

    uint8_t* pixels = new uint8_t[4 * 100 * 100];
    for (int i = 0; i < 100; i++)
    {
        for (int j = 0; j < 100; j++)
        {
            pixels[4 * (i * 100 + j)] = i; // R
            pixels[4 * (i * 100 + j) + 1] = j; // G
            pixels[4 * (i * 100 + j) + 2] = i + j; // B
            pixels[4 * (i * 100 + j) + 3] = 255; // A
        }
    }

    sf::Image img;
    img.create(100, 100, pixels);
    sf::Texture texture;
    texture.loadFromImage(img);
    sf::Sprite sprite(texture);
    window.clear(sf::Color::Black);
    window.draw(sprite);
    window.display();
    // run the program as long as the window is open
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // // clear the window with black color
        // window.clear(sf::Color::Black);

        // // draw everything here...
        // // window.draw(...);
        // window.draw(sprite);

        // // end the current frame
        // window.display();
    }

    return 0;
}