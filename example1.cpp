// example1.cpp: определяет точку входа для приложения.
//

using namespace ATL; 
using namespace std;

//подключение дополнительных заголовочных файлов 
#include "stdafx.h"
#include "example1.h"
#include "list_helper.h"
#include <ndllproj.h>

//подключаем заголовочные файлы для разработки приложений в ОС Windows
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <algorithm>
#include <atlbase.h>
#include <atlconv.h>
#include <atlstr.h>
#include <windows.h>
#include <shellapi.h>

//подключаем к приложению GDI и GDI+
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

//объявления глобальных переменных для рисования
static HDC memBit1;			//промежуточный контекст рисования
static HBITMAP hBitmap;  
static BITMAP bm;
static HBITMAP hBMP,hOldBMP;
static HDC hdc;			//контекст устройства для рисования в окне приложения
static PAINTSTRUCT ps;
HWND hwndScroll,hwndScroll1;	//хэндлы для полос прокрутки

//промежуточные переменные для работы программы
int argc;					//количество переданных программе аргументов командной строки
char** argv;					//массив аргументов командной строки программы
uint32_t basex;
uint32_t basey;
float scalex;
float scaley;
int x_ = 0;					//координата х=0 при скроллинге окна по горизонтали
int y_ = 0;					//координата y=0 при скроллинге окна по вертикали
int cx_ = 0;					//коэффициент масштабирования по оси х (зависит от state)
int cy_ = 0;					//коэффициент масштабирования по оси y (зависит от state)
uint32_t x,y;
bool AliasFlag = false;			//флаг сглаживания
//состояние масштаба окна - исходный = 0
//200 % по каждой оси = 1 
//300 % по каждой оси = 2
//50 % по каждой оси = -1
//33 % по каждой оси = -2
int state = 0;
datatype_t* head = NULL;

//пересчет координаты с учетом базиса
//coord - пересчитываемая координата
//scale - коэффициент масштабирования 
//base - базовый коэффициент пересчета
//возвращает пересчитанную координату
int recount(int coord, float scale,int base)
{
	return (int)(coord*scale+base);
}

//обратный пересчет координаты с учетом базиса
//coord - пересчитываемая координата
//scale - коэффициент масштабирования 
//base - базовый коэффициент пересчета
//возвращает обратно пересчитанную координату
//rev_recount(recount(coord,...),...) = coord - если при пересчете не было округлений
int rev_recount(int coord, float scale,int base)
{
	return (int)(coord-base)/scale;
}

// Функция для очистки списка
// head - вершина списка
void clearlist(datatype_t* head)
{
	datatype_t* ptr,*nptr;
	ptr = head;			// Установка указателя на вершину списка
	while(ptr)			// Пока не достигнут конец списка
	{
		nptr = ptr->next;	//Переход к следующему элементу списка
		LIST_DEL(head,ptr);	//Удаление предыдущего элемента списка 
		free(ptr->data);	//Очистка памяти, выделенной через calloc
		delete ptr->r;	//Очистка памяти региона
		free(ptr);
		ptr = nptr;
	}
}

