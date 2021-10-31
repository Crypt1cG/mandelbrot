#include <math.h>
#include "ComplexNum.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <exception>
#include <chrono>
#include <thread>
#include <future>
#define NUM_ITERATIONS 12
#define SCALE 1
double numIterations = 30;
double magnification = 1;

struct WindowInfo
{
    uint64_t step;
    double minA, maxA;
    double minB, maxB;
    double rangeA, rangeB;
    int pixelWidth, pixelHeight;
    WindowInfo(double minA, double maxA, double minB, double maxB, uint64_t step)
    {
        this->minA = minA;
        this->maxA = maxA;
        this->minB = minB;
        this->maxB = maxB;
        this->step = step;

        rangeA = maxA - minA;
        rangeB = maxB - minB;
        pixelWidth = rangeA * step + 1;
        pixelHeight = rangeB * step + 1;
    }
    void zoom(double zoomFactor, ComplexNum target)
    {
        numIterations *= 2;
        rangeA = rangeA / zoomFactor;
        rangeB = rangeB / zoomFactor;

        minA = target.a - rangeA / 2;
        maxA = target.a + rangeA / 2;
        minB = target.b - rangeB / 2;
        maxB = target.b + rangeB / 2;

        step *= zoomFactor;

        pixelWidth = rangeA * step + 1;
        pixelHeight = rangeB * step + 1;
        magnification *= zoomFactor;
    }
    friend std::ostream& operator<<(std::ostream& os, const WindowInfo& wi)
    {
        std::cout.precision(17);
        os << "\t min\t\t| max\n\t----------------+------\nA:\t "
           << wi.minA << "\t\t| " << wi.maxA << "\t(range: " << wi.rangeA << ")"
           << "\nB:\t " << wi.minB << "\t\t| " << wi.maxB << "\t(range: " << wi.rangeB << ")"
           << "\nStep: " << wi.step << "\nWidth (pixels): " << wi.pixelWidth
           << "\nHeight (pixels): " << wi.pixelHeight;
        return os;
    }
};

double iterate(ComplexNum& z, const ComplexNum& c, int itLeft, const int& totalIterations)
{
    if (itLeft == 0)
        return 0;
    z.square();
    z.a += c.a;
    z.b += c.b;
    // if (new_z.squaredModulus() >= 4) return totalIterations - (itLeft - 1);
    // if (new_z.abs() >= 1000) return totalIterations - (itLeft - 1);
    // if (new_z.abs() >= 2) return (totalIterations - itLeft) + 1 - log(log2(new_z.abs()));
    double R = z.squaredModulus();
    if (R >= 9) 
    {
        double V = log(R) / pow(2, totalIterations - itLeft);
        return V;
    }
    return iterate(z, c, itLeft - 1, totalIterations);
}

int iterateNotNormalized(ComplexNum& z, const ComplexNum& c, int itLeft, const int& totalIterations)
{
    if (itLeft == 0)
        return 0;
    z.square();
    z.a += c.a;
    z.b += c.b;
    if (z.squaredModulus() >= 4) return totalIterations - (itLeft - 1);
    return iterateNotNormalized(z, c, itLeft - 1, totalIterations);
}

