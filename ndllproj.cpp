// ndllproj.cpp: определяет экспортированные функции для приложения DLL.
//

#include "stdafx.h"
#include "ndllproj.h"
#include "list_helper.h"
#include <algorithm>
#include <atlbase.h>
#include <atlconv.h>
#include <atlstr.h>
using namespace ATL;
using namespace std;
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <malloc.h>
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")
extern "C" NDLLPROJ_API void addbasisdescr(FILE* fp,basisd_* basd)
{
    fprintf(fp,"%c%c%c%c",0xFE,0xDE,0xAE,0xBE);
	fwrite(basd,sizeof(basisd_),1,fp);
}
extern "C" NDLLPROJ_API void addpoint(FILE* fp,point_* p)
{
    fprintf(fp,"%c%c%c%c",0x50,0x4F,0x49,0x4E);
	fwrite(p,sizeof(point_),1,fp);
}
extern "C" NDLLPROJ_API void addline(FILE* fp,line_* l)
{
    fprintf(fp,"%c%c%c%c",0x4C,0x49,0x4E,0x45);
	fwrite(l,sizeof(line_),1,fp);
}
extern "C" NDLLPROJ_API void addbrokenline(FILE* fp,brokenline_* brl)
{
    fprintf(fp,"%c%c%c%c",0x42,0x52,0x4C,0x49);
	fwrite(brl,sizeof(brokenline_),1,fp);
}
extern "C" NDLLPROJ_API void addcircle(FILE* fp,circle_* cr)
{
    fprintf(fp,"%c%c%c%c",0x43,0x49,0x52,0x43);
	fwrite(cr,sizeof(circle_),1,fp);
}
extern "C" NDLLPROJ_API void addpolyline(FILE* fp,polyline_* pl)
{
    fprintf(fp,"%c%c%c%c",0x50,0x4F,0x4C,0x59);
	fwrite(pl,sizeof(polyline_),1,fp);
}
extern "C" NDLLPROJ_API ptype_t strread(FILE* fp,void** data)
{
    basisd_* bd;
    point_* p;
    line_* l;
    brokenline_* brl;
    circle_* cr;
    polyline_* pl;
    uint32_t id;
    //char strtype = 0;
    fread(&id,4,1,fp);
    id = SWAP_UINT32(id);
    switch(id)
    {
        case 0xFEDEAEBE:
			bd = (basisd_*)malloc(sizeof(basisd_));
			fread(bd,sizeof(basisd_),1,fp);
			*data = (void*)bd;
			return PTYPE_BASE;
        break;
        case 0x504F494E:
			p = (point_*)malloc(sizeof(point_));
			fread(p,sizeof(point_),1,fp);
			*data = (void*)p;
			return PTYPE_POINT;
        break;
        case 0x4C494E45:
			l = (line_*)malloc(sizeof(line_));
			fread(l,sizeof(line_),1,fp);
			*data = (void*)l;
			return PTYPE_LINE;
        break;
        case 0x42524C49:
			brl = (brokenline_*)malloc(sizeof(brokenline_));
			fread(brl,sizeof(brokenline_),1,fp);
			*data = (void*)brl;
			return PTYPE_BROKENLINE;
        break;
        case 0x43495243:
			cr = (circle_*)malloc(sizeof(circle_));
			fread(cr,sizeof(circle_),1,fp);
			*data = (void*)cr;
			return PTYPE_CIRCLE;
        break;
        case 0x504F4C59:
			pl = (polyline_*)malloc(sizeof(polyline_));
			fread(pl,sizeof(polyline_),1,fp);
			*data = (void*)pl;
			return PTYPE_POLYLINE;
        break;
		default:
			return PTYPE_NULL;
		break;
    }

}
extern "C" NDLLPROJ_API datatype_t* IsInside(uint32_t x,uint32_t y,datatype_t* head)
{
	datatype_t* currelem = head;
	datatype_t* ret = NULL;
	Point p;
	p.X = x;
	p.Y = y;
	while (currelem)
	{
		if(currelem->type!=PTYPE_BASE && currelem->type!=PTYPE_NULL)
		{
			if(currelem->r->IsVisible(p,NULL))
			{
				ret = currelem;
				break;
			}	
		}
		currelem = currelem->next;
	}
	return ret;
}
extern "C" NDLLPROJ_API void ppoint(Graphics* graphics,/*datatype_t* ptr,*/uint32_t x,uint32_t y,/*unsigned char aim_area,*/char red, char green, char blue/*, bool is_alias*/)
{
   //Graphics graphics(hdc);
   //if(!ptr->g)
   //{
	   //ptr->g = &graphics;
	   //ptr->g = new Graphics(hdc);
   //}
   //if(is_alias)
		//graphics.SetSmoothingMode(SmoothingModeAntiAlias);
   Pen* pen = new  Pen(Color(255, red, green, blue),1);
   graphics->DrawRectangle(pen,Rect(x,y,1,1));
   delete pen;
}
extern "C" NDLLPROJ_API void pline(Graphics* graphics,/*datatype_t* ptr,*/uint32_t x,uint32_t y,uint32_t x1,uint32_t y1,char thickness,/*unsigned char aim_area,*/char red,char green,char blue/*,bool is_alias*/)
{
   //Graphics graphics(hdc);
   //if(!ptr->g)
   //{
	   //ptr->g = &graphics;
	   //ptr->g = new Graphics(hdc);
   //}
   /*int k = ((y1-y)*aim_area)/(x1-x);
   POINT points[] = {{x-aim_area,k+y},{x+aim_area,-k+y},{x1+aim_area,-k+y1},{x1-aim_area,k+y1}};
   int leftup = 0;
   int leftdown = 0;
   int rightup = 0;
   int rightdown = 0;
   int swap_ = 0;
   for(int i = 0;i<4;i++)
    {
	   if(points[leftup].y > points[i].y)
		    leftup = i;
    }
    rightup = (leftup + 1) % 4;
    for(int i = 0;i<4;i++)
    {
	   if(i==leftup)
            continue;
	   if(points[rightup].y >= points[i].y)
		    rightup = i;
    }
    for(int i = 0;i<4;i++)
    {
	   if(i!=rightup && i!=leftup)
	   {
		   leftdown = rightdown;
		   rightdown = i;
	   }
    }
    if(points[leftup].x > points[rightup].x)
    {
	   swap_ = leftup;
	   leftup = rightup;
	   rightup = swap_;
    }
    if(points[leftdown].x > points[rightdown].x)
    {
	   swap_ = leftdown;
	   leftdown = rightdown;
	   rightdown = swap_;
    }
   POINT points_[4] = {points[rightdown],points[leftdown],points[leftup],points[rightup]};
   HRGN region = CreatePolygonRgn(points_, 4, WINDING);
   if(!ptr->r)
		ptr->r = new Region(region);
   DeleteObject(region);*/
   //if(is_alias)
		//graphics.SetSmoothingMode(SmoothingModeAntiAlias);
   Pen* pen = new Pen(Color(255, red, green, blue),thickness);
   graphics->DrawLine(pen, (int)x, (int)y, (int)x1, (int)y1);
   delete pen;
}
extern "C" NDLLPROJ_API void pbrokenline(Graphics* graphics,/*datatype_t* ptr,*/uint32_t* arrx,uint32_t* arry,int count,char thickness,char red,char green,char blue/*,bool is_alias*/)
{
   //Point parr[10];
   //POINT arrp[10];
   //HRGN region;
   //Graphics graphics(hdc);
   //if(!ptr->g)
   //{
	   //ptr->g = &graphics;
	   //ptr->g = new Graphics(hdc);
   //}
   //if(is_alias)
		//graphics.SetSmoothingMode(SmoothingModeAntiAlias);
   Pen* pen = new Pen(Color(255, red, green, blue),thickness);
   GraphicsPath grpath;
   for(int i=0;i<count-1;i++)
   {
	   //parr[i] = Point(arrx[i],arry[i]);
	   //parr[i+1] = Point(arrx[i+1],arry[i+1]);
	   grpath.AddLine(Point(arrx[i],arry[i]),Point(arrx[i+1],arry[i+1]));
	   //arrp[i].x = arrx[i];
	   //arrp[i].y = arry[i];
   }
   //arrp[count-1].x = arrx[count-1];
   //arrp[count-1].y = arry[count-1];
   /*region = CreatePolygonRgn(arrp,count,WINDING);
   if(!ptr->r)
		ptr->r = new Region(region);
   DeleteObject(region);*/
   graphics->DrawPath(pen,&grpath);
   delete pen;
}
extern "C" NDLLPROJ_API void pcircle(Graphics* graphics,/*datatype_t* ptr,*/uint32_t xcenter,uint32_t ycenter,uint32_t radius,char thickness,/*unsigned char aim_area,*/char red,char green,char blue/*,bool is_alias*/)
{
   //Region* r;
   //HRGN region;
   //Graphics graphics(hdc);
   //if(!ptr->g)
   //{
	   //ptr->g = &graphics;
	   //ptr->g = new Graphics(hdc);
   //}
   //region = CreateEllipticRgn(xcenter-radius-aim_area,ycenter+radius+aim_area,xcenter+radius+aim_area,ycenter-radius-aim_area);
   /*if(!ptr->r)
		ptr->r = new Region(region);
   DeleteObject(region);*/
   //if(is_alias)
		//graphics.SetSmoothingMode(SmoothingModeAntiAlias);
   Pen* pen = new Pen(Color(255, red, green, blue),thickness);
   RectF ellipseRect(xcenter-radius, ycenter-radius, 2*radius, 2*radius);
   //try {
   graphics->DrawEllipse(pen,ellipseRect);
   //} catch(...) {};
   delete pen;
}
extern "C" NDLLPROJ_API void ppolyline(Graphics* graphics,/*datatype_t* ptr,*/uint32_t* arrx,uint32_t* arry,int count,char thickness,char red,char green,char blue/*,bool is_alias*/)
{
   POINT arrp[10];
   HRGN region;
   //Graphics graphics(hdc);
   //if(!ptr->g)
   //{
	   //ptr->g = &graphics;
	   //ptr->g = new Graphics(hdc);
   //}
   //if(is_alias)
		//graphics.SetSmoothingMode(SmoothingModeAntiAlias);
   Pen* pen = new Pen(Color(255, red, green, blue),thickness);
   GraphicsPath grpath;
   for(int i=0;i<count-1;i++)
   {
	   grpath.AddLine(Point(arrx[i],arry[i]),Point(arrx[i+1],arry[i+1]));
	   //arrp[i].x = arrx[i];
	   //arrp[i].y = arry[i];
   }
   //arrp[count-1].x = arrx[count-1];
   //arrp[count-1].y = arry[count-1];
   grpath.AddLine(Point(arrx[0],arry[0]),Point(arrx[count-1],arry[count-1]));
   /*region = CreatePolygonRgn(arrp,count,WINDING);
   if(!ptr->r)
		ptr->r = new Region(region);
   DeleteObject(region);*/
   graphics->DrawPath(pen,&grpath);
   delete pen;
}
int getWindowHeadSize(HWND hWnd)
{
    RECT Rect;
    GetWindowRect(hWnd, &Rect);
    POINT point = { 0, 0 };
    ClientToScreen(hWnd, &point);
    return point.y - Rect.top + GetSystemMetrics(SM_CYSIZEFRAME) - GetSystemMetrics(SM_CYEDGE) * 2;
}
int getWindowHeadSizew(HWND hWnd)
{
    RECT Rect;
    GetWindowRect(hWnd, &Rect);
    POINT point = { 0, 0 };
    ClientToScreen(hWnd, &point);
    return point.x - Rect.left + GetSystemMetrics(SM_CXSIZEFRAME) - GetSystemMetrics(SM_CXEDGE) * 2;
}
void PaintBitmapinHDC(HWND hWnd,HWND hWndScroll,HWND hWndScroll1,HDC hDC,HDC memBit,int state,int x_,int y_,int width,int height)
{
	  SetStretchBltMode(hDC,COLORONCOLOR);
	  switch(state)
	  {
			case 0:
				SetWindowPos(hWnd,0,0,0,width+getWindowHeadSizew(hWnd)-1,height+getWindowHeadSize(hWnd)-1,SWP_NOMOVE);
				StretchBlt(hDC,0,0,width,height,memBit,0,0,width,height,SRCCOPY);
				ShowWindow(hWndScroll,SW_HIDE);
				ShowWindow(hWndScroll1,SW_HIDE);
			break;
			case 1:
				StretchBlt(hDC,0,0,2*width,2*height,memBit,x_,y_,width+17,height+17,SRCCOPY);
				SetScrollRange(hWndScroll, SB_CTL, 0, width/2, TRUE);
				SetScrollRange(hWndScroll1, SB_CTL, 0, height/2, TRUE);
				ShowWindow(hWndScroll,SW_SHOW);
				ShowWindow(hWndScroll1,SW_SHOW);
			break;
			case 2:
				SetScrollRange(hWndScroll, SB_CTL, 0, width*2/3, TRUE);
				SetScrollRange(hWndScroll1, SB_CTL, 0, height*2/3, TRUE);
				StretchBlt(hDC,0,0,3*width,3*height,memBit,x_,y_,width+17,height+17,SRCCOPY);
			break;
			case -1:
				SetWindowPos(hWnd,0,0,0,width/2+getWindowHeadSizew(hWnd)-1,height/2+getWindowHeadSize(hWnd)-1,SWP_NOMOVE);
				StretchBlt(hDC,0,0,width,height,memBit,0,0,2*width,2*height,SRCCOPY);
			break;
			case -2:
				SetWindowPos(hWnd,0,0,0,width/3+getWindowHeadSizew(hWnd)-1,height/3+getWindowHeadSize(hWnd)-1,SWP_NOMOVE);
				StretchBlt(hDC,0,0,width,height,memBit,0,0,3*width,3*height,SRCCOPY);
			break;
	  }
}