//заполнение списка данными о примитивах из файла
//fname - строка с именем открываемого файла
//head - указатель на указатель на вершину списка
bool readfile(char* fname,datatype_t** head)
{
	//промежуточные переменные для заполнения списка и вычислений
	void* v;
	ptype_t type = PTYPE_NULL;
	datatype_t* ptr;
	FILE* fp;
	basisd_* ptrb;
	point_* ptrp;
	line_* ptrl;
	brokenline_* ptrbrl;
	circle_* ptrc;
	polyline_* ptrpl;
	POINT points[4];
	POINT arrp[10];
	uint32_t arrx[10];
	uint32_t arry[10];
	int x,y,x1,y1,aim_area,count,xcenter,ycenter,radius;
	HRGN region;

	// Если список уже содержит элементы, возвращаем true
	if(*head)
		return true;
	// Открываем файл для чтения
	if ((fp = fopen(fname, "rb")) == NULL) //открываем файл для чтения данных
		 return false;
	else
	{
			// Читаем данные из файла, пока не достигнем конца файла
			while((type=strread(fp,&v))!=PTYPE_NULL)
			{

				//Eсли первый элемент файла не базис - файл поврежден
				if(*head==NULL && type!=PTYPE_BASE)
					return false;

				// Создаем новый элемент списка и устанавливаем его тип и данные
				ptr = (datatype_t*)calloc(1,sizeof(datatype_t));
				ptr->type = type;
				ptr->data = v;

				// Создаем объект региона, связанный с элементом, в зависимости от типа данных
				switch(type)
				{
					//примитиву базиса регион не требуется
					case PTYPE_BASE:
						ptr->r = NULL;
					break;
					//примитив точка - квадратный регион размерами 2*aim_area на 2*aim_area
					case PTYPE_POINT:
						aim_area = ((point_*)(ptr->data))->aim_area;
						x = ((point_*)(ptr->data))->x;
						y = ((point_*)(ptr->data))->y;
						//создание объекта HRGN
						region = CreateRectRgn(x-aim_area,y-aim_area,x+aim_area,y+aim_area);	
						//создание объекта региона на основе объекта HRGN	
						ptr->r = new Region(region);									//удаление объекта HRGN
						DeleteObject(region);													
					break;
					//примитив линия - квадратный регион размерами 2*aim_area на 2*aim_area
					case PTYPE_LINE:
						aim_area = ((line_*)(ptr->data))->aim_area;
						x = ((line_*)(ptr->data))->x;
						y = ((line_*)(ptr->data))->y;
						x1 = ((line_*)(ptr->data))->x1;
						y1 = ((line_*)(ptr->data))->y1;

						//вычисление координат середины прямой
						xcenter = (x1 + x)/2;
						ycenter = (y1 + y)/2;

						//создание объекта HRGN
						region = CreateRectRgn(xcenter-aim_area,ycenter-aim_area,xcenter+aim_area,ycenter+aim_area);
						//создание объекта региона на основе объекта HRGN		
						ptr->r = new Region(region);
						//удаление объекта HRGN
						DeleteObject(region);									break;
					case PTYPE_BROKENLINE:
						count = ((brokenline_*)(ptr->data))->count;

						//разбиение массива arr на массивы абсцисс и ординат точек
						for(int i=0;i<10;i++)
						{
							arrx[i]=((brokenline_*)(ptr->data))->arr[2*i];
							arry[i]=((brokenline_*)(ptr->data))->arr[2*i+1];
						}

						//создание объектов point на основе arrx и arry
						for(int i=0;i<=count-1;i++)
						{
							arrp[i].x = arrx[i];
							arrp[i].y = arry[i];
						}
						//создание объекта HRGN	
						region = CreatePolygonRgn(arrp,count,WINDING);	
						//создание объекта региона на основе объекта HRGN
						ptr->r = new Region(region);									//удаление объекта HRGN
						DeleteObject(region);				
					break;
					case PTYPE_CIRCLE:
						xcenter = ((circle_*)(ptr->data))->xcenter;
						ycenter = ((circle_*)(ptr->data))->ycenter;
						radius = ((circle_*)(ptr->data))->radius;
						aim_area = ((circle_*)(ptr->data))->aim_area;

						//создание объекта HRGN	
						region = CreateEllipticRgn(xcenter-radius-aim_area,ycenter+radius+aim_area,xcenter+radius+aim_area,ycenter-radius-aim_area);
						//создание объекта региона на основе объекта HRGN
						ptr->r = new Region(region);									//удаление объекта HRGN
						DeleteObject(region);				
					break;
					case PTYPE_POLYLINE:
						count = ((brokenline_*)(ptr->data))->count;

						//разбиение массива arr на массивы абсцисс и ординат точек
						for(int i=0;i<10;i++)
						{
							arrx[i]=((brokenline_*)(ptr->data))->arr[2*i];
							arry[i]=((brokenline_*)(ptr->data))->arr[2*i+1];
						}

						//создание объектов point на основе arrx и arry
						for(int i=0;i<=count-1;i++)
						{
							arrp[i].x = arrx[i];
							arrp[i].y = arry[i];
						}
//создание объекта HRGN
						region = CreatePolygonRgn(arrp,count,WINDING);							//создание объекта региона на основе объекта HRGN
						ptr->r = new Region(region);									//удаление объекта HRGN
						DeleteObject(region);				
					break;
				}
				LIST_ADD(*head,ptr);	// Добавляем элемент в список
			}
		return true;
	}
}