double* calculate(const WindowInfo& info, int numIterations)
{
    auto t1 = std::chrono::high_resolution_clock::now();
    double* results = new double[info.pixelWidth * info.pixelHeight]; // allocate on heap bc stack not big enough
    int index = 0;

    ComplexNum z = ComplexNum(0, 0);
    ComplexNum c = ComplexNum(0, 0);
    double res;
    for (double b = info.minB; b <= info.maxB; b += 1.0 / info.step)
    {
        for (double a = info.minA; a <= info.maxA; a += 1.0 / info.step)
        {
            double sum = 0;
            for (int i = -1; i < 2; i++)
            {
                for (int j = -1; j < 2; j++)
                {
                    c.a = a + i * (1 / (info.step * 3.0));
                    c.b = b + j * (1 / (info.step * 3.0));
                    // c.a = a;
                    // c.b = b;
                    z.a = 0;
                    z.b = 0;

                    sum += iterateNotNormalized(z, c, numIterations, numIterations);
                }
            }
            results[index] = sum / 9;
            // c.a = a;
            // c.b = b;
            // z.a = 0;
            // z.b = 0;
            // results[index] = iterateNotNormalized(z, c, numIterations, numIterations);
            index++;
        }
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    std::cout << duration.count() << " milliseconds" << std::endl;
    return results;
}

// https://www.codespeedy.com/hsv-to-rgb-in-cpp/
// ******assuming saturation and value is always 1
sf::Color HSVToRGB(int h, float s, float v)
{
    // if (s != 1.0 || v != 1.0)
    //     throw std::invalid_argument("Saturation and Value should be 1.0 for rn");
    
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
    else if (V <= 25)
    {
        int R = 0;
        int G = V * 10;
        int B = 0;
        return sf::Color(R, G, B);
    }
    else
    {
        int H = ((((int)V - 25) * 2) % 360 + 120) % 360;
        // H = ((((val - 25) / 3) * 2) % 360 + 120) % 360;
        // H = 120 - sin((val - 25) / 4.0) * 100;
        // H = 42 + (val % 3) * 140;
        // float V = 1.0 - ((val - 25) / 360) * 0.1;
        sf::Color color = HSVToRGB(H, 1, 0.9);
        return color;
    }
    // double K = 5;
    // V = log(V) / K;
    // // // uint8_t R = 255 * ((1.0 - cos(V)) / 2.0);
    // // // uint8_t G = 255 * ((1.0 - cos(V / (3 * M_SQRT2))) / 2.0);
    // // // uint8_t B = 255 * ((1.0 - cos(V / (7 * pow(3, 1.0 / 8)))) / 2.0);
    // // uint8_t R = 255 * ((1.0 - cos(V / 10)) / 2.0);
    // // // uint8_t R = 30;
    // // uint8_t G = 255 * ((1.0 - cos(V / (3 * M_SQRT2))) / 2.0);
    // // uint8_t B = 255 * ((1.0 - cos(V / (7 * pow(3, 1.0 / 8)))) / 2.0);
    // // // uint8_t B = 30;
    // // return sf::Color(R, G, B, 255);

    // int H = 174 + 50 * cos(V / 15);
    // double v = 0.625 - (cos(V / 4) * .375);
    // double s = 1.0 - (sin(V / 10) / 2 + 0.5);
    // return HSVToRGB(H, s, v);
}

void display(double* results, const WindowInfo& info, sf::RenderWindow& window, const ComplexNum& target)
{
    window.clear(sf::Color::White);
    int scale = SCALE;
    // uint8_t color_map_thing[][3] = {{0, 0, 0},   {0, 10, 0},  {0, 25, 0},  {0, 40, 0}, 
    //                                 {0, 55, 0},  {0, 70, 0},  {0, 85, 0},  {0, 100, 0}, 
    //                                 {0, 115, 0}, {0, 130, 0}, {0, 145, 0}, {0, 160, 0}, 
    //                                 {0, 175, 0}, {0, 190, 0}, {0, 205, 0}, {0, 220, 0}, 
    //                                 {0, 235, 0}, {0, 250, 0}, {255, 0, 0}};
    sf::Image img;
    uint8_t* pixels = new uint8_t[4 * info.pixelWidth * info.pixelHeight];
    for (int y = 0; y < info.pixelHeight; y++)
    {
        for (int x = 0; x < info.pixelWidth; x++)
        {
            // int RSum, GSum, BSum;
            // for (int i = 0; i < 2; i++)
            // {
            //     for (int j = 0; j < 2; j++)
            //     {
            //         double V = results[(y + j) * info.pixelWidth + x + i];
            //         sf::Color color = getColor(V);
            //         RSum += color.r;
            //         GSum += color.g;
            //         BSum += color.b;
            //     }
            // }
            // uint8_t R = RSum == 0 ? RSum : RSum / 4;
            // uint8_t G = GSum == 0 ? GSum : GSum / 4;
            // uint8_t B = BSum == 0 ? BSum : BSum / 4;
            sf::Color color = getColor(results[y * info.pixelWidth + x]);
            uint8_t R = color.r;
            uint8_t G = color.g;
            uint8_t B = color.b;
            // std::cout << "Val: " << val << " H: " << H << " R: " << (int)R << " G: " << (int)G << " B: " << (int)B << std::endl;
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
                   "\tIterations: " + std::to_string((int)(numIterations)) + 
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
    sf::RenderWindow window(sf::VideoMode(800, 800), "Mandelbrot");
    window.setFramerateLimit(60);

    WindowInfo initial(-2, 1, -1.5, 1.5, 256);

    double* results = calculate(initial, numIterations);
    ComplexNum target = ComplexNum(0, 0);
    target.a = 0.360538544808150618340;
    target.b = -0.64122620321722934023;
    display(results, initial, window, target);

    const auto numCPUs = std::thread::hardware_concurrency();
    std::cout << "Num cpus: " << numCPUs << std::endl;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            std::cout << "Event received" << std::endl;
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Enter)
                {
                    initial.zoom(4, target);
                    std::cout << initial << std::endl;
                    delete [] results;
                    if (numCPUs == 2)
                    {
                        WindowInfo half1 = WindowInfo(initial.minA, initial.maxA, initial.minB, 
                                                    initial.minB + initial.rangeB / 2, initial.step);
                        WindowInfo half2 = WindowInfo(initial.minA, initial.maxA, initial.minB + 
                                                    initial.rangeB / 2 + 1.0 / initial.step, initial.maxB, 
                                                    initial.step);
                        // results = calculate(initial, numIterations);
                        std::future<double*> ret = std::async(calculate, half1, numIterations);
                        std::future<double*> ret2 = std::async(calculate, half2, numIterations);
                        double* results2 = ret.get();
                        results = new double[initial.pixelWidth * initial.pixelHeight];
                        std::copy(results2, results2 + half1.pixelHeight * half1.pixelWidth, results);
                        delete [] results2;
                        results2 = ret2.get();
                        std::copy(results2, results2 + half2.pixelWidth * half2.pixelHeight, results + half1.pixelHeight * half1.pixelWidth);
                    }
                    else
                    {
                        WindowInfo quarter1 = WindowInfo(initial.minA, initial.maxA,
                                                        initial.minB, initial.minB + initial.rangeB / 4,
                                                        initial.step);
                        WindowInfo quarter2 = WindowInfo(initial.minA, initial.maxA,
                                                        initial.minB + initial.rangeB / 4 + 1.0 / initial.step,
                                                        initial.minB + initial.rangeB / 2,
                                                        initial.step);
                        WindowInfo quarter3 = WindowInfo(initial.minA, initial.maxA,
                                                        initial.minB + initial.rangeB / 2 + 1.0 / initial.step,
                                                        initial.minB + initial.rangeB * 0.75,
                                                        initial.step);
                        WindowInfo quarter4 = WindowInfo(initial.minA, initial.maxA,
                                                        initial.minB + initial.rangeB * 0.75 + 1.0 / initial.step, initial.maxB,
                                                        initial.step);
                        // std::cout << quarter1 << std::endl << quarter2 << std::endl << quarter3 << std::endl << quarter4 << std::endl;
                        results = new double[initial.pixelWidth * initial.pixelHeight];
                        std::future<double*> ret1 = std::async(calculate, quarter1, numIterations);
                        std::future<double*> ret2 = std::async(calculate, quarter2, numIterations);
                        std::future<double*> ret3 = std::async(calculate, quarter3, numIterations);
                        std::future<double*> ret4 = std::async(calculate, quarter4, numIterations);
                        double* tempResults = ret1.get();
                        std::copy(tempResults, tempResults + quarter1.pixelWidth * quarter1.pixelHeight, results);
                        delete [] tempResults;
                        tempResults = ret2.get();
                        std::copy(tempResults, tempResults + quarter2.pixelWidth * quarter2.pixelHeight, results + quarter1.pixelWidth * quarter1.pixelHeight);
                        delete [] tempResults;
                        tempResults = ret3.get();
                        std::copy(tempResults, tempResults + quarter3.pixelWidth * quarter3.pixelHeight, results + quarter1.pixelHeight * quarter1.pixelWidth + 
                                                                                                                quarter2.pixelHeight * quarter2.pixelWidth);
                        delete [] tempResults;
                        tempResults = ret4.get();
                        std::copy(tempResults, tempResults + quarter4.pixelWidth * quarter4.pixelHeight, results + quarter1.pixelHeight * quarter1.pixelWidth + 
                                                                                                                quarter2.pixelHeight * quarter2.pixelWidth +
                                                                                                                quarter3.pixelHeight * quarter3.pixelWidth);
                    }
                }
                else if (event.key.code == sf::Keyboard::Escape)
                {
                    initial.zoom(0.25, target);
                    delete [] results;
                    results = calculate(initial, numIterations);
                }
                else if (event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::D)
                {
                    target.a += 2.0 / initial.step;
                }
                else if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::A)
                {
                    target.a -= 2.0 / initial.step;
                }
                else if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W)
                {
                    target.b -= 2.0 / initial.step;
                }
                else if (event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S)
                {
                    target.b += 2.0 / initial.step;
                }
                else if (event.key.code == sf::Keyboard::Space)
                {
                    std::cout.precision(20);
                    std::cout << target.a << " + " << target.b << "i" << std::endl;
                }
            }
            display(results, initial, window, target);
        }
    }
    return 0;
}

// int main()
// {
//     ComplexNum C = ComplexNum(-1.2566303, 0.0308026);
//     ComplexNum zero = ComplexNum(0, 0);
//     int res = iterate(zero, C, 500, 500);
//     std::cout << res << std::endl;
// }