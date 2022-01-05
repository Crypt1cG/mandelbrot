#include <math.h>
#include <vector>
#include <wx/wx.h>
#include "include/ComplexNum.hpp"
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
double value = 0.625;

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
wxImage::RGBValue HSVToRGB(int h, float s, float v)
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

    return wxImage::RGBValue(R, G, B);
}


/**
 * Gets the appropriate color for a pixel based on its value V (which comes
 * from the iterate function)
 *
 * @param V the value of the pixel (returned from iterate)
 * @return an SFML color
 */
wxImage::RGBValue getColor(double V)
{
    if (V == 0) // num is in the mandelbrot set
        return wxImage::RGBValue(0, 0, 0);
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

    int H = (int)(hue + hueDelta * cos(V / hueRateOfChange));
	if (H < 0) H += 359;
	else H %= 359;
    double v = std::min(abs(value - (cos(V / 4) * .375)), 1.0);
    double s = 1.0 - (sin(V / 3) / 2 + 0.5);
    // hue is passed through functions for a random color each run
	wxImage::HSVValue hsv(H / 360.0, s, v);
    return wxImage::HSVtoRGB(hsv);
}

class App : public wxApp
{
public:
	virtual bool OnInit();
};

class Panel : public wxPanel
{
public:
	Panel(wxWindow* parent);
private:
	WindowInfo wInfo;
	double* results;
	ComplexNum target;
	double magnification;
	double movement_factor;

	wxSlider* hueSlider;
	wxStaticText* hueText;
	wxSlider* hueDeltaSlider;
	wxStaticText* hueDeltaText;
	wxSlider* hueROCSlider;
	wxStaticText* hueROCText;
	wxSlider* valueSlider;
	wxStaticText* valueText;
	wxSlider* kSlider;
	wxStaticText* kText;

	void render(wxDC& dc);
	void paintEvent(wxPaintEvent& event);
	void paintNow();
	void OnKeyPressed(wxKeyEvent& event);
	void OnSlider(wxCommandEvent& event);
};

class Frame : public wxFrame
{
public:
	Frame();

private:
	Panel* panel;
	void OnExit(wxCommandEvent& event);
};

bool App::OnInit()
{
	wxInitAllImageHandlers();
#ifdef OPENCL
	setup();
#endif
	Frame* frame = new Frame();
	frame->Show(true);
	SetTopWindow(frame);
	return true;
}

Frame::Frame() : wxFrame(NULL, wxID_ANY, "Mandelbrot")
{
	SetInitialSize(wxSize(1000, 800));
	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
	panel = new Panel(this);
	sizer->Add(panel, 1, wxEXPAND);
	this->SetSizer(sizer);
	Bind(wxEVT_MENU, &Frame::OnExit, this, wxID_EXIT);
}