// Функция для отрисовки графических объектов из списка в контексте устройства
void vgraph(datatype_t* head_,HDC hdc,int reswidth,int resheight,bool isalias)
{
	 ptype_t type;
	 datatype_t* ptr = head_;
	 basisd_* ptrb;
	 point_* ptrp;
	 line_* ptrl;
	 brokenline_* ptrbrl;
	 circle_* ptrc;
	 polyline_* ptrpl;
	 uint32_t arrx[10];
	 uint32_t arry[10];
	 bool res = false;
	 Graphics* g = new Graphics(hdc);

	 // Если isalias равен true, устанавливаем режим сглаживания
	 if(isalias)
		g->SetSmoothingMode(SmoothingModeAntiAlias);

	 // Проходим по всем элементам списка и отрисовываем их в зависимости от типа данных
	 while(ptr)
	 {
		switch(ptr->type)
		{
			case PTYPE_BASE:
				ptrb = (basisd_*)ptr->data;
				basex = ptrb->basex;
				basey = ptrb->basey;
				scalex = reswidth/(float)(ptrb->refwidth);
				scaley = resheight/(float)(ptrb->refheight);
			break;
			case PTYPE_POINT:
				ptrp = (point_*)ptr->data;
				ppoint(g,recount(ptrp->x,scalex,basex),recount(ptrp->y,scaley,basey),/*ptrp->aim_area,*/ptrp->red,ptrp->green,ptrp->blue);
			break;
			case PTYPE_LINE:
				ptrl = (line_*)ptr->data;
				pline(g,recount(ptrl->x,scalex,basex),recount(ptrl->y,scaley,basey),recount(ptrl->x1,scalex,basex),recount(ptrl->y1,scaley,basey),ptrl->thickness,ptrl->red,ptrl->green,ptrl->blue);
			break;
			case PTYPE_BROKENLINE:
				ptrbrl = (brokenline_*)ptr->data;
				for(int i=0;i<10;i++)
				{
					arrx[i]=recount((ptrbrl->arr)[2*i],scalex,basex);
					arry[i]=recount((ptrbrl->arr)[2*i+1],scaley,basey);
				}
				pbrokenline(g,arrx,arry,ptrbrl->count,ptrbrl->thickness,ptrbrl->red,ptrbrl->green,ptrbrl->blue);
			break;
			case PTYPE_CIRCLE:
				ptrc = (circle_*)ptr->data;
				pcircle(g,recount(ptrc->xcenter,scalex,basex),recount(ptrc->ycenter,scaley,basey),ptrc->radius*(scalex <= scaley ? scalex : scaley),ptrc->thickness,ptrc->red,ptrc->green,ptrc->blue);
			break;
			case PTYPE_POLYLINE:
				ptrpl = (polyline_*)ptr->data;
				for(int i=0;i<10;i++)
				{
					arrx[i]=recount((ptrpl->arr)[2*i],scalex,basex);
					arry[i]=recount((ptrpl->arr)[2*i+1],scaley,basey);
				}
				ppolyline(g,arrx,arry,ptrpl->count,ptrpl->thickness,ptrpl->red,ptrpl->green,ptrpl->blue);
			 break;
			}
			ptr = ptr->next;		//переход к следующему элементу списка
		}
		delete g;					//удаление объекта Graphics
}
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//точка входа в оконное приложение
//hInstance - дескриптор экземпляра приложения, который был передан операционной системой при запуске приложения.
//lpCmdLine - указатель на строку, содержащую аргументы командной строки, переданные приложению.
//nCmdShow - флаг, указывющий, как окно приложения должно быть показано при запуске.
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, INT iCmdShow)
{
   HWND                hWnd;
   MSG                 msg;
   WNDCLASS            wndClass;
   GdiplusStartupInput gdiplusStartupInput;
   ULONG_PTR           gdiplusToken;

   // Инициализация GDI+.
   GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
   wndClass.style          = CS_HREDRAW | CS_VREDRAW;
   wndClass.lpfnWndProc    = WndProc;
   wndClass.cbClsExtra     = 0;
   wndClass.cbWndExtra     = 0;
   wndClass.hInstance      = hInstance;
   wndClass.hIcon          = LoadIcon(NULL, IDI_APPLICATION);
   wndClass.hCursor        = LoadCursor(NULL, IDC_ARROW);
   wndClass.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
   wndClass.lpszMenuName   = NULL;
   wndClass.lpszClassName  = TEXT("MyApp");
   
   RegisterClass(&wndClass);

   // Преобразование командной строки в массив аргументов в формате Unicode
   LPWSTR* lpArgv = CommandLineToArgvW( GetCommandLineW(), &argc );		
   argv = (char**)malloc(argc*sizeof(char*));	// Выделение памяти для массива аргументов в формате ASCII	
   int size, i = 0;							// Инициализация счетчика итераций
   // Цикл для преобразования каждого аргумента из формата Unicode в формат ASCII
   for( ; i < argc; ++i )
   {
		size = wcslen(lpArgv[i]) + 1;			// Вычисление размера аргумента в символах, включая нулевой символ
		argv[i] = (char*)malloc(size);			// Выделение памяти для аргумента в формате ASCII
		wcstombs(argv[i], lpArgv[i], size);		// Преобразование аргумента из формата Unicode в формат ASCII
   }
   LocalFree( lpArgv );						// Освобождение памяти, выделенной для массива аргументов в формате Unicode

   //создание основного окна приложения
   hWnd = CreateWindow(
	  TEXT("MyApp"),					// название класса окна
	  TEXT("MyApp"),					// заголовок окна
	  WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,      // стиль окна
	  CW_USEDEFAULT,					// начальная позиция x
	  CW_USEDEFAULT,					// начальная позиция y
	  CW_USEDEFAULT,					// начальный размер x
	  CW_USEDEFAULT,					// начальный размер y
	  NULL,											// дескриптор родительского окна
	  NULL,											// дескриптор меню окна
	  hInstance,						// дескриптор экземпляра программы
	  NULL);						// параметры создания

   RECT rcParent;
   GetClientRect(hWnd, &rcParent);
   ShowWindow(hWnd, iCmdShow);
   UpdateWindow(hWnd);
   
   //цикл выборки и обработки сообщений
   while(GetMessage(&msg, NULL, 0, 0))				// Цикл выполняется, пока не получено сообщение WM_QUIT
   {
	  TranslateMessage(&msg);					// Преобразование виртуальных кодов клавиш в символьные коды для сообщений WM_KEYDOWN и WM_KEYUP
	  DispatchMessage(&msg);					// Передача сообщения в оконную процедуру соответствующего окна
   }
   
   GdiplusShutdown(gdiplusToken);
   return msg.wParam;
}  // WinMain

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, 
   WPARAM wParam, LPARAM lParam)
{
   // Обработчик сообщений окна
   switch(message)
   {
   // Обработка сообщения WM_CREATE
   case WM_CREATE:
	  //Если программе передано недостаточное число аргументов
	  if(argc<3)
	  {
		  // Вывод сообщения об ошибке и завершение работы приложения
		  MessageBox(hWnd,TEXT("Command line arguments are not set!"),TEXT("Error!"),MB_OK);
		  PostQuitMessage(0);
		  return 0;
	  }
	  // Загрузка изображения из файла
	  hBMP = (HBITMAP)LoadImageA( NULL, (LPCSTR)argv[1], IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	  // Проверка успешности загрузки изображения
	  if(hBMP == NULL)
	  {
		  // Вывод сообщения об ошибке и завершение работы приложения
		  MessageBox(hWnd,TEXT("Unable to open bitmap file!"),TEXT("Error!"),MB_OK);
		  PostQuitMessage(0);
	  }
	  // Получение информации об изображении
	  GetObject(hBMP, sizeof(bm), &bm);
	  // Создание горизонтального скроллбара
	  hwndScroll = CreateWindowEx(0, L"SCROLLBAR", NULL, WS_CHILD | WS_VISIBLE | SBS_HORZ, 0, bm.bmHeight-17, bm.bmWidth, 17, hWnd, NULL,(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
	  // Создание вертикального скроллбара
	  hwndScroll1 = CreateWindowEx(0, L"SCROLLBAR", NULL, WS_CHILD | WS_VISIBLE | SBS_VERT, bm.bmWidth-17, 0, 17, bm.bmHeight-17, hWnd, NULL,(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
	  // Чтение векторного файла
	  if(!readfile(argv[2],&head))
	  {
		  // Очистка списка и вывод сообщения об ошибке
		  clearlist(head);
		  MessageBox(hWnd,TEXT("Unable to open vector file!"),TEXT("Error!"),MB_OK);
		  PostQuitMessage(0);
	  }
	  return 0;
   // Обработка сообщения WM_LBUTTONDOWN
   case WM_LBUTTONDOWN:
	   char buff[300];	// Буфер для вывода информации о фигуре
	   datatype_t* ptrd;	// Указатель на данные фигуры

	   // Получение координат клика мыши
	   x=lParam & 0x0000FFFF;
	   y=(lParam & 0xFFFF0000)>>16;

	   // Преобразование координат в зависимости от масштаба
	   switch(state)
	   {
			case 1:
				x = x/2;
				y = y/2;
				break;
			case 2:
				x = x/3;
				y = y/3;
				break;
			case -1:
				x = x*2;
				y = y*2;
				break;
			case -2:
				x = x*3;
				y = y*3;
				break;
	   }

	   // Смещение координат на текущее положение изображения
	   x += x_;
	   y += y_;

	   // Проверка, находится ли клик внутри какой-либо фигуры
	if((ptrd=IsInside(rev_recount(x,scalex,basex),rev_recount(y,scaley,basey),head))!=NULL)
	   {

		   // Вывод информации о фигуре в буфер
		   switch(ptrd->type)
		   {
				case PTYPE_POINT:
					sprintf_s(buff, "POINT:\nCoords: %d %d\nAiming area: %d\nColour: %d %d %d",((point_*)ptrd->data)->x,((point_*)ptrd->data)->y,((point_*)ptrd->data)->aim_area,((point_*)ptrd->data)->red,((point_*)ptrd->data)->green,((point_*)ptrd->data)->blue);
				break;
				case PTYPE_LINE:
					sprintf_s(buff, "LINE:\nCoords of the beginning point: %d %d\nCoords of the ending point: %d %d\nThickness: %d\nAiming area: %d\nColour: %d %d %d",((line_*)ptrd->data)->x,((line_*)ptrd->data)->y,((line_*)ptrd->data)->x1,((line_*)ptrd->data)->y1,((line_*)ptrd->data)->thickness,((line_*)ptrd->data)->aim_area,((line_*)ptrd->data)->red,((line_*)ptrd->data)->green,((line_*)ptrd->data)->blue);
				break;
				case PTYPE_BROKENLINE:
					sprintf_s(buff, "BROKENLINE:\nThickness: %d\nColour: %d %d %d\nPoints:\n",((brokenline_*)ptrd->data)->thickness,((brokenline_*)ptrd->data)->red,((brokenline_*)ptrd->data)->green,((brokenline_*)ptrd->data)->blue);
					for(int i=0;i<((brokenline_*)ptrd->data)->count;i++)
						sprintf_s(buff+strlen(buff),sizeof(buff)-strlen(buff),"%d)%d %d\n",i+1,((brokenline_*)ptrd->data)->arr[i*2],((brokenline_*)ptrd->data)->arr[i*2+1]);
				break;
				case PTYPE_CIRCLE:
					sprintf_s(buff, "CIRCLE:\nCoords of the center: %d %d\nRadius: %d\nThickness: %d\nAiming area: %d\nColour: %d %d %d\n",((circle_*)ptrd->data)->xcenter,((circle_*)ptrd->data)->ycenter,((circle_*)ptrd->data)->radius,((circle_*)ptrd->data)->thickness,((circle_*)ptrd->data)->aim_area,((circle_*)ptrd->data)->red,((circle_*)ptrd->data)->green,((circle_*)ptrd->data)->blue);
				break;
				case PTYPE_POLYLINE:
					sprintf_s(buff, "POLYLINE:\nThickness: %d\nColour: %d %d %d\nPoints:\n",((polyline_*)ptrd->data)->thickness,((polyline_*)ptrd->data)->red,((polyline_*)ptrd->data)->green,((polyline_*)ptrd->data)->blue);
					for(int i=0;i<((brokenline_*)ptrd->data)->count;i++)
						sprintf_s(buff+strlen(buff),sizeof(buff)-strlen(buff),"%d)%d %d\n",i+1,((polyline_*)ptrd->data)->arr[i*2],((polyline_*)ptrd->data)->arr[i*2+1]);
				break;
		   }

		   // Вывод информации о фигуре в диалоговое окно
		   MessageBox(hWnd,CA2W(buff),TEXT("Aiming!"),MB_OK);
	   }
	   return 0;
   case WM_KEYDOWN:

	   //включение/выключение сглаживания нажатием клавиши А
	   if(wParam=='A')
	   {
		   AliasFlag = !AliasFlag;

		   // Вызов функций перерисовки окна
		   InvalidateRect(hWnd,NULL,TRUE);
		   UpdateWindow(hWnd);
	   }

	   //увеличение масштаба окна нажатием клавиши M
	   if(wParam=='M')
	   {
		   // Если текущий масштаб меньше 2
		   if(state<2)
		   {

				// Сброс смещения и положения скроллбаров
				x_ = 0;
				y_ = 0;
				SetScrollPos(hwndScroll,SB_CTL,0,true);
				SetScrollPos(hwndScroll1,SB_CTL,0,true);


				state++;	// Увеличение масштаба на 1

				// Вызов функций перерисовки окна
				InvalidateRect(hWnd,NULL,TRUE);
				UpdateWindow(hWnd);
		   }	   
	   }

	   //уменьшение масштаба окна нажатием клавиши M
	   if(wParam=='L')
	   {
		   // Если текущий масштаб больше -2
		   if(state>-2)
		   {
				// Сброс смещения и положения скроллбаров
				x_ = 0;
				y_ = 0;
				SetScrollPos(hwndScroll,SB_CTL,0,false);
				SetScrollPos(hwndScroll1,SB_CTL,0,false);

				state--;	// Уменьшение масштаба на 1

				// Вызов функций перерисовки окна
				InvalidateRect(hWnd,NULL,TRUE);
				UpdateWindow(hWnd);
		   }   
	   }
	   return 0;
   // Обработчик сообщения WM_VSCROLL
   case WM_VSCROLL:
	   // Если перемещен ползунок скроллбара
	   if (LOWORD(wParam) == SB_THUMBPOSITION)
	   {
		   int nPos = HIWORD(wParam);				// Получение новой позиции ползунка
		   SetScrollPos(hwndScroll1, SB_CTL, nPos, false);	// Установка новой позиции скроллбара
		   y_ = nPos;							// Установка нового значения смещения по оси Y

		   // Вызов функций перерисовки окна
		   InvalidateRect(hWnd,NULL,TRUE);
		   UpdateWindow(hWnd);
	   }

	   // Если нажата кнопка "Стрелка вверх"
	   if (LOWORD(wParam) == SB_LINEUP)
	   {
		   int nPos = GetScrollPos(hwndScroll1, SB_CTL);	// Получение текущей позиции скроллбара
		   nPos -= 10;						// Уменьшение позиции на 10
		   nPos = (nPos < 0) ? 0 : nPos;				// Если позиция меньше 0, установка в 0
		   SetScrollPos(hwndScroll1, SB_CTL, nPos, true);	// Установка новой позиции скроллбара
		   y_ = nPos;							// Установка нового значения смещения по оси Y

		   // Вызов функций перерисовки окна
		   InvalidateRect(hWnd,NULL,TRUE);
		   UpdateWindow(hWnd);
	   }

	   // Если нажата кнопка "Стрелка вниз"
	   if (LOWORD(wParam) == SB_LINEDOWN)
	   {
		   int nPos = GetScrollPos(hwndScroll1, SB_CTL);	// Получение текущей позиции скроллбара
		   int nMin,nMax;
		   GetScrollRange(hwndScroll1, SB_CTL, &nMin, &nMax);	// Получение минимальной и максимальной позиции скроллбара
		   nPos+=10;							// Увеличение позиции на 10
		   nPos = (nPos > nMax) ? nMax : nPos;			// Если позиция больше максимальной, то установка в максимальную
		   SetScrollPos(hwndScroll1, SB_CTL, nPos, true);	// Установка новой позиции скроллбара
		   y_ = nPos;							// Установка нового значения смещения по оси Y

		   // Вызов функций перерисовки окна
		   InvalidateRect(hWnd,NULL,TRUE);
		   UpdateWindow(hWnd);
	   }
	   return 0;

   // Обработчик сообщения WM_HSCROLL
   case WM_HSCROLL:
	   // Если перемещен ползунок скроллбара
	   if (LOWORD(wParam) == SB_THUMBPOSITION)
	   {
			int nPos = HIWORD(wParam);				// Получение новой позиции ползунка
			SetScrollPos(hwndScroll, SB_CTL, nPos, false);	// Установка новой позиции скроллбара
			x_ = nPos;										// Установка нового значения смещения по оси X

			//Вызов функций перерисовки окна
			InvalidateRect(hWnd,NULL,TRUE);
			UpdateWindow(hWnd);
	   }

	   // Если нажата кнопка "Стрелка влево"
	   if (LOWORD(wParam) == SB_LINEUP)
	   {
			int nPos = GetScrollPos(hwndScroll, SB_CTL);	// Получение текущей позиции скроллбара
			nPos -= 10;										// Уменьшение позиции на 10
			nPos = (nPos < 0) ? 0 : nPos;			// Если позиция меньше 0, то установка в 0
			SetScrollPos(hwndScroll, SB_CTL, nPos, false);	// Установка новой позиции скроллбара
			x_ = nPos;										// Установка нового значения смещения по оси X

			// Вызов функций перерисовки окна
			InvalidateRect(hWnd,NULL,TRUE);
			UpdateWindow(hWnd);
	   }

	   // Если нажата кнопка "Стрелка вправо"
	   if (LOWORD(wParam) == SB_LINEDOWN)
	   {
			int nPos = GetScrollPos(hwndScroll, SB_CTL);		// Получение текущей позиции скроллбара
			int nMin,nMax;
			GetScrollRange(hwndScroll, SB_CTL, &nMin, &nMax);	// Получение минимальной и максимальной позиции скроллбара
			nPos+=10;											// Увеличение позиции на 10
			nPos = (nPos > nMax) ? nMax : nPos;					// Если позиция больше максимальной, то установка в максимальную
			SetScrollPos(hwndScroll, SB_CTL, nPos, false);		// Установка новой позиции скролбара
			x_ = nPos;							// Установка нового значения смещения по оси X

			// Вызов функций перерисовки окна
			InvalidateRect(hWnd,NULL,TRUE);
			UpdateWindow(hWnd);
	   }
	   return 0;
   // Обработчик сообщения WM_PAINT
   case WM_PAINT:
	  //Если программе передано недостаточное число аргументов
	  if(argc<3)
		  return 0;
	  hdc = BeginPaint(hWnd, &ps);																// Получение контекста устройства для окна hWnd
	  memBit1 = CreateCompatibleDC(hdc);														// Создание совместимого контекста устройства
	  //Загрузка изображения из файла в виде битовой карты
	  hBMP = (HBITMAP)LoadImageA( NULL, (LPCSTR)argv[1], IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);	
	  // Выбор загруженной битовой карты в контексте устройства memBit1
	  hOldBMP = (HBITMAP)SelectObject(memBit1,hBMP);											
	  // Вызов функции vgraph для отрисовки графики на битовой карте
	  vgraph(head,memBit1,bm.bmWidth,bm.bmHeight,AliasFlag);
	  // Вызов функции PaintBitmapinHDC для отрисовки битовой карты в контексте устройства hdc
	  PaintBitmapinHDC(hWnd,hwndScroll,hwndScroll1,hdc,memBit1,state,x_,y_,bm.bmWidth,bm.bmHeight);
	  SelectObject(memBit1, hOldBMP);	// Возврат предыдущего объекта контекст устройства memBit1
	  DeleteObject(hBMP);		// Удаление объекта HBITMAP
	  DeleteDC(memBit1);			// Удаление контекста устройства memBit1

	  //Завершение рисования и освобождение контекста устройства hdc
	  EndPaint(hWnd, &ps);
	  ReleaseDC(hWnd,hdc);
	  return 0;
   case WM_DESTROY:
	  //очистка связанного списка
	  clearlist(head);
	  //очистка массива для хранения переданных программе аргументов командной строки
	  for(int i=0 ; i < argc; ++i )
		free( argv[i] );
	  free( argv );
	  //завершение работы программы
	  PostQuitMessage(0);
	  return 0;
   default:
	   //реакция на прочие сообщения окну - по умолчанию
	  return DefWindowProc(hWnd, message, wParam, lParam);
   }
} // WndProc
// Глобальные переменные:
HINSTANCE hInst;								// текущий экземпляр
TCHAR szTitle[MAX_LOADSTRING];					// Текст строки заголовка
TCHAR szWindowClass[MAX_LOADSTRING];			// имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
					 LPTSTR    lpCmdLine,
					 int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	// TODO: разместите код здесь.
	WinMain(hInstance,hPrevInstance,reinterpret_cast<PSTR>(lpCmdLine),nCmdShow);
	MSG msg;
	HACCEL hAccelTable;

	// Инициализация глобальных строк
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_EXAMPLE1, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Выполнить инициализацию приложения:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_EXAMPLE1));
	// Цикл основного сообщения:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  НАЗНАЧЕНИЕ: регистрирует класс окна.
//
//  КОММЕНТАРИИ:
//
//    Эта функция и ее использование необходимы только в случае, если нужно, чтобы данный код
//    был совместим с системами Win32, не имеющими функции RegisterClassEx'
//    которая была добавлена в Windows 95. Вызов этой функции важен для того,
//    чтобы приложение получило "качественные" мелкие значки и установило связь
//    с ними.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_EXAMPLE1));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_EXAMPLE1);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   НАЗНАЧЕНИЕ: сохраняет обработку экземпляра и создает главное окно.
//
//   КОММЕНТАРИИ:
//
//        В данной функции дескриптор экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится на экран главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Сохранить дескриптор экземпляра в глобальной переменной
   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	  CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
	  return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


