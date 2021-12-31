#include <wx/wx.h>
class MandelbrotApp : public wxApp
{
public:
	virtual bool OnInit();
};

class MyFrame : public wxFrame
{
public:
	MyFrame();

private:
	void OnExit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
};

bool MandelbrotApp::OnInit()
{
	MyFrame* frame = new MyFrame();
	frame->Show(true);
	return true;
}

MyFrame::MyFrame() : wxFrame(NULL, wxID_ANY, "hello world")
{

}
void MyFrame::OnExit(wxCommandEvent& event)
{
	Close(true);
}

wxIMPLEMENT_APP(MandelbrotApp);
