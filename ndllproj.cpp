// ndllproj.cpp: определяет экспортированные функции для приложения DLL.
//
//подключение дополнительных заголовочных файлов 
#include "stdafx.h"
#include "ndllproj.h"
#include "list_helper.h"

using namespace ATL;
using namespace std;


//подключаем заголовочные файлы для разработки приложений в ОС Windows
#include <algorithm>
#include <atlbase.h>
#include <atlconv.h>
#include <atlstr.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <malloc.h>
#include <windows.h>
#include <objidl.h>


//подключаем к приложению GDI и GDI+
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

//Добавление примитива "базис" в файл
//fp - указатель на открытый файловый поток
//basd - указатель на структуру типа basisd_
extern "C" NDLLPROJ_API void addbasisdescr(FILE* fp,basisd_* basd)
{
    fprintf(fp,"%c%c%c%c",0xFE,0xDE,0xAE,0xBE);
    fwrite(basd,sizeof(basisd_),1,fp);
}
//Добавление примитива "точка" в файл
//fp - указатель на открытый файловый поток
//p - указатель на структуру типа point_
extern "C" NDLLPROJ_API void addpoint(FILE* fp,point_* p)
{
    fprintf(fp,"%c%c%c%c",0x50,0x4F,0x49,0x4E);
    fwrite(p,sizeof(point_),1,fp);
}
//Добавление примитива "линия" в файл
//fp - указатель на открытый файловый поток
//line - указатель на структуру типа line_
extern "C" NDLLPROJ_API void addline(FILE* fp,line_* l)
{
    fprintf(fp,"%c%c%c%c",0x4C,0x49,0x4E,0x45);
    fwrite(l,sizeof(line_),1,fp);
}
//Добавление примитива "ломаная" в файл
//fp - указатель на открытый файловый поток
//brl - указатель на структуру типа brokenline_
extern "C" NDLLPROJ_API void addbrokenline(FILE* fp,brokenline_* brl)
{
    fprintf(fp,"%c%c%c%c",0x42,0x52,0x4C,0x49);
    fwrite(brl,sizeof(brokenline_),1,fp);
}
//Добавление примитива "окружность" в файл
//fp - указатель на открытый файловый поток
//cr - указатель на структуру типа circle_
extern "C" NDLLPROJ_API void addcircle(FILE* fp,circle_* cr)
{
    fprintf(fp,"%c%c%c%c",0x43,0x49,0x52,0x43);
    fwrite(cr,sizeof(circle_),1,fp);
}
//Добавление примитива "многоугольник" в файл
//fp - указатель на открытый файловый поток
//pl - указатель на структуру типа polyline_
extern "C" NDLLPROJ_API void addpolyline(FILE* fp,polyline_* pl)
{
    fprintf(fp,"%c%c%c%c",0x50,0x4F,0x4C,0x59);
    fwrite(pl,sizeof(polyline_),1,fp);
}
// Функция strread считывает данные из файла и возвращает тип данных прочитанной структуры
// Функция принимает указатель на файл fp и указатель на указатель на данные data
extern "C" NDLLPROJ_API ptype_t strread(FILE* fp,void** data)
{
    basisd_* bd;
    point_* p;
    line_* l;
    brokenline_* brl;
    circle_* cr;
    polyline_* pl;
    uint32_t id;
    // Считываем первые 4 байта из файла и переставляем байты в порядке little-end
    fread(&id,4,1,fp);
    id = SWAP_UINT32(id);

    // Используем оператор switch для определения типа данных, которые нужно считать из файла
    switch(id)
    {
        // Если id соответствует типу PTYPE_BASE, выделяем память для структуры basisd_ и считываем ее из файла
        case 0xFEDEAEBE:
            bd = (basisd_*)malloc(sizeof(basisd_));
            fread(bd,sizeof(basisd_),1,fp);
            *data = (void*)bd;
            return PTYPE_BASE;
        break;

        // Если id соответствует типу PTYPE_POINT, выделяем память для структуры point_ и считываем ее из файла
        case 0x504F494E:
            p = (point_*)malloc(sizeof(point_));
            fread(p,sizeof(point_),1,fp);
            *data = (void*)p;
            return PTYPE_POINT;
        break;

        // Если id соответствует типу PTYPE_LINE, выделяем память для структуры line_ и считываем ее из файла
        case 0x4C494E45:
            l = (line_*)malloc(sizeof(line_));
            fread(l,sizeof(line_),1,fp);
            *data = (void*)l;
            return PTYPE_LINE;
        break;

        // Если id соответствует типу PTYPE_BROKENLINE, выделяем память для структуры brokenline_ и считываем ее из файла
        case 0x42524C49:
            brl = (brokenline_*)malloc(sizeof(brokenline_));
            fread(brl,sizeof(brokenline_),1,fp);
            *data = (void*)brl;
            return PTYPE_BROKENLINE;
        break;

        // Если id соответствует типу PTYPE_CIRCLE, выделяем память для структуры circle_ и считываем ее из файла
        case 0x43495243:
            cr = (circle_*)malloc(sizeof(circle_));
            fread(cr,sizeof(circle_),1,fp);
            *data = (void*)cr;
            return PTYPE_CIRCLE;
        break;

        // Если id соответствует типу PTYPE_POLYLINE, выделяем память для структуры polyline_ и считываем ее из файла
        case 0x504F4C59:
            pl = (polyline_*)malloc(sizeof(polyline_));
            fread(pl,sizeof(polyline_),1,fp);
            *data = (void*)pl;
            return PTYPE_POLYLINE;
        break;

        // Если id не соответствует ни одному из известных типов данных, возвращаем PTYPE_NULL
        default:
            return PTYPE_NULL;
        break;
    }

}
// Функция IsInside проверяет, находится ли точка с координатами (x, y) внутри какой-либо фигуры из списка head.
// Функция принимает три аргумента: координаты точки (x, y), указатель на первый элемент списка head и возвращает указатель на элемент списка, внутри которого находится точка.
// Если точка не находится внутри ни одной фигуры, функция возвращает NULL.
extern "C" NDLLPROJ_API datatype_t* IsInside(uint32_t x,uint32_t y,datatype_t* head)
{
    datatype_t* currelem = head;
    datatype_t* ret = NULL;

    // Создаем объект Point с координатами (x, y)
    Point p;
    p.X = x;
    p.Y = y;

    // Проходим по всем элементам списка
    while (currelem)
    {
        // Если тип элемента не является типом базиса или пустым типом
        if(currelem->type!=PTYPE_BASE && currelem->type!=PTYPE_NULL)
        {
            // Если точка находится внутри текущей фигуры
            if(currelem->r->IsVisible(p,NULL))
            {
                // Запоминаем указатель на текущий элемент и выходим из цикла
                ret = currelem;
                break;
            }	
        }
        currelem = currelem->next;			// Переходим к следующему элементу списка
    }
    return ret;								// Возвращаем указатель на элемент списка, внутри которого находится точка
}
// Функция ppoint рисует точку на графическом контексте graphics с координатами (x, y) и цветом, заданным значениями red, green и blue.
// Функция принимает 6 аргументов: указатель на графический контекст graphics, координаты точки (x, y) и значения цвета red, green и blue.
extern "C" NDLLPROJ_API void ppoint(Graphics* graphics,uint32_t x,uint32_t y,char red, char green, char blue)
{
   Pen* pen = new  Pen(Color(255, red, green, blue),1);	// Создаем объект Pen с цветом, заданным значениями red, green и blue и толщиной, равной 1
   graphics->DrawRectangle(pen,Rect(x,y,1,1));			// Рисуем прямоугольник на графическом контексте graphics с координатами (x, y) и размером 1x1, используя объект Pen
   delete pen;											// Освобождаем память, занятую объектом Pen
}
// Функция pline рисует линию на графическом контексте graphics, соединяющую точки (x, y) и (x1, y1) с толщиной и цветом, заданными значения thickness, red, green и blue.
// Функция принимает девять аргументов: указатель на графический контекст graphics, координаты начальной точки (x, y), координаты конечной точки (x1, y1), толщину линии thickness и значения цвета red, green и blue.
extern "C" NDLLPROJ_API void pline(Graphics* graphics,uint32_t x,uint32_t y,uint32_t x1,uint32_t y1,char thickness,char red,char green,char blue)
{
   Pen* pen = new Pen(Color(255, red, green, blue),thickness);	// Создаем объект Pen с цветом, заданным значениями red, green и blue, и толщиной, заданной значением thickness
   graphics->DrawLine(pen, (int)x, (int)y, (int)x1, (int)y1);	// Рисуем линию на графическом контексте graphics, соединяющую точки (x, y) и (x1, y1), используя объект Pen
   delete pen;													// Освобождаем память, занятую объектом Pen
}
// Функция pbrokenline рисует ломаную линию на графическом контексте graphics, проходящую через точки, заданные массивами arrx и arry, с толщиной и цветом, заданными значениями thickness, red, green и blue.
// Функция принимает восемь аргументов: указатель на графический контекст graphics, массивы координат точек arrx и arry, количество точек count, толщину линии thickness и значения цвета red, green и blue.
extern "C" NDLLPROJ_API void pbrokenline(Graphics* graphics,uint32_t* arrx,uint32_t* arry,int count,char thickness,char red,char green,char blue)
{
   Pen* pen = new Pen(Color(255, red, green, blue),thickness);	// Создаем объект Pen с цветом, заданным значениями red, green и blue, и толщиной, заданной значением thickness

   // Добавляем линии между точками, заданными массивами arrx и arry, в объект GraphicsPath
   GraphicsPath grpath;
   for(int i=0;i<count-1;i++)
   {
       grpath.AddLine(Point(arrx[i],arry[i]),Point(arrx[i+1],arry[i+1]));
   }

   graphics->DrawPath(pen,&grpath);		// Рисуем ломаную линию на графическом контексте, используя объект Pen и объект GraphicsPath
   delete pen;							// Освобождаем память, занятую объектом Pen
}
// Функция pcircle рисует окружность на графическом контексте graphics с центром в точке (xcenter, ycenter) и радиусом radius, с толщиной и цветом заданными значениями thickness, red, green и blue.
// Функция принимает восемь аргументов: указатель на графический контекст graphics, координаты центра окружности xcenter и ycenter радиус окружности radius, толщину линии thickness и значения цвета red, green и blue.
extern "C" NDLLPROJ_API void pcircle(Graphics* graphics,uint32_t xcenter,uint32_t ycenter,uint32_t radius,char thickness,char red,char green,char blue)
{
   Pen* pen = new Pen(Color(255, red, green, blue),thickness);				// Создаем объект Pen с цветом, заданным значениями red, green и blue, и толщиной, заданной значением thickness
   RectF ellipseRect(xcenter-radius, ycenter-radius, 2*radius, 2*radius);	// Создаем объект RectF, задающий квадрат, в который вписана окружность
   graphics->DrawEllipse(pen,ellipseRect);			// Рисуем окружность на графическом контексте graphics, используя объект Pen и объект RectF
   delete pen;										// Освобождаем память, занятую объектом Pen
}
// Функция ppolyline рисует многоугольник на графическом контексте graphics, проходящую через точки, заданные массивами arrx и arry, с толщиной и цветом, заданнымими thickness, red, green и blue.
// Функция принимает восемь аргументов: указатель на графический контекст graphics, массивы координат точек arrx и arr, количество точек count, толщину линии thickness и значения цвета red, green и blue.
extern "C" NDLLPROJ_API void ppolyline(Graphics* graphics,uint32_t* arrx,uint32_t* arry,int count,char thickness,char red,char green,char blue)
{
   POINT arrp[10];		// Создаем массив POINT arrp для хранения точек полилинии
   HRGN region;
   Pen* pen = new Pen(Color(255, red, green, blue),thickness);		// Создаем объект Pen с цветом, заданным значениями red, green и blue, и толщиной, заданной значением thickness
   GraphicsPath grpath;

   // Добавляем линии между точками, заданными массивами arrx и arry, в объект GraphicsPath
   for(int i=0;i<count-1;i++)
   {
       grpath.AddLine(Point(arrx[i],arry[i]),Point(arrx[i+1],arry[i+1]));
   }

   grpath.AddLine(Point(arrx[0],arry[0]),Point(arrx[count-1],arry[count-1]));	// Добавляем линию, соединяющую последнюю точку с первой, в объект GraphicsPath
   graphics->DrawPath(pen,&grpath);		// Рисуем многоугольник на графическом контексте, используя объект Pen и объект GraphicsPath
   delete pen;							//Освобождаем память, занятую объектом Pen
}
// Функция getWindowHeadSize возвращает высоту заголовка окна, заданного дескриптором hWnd.
// Функция принимает один аргумент: дескриптор окна hWnd.
int getWindowHeadSize(HWND hWnd)
{
    RECT Rect;		
    GetWindowRect(hWnd, &Rect);
    POINT point = { 0, 0 };
    ClientToScreen(hWnd, &point);
    return point.y - Rect.top + GetSystemMetrics(SM_CYSIZEFRAME) - GetSystemMetrics(SM_CYEDGE) * 2;
}
// Функция getWindowHeadSizew возвращает ширину заголовка окна, заданного дескриптором hWnd.
// Функция принимает один аргумент: дескритор окна hWnd.
int getWindowHeadSizew(HWND hWnd)
{
    RECT Rect;
    GetWindowRect(hWnd, &Rect);
    POINT point = { 0, 0 };
    ClientToScreen(hWnd, &point);
    return point.x - Rect.left + GetSystemMetrics(SM_CXSIZEFRAME) - GetSystemMetrics(SM_CXEDGE) * 2;
}
// Функция PaintBitmapinHDC отображает изображение в контексте устройства (HDC) окна (HWND).
// Функция принимает следующие аргументы:
// hWnd - дкриптор окна, в котором будет отображаться изображение;
// hWndScroll - дескриптор окна, содержащего горизонтальный ползунок прокрутки;
// hWndScroll1 - дескриптор окна, содержащего вертикальный ползунок прокрутки;
// hDC - контекст устройства окна, в котором будет отображаться изображение;
// memBit - контекст устройства, содержщий изображение;
// state - состояние отображения изображения (0 - без масштабирования, 1 - масштабирование в 2 раза, 2 - масштабирование в 3 раза, 
// -1 - уменьшение в 2 раза, -2 - уменьшение в 3 раза);
// x_ - координата x верхнего левого угла области изображения, которая будет отображаться в окне;
// y_ - координата y верхнего левого угла области изображения, которая будет отображаться в окне;
// width - ширина области изображения, которая будет отображаться в окне;
// height - высота области изображения, которая будет отображаться в окне.
void PaintBitmapinHDC(HWND hWnd,HWND hWndScroll,HWND hWndScroll1,HDC hDC,HDC memBit,int state,int x_,int y_,int width,int height)
{
      SetStretchBltMode(hDC,COLORONCOLOR);

      // Выполняем действия в зависимости от состояния отображения изображения
      switch(state)
      {

            // Если состояние равно 0, то устанавливаем размер окна, отображаем изображение без масштабирования, скрываем ползунки прокрутки
            case 0:
                SetWindowPos(hWnd,0,0,0,width+getWindowHeadSizew(hWnd)-1,height+getWindowHeadSize(hWnd)-1,SWP_NOMOVE);
                StretchBlt(hDC,0,0,width,height,memBit,0,0,width,height,SRCCOPY);
                ShowWindow(hWndScroll,SW_HIDE);
                ShowWindow(hWndScroll1,SW_HIDE);
            break;

             // Если состояние равно 1, то масштабируем изображение в 2 раза, отображаем ползунки прокрутки
            case 1:
                StretchBlt(hDC,0,0,2*width,2*height,memBit,x_,y_,width+17,height+17,SRCCOPY);
                SetScrollRange(hWndScroll, SB_CTL, 0, width/2, TRUE);
                SetScrollRange(hWndScroll1, SB_CTL, 0, height/2, TRUE);
                ShowWindow(hWndScroll,SW_SHOW);
                ShowWindow(hWndScroll1,SW_SHOW);
            break;

            // Если состояние равно 2, то масштабируем изображение в 3 раза, отображаем ползунки прокрутки
            case 2:
                SetScrollRange(hWndScroll, SB_CTL, 0, width*2/3, TRUE);
                SetScrollRange(hWndScroll1, SB_CTL, 0, height*2/3, TRUE);
                     StretchBlt(hDC,0,0,3*width,3*height,memBit,x_,y_,width+17,height+17,SRCCOPY);
            break;

            // Если состояние равно -1, то уменьшаем изображение в 2 раза, устанавливаем размер окна
            case -1:
                SetWindowPos(hWnd,0,0,0,width/2+getWindowHeadSizew(hWnd)-1,height/2+getWindowHeadSize(hWnd)-1,SWP_NOMOVE);
                StretchBlt(hDC,0,0,width,height,memBit,0,0,2*width,2*height,SRCCOPY);
            break;

            // Если состояние равно -2, то уменьшаем изображение в 3 раза, устанавливаем размер окна
            case -2:
                SetWindowPos(hWnd,0,0,0,width/3+getWindowHeadSizew(hWnd)-1,height/3+getWindowHeadSize(hWnd)-1,SWP_NOMOVE);
                StretchBlt(hDC,0,0,width,height,memBit,0,0,3*width,3*height,SRCCOPY);
            break;
      }
}
