/*
 * gui_default.cpp
 *
 *  Created on: Nov 4, 2008
 *      Author: gheckler
 */

#include "gui.h"

DECLARE_APP(GUI_App)

/*----------------------------------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(GUI_Main, wxFrame)
    EVT_CLOSE(GUI_Main::onClose)
END_EVENT_TABLE()
/*----------------------------------------------------------------------------------------------*/

GUI_Main::GUI_Main():iGUI_Main(NULL, wxID_ANY, wxT("Main"), wxDefaultPosition, wxSize(980,600), wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL)
{

}

GUI_Main::~GUI_Main()
{


}

void GUI_Main::onClose(wxCloseEvent& evt)
{
	wxCommandEvent cevt;
	evt.Veto();
	pToplevel->onMain(cevt);
}

void GUI_Main::paintNow()
{
    wxClientDC dc(this);
    render(dc);
}

void GUI_Main::render(wxDC& dc)
{

	renderCN0();
	renderSkyPlot();
	renderPVT();

}

void GUI_Main::renderCN0()
{

	Channel_M *pchan;
	SPS_M *pNav = &p->sps;
	int mX, mY, lcv, gval, rval;
	double maxX, maxY, svX, svY, dX, dY;
	double scaleX, scaleY;
	wxPoint bar[4];

	wxString str;
//	wxPaintDC dc(pCN0);
	wxBufferedPaintDC dc(pCN0, wxBUFFER_CLIENT_AREA);
	dc.SetBackground(wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_BACKGROUND)));
	dc.Clear();

	maxX = maxY = 1000;

	wxCoord w, h;
	pCN0->GetClientSize(&w, &h);

	scaleX = w/maxX; scaleY = h/maxY;
	mX = w/2; mY = h/2;

	dc.SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxNORMAL, wxNORMAL));

	dc.SetPen(wxPen(wxColor(0,0,0), 1, wxLONG_DASH ));
	for(lcv = 200; lcv < 1000; lcv+=40)
		dc.DrawLine(mX-500*scaleX, h-lcv*scaleY, mX+500*scaleX, h-lcv*scaleY);

    dc.SetPen(wxPen(wxColor(0,0,0), 1));
	for(lcv = 200; lcv < 1000; lcv+=200)
		dc.DrawLine(mX-500*scaleX, h-lcv*scaleY, mX+500*scaleX, h-lcv*scaleY);

    dc.DrawText(wxT("60 dB-Hz"), mX - 500*scaleX, h - 1000*scaleY);
    dc.DrawText(wxT("50 dB-Hz"), mX - 500*scaleX, h - 800*scaleY);
    dc.DrawText(wxT("40 dB-Hz"), mX - 500*scaleX, h - 600*scaleY);
    dc.DrawText(wxT("30 dB-Hz"), mX - 500*scaleX, h - 400*scaleY);
    dc.DrawText(wxT("20 dB-Hz"), mX - 500*scaleX, h - 200*scaleY);

    dX = (1000-150)/MAX_CHANNELS;
    dX *= scaleX;

    dc.DrawText(wxT("CH #"),mX-500*scaleX, h - 30*scaleY);
    dc.DrawText(wxT("SV #"),mX-500*scaleX, h - 60*scaleY);
    dc.DrawText(wxT("Locks"),mX-500*scaleX, h - 90*scaleY);

	for(lcv = 0; lcv < MAX_CHANNELS; lcv++)
	{
		pchan = &p->channel[lcv];

		dc.SetFont(wxFont(10, wxDEFAULT, wxNORMAL, wxNORMAL));

		if((pchan->count > 2000.0) && (pchan->CN0 > 20.0))
		{

			if(pNav->nsvs >> lcv)
				dc.SetFont(wxFont(10, wxDEFAULT, wxNORMAL, wxBOLD));

			str.Printf(wxT("%02d"),(int)pchan->sv+1);
			dc.DrawText(str, lcv*dX + 150*scaleX, h - 60*scaleY);

			dY = pchan->CN0 - 20.0;
			dY /= 40.0;
			dY *= 800;
			dY *= scaleY;
			bar[0].x = 0;		bar[0].y = 0;
			bar[1].x = dX/2;	bar[1].y = 0;
			bar[2].x = dX/2;	bar[2].y = -dY;
			bar[3].x = 0;		bar[3].y = -dY;

			gval = 255.0*(pchan->CN0 - 20.0)/40.0;
			rval = 122.0 - 122.0*(pchan->CN0 - 20.0)/40.0;
			dc.SetBrush(wxBrush(wxColor(rval,gval,0)));
			dc.SetPen(wxPen(wxColor(0,0,0), 1));
			dc.DrawPolygon(4, bar, lcv*dX + 150*scaleX, h-200*scaleY);

			str.Clear();
			((int32)pchan->bit_lock) ? 		str += 'B' : str += '_';
			((int32)pchan->frame_lock) ? 	str += 'F' : str += '_';
			dc.DrawText(str, lcv*dX + 150*scaleX, h - 90*scaleY);

		}
		else
		{
			dc.DrawText(wxT("__"), lcv*dX + 150*scaleX, h - 60*scaleY);
			dc.DrawText(wxT("__"), lcv*dX + 150*scaleX, h - 90*scaleY);
		}

		str.Printf(wxT("%02d"),lcv);
		dc.DrawText(str, lcv*dX + 150*scaleX, h - 30*scaleY);
	}

}

