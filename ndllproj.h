// Приведенный ниже блок ifdef - это стандартный метод создания макросов, упрощающий процедуру 
// экспорта из библиотек DLL. Все файлы данной DLL скомпилированы с использованием символа NDLLPROJ_EXPORTS,
// в командной строке. Этот символ не должен быть определен в каком-либо проекте
// использующем данную DLL. Благодаря этому любой другой проект, чьи исходные файлы включают данный файл, видит 
// функции NDLLPROJ_API как импортированные из DLL, тогда как данная DLL видит символы,
// определяемые данным макросом, как экспортированные.
#ifdef NDLLPROJ_EXPORTS
#define NDLLPROJ_API __declspec(dllexport)
#else
#define NDLLPROJ_API __declspec(dllimport)
#endif


using namespace ATL; 
using namespace std;


//подключаем заголовочные файлы для разработки приложений в ОС Windows
#include <stdint.h>
#include <stdio.h>
#include <windows.h>
#include <algorithm>
#include <atlbase.h>
#include <atlconv.h>
#include <atlstr.h>
#include <objidl.h>


//подключаем к приложению GDI+
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

//Перестановка байтов в 32-битном беззнаковом целом числе
#define SWAP_UINT32(x) (((x) >> 24) | (((x) & 0x00FF0000) >> 8) | (((x) & 0x0000FF00) << 8) | ((x) << 24))

//Объявление структур примитивов
#pragma pack(push, 1)
typedef struct basisdescr
{
    uint32_t refwidth;
    uint32_t refheight;
    int32_t basex;
    int32_t basey;
} basisd_;
typedef struct point
{
    uint32_t x;
    uint32_t y;
	unsigned char aim_area;
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} point_;
typedef struct line
{
    uint32_t x;
    uint32_t y;
    uint32_t x1;
    uint32_t y1;
    unsigned char thickness;
	unsigned char aim_area;
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} line_;
typedef struct brokenline
{
    unsigned char thickness;
    unsigned char red;
    unsigned char green;
    unsigned char blue;
	unsigned char count;
    uint32_t arr[20];
} brokenline_;
typedef struct circle
{
    uint32_t xcenter;
    uint32_t ycenter;
    uint32_t radius;
    unsigned char thickness;
	unsigned char aim_area;
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} circle_;
typedef struct polyline
{
    unsigned char thickness;
    unsigned char red;
    unsigned char green;
    unsigned char blue;
	unsigned char count;
    uint32_t arr[20];
} polyline_;
#pragma pack(pop)
typedef enum {
	PTYPE_NULL=0,
	PTYPE_BASE,
	PTYPE_POINT,
	PTYPE_LINE,
	PTYPE_BROKENLINE,
	PTYPE_CIRCLE,
	PTYPE_POLYLINE,
} ptype_t;
//элемент направленного списка
typedef struct datatype {
	void* data;			//данные примитива
	ptype_t type;			//тип примитива
	Gdiplus::Region* r;		//регион прицеливания
	//указатели на следующий/предыдущий элемент направленного списка
	struct datatype* next;
	struct datatype* prev;
} datatype_t;
//экспортируемые из DLL-функции
extern "C" NDLLPROJ_API void addbasisdescr(FILE* fp,basisd_* basd);
extern "C" NDLLPROJ_API void addpoint(FILE* fp,point_* p);
extern "C" NDLLPROJ_API void addline(FILE* fp,line_* l);
extern "C" NDLLPROJ_API void addbrokenline(FILE* fp,brokenline_* brl);
extern "C" NDLLPROJ_API void addcircle(FILE* fp,circle_* cr);
extern "C" NDLLPROJ_API void addpolyline(FILE* fp,polyline_* pl);
extern "C" NDLLPROJ_API ptype_t strread(FILE* fp,void** data);
extern "C" NDLLPROJ_API datatype_t* IsInside(uint32_t x,uint32_t y,datatype_t* head);
extern "C" NDLLPROJ_API void ppoint(Graphics* graphics,uint32_t x,uint32_t y,char red, char green, char blue);
extern "C" NDLLPROJ_API void pline(Graphics* graphics,uint32_t x,uint32_t y,uint32_t x1,uint32_t y1,char thickness,char red,char green,char blue);
extern "C" NDLLPROJ_API void pbrokenline(Graphics* graphics,uint32_t* arrx,uint32_t* arry,int count,char thickness,char red,char green,char blue);
extern "C" NDLLPROJ_API void pcircle(Graphics* graphics,uint32_t xcenter,uint32_t ycenter,uint32_t radius,char thickness,char red,char green,char blue);
extern "C" NDLLPROJ_API void ppolyline(Graphics* graphics,uint32_t* arrx,uint32_t* arry,int count,char thickness,char red,char green,char blue);
extern "C" NDLLPROJ_API void PaintBitmapinHDC(HWND hWnd,HWND hWndScroll,HWND hWndScroll1,HDC hDC,HDC memBit,int state,int x_,int y_,int width,int height);
