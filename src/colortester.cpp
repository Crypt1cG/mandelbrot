#include <SFML/Graphics.hpp>
#include <cstdint>
#include <math.h>
#include <iostream>

sf::Color getColor(double V)
{
    if (V == 0)
        return sf::Color::Black;
    // else
    // {
    //     R = 255 * ((1 + cos(2 * M_PI * val)) / 2);
    //     G = 255 * ((1 + cos(2 * M_PI * val)) / 2);
    //     B = 255 * ((1 + cos(2 * M_PI * val)) / 2);
    // }
    // else
    // {
    //     H = 360 * val / numIterations;
    //     sf::Color color = HSVToRGB(H, 1, 0.9);
    //     R = color.r;
    //     G = color.g;
    //     B = color.b;
    // }
    // else if (val <= 25)
    // {
    //     R = 0;
    //     G = val * 10;
    //     B = 0;
    // }
    // else
    // {
    //     // H = (((val - 25) * 2) % 360 + 120) % 360;
    //     // H = ((((val - 25) / 3) * 2) % 360 + 120) % 360;
    //     // H = 120 - sin((val - 25) / 4.0) * 100;
    //     // H = 42 + (val % 3) * 140;
    //     // float V = 1.0 - ((val - 25) / 360) * 0.1;
    //     sf::Color color = HSVToRGB(H, 1, 0.9);
    //     R = color.r;
    //     G = color.g;
    //     B = color.b;
    // }
    double K = 3;
    V = log(V) / K;
    uint8_t R = 255 * ((1.0 - cos(V)) / 2.0);
    uint8_t G = 255 * ((1.0 - cos(V / (3 * M_SQRT2))) / 2.0);
    uint8_t B = 255 * ((1.0 - cos(V / (7 * pow(3, 1.0 / 8)))) / 2.0);
    return sf::Color(R, G, B, 255);
}

int main()
{
    // create the window
    sf::RenderWindow window(sf::VideoMode(1100, 600), "My window");

    uint8_t* pixels = new uint8_t[4 * 400 * 1024];
    for (int k = 0; k < 20; k++)
    {
        for (int i = 0; i < 1024; i++)
        {
            sf::Color color = getColor((i + 1024 * k) / 1844674407.0);
            for (int j = 0; j < 20; j++)
            {
                pixels[4 * ((j + k * 20) * 1024 + i)] = color.r; // R
                pixels[4 * ((j + k * 20) * 1024 + i) + 1] = color.g; // G
                pixels[4 * ((j + k * 20) * 1024 + i) + 2] = color.b; // B
                pixels[4 * ((j + k * 20) * 1024 + i) + 3] = 255; // A
            }
        }
    }

    sf::Image img;
    img.create(1024, 400, pixels);
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
        window.clear(sf::Color::White);

        // // draw everything here...
        // // window.draw(...);
        window.draw(sprite);

        // // end the current frame
        window.display();
    }

    return 0;
}