void GUI_Main::renderSkyPlot()
{

	int mX, mY, lcv, rval, gval;
	double maxX, maxY, svX, svY;
	double scaleX, scaleY;
	wxString str;

	SPS_M *pNav = &p->sps;
	SV_Position_M *psv;
	Channel_M *pchan;

	maxX = maxY = 1100;

	//wxPaintDC dc(pSkyPlot);
	//wxBufferedPaintDC dc(pSkyPlot, pSkyPlot->GetSize(), wxBUFFER_CLIENT_AREA);
	wxBufferedPaintDC dc(pSkyPlot, wxBUFFER_CLIENT_AREA);
	dc.SetBackground(wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_BACKGROUND)));
	dc.Clear();

	wxCoord w, h;
	pSkyPlot->GetClientSize(&w, &h);

	scaleX = .5*w/maxX; scaleY = .5*h/maxY;

	mX = w/2; mY = h/2;

    /* Draw a circle */
    dc.SetPen(wxPen(wxColor(0,0,0), 1 ));
    dc.DrawCircle(mX, mY, 900*scaleY);
    dc.SetPen(wxPen(wxColor(0,0,0), 1, wxLONG_DASH ));
    /* Draw the circles */
    for(lcv = 5; lcv > 0; lcv--)
    {
    	dc.DrawCircle(mX, mY, lcv*150*scaleY);
    }

    /* Draw the 30 deg lines */
    for(lcv = 0; lcv < 6; lcv++)
    {
    	dc.DrawLine(mX-900*scaleY*cos(lcv*30*DEG_2_RAD),
					mY-900*scaleY*sin(lcv*30*DEG_2_RAD),
					mX+900*scaleY*cos(lcv*30*DEG_2_RAD),
					mY+900*scaleY*sin(lcv*30*DEG_2_RAD));
    }

    dc.SetPen(wxPen(wxColor(0,0,0), 1 ));
    dc.DrawLine(mX, mY-900*scaleY, mX, mY+900*scaleY);
    dc.DrawLine(mX-900*scaleY, mY, mX+900*scaleY, mY);

    dc.SetBrush(wxBrush(wxColor(0,0,0)));
    dc.SetFont(wxFont(14, wxDEFAULT, wxNORMAL, wxBOLD));

    /* Draw the "NESW" */
    str = wxT("N"); dc.DrawText(str,mX-5, mY-900*scaleY-20);
    str = wxT("S"); dc.DrawText(str,mX-5, mY+900*scaleY);
    str = wxT("E"); dc.DrawText(str,mX+900*scaleY+5, mY-10);
    str = wxT("W"); dc.DrawText(str,mX-900*scaleY-25, mY-10);

    /* Now place the SVs */
    for(lcv = 0; lcv < MAX_CHANNELS; lcv++)
    {
    	pchan = &p->channel[lcv];
    	psv = &p->sv_positions[lcv];
    	if((pNav->nsvs >> lcv) & 0x1)
    	{
			gval = 255.0*(pchan->CN0 - 20.0)/40.0;
			rval = 122.0 - 122.0*(pchan->CN0 - 20.0)/40.0;
			dc.SetBrush(wxBrush(wxColor(rval,gval,0)));

    		if(psv->elev > 0.0)
    		{
    			svX = scaleY*(900 - 10.0*RAD_2_DEG*psv->elev) * cos(psv->azim - PI_OVER_2);
    			svY = scaleY*(900 - 10.0*RAD_2_DEG*psv->elev) * sin(psv->azim - PI_OVER_2);
    			dc.SetPen(wxPen(wxColor(0,0,0), 1 ));
    			dc.SetTextForeground(wxColor(0,0,0));
    		}
    		else
    		{
    			svX = scaleY*(900 + 10.0*RAD_2_DEG*psv->elev) * cos(psv->azim - PI_OVER_2);
    			svY = scaleY*(900 + 10.0*RAD_2_DEG*psv->elev) * sin(psv->azim - PI_OVER_2);
    			dc.SetPen(wxPen(wxColor(200,200,200), 1 ));
    			dc.SetTextForeground(wxColor(200,200,200));
    		}

    		str.Printf(wxT("%02d"),(int)pchan->sv+1);
    		dc.DrawCircle(mX + svX, mY + svY, 3);
    		dc.DrawText(str, mX + svX, mY + svY);
    	}
    }

}


