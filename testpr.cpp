//исходный код программы-редактора
//консольное приложение ОС Windows

//подключаем заголовочные файлы для разработки приложений в ОС Windows
#include <stdio.h>
#include "stdafx.h"
#include <stdlib.h>
#include <stdint.h>
#include <conio.h>
#include <malloc.h>
#include <windows.h>
#include "ndllproj.h"	//заголовочный файл DLL-библиотеки ndllproj


int main()
{
    char a,b;			//для хранения выбора пунктов меню
    void* v;			//указатель приведения
    char name[30];		//имя открываемого файла
    int arr[6]={sizeof(basisd_)+4,sizeof(point_)+4,sizeof(line_)+4,sizeof(brokenline_)+4,sizeof(circle_)+4,sizeof(polyline_)+4};		//размеры примитивов файла с учетом идентификатора
    
    //промежуточные данные обработки
    char str[256], res[256];
    int n,k,strnum;
    ptype_t t;			//хранит тип прочитанного через strread примитива
    uint32_t x,y;
    FILE* fp,*fp2;		//указатели потоков для работы с файлами
    bool flag = true;
    bool flag1;
    basisd_ bd;
    point_ p;
    line_ l;
    brokenline_ brl;
    circle_ cr;
    polyline_ pl;
    int si=0;
    char* nametmp;

    //включения консольного вывода на русском языке
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    while(flag)
    {

            flag1 = true;		//флаг завершения работы программы
            //вывод пунктов интерактивного меню
            printf("----------Выбор действия----------\n");
            printf("1 - создать новый файл\n");
            printf("2 - редактировать уже существующий файл\n");
            printf("3 - завершить работу програмы\n");
            printf("4 - удалить из существующего файла структуру(-ы)\n");

            printf("Выберите действие:\n");
            a = (char)getch();
            switch(a)
            {
                case '1':
                    printf("Введите название создаваемого файла:\n");
                    scanf("%s",name);
                    if ((fp = fopen(name, "wb")) == NULL) //создаем файл для записи данных
                        printf("Не удалось создать файл!\n");
                    else
                    {
                        while(flag1)
                        {

                            //интерактивное меню
                            printf("--------Добавление примитивов--------\n");
                            printf("Клавиши и их соответствие примитивам\n");
                            printf("A - установить параметры базиса\n");
                            printf("B - точка\n");
                            printf("C - линия\n");
                            printf("D - ломаная\n");
                            printf("E - окружность\n");
                            printf("F - многоугольник\n");
                            printf("G - в главное меню\n");


                            //Выбор пункта меню пользователем
                            printf("Выберите опцию:\n");
                            a = (char)getch();

                            //Инициализация структур и их добавление в файл
                            switch(toupper(a))  //выбор пунктов с помощью символов обеих регистров
                            {
                                case 'A':
                                    // Инициализация структуры basisd_ нулями
                                    memset(&bd,0,sizeof(basisd_));

                                    // Запрос у пользователя значений для заполнения структуры
                                    printf("Введите refwidth:");
                                    scanf("%d",&(bd.refwidth));
                                    printf("Введите refheight:");
                                    scanf("%d",&(bd.refheight));
                                    printf("Введите basex:");
                                    scanf("%d",&(bd.basex));
                                    printf("Введите basey:");
                                    scanf("%d",&(bd.basey));
                                    
                                    addbasisdescr(fp,&bd); // Добавление структуры в файл
                                    break;

                                case 'B':
                                    // Инициализация структуры point_ нулями
                                    memset(&p,0,sizeof(p));

                                    // Запрос у пользователя значений для заполнения структуры
                                    printf("Введите координату точки по горизонтали:");
                                    scanf("%d",&(p.x));
                                    printf("Введите координату точки по вертикали:");
                                    scanf("%d",&(p.y));
                                    printf("Введите размер области прицеливания:");
                                    scanf("%d",&si);
                                    p.aim_area = (char)(si & 0xff);
                                    printf("Введите интенсивность цветового канала R:");
                                    scanf("%d",&si);
                                    p.red = (char)(si & 0xff);
                                    printf("Введите интенсивность цветового канала G:");
                                    scanf("%d",&si);
                                    p.green = (char)(si & 0xff);
                                    printf("Введите интенсивность цветового канала B:");
                                    scanf("%d",&si);
                                    p.blue = (char)(si & 0xff);

                                    // Проверка входных значений и добавление структуры в файл
                                    if(p.aim_area==0)
                                    {
                                        printf("Неверный набор входных значений! Повторите ввод!\n");
                                        continue;
                                    }
                                    else
                                        addpoint(fp,&p);
                                    break;

                                case 'C':
                                    // Инициализация структуры line_ нулями
                                    memset(&l,0,sizeof(line_));

                                    // Запрос у пользователя значений для заполнения структуры
                                    printf("Введите координату начала линии по горизонтали:");
                                    scanf("%d",&(l.x));
                                    printf("Введите координату начала линии по вертикали:");
                                    scanf("%d",&(l.y));
                                    printf("Введите координату конца линии по горизонтали:");
                                    scanf("%d",&(l.x1));
                                    printf("Введите координату конца линии по вертикали:");
                                    scanf("%d",&(l.y1));
                                    printf("Введите толщину линии:");
                                    scanf("%d",&(l.thickness));
                                    printf("Введите размер области прицеливания:");
                                    scanf("%d",&si);
                                    l.aim_area = (char)(si & 0xff);
                                    printf("Введите интенсивность цветового канала R:");
                                    scanf("%d",&si);
                                    l.red = (char)(si & 0xff);
                                    printf("Введите интенсивность цветового канала G:");
                                    scanf("%d",&si);
                                    l.green = (char)(si & 0xff);
                                    printf("Введите интенсивность цветового канала B:");
                                    scanf("%d",&si);
                                    l.blue = (char)(si & 0xff);

                                    // Проверка входных значений и добавление структуры в файл
                                    if(l.aim_area==0 || l.thickness==0 || l.aim_area==0)
                                    {
                                        printf("Неверный набор входных значений! Повторите ввод!\n");
                                        continue;
                                    }
                                    else
                                        addline(fp,&l);
                                    break;


                                case 'D':
                                    // Инициализация структуры brl нулями
                                    memset(&brl,0,sizeof(brokenline_));

                                    // Запрос у пользователя значений для заполнения структуры
                                    printf("Введите толщину линии:");
                                    scanf("%d",&(brl.thickness));
                                    printf("Введите интенсивность цветового канала R:");
                                    scanf("%d",&si);
                                    brl.red = (char)(si & 0xff);
                                    printf("Введите интенсивность цветового канала G:");
                                    scanf("%d",&si);
                                    brl.green = (char)(si & 0xff);
                                    printf("Введите интенсивность цветового канала B:");
                                    scanf("%d",&si);
                                    brl.blue = (char)(si & 0xff);
                                    printf("Введите количество точек ломаной (до 10 штук):");
                                    scanf("%d",&n);
                                    brl.count = n;

                                    // Заполнение массива точек ломаной
                                    for(int i=0;i<n;i++)
                                    {
                                        printf("Введите координату точки по горизонтали:");
                                        scanf("%d",&x);
                                        printf("Введите координату точки по вертикали:");
                                        scanf("%d",&y);
                                        brl.arr[2*i]=x;
                                        brl.arr[2*i+1]=y;
                                    }

                                    // Проверка входных значений и добавление структуры в файл
                                    if(brl.count==0 || brl.thickness==0)
                                    {
                                        printf("Неверный набор входных значений! Повторите ввод!\n");
                                        continue;
                                    }
                                    else
                                        addbrokenline(fp,&brl);
                                    break;

                                case 'E':
                                    // Инициализация структуры cr нулями
                                    memset(&cr,0,sizeof(circle_));

                                    // Запрос у пользователя значений для заполнения структуры
                                    printf("Введите координату точки по горизонтали:");
                                    scanf("%d",&(cr.xcenter));
                                    printf("Введите координату точки по вертикали:");
                                    scanf("%d",&(cr.ycenter));
                                    printf("Введите радиус окружности:");
                                    scanf("%d",&(cr.radius));
                                    printf("Введите толщину линии окружности:");
                                    scanf("%d",&(cr.thickness));
                                    printf("Введите размер области прицеливания:");
                                    scanf("%d",&si);
                                    cr.aim_area = (char)(si & 0xff);
                                    printf("Введите интенсивность цветового канала R:");
                                    scanf("%d",&si);
                                    cr.red = (char)(si & 0xff);
                                    printf("Введите интенсивность цветового канала G:");
                                    scanf("%d",&si);
                                    cr.green = (char)(si & 0xff);
                                    printf("Введите интенсивность цветового канала B:");
                                    scanf("%d",&si);
                                    cr.blue = (char)(si & 0xff);

                                    // Проверка входных значений и добавление структуры в файл
                                    if(cr.radius==0 || cr.thickness==0 || cr.aim_area==0)
                                    {
                                        printf("Неверный набор входных значений! Повторите ввод!\n");
                                        continue;
                                    }
                                    else
                                        addcircle(fp,&cr);
                                    break;

                                case 'F':
                                    // Инициализация структуры pl нулями
                                    memset(&pl,0,sizeof(polyline_));

                                    // Запрос у пользователя значений для заполнения структуры
                                    printf("Введите толщину линии:");
                                    scanf("%d",&(pl.thickness));
                                    printf("Введите интенсивность цветового канала R:");
                                    scanf("%d",&si);
                                    pl.red = (char)(si & 0xff);
                                    printf("Введите интенсивность цветового канала G:");
                                    scanf("%d",&si);
                                    pl.green = (char)(si & 0xff);
                                    printf("Введите интенсивность цветового канала B:");
                                    scanf("%d",&si);
                                    pl.blue = (char)(si & 0xff);
                                    printf("Введите количество точек многоугольника (до 10 штук):");
                                    scanf("%d",&n);
                                    pl.count = n;

                                    // Заполнение массива точек  многоугольника
                                    for(int i=0;i<n;i++)
                                    {
                                        printf("Введите координату точки по горизонтали:");
                                        scanf("%d",&x);
                                        printf("Введите координату точки по вертикали:");
                                        scanf("%d",&y);
                                        pl.arr[2*i]=x;
                                        pl.arr[2*i+1]=y;
                                    }

                                    // Проверка входных значений и добавление структуры в файл
                                    if(pl.count==0 || pl.thickness==0)
                                    {
                                        printf("Неверный набор входных значений! Повторите ввод!\n");
                                        continue;
                                    }
                                    else
                                        addpolyline(fp,&pl);
                                    break;

                                //при завершении работы программы
                                case 'G':
                                    flag1 = false;
                                    break;

                                //при нажатии клавиши, не ассоциированной с пунктом меню
                                default:
                                    printf("В меню отсутствует данный пункт!\n");
                                    break;
                            }
                    }
                    fclose(fp);
                    }
                    break;
                case '2':
                    // Запрос у пользователя имени/адреса файла
                    printf("Введите имя/адрес файла:\n");
                    scanf("%s",name);

                    // Проверка наличия файла и открытие его для чтения и записи
                    if ((fp = fopen(name, "r+b")) == NULL) //создаем файл для записи данных
                        printf("Не удалось открыть файл!\n");
                    else
                    {
                        k = 0;		// Инициализация переменной k для подсчета структур в файле

                        // Чтение структур из файла и вывод их на экран
                        while((t=strread(fp,&v))!=PTYPE_NULL)
                        {
                            switch(t)
                            {
                                case PTYPE_BASE:
                                printf("%d)BASEDESCR{(%d,%d),(%d,%d)};\n",++k,((basisd_*)v)->refwidth,((basisd_*)v)->refheight,((basisd_*)v)->basex,((basisd_*)v)->basey);
                                break;
                                case PTYPE_POINT:
                                printf("%d)POINT(%d,%d);\n",++k,((point_*)v)->x,((point_*)v)->y);
                                break;
                                case PTYPE_LINE:
                                printf("%d)LINE{(%d,%d),(%d,%d)};\n",++k,((line_*)v)->x,((line_*)v)->y,((line_*)v)->x1,((line_*)v)->y1);
                                break;
                                case PTYPE_BROKENLINE:
                                printf("%d)BROKENLINE{(%d,%d),(%d,%d),(%d,%d),...};\n",++k,(((brokenline_*)v)->arr)[0],(((brokenline_*)v)->arr)[1],(((brokenline_*)v)->arr)[2],(((brokenline_*)v)->arr)[3],(((brokenline_*)v)->arr)[4],(((brokenline_*)v)->arr)[5]);
                                break;
                                case PTYPE_CIRCLE:
                                printf("%d)CIRCLE{(%d,%d),%d};\n",++k,((circle_*)v)->xcenter,((circle_*)v)->ycenter,((circle_*)v)->radius);
                                break;
                                case PTYPE_POLYLINE:
                                printf("%d)POLYLINE{(%d,%d),(%d,%d),(%d,%d),...};\n",++k,(((polyline_*)v)->arr)[0],(((polyline_*)v)->arr)[1],(((polyline_*)v)->arr)[2],(((polyline_*)v)->arr)[3],(((polyline_*)v)->arr)[4],(((polyline_*)v)->arr)[5]);
                                break;
                            }
                        }

                        // Проверка наличия структур в файле
                        if(k==0)
                            printf("Файл пуст!\n");
                        else 
                        {
                            // Запрос у пользователя номера редактируемой струкры
                            printf("Введите номер редактируемой структуры:");
                            scanf("%d",&strnum);

                            // Перемещение указателя на начало выбранной структуры
                            fseek(fp,0,SEEK_SET);
                            for(int i=0;i<strnum;i++)
                                t=strread(fp,&v);
                            fseek(fp,-arr[t-1],SEEK_CUR);

                            // Редактирование выбранной структуры в зависимости от ее типа
                            switch(t)
                            {
                                case PTYPE_BASE:
                                        // Запрос у пользователя значений для заполнения структуры
                                        printf("Введите refwidth:");
                                        scanf("%d",&(bd.refwidth));
                                        printf("Введите refheight:");
                                        scanf("%d",&(bd.refheight));
                                        printf("Введите basex:");
                                        scanf("%d",&(bd.basex));
                                        printf("Введите basey:");
                                        scanf("%d",&(bd.basey));

                                        addbasisdescr(fp,&bd);		// Добавление структуры в файл
                                break;
                                case PTYPE_POINT:
                                        // Запрос у пользователя значений для заполнения структуры
                                        printf("Введите координату точки по горизонтали:");
                                        scanf("%d",&(p.x));
                                        printf("Введите координату точки по вертикали:");
                                        scanf("%d",&(p.y));
                                        printf("Введите размер области прицеливания:");
                                        scanf("%d",&si);
                                        p.aim_area = (char)(si & 0xff);
                                        printf("Введите интенсивность цветового канала R:");
                                        scanf("%d",&si);
                                        p.red = (char)(si & 0xff);
                                        printf("Введите интенсивность цветового канала G:");
                                        scanf("%d",&si);
                                        p.green = (char)(si & 0xff);
                                        printf("Введите интенсивность цветового канала B:");
                                        scanf("%d",&si);
                                        p.blue = (char)(si & 0xff);

                                        // Проверка входных значений и добавление структуры в файл
                                        if(p.aim_area==0)
                                        {
                                            printf("Неверный набор входных значений! Повторите ввод!\n");
                                            continue;
                                        }
                                        else
                                            addpoint(fp,&p);
                                break;
                                case PTYPE_LINE:
                                        // Запрос у пользователя значений для заполнения структуры
                                        printf("Введите координату начала линии по горизонтали:");
                                        scanf("%d",&(l.x));
                                        printf("Введите координату начала линии по вертикали:");
                                        scanf("%d",&(l.y));
                                        printf("Введите координату конца линии по горизонтали:");
                                        scanf("%d",&(l.x1));
                                        printf("Введите координату конца линии по вертикали:");
                                        scanf("%d",&(l.y1));
                                        printf("Введите толщину линии:");
                                        scanf("%d",&(l.thickness));
                                        printf("Введите размер области прицеливания:");
                                        scanf("%d",&si);
                                        l.aim_area = (char)(si & 0xff);
                                        printf("Введите интенсивность цветового канала R:");
                                        scanf("%d",&si);
                                        l.red = (char)(si & 0xff);
                                        printf("Введите интенсивность цветового канала G:");
                                        scanf("%d",&si);
                                        l.green = (char)(si & 0xff);
                                        printf("Введите интенсивность цветового канала B:");
                                        scanf("%d",&si);
                                        l.blue = (char)(si & 0xff);

                                        // Проверка входных значений и добавление структуры в файл
                                        if(l.aim_area==0 || l.thickness==0 || l.aim_area==0)
                                        {
                                            printf("Неверный набор входных значений! Повторите ввод!\n");
                                            continue;
                                        }
                                        else
                                            addline(fp,&l);
                                break;
                                case PTYPE_BROKENLINE:
                                        // Запрос у пользователя значений для заполнения структуры
                                        printf("Введите толщину линии:");
                                        scanf("%d",&(brl.thickness));
                                        printf("Введите интенсивность цветового канала R:");
                                        scanf("%d",&si);
                                        brl.red = (char)(si & 0xff);
                                        printf("Введите интенсивность цветового канала G:");
                                        scanf("%d",&si);
                                        brl.green = (char)(si & 0xff);
                                        printf("Введите интенсивность цветового канала B:");
                                        scanf("%d",&si);
                                        brl.blue = (char)(si & 0xff);
                                        printf("Введите количество точек ломаной (до 10 штук):");
                                        scanf("%d",&n);
                                        brl.count = n;

                                        // Заполнение массива точек  ломаной
                                        for(int i=0;i<n;i++)
                                        {
                                            printf("Введите координату точки по горизонтали:");
                                            scanf("%d",&x);
                                            printf("Введите координату точки по вертикали:");
                                            scanf("%d",&y);
                                            brl.arr[2*i]=x;
                                            brl.arr[2*i+1]=y;
                                        }

                                        // Проверка входных значений и добавление структуры в файл
                                        if(brl.count==0 || brl.thickness==0)
                                        {
                                            printf("Неверный набор входных значений! Повторите ввод!\n");
                                            continue;
                                        }
                                        else
                                            addbrokenline(fp,&brl);
                                break;
                                case PTYPE_CIRCLE:
                                        // Запрос у пользователя значений для заполнения структуры
                                        printf("Введите координату точки по горизонтали:");
                                        scanf("%d",&(cr.xcenter));
                                        printf("Введите координату точки по вертикали:");
                                        scanf("%d",&(cr.ycenter));
                                        printf("Введите радиус окружности:");
                                        scanf("%d",&(cr.radius));
                                        printf("Введите толщину линии окружности:");
                                        scanf("%d",&(cr.thickness));
                                        printf("Введите размер области прицеливания:");
                                        scanf("%d",&si);
                                        cr.aim_area = (char)(si & 0xff);
                                        printf("Введите интенсивность цветового канала R:");
                                        scanf("%d",&si);
                                        cr.red = (char)(si & 0xff);
                                        printf("Введите интенсивность цветового канала G:");
                                        scanf("%d",&si);
                                        cr.green = (char)(si & 0xff);
                                        printf("Введите интенсивность цветового канала B:");
                                        scanf("%d",&si);
                                        cr.blue = (char)(si & 0xff);

                                        // Проверка входных значений и добавление структуры в файл
                                        if(cr.radius==0 || cr.thickness==0 || cr.aim_area==0)
                                        {
                                            printf("Неверный набор входных значений! Повторите ввод!\n");
                                            continue;
                                        }
                                        else
                                            addcircle(fp,&cr);
                                break;
                                case PTYPE_POLYLINE:
                                        // Запрос у пользователя значений для заполнения структуры
                                        printf("Введите толщину линии:");
                                        scanf("%d",&(pl.thickness));
                                        printf("Введите интенсивность цветового канала R:");
                                        scanf("%d",&si);
                                        pl.red = (char)(si & 0xff);
                                        printf("Введите интенсивность цветового канала G:");
                                        scanf("%d",&si);
                                        pl.green = (char)(si & 0xff);
                                        printf("Введите интенсивность цветового канала B:");
                                        scanf("%d",&si);
                                        pl.blue = (char)(si & 0xff);
                                        printf("Введите количество точек многоугольника (до 10 штук):");
                                        scanf("%d",&n);
                                        pl.count = n;

                                        // Заполнение массива точек многоугольника
                                        for(int i=0;i<n;i++)
                                        {
                                            printf("Введите координату точки по горизонтали:");
                                            scanf("%d",&x);
                                            printf("Введите координату точки по вертикали:");
                                            scanf("%d",&y);
                                            pl.arr[2*i]=x;
                                            pl.arr[2*i+1]=y;
                                        }

                                        // Проверка входных значений и добавление структуры в файл
                                        if(pl.count==0 || pl.thickness==0)
                                        {
                                            printf("Неверный набор входных значений! Повторите ввод!\n");
                                            continue;
                                        }
                                        else
                                            addpolyline(fp,&pl);
                                break;

                                //вывод сообщения об ошибке, если нажатая клавиша не принадлежит ни одному из пунктов меню
                                default:
                                    printf("Неверный номер редактируемого примитива!\n");
                                    break;
                            }
                        }
                        fclose(fp);		//закрытие файла
                    }
                break;

                //для завершения работы программы сбрасываем флаг работы
                case '3':
                    flag = false;
                    break;
                case '4':
                    // Запрос у пользователя имени/адреса файла
                    printf("Введите имя/адрес файла:\n");
                    scanf("%s",name);	// Считывание имени файла

                    nametmp = (char*)calloc(strlen(name)+5,1);	// Выделение памяти для временного имени файла
                    sprintf(nametmp,"%s.tmp",name);				// Формирование имени временного файла
                    if ((fp = fopen(name, "r+b")) == NULL || (fp2 = fopen(nametmp, "w+b")) == NULL) //создаем файл для временной записи данных
                        printf("Не удалось открыть файл!\n");
                    else
                    {
                        k = 0;
                        //Чтение структур из файла и вывод информации о них
                        for(int i=0;(t = strread(fp,&v))!=PTYPE_NULL;i++)
                        {
                            if(t==PTYPE_BASE)
                                printf("%d)BASEDESCR{(%d,%d),(%d,%d)};\n",++k,((basisd_*)v)->refwidth,((basisd_*)v)->refheight,((basisd_*)v)->basex,((basisd_*)v)->basey);
                            if(t==PTYPE_POINT)
                                printf("%d)POINT(%d,%d);\n",++k,((point_*)v)->x,((point_*)v)->y);
                            if(t==PTYPE_LINE)
                                printf("%d)LINE{(%d,%d),(%d,%d)};\n",++k,((line_*)v)->x,((line_*)v)->y,((line_*)v)->x1,((line_*)v)->y1);
                            if(t==PTYPE_BROKENLINE)
                                printf("%d)BROKENLINE{(%d,%d),(%d,%d),(%d,%d),...};\n",++k,(((brokenline_*)v)->arr)[0],(((brokenline_*)v)->arr)[1],(((brokenline_*)v)->arr)[2],(((brokenline_*)v)->arr)[3],(((brokenline_*)v)->arr)[4],(((brokenline_*)v)->arr)[5]);
                            if(t==PTYPE_CIRCLE)
                                printf("%d)CIRCLE{(%d,%d),%d};\n",++k,((circle_*)v)->xcenter,((circle_*)v)->ycenter,((circle_*)v)->radius);
                            if(t==PTYPE_POLYLINE)
                                printf("%d)POLYLINE{(%d,%d),(%d,%d),(%d,%d),...};\n",++k,(((polyline_*)v)->arr)[0],(((polyline_*)v)->arr)[1],(((polyline_*)v)->arr)[2],(((polyline_*)v)->arr)[3],(((polyline_*)v)->arr)[4],(((polyline_*)v)->arr)[5]);
                           
                            // Запрос пользователя на удаление структуры
                            printf("Удалить структуру?Y/N\n");
                            b = (char)getch();

                             // Если пользователь согласен удалить структуру, то переходим к следующей итерации цикла
                            if(b=='Y' || b=='y')
                                continue;
                            else
                            {
                                // В зависимости от типа структуры записываем ее во временный файл
                                switch(t)
                                {
                                    case PTYPE_BASE:
                                        bd = *((basisd_*)v);
                                        addbasisdescr(fp2,&bd);
                                    break;
                                    case PTYPE_POINT:
                                        p = *((point_*)v);
                                        addpoint(fp2,&p);
                                    break;
                                    case PTYPE_LINE:
                                        l = *((line_*)v);
                                        addline(fp2,&l);
                                    break;
                                    case PTYPE_BROKENLINE:
                                        brl = *((brokenline_*)v);
                                        addbrokenline(fp2,&brl);
                                    break;
                                    case PTYPE_CIRCLE:
                                        cr = *((circle_*)v);
                                        addcircle(fp2,&cr);
                                    break;
                                    case PTYPE_POLYLINE:
                                        pl = *((polyline_*)v);
                                        addpolyline(fp2,&pl);
                                    break;
                                }
                                if(b!='N' && b!='n')
                                    printf("Неверный ввод! Файл не изменен!\n");
                            }
                        }
                        //проверка наличия структур в файле и закрытие потоков
                        if(k==0)
                            printf("Файл пуст!\n");
                        fclose(fp);
                        fclose(fp2);

                        //переименование временного файла в редактируемый и удаление старой версии редактируемого файла
                        remove(name);
                        rename(nametmp,name);
                        free(nametmp);
                    }
                break;
            //при выборе пункта, который отсутствует в меню первого уровня
            default:
                printf("В меню отсутствует пункт с таким номером!\n");
                break;
            }
    }
}

