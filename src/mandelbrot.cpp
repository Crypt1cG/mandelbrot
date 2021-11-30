#include <math.h>
#include "include/ComplexNum.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <exception>
#include <chrono>
// #include <OpenCL/cl.h>
// #include <CL/opencl.hpp>
// #include <gmp.h>
#include <thread>
#include "include/WindowInfo.hpp"
#ifdef OPENCL
    #include "include/openclStuff.h"
#endif
#include "include/calculations.hpp"
#define SCALE 1
#include <cstdlib>
#include <ctime>
double magnification = 1;
double K = 5;
int hue = 174;
int hueDelta = 50;
int hueRateOfChange = 15; // bigger = lower rate of change
/**
 * this just converts a color from HSV to RGB, it is useful because SMFL uses
 * RGB only, and sometimes generating colors in HSV is nice
 * source: https://www.codespeedy.com/hsv-to-rgb-in-cpp/
 * @param h the hue of the color (0 - 360)
 * @param s the saturation value of the color (0.0 - 1.0)
 * @param v the value of the volor (0.0 - 1.0)
 * 
 * @return an SMFL color object with the correct RGB values
 */
sf::Color HSVToRGB(int h, float s, float v)
{   
    double C = s * v;
    double X = C * (1 - abs((fmod(h / 60.0, 2)) - 1));
    double m = v - C;
    double r, g, b;
    if (h >= 0 && h < 60)
    {
        r = C;
        g = X;
        b = 0;
    }
    else if (60 <= h && h < 120)
    {
        r = X;
        g = C;
        b = 0;
    }
    else if (120 <= h && h < 180)
    {
        r = 0;
        g = C;
        b = X;
    }
    else if (180 <= h && h < 240)
    {
        r = 0;
        g = X;
        b = C;
    }
    else if (240 <= h && h < 300)
    {
        r = X;
        g = 0;
        b = C;
    }
    else if (300 <= h && h < 360)
    {
        r = C;
        g = 0;
        b = X;
    }

    uint8_t R = (r + m) * 255;
    uint8_t G = (g + m) * 255;
    uint8_t B = (b + m) * 255;

    return sf::Color(R, G, B, 255);
}

/**
 * Gets the appropriate color for a pixel based on its value V (which comes
 * from the iterate function)
 * 
 * @param V the value of the pixel (returned from iterate)
 * @return an SFML color
 */
sf::Color getColor(double V)
{
    if (V == 0) // num is in the mandelbrot set
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
    // else if (V <= 25)
    // {
    //     int R = 0;
    //     int G = V * 10;
    //     int B = 0;
    //     return sf::Color(R, G, B);
    // }
    // else
    // {
    //     int H = ((((int)V - 25) * 2) % 360 + 120) % 360;
    //     // H = ((((val - 25) / 3) * 2) % 360 + 120) % 360;
    //     // H = 120 - sin((val - 25) / 4.0) * 100;
    //     // H = 42 + (val % 3) * 140;
    //     // float V = 1.0 - ((val - 25) / 360) * 0.1;
    //     sf::Color color = HSVToRGB(H, 1, 0.9);
        // return color;
    // }
    // double K = 5;
    V = log(V) / K;
    // // uint8_t R = 255 * ((1.0 - cos(V)) / 2.0);
    // // uint8_t G = 255 * ((1.0 - cos(V / (3 * M_SQRT2))) / 2.0);
    // // uint8_t B = 255 * ((1.0 - cos(V / (7 * pow(3, 1.0 / 8)))) / 2.0);
    // uint8_t R = 255 * ((1.0 - cos(V / 10)) / 2.0);
    // // uint8_t R = 30;
    // uint8_t G = 255 * ((1.0 - cos(V / (3 * M_SQRT2))) / 2.0);
    // uint8_t B = 255 * ((1.0 - cos(V / (7 * pow(3, 1.0 / 8)))) / 2.0);
    // // uint8_t B = 30;
    // return sf::Color(R, G, B, 255);

    int H = (int)(hue + hueDelta * cos(V / hueRateOfChange)) % 359;
    double v = 0.625 - (cos(V / 4) * .375);
    double s = 1.0 - (sin(V / 3) / 2 + 0.5);
    // hue is passed through functions for a random color each run
    return HSVToRGB(H, s, v);
}

