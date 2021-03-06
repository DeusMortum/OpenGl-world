// OpenDeus.cpp: Определяет точку входа для приложения.
//

#include "stdafx.h"
#include "OpenDeus.h"


#define MAX_LOADSTRING 100
const double PI = 3.141592653589f;

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна



int drawSelection = 0;							// Выбор рисования
double xrot = 0, yrot = 0 ,yrt = 0, xrt = 0;
double xr = 1.0f;
GLuint textures[1];
bool lineMode = false;
GLuint  TextureArray[1];

struct PlayerPos {
public:
	GLdouble x;
	GLdouble y;
	GLdouble z = 2.0f;

	GLdouble Xrot;
	GLdouble Yrot;

	POINT mousePos;

	float sense = 0.23333f;			// Чувствительность камеры
	bool Player_HALT = true;	
	float moveSpeed = 0.0f;
	float FOV = 45.0f;
} Player;

struct LightbulbPos {
	GLdouble x = 1.2f;
	GLdouble y = 1.2f;
	GLdouble z = 1.2f;

	bool LIGHTBULB_HALT = false;
	float moveSpeed = 0.0025f;
} LIGHTBULB;

BOOL SMTH_P = FALSE, SMTH_L = FALSE;


/// КОНТЕКСТЫ ДЛЯ РИСОВАНИЯ

HDC hDC;
HGLRC hGLRC;

PIXELFORMATDESCRIPTOR pfd;

/// -----







// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_OPENDEUS, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OPENDEUS));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
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
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OPENDEUS));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_OPENDEUS);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
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
   hInst = hInstance; // Сохранить дескриптор экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }
   memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
   pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
   pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
   pfd.iPixelType = PFD_TYPE_RGBA;
   pfd.cColorBits = 24;
   pfd.cDepthBits = 16;
   pfd.iLayerType = PFD_MAIN_PLANE;

   hDC = GetDC(hWnd);

   int pxFormat = ChoosePixelFormat(hDC, &pfd);
   SetPixelFormat(hDC, pxFormat, &pfd);

   hGLRC = wglCreateContext(hDC);
   wglMakeCurrent(hDC, hGLRC);


   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}