void GUI_Main::renderPVT()
{

	wxDateTime theTime;
	wxString str;
	time_t utcsec;
	SPS_M *pNav = &p->sps;
	Clock_M *pClock = &p->clock;

	str.Printf(wxT("% 15.2f"),pNav->x);						px->SetLabel(str);
	str.Printf(wxT("% 15.2f"),pNav->y);						py->SetLabel(str);
	str.Printf(wxT("% 15.2f"),pNav->z);						pz->SetLabel(str);
	str.Printf(wxT("%15.2f"),100*pNav->vx);					vx->SetLabel(str);
	str.Printf(wxT("%15.2f"),100*pNav->vy);					vy->SetLabel(str);
	str.Printf(wxT("%15.2f"),100*pNav->vz);					vz->SetLabel(str);
	str.Printf(wxT("%15.9f"),pNav->latitude*RAD_2_DEG);		lat->SetLabel(str);
	str.Printf(wxT("%15.9f"),pNav->longitude*RAD_2_DEG);	lon->SetLabel(str);
	str.Printf(wxT("%15.2f"),pNav->altitude);				alt->SetLabel(str);
	str.Printf(wxT("%15.6f"),pClock->bias);					cb->SetLabel(str);
	str.Printf(wxT("%15.6f"),pClock->rate);					cr->SetLabel(str);
	str.Printf(wxT("%15.6f"),pClock->time);					gpst->SetLabel(str);

	/* Get into unix time */
	utcsec = floor(pClock->time);
	utcsec += pClock->week*SECONDS_IN_WEEK;
	utcsec += SECONDS_IN_1024_WEEKS;
	utcsec += 315964819;
	utcsec += 13;

	theTime.Set(utcsec);
	date->SetLabel(theTime.FormatISODate());
	hrs->SetLabel(theTime.FormatISOTime());
	str.Printf(wxT("%15.6f"),fmod(pClock->time,1.0)); 		sec->SetLabel(str);

}