void display(double* results, const WindowInfo& info, sf::RenderWindow& window, const ComplexNum& target)
{
    window.clear(sf::Color::White);
    int scale = SCALE;
    sf::Image img;
    uint8_t* pixels = new uint8_t[4 * info.pixelWidth * info.pixelHeight];
    for (int y = 0; y < info.pixelHeight; y++)
    {
        for (int x = 0; x < info.pixelWidth; x++)
        {
            sf::Color color = getColor(results[y * info.pixelWidth + x]);
            uint8_t R = color.r;
            uint8_t G = color.g;
            uint8_t B = color.b;
            pixels[4 * (y * info.pixelWidth + x)] = R;
            pixels[4 * (y * info.pixelWidth + x) + 1] = G;
            pixels[4 * (y * info.pixelWidth + x) + 2] = B;
            pixels[4 * (y * info.pixelWidth + x) + 3] = 255;
        }
    }

    double midA = (info.minA + info.maxA) / 2;
    double deltaA = target.a - midA;
    int pixelX = info.pixelWidth / 2 + deltaA * info.step;

    double midB = (info.minB + info.maxB) / 2;
    double deltaB = target.b - midB;
    int pixelY = info.pixelHeight / 2 + deltaB * info.step;
    // white crosshair
    int offsets[][2] = {{0, 0}, {0, 1}, {0, -1}, {1, 0}, {-1, 0}};
    for (auto offset : offsets)
    {
        int x = pixelX + offset[0];
        int y = pixelY + offset[1];
        int index = 4 * (y * info.pixelWidth + x);
        for (int i = 0; i < 3; i++)
            pixels[index + i] = 255;
    }

    // black outline
    int offsets2[][2] = {{-2, 0},  {2, 0},   {0, 2},  {0, -2},
                         {-1, -1}, {-1, 1},  {1, 1},  {1, -1},
                         {-2, 1},  {-2, -1}, {2, 1},  {2, -1},
                         {1, 2},   {-1, 2},  {1, -2}, {-1, -2}};
    for (auto offset : offsets2)
    {
        int x = pixelX + offset[0];
        int y = pixelY + offset[1];
        int index = 4 * (y * info.pixelWidth + x);
        for (int i = 0; i < 3; i++)
            pixels[index + i] = 0;
    }
    
    img.create(info.pixelWidth, info.pixelHeight, pixels);
    sf::Texture texture;
    texture.loadFromImage(img);
    sf::Sprite sprite(texture);
    window.draw(sprite);
    sf::Font font;
    font.loadFromFile("res/Arial Unicode.ttf");
    sf::Text text;
    text.setString("Magnification: " + std::to_string(magnification) + 
                   "\tIterations: " + std::to_string((int)(info.numIterations)) + 
                   "\tCrosshair: " + std::to_string(target.a) + " + " + std::to_string(-target.b) + "i");
    text.setFont(font);
    text.setPosition(10, 768);
    text.setCharacterSize(20);
    text.setFillColor(sf::Color::Black);
    window.draw(text);

    window.display();
    delete [] pixels;
}

int main()
{
    #ifdef OPENCL
        setup();
    #endif
    sf::RenderWindow window(sf::VideoMode(800, 800), "Mandelbrot");
    window.setFramerateLimit(60);

    WindowInfo wInfo(-2, 1, -1.5, 1.5, 256, 30);
    double movement_factor = 2.0;
    double shift = movement_factor;
    double* results = getResults(wInfo);
    ComplexNum target = ComplexNum(0, 0);
    target.a = 0.360538544808150618340;
    target.b = -0.64122620321722934023;
    std::srand(std::time(0)); //so rand num is not the same every time !!! (rand based off of current time)
    display(results, wInfo, window, target);
    // return 0;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            // std::cout << "Event received" << std::endl;
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Enter)
                {
                    std::cout << "you pressed enter" << std::endl;
                    wInfo.zoom(4, target, magnification);
                    std::cout << wInfo << std::endl;
                    delete [] results;
                    results = getResults(wInfo);
                }
                else if (event.key.code == sf::Keyboard::Escape)
                {
                    wInfo.zoom(0.25, target, magnification);
                    delete [] results;
                    results = getResults(wInfo);
                }
                //added hold shift for faster cursor (sorry need to fix magic numbers ik)
                else if (event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::D)
                {
                	if (sf::Keyboard::isKeyPressed(sf::Keyboard::RShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                    {
				        shift = movement_factor * 3;
                    }
			        else shift = movement_factor;
			        target.a += shift / wInfo.step;
                }
                else if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::A)
                {
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::RShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                    {
				        shift = movement_factor * 3;
                    }
			        else shift = movement_factor;
                    target.a -= shift / wInfo.step;
                }
                else if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W)
                {
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::RShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                    {
				        shift = movement_factor * 3;
                    }
			        else shift = movement_factor;
                    target.b -= shift / wInfo.step;
                }
                else if (event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S)
                {
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::RShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                    {
				        shift = movement_factor * 3;
                    }
			        else shift = movement_factor;
                    target.b += shift / wInfo.step;
                }
                else if (event.key.code == sf::Keyboard::Space)
                {
                    std::cout.precision(20);
                    std::cout << target.a << " + " << target.b << "i" << std::endl;
                }
                else if (event.key.code == sf::Keyboard::I)
                {
                    wInfo.numIterations *= 2;
                    delete [] results;
                    results = getResults(wInfo);
                }
                else if (event.key.code == sf::Keyboard::K)
                {
                    double oldK = K;
                    K = (std::rand() % 40 + 1) / 8.0; // random number from 0.5 - 5.5 (increments of 0.5)
                    while (oldK == K)
                    {
                        K = (std::rand() % 40 + 1) / 8.0; // random number from 0.5 - 5.5 (increments of 0.5)
                    }
                    // std::cout << "new K: " << K << std::endl;
                }
                else if (event.key.code == sf::Keyboard::C)
                {
                    double oldHue = hue;
                    hue = std::rand() % 359;
                    while (oldHue == hue)
                    {
                        hue = std::rand() % 359;
                    }
                    std::cout << "new hue: " << hue << std::endl;
                }
                else if (event.key.code == sf::Keyboard::X)
                {
                    double oldHueDelta = hueDelta;
                    hueDelta = std::rand() % 359;
                    while (oldHueDelta == hueDelta)
                        hueDelta = std::rand() % 359;
                    std::cout << "new hue delta: " << hueDelta << std::endl;
                }
                else if (event.key.code == sf::Keyboard::Z)
                {
                    double oldROC = hueRateOfChange;
                    hueRateOfChange = std::rand() % 100 + 1;
                    while (oldROC == hueRateOfChange)
                        hueRateOfChange = std::rand() % 100 + 1;
                    std::cout << "new rate of change: " << hueRateOfChange << std::endl;
                }
                display(results, wInfo, window, target);
            }
        }
    }
    return 0;
}