//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  НАЗНАЧЕНИЕ:  обрабатывает сообщения в главном окне.
//
//  WM_COMMAND — обработать меню приложения
//  WM_PAINT — отрисовать главное окно
//  WM_DESTROY — отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HMENU hMenu = GetMenu(hWnd);

	int width = 0, height = 1;
	RECT rect;
	if (GetClientRect(hWnd, &rect))
	{
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;

		height < 1 ? height = 1 : NULL;
	}
    switch (message)
    {
	case WM_KEYDOWN:
		switch (wParam) {

		case VK_ESCAPE:
			Player.Player_HALT ^= 1;
			if (!Player.Player_HALT) {
				SetCursorPos(width / 2, height / 2);
				ShowCursor(false);
			}
			else ShowCursor(true);
			break;
		case VK_ADD:
			Player.FOV < 75.0f ? Player.FOV += 1.0f : Player.FOV = 75.0f;
			break;
		case VK_SUBTRACT:
			Player.FOV > 30.0f ? Player.FOV -= 1.0f : Player.FOV = 30.0f;
			break;

		default: break;
		}

		InvalidateRect(hWnd, &rect, FALSE);

		break;


    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
			InvalidateRect(hWnd, NULL, TRUE);
            // Разобрать выбор в меню:
			if ((wmId == Fract2d))
			{
				drawSelection = wmId;
			}

			if ((wmId == World3d) ||(wmId == ID_Tor))
			{
				xrot = 0.0f;
				yrot = 0.0f;

				drawSelection = wmId;
			}
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
							
								case ID_Line:
				if (SMTH_L) CheckMenuItem(hMenu, ID_Line, MF_UNCHECKED);
				else CheckMenuItem(hMenu, ID_Line, MF_CHECKED);

				SMTH_L ^= TRUE;
				break;
								case ID_Pol:
				if (SMTH_P) CheckMenuItem(hMenu, ID_Pol, MF_UNCHECKED);
				else CheckMenuItem(hMenu, ID_Pol, MF_CHECKED);

				SMTH_P ^= TRUE;
				break;
								case ID_Zaliv:
				if (!lineMode) CheckMenuItem(hMenu, ID_Zaliv, MF_UNCHECKED);
				else CheckMenuItem(hMenu, ID_Zaliv, MF_CHECKED);

				lineMode ^= TRUE;
				break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
			// Инициализация

			glClearColor(1.f, 1.f, 1.f, 0.f);
			glClearDepth(1.0f);

			glEnable(GL_DEPTH_TEST);

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_BLEND);
			glShadeModel(GL_SMOOTH);

			if (SMTH_L) {
				glEnable(GL_LINE_SMOOTH);
				glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
			}

			if (SMTH_P) {
				glEnable(GL_POLYGON_SMOOTH);
				glHint(GL_POLYGON_SMOOTH_HINT, GL_DONT_CARE);
			}

			glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
			glEnable(GL_NORMALIZE);

		
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Управление 

			if (!Player.Player_HALT) {

				//Мышь
				GetCursorPos(&Player.mousePos);
				Player.Xrot += ((width / 2) - Player.mousePos.x) * Player.sense;
				Player.Yrot += ((height / 2) - Player.mousePos.y) * Player.sense;
								SetCursorPos(width / 2, height / 2);

				

				
				//Ускорение
				if ((GetKeyState(0x41) <= -127) ||
					(GetKeyState(0x44) <= -127) ||
					(GetKeyState(0x53) <= -127) ||
					(GetKeyState(0x57) <= -127)) {
					GetKeyState(VK_SHIFT) <= -127 ? Player.moveSpeed = 0.075f : Player.moveSpeed = 0.005f;
				}
			


				//WASD
				if (GetKeyState(0x57) <= -127) {
					Player.x -= (float)sin(Player.Xrot / 180 * PI) * Player.moveSpeed;
					Player.z -= (float)cos(Player.Xrot / 180 * PI) * Player.moveSpeed;
					xr += 0.01f;
				}
				if (GetKeyState(0x53) <= -127) { 
					Player.x += (float)sin(Player.Xrot / 180 * PI) * Player.moveSpeed;
					Player.z += (float)cos(Player.Xrot / 180 * PI) * Player.moveSpeed;
				}
				if (GetKeyState(0x41) <= -127) {
					Player.x += (float)sin((Player.Xrot - 90) / 180 * PI) * Player.moveSpeed;
					Player.z += (float)cos((Player.Xrot - 90) / 180 * PI) * Player.moveSpeed;
				}
				if (GetKeyState(0x44) <= -127) { 
					Player.x += (float)sin((Player.Xrot + 90) / 180 * PI) * Player.moveSpeed;
					Player.z += (float)cos((Player.Xrot + 90) / 180 * PI)  *Player.moveSpeed;
				}





				//Приседание
				GetKeyState(VK_CONTROL) <= -127 ? Player.y = -0.5f : Player.y = 0.0f;

				Player.x >  50.f ? Player.x = 50.f : NULL;
				Player.x < -50.f ? Player.x = -50.f : NULL;
				Player.z >  50.f ? Player.z = 50.f : NULL;
				Player.z < -50.f ? Player.z = -50.f : NULL;
			}


			// Управление Светом

			if (!LIGHTBULB.LIGHTBULB_HALT) {
				// Управление с клавиатуры:

				if (GetKeyState(VK_NUMPAD9) <= -127) LIGHTBULB.z += LIGHTBULB.moveSpeed;
				

				if (GetKeyState(VK_NUMPAD3) <= -127) LIGHTBULB.z -= LIGHTBULB.moveSpeed;

				if (GetKeyState(VK_NUMPAD4) <= -127) LIGHTBULB.x -= LIGHTBULB.moveSpeed;
				

				if (GetKeyState(VK_NUMPAD6) <= -127) LIGHTBULB.x += LIGHTBULB.moveSpeed;
				

				if (GetKeyState(VK_NUMPAD8) <= -127) LIGHTBULB.y += LIGHTBULB.moveSpeed;
			

				if (GetKeyState(VK_NUMPAD2) <= -127) LIGHTBULB.y -= LIGHTBULB.moveSpeed;

							}


			glViewport(0, 0, width, height);

			GLdouble fAspect = ((GLdouble)(rect.right - rect.left)) / ((GLdouble)(rect.bottom - rect.top));

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective(Player.FOV, fAspect, 0.0065f, 100.0f);
			gluLookAt(
				Player.x,
				Player.y + 0.5f,
				Player.z,
				Player.x - sin(Player.Xrot / 180 * PI),
				Player.y + 0.5f + (tan(Player.Yrot / 180 * PI)),
				Player.z - cos(Player.Xrot / 180 * PI),
				0, 1, 0);


	

			glPointSize(10.0f);
			glLineWidth(2.0f);
			switch (drawSelection) {
			
			case Fract2d:
			{
									
				GLfloat x, y, z, angle; 

				glPushMatrix();
				glBegin(GL_LINES);
				z = -100.0f;
				for (angle = 0.0f; angle <= (300.0f*PI)*3.0f; angle += 0.1f) {
					x = 40.0f*sin(angle);
					y = 40.0f*cos(angle);
					
					// Задаем точку и немного смещаем значение z 
					 glColor4ub(angle, 0, angle, 255);
					glVertex3f(x, y, z);
					z += 0.01f;
					
				}
				glPopMatrix();
									
				glEnd();
				// Восстанавливаем преобразования 

			}
			break;

			case World3d:
			{
				glClear(GL_COLOR_BUFFER_BIT);
				glEnable(GL_STENCIL_TEST);
		
				
				glPushMatrix();

				glTranslated(LIGHTBULB.x, LIGHTBULB.y, LIGHTBULB.z);

				lineMode ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			
				gluSphere(gluNewQuadric(), 0.05f, 10, 10);
			
				glPopMatrix();

				// Свойства света

				GLfloat light_ambient[] = { 0.1, 0.15, 0.2 };
				GLfloat light_diffuse[] = { 0.7, 0.65, 0.6 };
				GLfloat light_specular[] = { 1.0, 1.0, 1.0 };
				GLfloat light_position[] = { LIGHTBULB.x, LIGHTBULB.y, LIGHTBULB.z, 0.0 };

				glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
				glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
				glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
				glLightfv(GL_LIGHT0, GL_POSITION, light_position);

				// Свойств

				GLfloat diffuse_material[] = { 0.0, 0.0, 0.3, 1.0 };
				GLfloat specular_material[] = { 0.7, 0.7, 1.0, 1.0 };
				GLfloat shininess_material[] = { 50.0 };

				glEnable(GL_LIGHTING);
				glEnable(GL_LIGHT0);

				lineMode ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

				glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_material);
				glMaterialfv(GL_FRONT, GL_SPECULAR, specular_material);
				glMaterialfv(GL_FRONT, GL_SHININESS, shininess_material);

				glStencilFunc(GL_ALWAYS, 1, 0);
				glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
				glutSolidCube(2.5);

				
				glStencilFunc(GL_ALWAYS, 2, 0);
				glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
				glutSolidSphere(1.5,10,10);
				
				// очищаем буфферы цвета и глубины
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glDisable(GL_STENCIL_TEST);

				gluSphere(gluNewQuadric(), 0.3f, 50, 50);

				glEnable(GL_STENCIL_TEST);
				glStencilFunc(GL_EQUAL, 1, 255);
				glutSolidCube(2.5);

				glDisable(GL_LIGHT0);
				glDisable(GL_LIGHTING);

				glPopMatrix();
				glEnd();
			}
			break;
			case ID_Tor:
			{
				int i, j, k;
				int sshena, ring;
				double s, t, x, y, z;
				float r1, r2;
				glPushMatrix();

				glRotated(yrot, 1.0f, 0.0f, 0.0f);
				glRotated(xrot, 0.0f, 1.0f, 0.0f);

				
				lineMode ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				//glTranslated(1, 1, 1);

				glColor4ub(0, 0, 0, 255);
				
				glutSolidSphere(10, 100, 100);
				glPopMatrix();
				glEnd();
				
				glPushMatrix();
				r1 = 30.00f; // Радиус центра кольца
				r2 = 10.00f; // Радиус сегмента
				sshena = 160; // Точность сегмента
				ring = 64; // Кол-во сегментов

				for (i = 0; i < sshena; i++) {
					glBegin(GL_QUAD_STRIP);
					for (j = 0; j <= ring; j++) {
						for (k = 1; k >= 0; k--) {
							s = (i + k) % sshena + 0.5;
							t = j % ring;

							x = (r1 + r2 * cos(s * (PI * 2) / sshena)) * cos(t * (PI * 2) / ring);
							y = (r1 + r2 * cos(s * (PI * 2) / sshena)) * sin(t * (PI * 2) / ring);
							z = r2 * sin(s * (PI * 2) / sshena);

							glColor3f(x, y, z);
							glVertex3f(x, y, z);
						}
					}
				}
				glEnd();
				glPopMatrix();

				glColor3d(0, 1, 0);
				glPushMatrix();
				glRotated(90, 0, 1, 0);
				glutSolidTorus(11, 30,160,64);
				glPopMatrix();
				glEnd();
				

			}
			break;

			default:
			{
				
			}
			break;
			}
					//Поле
	
			for (float i = -50.f; i <= 50.f; i += 1.f) {
				glColor4ub(0, 0, 255, 255);
				glBegin(GL_LINES);
				glVertex3f(-50.f, -1.f, i);
				glVertex3f(50.f, -1.f, i);
				glEnd();
				glColor4ub(0, 0, 255, 255);
				glBegin(GL_LINES);
				glVertex3f(i, -1.f, -50.f);
				glVertex3f(i, -1.f, 50.f);
				glEnd();
			}

			glDisable(GL_NORMALIZE);
			if (SMTH_L) glDisable(GL_LINE_SMOOTH);
			if (SMTH_P) glDisable(GL_POLYGON_SMOOTH);
			glDisable(GL_BLEND);
			glDisable(GL_DEPTH_TEST);

			SwapBuffers(hDC);
			EndPaint(hWnd, &ps);
			InvalidateRect(hWnd, &rect, false);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
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