Panel::Panel(wxWindow* parent) : wxPanel(parent)
{
	movement_factor = 2.0;
	wInfo = WindowInfo(-2, 1, -1.5, 1.5, 256, 30);
	results = getResults(wInfo);

	hueSlider = new wxSlider(this, 0, 174, 0, 360, wxDefaultPosition, wxDefaultSize, wxSL_VALUE_LABEL);
	hueSlider->SetPosition(wxPoint(800, 20));

	hueText = new wxStaticText(this, wxID_ANY, "Hue:", wxPoint(800, 45));

	hueDeltaSlider = new wxSlider(this, 1, 50, 0, 360, wxDefaultPosition, wxDefaultSize, wxSL_VALUE_LABEL);
	hueDeltaSlider->SetPosition(wxPoint(800, 70));

	hueDeltaText = new wxStaticText(this, wxID_ANY, "Hue range:", wxPoint(800, 95));

	hueROCSlider = new wxSlider(this, 2, 15, 1, 50, wxDefaultPosition, wxDefaultSize, wxSL_VALUE_LABEL);
	hueROCSlider->SetPosition(wxPoint(800, 120));

	hueROCText = new wxStaticText(this, wxID_ANY, "Hue period:", wxPoint(800, 145));

	valueSlider = new wxSlider(this, 3, 62, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_VALUE_LABEL);
	valueSlider->SetPosition(wxPoint(800, 170));

	valueText = new wxStaticText(this, wxID_ANY, "Value:", wxPoint(800, 195));

	kSlider = new wxSlider(this, 4, 80, 2, 160, wxDefaultPosition, wxDefaultSize, wxSL_VALUE_LABEL);
	kSlider->SetPosition(wxPoint(800, 220));

	kText = new wxStaticText(this, wxID_ANY, "K:", wxPoint(800, 245));

    // target.a = 0.360538544808150618340;
	target = ComplexNum(0, 0);
    target.a = 0.360240443437614363236125244449545308482607807958585750488375814740195346059218100311752936722773426396233731729724987737320035372683285317664532401218521579554288661726564324134702299962817029213329980895208036363104546639698106204384566555001322985619004717862781192694046362748742863016467354574422779443226982622356594130430232458472420816652623492974891730419252651127672782407292315574480207005828774566475024380960675386215814315654794021855269375824443853463117354448779647099224311848192893972572398662626725254769950976527431277402440752868498588785436705371093442460696090720654908973712759963732914849861213100695402602927267843779747314419332179148608587129105289166676461292845685734536033692577618496925170576714796693411776794742904333484665301628662532967079174729170714156810530598764525260869731233845987202037712637770582084286587072766838497865108477149114659838883818795374195150936369987302574377608649625020864292915913378927790344097552591919409137354459097560040374880346637533711271919419723135538377394364882968994646845930838049998854075817859391340445151448381853615103761584177161812057928;
    // target.b = -0.64122620321722934023;
    target.b = -0.6413130610648031748603750151793020665794949522823052595561775430644485741727536902556370230689681162370740565537072149790106973211105273740851993394803287437606238596262287731075999483940467161288840614581091294325709988992269165007394305732683208318834672366947550710920088501655704252385244481168836426277052232593412981472237968353661477793530336607247738951625817755401065045362273039788332245567345061665756708689359294516668271440525273653083717877701237756144214394870245598590883973716531691124286669552803640414068523325276808909040317617092683826521501539932397262012011082098721944643118695001226048977430038509470101715555439047884752058334804891389685530946112621573416582482926221804767466258346014417934356149837352092608891639072745930639364693513216719114523328990690069588676087923656657656023794484324797546024248328156586471662631008741349069961493817600100133439721557969263221185095951241491408756751582471307537382827924073746760884081704887902040036056611401378785952452105099242499241003208013460878442953408648178692353788153787229940221611731034405203519945313911627314900851851072122990492499999999999999999991;
	std::srand(std::time(0)); // so rand num is not the same every time ! (rand based on current time)

	Bind(wxEVT_CHAR, &Panel::OnKeyPressed, this);
	Bind(wxEVT_PAINT, &Panel::paintEvent, this);
	//EVT_SCROLL(Panel::OnSlider);
	//hueSlider->Bind(wxEVT_SCROLL_CHANGED, &Panel::OnSlider, this);
	Bind(wxEVT_COMMAND_SLIDER_UPDATED, &Panel::OnSlider, this);
}

void Frame::OnExit(wxCommandEvent& event)
{
	std::cout << "OnExit called" << std::endl;
	Close(true);
}

void Panel::OnKeyPressed(wxKeyEvent& event)
{
	wxChar uc = event.GetUnicodeKey();
	if (uc != WXK_NONE)
	{
		std::cout << "you pressed: " << (char)uc << std::endl;
		if (uc == WXK_RETURN) // enter key was pressed
		{
			// zoom in, redraw
			std::cout << "you pressed enter" << std::endl;
			wInfo.zoom(4, target, magnification);
			std::cout << wInfo << std::endl;
			delete [] results;
			results = getResults(wInfo);
		}
		else if (uc == WXK_ESCAPE)
		{
			wInfo.zoom(0.25, target, magnification);
			delete [] results;
			results = getResults(wInfo);
		}
		else if (uc == 'w')
			target.b -= movement_factor / wInfo.step;
		else if (uc == 's')
			target.b += movement_factor / wInfo.step;
		else if (uc == 'd')
			target.a += movement_factor / wInfo.step;
		else if (uc == 'a')
			target.a -= movement_factor / wInfo.step;
		// capital letters indicate that shift was held down
		else if (uc == 'W')
			target.b -= (movement_factor * 3) / wInfo.step;
		else if (uc == 'S')
			target.b += (movement_factor * 3) / wInfo.step;
		else if (uc == 'D')
			target.a += (movement_factor * 3) / wInfo.step;
		else if (uc == 'A')
			target.a -= (movement_factor * 3) / wInfo.step;

		else if (uc == 'z')
		{
			int oldROC = hueRateOfChange;
			hueRateOfChange = std::rand() % 100 + 1;
			while (oldROC == hueRateOfChange)
				hueRateOfChange = std::rand() % 100 + 1;
			std::cout << "new hue rate of change: " << hueRateOfChange << std::endl;
		}
		else if (uc == 'x')
		{
			int oldHueDelta = hueDelta;
			while (oldHueDelta == hueDelta)
				hueDelta = std::rand() % 359;
			std::cout << "new hue delta: " << hueDelta << std::endl;
		}
		else if (uc == 'c')
		{
			int oldHue = hue;
			while (oldHue == hue)
				hue = std::rand() % 359;
			std::cout << "new hue: " << hue << std::endl;
		}
		else if (uc == 'v')
		{
			double oldValue = value;
			while (oldValue == value)
				value = (double)std::rand() / RAND_MAX;
			std::cout << "new vaue: " << value << std::endl;
		}
		else if (uc == 'k')
		{
			double oldK = K;
			while (oldK == K)
				K = (std::rand() % 40 + 1) / 8.0; // random number from 0.5 - 5.5 (increments of 0.5)
		}
		else if (uc == 'i')
		{
			wInfo.numIterations *= 2;
			delete [] results;
			results = getResults(wInfo);
		}
		paintNow();
	}
	else
	{
		bool shift = event.ShiftDown();
		int num = event.GetKeyCode();
		double move_amnt = movement_factor;
		if (shift) move_amnt *= 3;
		if (num == WXK_UP)
			target.b -= move_amnt / wInfo.step;
		else if (num == WXK_DOWN)
			target.b += move_amnt / wInfo.step;
		else if (num == WXK_RIGHT)
			target.a += move_amnt / wInfo.step;
		else if (num == WXK_LEFT)
			target.a -= move_amnt / wInfo.step;
		paintNow();
	}
}

void Panel::paintEvent(wxPaintEvent& event)
{
	wxPaintDC dc(this);
	render(dc);
}

void Panel::paintNow()
{
	wxClientDC dc(this);
	render(dc);
}

void Panel::render(wxDC& dc)
{
	wxImage image(wInfo.pixelWidth, wInfo.pixelHeight);
	for (int x = 0; x < wInfo.pixelWidth; x++)
	{
		for (int y = 0; y < wInfo.pixelHeight; y++)
		{
			auto color = getColor(results[(y) * wInfo.pixelWidth + x]);
			image.SetRGB(x, y, color.red, color.green, color.blue);
		}
	}
	int targetX = (target.a - wInfo.minA) * wInfo.step + 1;
	int targetY = (target.b - wInfo.minB) * wInfo.step + 1;
	image.SetRGB(targetX, targetY, 255, 255, 255);
	image.SetRGB(targetX + 1, targetY, 255, 255, 255);
	image.SetRGB(targetX - 1, targetY, 255, 255, 255);
	image.SetRGB(targetX, targetY + 1, 255, 255, 255);
	image.SetRGB(targetX, targetY - 1, 255, 255, 255);

	std::vector<std::vector<int>> coords = {
		{2, 0}, {2, 1}, {2, -1}, {-2, 0}, {-2, 1}, {-2, -1},
		{0, 2}, {1, 2}, {-1, 2}, {0, -2}, {1, -2}, {-1, -2},
		{-1, 1}, {-1, -1}, {1, 1}, {1, -1}
	};
	for (auto c : coords)
		image.SetRGB(targetX + c[0], targetY + c[1], 0, 0, 0);

	wxBitmap bmp(image);
	dc.DrawBitmap(bmp, 0, 0, false);
}

void Panel::OnSlider(wxCommandEvent& event)
{
	int id = event.GetId();
	int val = event.GetInt();
	if (id == 0) // hue slider
	{
		hue = val;
		paintNow();
	}
	else if (id == 1) // hue delta slider
	{
		hueDelta = val;
		paintNow();
	}
	else if (id == 2) // hue rate of change
	{
		hueRateOfChange = val;
		paintNow();
	}
	else if (id == 3) // value
	{
		value = val / 100.0;
		paintNow();
	}
	else if (id == 4) // K
	{
		K = val / 16.0;
		paintNow();
	}
}

wxIMPLEMENT_APP(App);
