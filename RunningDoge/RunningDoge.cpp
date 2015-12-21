#include "RunningDoge.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wcex;
	HWND hWnd;
	MSG msg;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("Running Doge"),
			MB_OK);

		return 1;
	}

	// The parameters to CreateWindow explained:
	// szWindowClass: the name of the application
	// szTitle: the text that appears in the title bar
	// WS_OVERLAPPEDWINDOW: the type of window to create,~WS_THICKFRAME  fixed window size
	// CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)
	// WNDWIDTH, WNDHEIGHT: initial size (width, length)
	// NULL: the parent of this window
	// NULL: this application does not have a menu bar
	// hInstance: the first parameter from WinMain
	// NULL: not used in this application
	hWnd = CreateWindow(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, //���ô�����ʽ�����ɸı��С���������
		CW_USEDEFAULT, CW_USEDEFAULT,
		WNDWIDTH, WNDHEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL
		);

	if (!hWnd)
	{
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			_T("Running Doge"),
			MB_OK);

		return 1;
	}

	// The parameters to ShowWindow explained:
	// hWnd: the value returned from CreateWindow
	// nCmdShow: the fourth parameter from WinMain
	ShowWindow(hWnd,
		nCmdShow);
	UpdateWindow(hWnd);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		//��ʼ��
		Init(hWnd, wParam, lParam);
		break;
	case WM_PAINT:
		//����
		Render(hWnd);
		break;
	case WM_KEYDOWN:
		//���̰����¼�
		KeyDown(hWnd, wParam, lParam);
		break;
	case WM_LBUTTONDOWN:
		//������¼�
		LButtonDown(hWnd, wParam, lParam);
		break;
	case WM_TIMER:
		//��ʱ���¼�
		switch (wParam)
		{
		case TIMER_ID:
			//������Ϸ״̬��ʱ��
			if (m_gameStatus.totalDist > SCALE3 && m_gameStatus.situation == 2)
				BossStart(hWnd);
			if (m_gameStatus.totalDist > SCALE4 && m_gameStatus.situation == 9)
				BossStart(hWnd);


			TimerUpdate(hWnd, wParam, lParam);
			break;
		case JUMP_TIMER:
			//��Ծ��ʱ��
			if (speed_jump >= 1)
				m_hero.pos.y -= speed_jump;
			else if (speed_jump == 0)
				m_hero.curFrameIndex = 5;
			else if (speed_jump <= -1)
				m_hero.pos.y -= speed_jump;
			speed_jump--;
			if (BeBorn() >= 0)
			{
				m_hero.pos.y = m_terrian[BeBorn()].pos.y - m_hero.size.cy;
				jump_status = 0;
				KillTimer(hWnd, JUMP_TIMER);
			}
			break;
		case DOWN_TIMER:
			//�����ʱ��
			if (jump_status!=0)
			{
				KillTimer(hWnd, DOWN_TIMER);
				down_status = 0;
				break;
			}
			m_hero.pos.y -= speed_jump;
			speed_jump--;
			if (BeBorn() >= 0)
			{
				m_hero.pos.y = m_terrian[BeBorn()].pos.y - m_hero.size.cy;
				KillTimer(hWnd, DOWN_TIMER);
				down_status = 0;
			}
			break;
		case PICTURE_TIMER:
			//����ͼƬ��ʱ������ʾ1��
			switch (m_gameStatus.situation)
			{
			case 15:
				m_gameStatus.situation = 2;
				PlaySound((LPCWSTR)IDR_THEME, NULL, SND_RESOURCE | SND_ASYNC | SND_LOOP);
				KillTimer(hWnd, PICTURE_TIMER);
				SetTimer(hWnd, TIMER_ID, TIMER_ELAPSE, NULL);
				InvalidateRect(hWnd, NULL, FALSE);
				break;
			case 16:
			case 17:
				m_hero = CreateHero(-52, 170, HERO_SIZE_X, HERO_SIZE_Y, m_hHeroBmp, 0, HERO_MAX_FRAME_NUM);
				//��������
				int k;
				for (k = 0; k < MAX_TERRIAN_NUM; ++k)
				{
					m_terrian[k] = CreateTerrian(k * 64, 220, STEP_SIZE_X, STEP_SIZE_Y, m_hStepBmp, STEP_SIZE_Y);
					m_terrian[k].items = 0;
				}
				//����״̬
				jump_status = 0;
				down_status = 0;
				terriansituation = 11;
				switch (difficulty)
				{
				case 3:
					m_gameStatus.totalDist = 0;
					m_gameStatus.situation = 2;
					break;
				case 4:
					m_gameStatus.totalDist = SCALE1;
					m_gameStatus.situation = 2;
					break;
				case 5:
					m_gameStatus.totalDist = SCALE2;
					m_gameStatus.situation = 2;
					break;
				case 6:
					m_gameStatus.totalDist = SCALE3;
					m_gameStatus.situation = 9;
					break;
				default:
					break;
				}
				//�����Ѷ�ѡ������
				if(difficulty == 6)
					PlaySound((LPCWSTR)IDR_BOSS, NULL, SND_RESOURCE | SND_ASYNC | SND_LOOP);
				else
					PlaySound((LPCWSTR)IDR_THEME, NULL, SND_RESOURCE | SND_ASYNC | SND_LOOP);
				KillTimer(hWnd, PICTURE_TIMER);
				SetTimer(hWnd, TIMER_ID, TIMER_ELAPSE, NULL);
				InvalidateRect(hWnd, NULL, FALSE);
				break;
			default:
				break;
			}
			break;
		case LIGHTING_TIMER:
			//����״̬�仯�������ж�
			lighting_situation++;
			if (lighting_situation == 102)
			{
				KillTimer(hWnd, LIGHTING_TIMER);
				lighting_situation = 0;
				if (life != 1)
				{
					switch (life)
					{
					case 3:
						m_gameStatus.situation = 16;
						break;
					case 2:
						m_gameStatus.situation = 17;
						break;
					default:
						break;
					}
					life--;
					SetTimer(hWnd, PICTURE_TIMER, PICTURE_ELAPSE, NULL);
				}
				else if (life == 1)
					m_gameStatus.situation = 3;
				InvalidateRect(hWnd, NULL, FALSE);
			}
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		default:
			break;
		}
		break;
	case WM_DESTROY:
		//���ڹر�
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	return 0;
}

VOID Init(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	//���ؿ�ʼ����λͼ
	m_hStartBmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_START));
	//���ع��½���λͼ
	m_hStoryStartBmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_STORY_START));
	//���ر���λͼ
	m_hBackgroundBmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_BACKGROUND));
	m_hBossBackgroundBmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_BACKGROUNDBOSS));
	//����Ӣ��λͼ
	m_hHeroBmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_HERODOGE_GRAY));
	//������Ϸ״̬λͼ
	m_hGameStatusBmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_GAME_STATUS));
	//���ص��ο�λͼ
	m_hStepBmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_STEP));
	//������������
	m_hDeadBmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_DEAD));
	//����BOSS�ؿ�PPT����������
	m_hBoss1Bmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_BOSS1));
	m_hBoss2Bmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_BOSS2));
	m_hBoss3Bmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_BOSS3));
	m_hBoss4Bmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_BOSS4));
	m_hBoss5Bmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_BOSS5));
	m_hBoss6Bmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_BOSS6));
	m_hBoss7Bmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_BOSS7));
	m_hBoss8Bmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_BOSS8));
	m_hStoryEndBmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_STORYEND));
	m_hEndBmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_END));
	//��������ͼƬ
	m_hLife1Bmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_LIFE1));
	m_hLife2Bmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_LIFE2));
	m_hLife3Bmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_LIFE3));
	//���ص���ͼƬ
	m_hBoneBmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_BONE));
	m_hHeartBmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_HEART));
	//��������ͼƬ
	m_hLightingBmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_LIGHTING));
	//����Ӣ�ۡ�����
	m_hero = CreateHero(-52, 170, HERO_SIZE_X, HERO_SIZE_Y, m_hHeroBmp, 0, HERO_MAX_FRAME_NUM);
	//��������
	int k;
	for (k = 0; k < MAX_TERRIAN_NUM; ++k)
	{
		m_terrian[k] = CreateTerrian(k * 64, 220, STEP_SIZE_X, STEP_SIZE_Y, m_hStepBmp, STEP_SIZE_Y);
		m_terrian[k].items = 0;
	}
	//������Ϸ״̬
	m_gameStatus = CreateGameStatus(0, 0, GAME_STATUS_SIZE_X, GAME_STATUS_SIZE_Y, m_hGameStatusBmp);

	//����״̬
	jump_status = 0;
	down_status = 0;
	terriansituation = 11;
	life = 3;
	seed = ((unsigned)time(NULL) * terriansituation * life) % 100000000;
	if (seed == 0)
		seed += 19491001;
	lighting_situation = 0;
}

VOID Render(HWND hWnd)
{
	PAINTSTRUCT ps;
	HDC hdc;
	//��ʼ����
	hdc = BeginPaint(hWnd, &ps);

	HDC	hdcBmp, hdcBuffer;
	HBITMAP	cptBmp;

	cptBmp = CreateCompatibleBitmap(hdc, WNDWIDTH, WNDHEIGHT);
	hdcBmp = CreateCompatibleDC(hdc);
	hdcBuffer = CreateCompatibleDC(hdc);
	SelectObject(hdcBuffer, cptBmp);

	switch (m_gameStatus.situation)
	{
	case 0:
		//��ʼ����
		SelectObject(hdcBmp, m_hStartBmp);
		BitBlt(hdcBuffer, 0, 0, WNDWIDTH, WNDHEIGHT,
			hdcBmp, 0, 0, SRCCOPY);
		break;
	case 1:
		//��һ�����½�������
		SelectObject(hdcBmp, m_hStoryStartBmp);
		BitBlt(hdcBuffer, 0, 0, WNDWIDTH, WNDHEIGHT,
			hdcBmp, 0, 0, SRCCOPY);
		break;
	case 2: //��һ�׶���Ϸ
	case 9: //�ڶ��׶���Ϸ
		//���Ʊ���������
		if (m_gameStatus.situation == 2)
		{
			SelectObject(hdcBmp, m_hBackgroundBmp);
		} 
		else if (m_gameStatus.situation == 9)
		{
			SelectObject(hdcBmp, m_hBossBackgroundBmp);
		}
		BitBlt(hdcBuffer, 0, 0, WNDWIDTH, WNDHEIGHT,
			hdcBmp, 0, 0, SRCCOPY);

		//���Ƶ���
		int k;
		for (k = 0; k < MAX_TERRIAN_NUM; ++k)
		{
			Terrian terrian = m_terrian[k];
			SelectObject(hdcBmp, terrian.hStepBmp);
			TransparentBlt(
				hdcBuffer, terrian.pos.x, terrian.pos.y,
				terrian.stepWidth, terrian.stepHeight,
				hdcBmp, 0, 0, terrian.stepWidth, terrian.stepHeight,
				RGB(255, 255, 255)
				);
			
			switch (terrian.items)
			{
			case 0:
				break;
			case 1:
				SelectObject(hdcBmp, m_hBoneBmp);
				TransparentBlt(
					hdcBuffer, terrian.pos.x + 9, terrian.pos.y - 33,
					BONE_SIZE_X, BONE_SIZE_Y,
					hdcBmp, 0, 0, BONE_SIZE_X, BONE_SIZE_Y,
					RGB(255, 255, 255)
					);
				break;
			case 2:
				SelectObject(hdcBmp, m_hHeartBmp);
				TransparentBlt(
					hdcBuffer, terrian.pos.x + 21, terrian.pos.y - 40,
					HEART_SIZE_X, HEART_SIZE_Y,
					hdcBmp, 0, 0, HEART_SIZE_X, HEART_SIZE_Y,
					RGB(255, 255, 255)
					);
				break;
			default:
				break;
			}
		}

		//����״̬ʱ��������
		if (lighting_situation != 0)
		{
			BLENDFUNCTION lighting;
			lighting.BlendOp = AC_SRC_OVER;
			lighting.BlendFlags = 0;
			if (lighting_situation <= 51)
				lighting.SourceConstantAlpha = 5 * lighting_situation;
			else
				lighting.SourceConstantAlpha = 510 - 5 * lighting_situation;
			lighting.AlphaFormat = 0;

			SelectObject(hdcBmp, m_hLightingBmp);
			AlphaBlend(hdcBuffer, 0, 0, WNDWIDTH, WNDHEIGHT,
				hdcBmp, 0, 0, WNDWIDTH, WNDHEIGHT, lighting);
		}

		//����Hero������
		SelectObject(hdcBmp, m_hero.hBmp);
		TransparentBlt(
			hdcBuffer, m_hero.pos.x, m_hero.pos.y,
			m_hero.size.cx, m_hero.size.cy,
			hdcBmp, 0, m_hero.size.cy * m_hero.curFrameIndex, m_hero.size.cx, m_hero.size.cy,
			RGB(255, 255, 255)
			);

		//������Ϸ״̬
		SelectObject(hdcBmp, m_gameStatus.hBmp);
		TransparentBlt(hdcBuffer, m_gameStatus.pos.x, m_gameStatus.pos.y, m_gameStatus.size.cx, m_gameStatus.size.cy,
			hdcBmp, 0, m_gameStatus.size.cy * m_gameStatus.isPaused,
			m_gameStatus.size.cx, m_gameStatus.size.cy, RGB(255, 255, 255));

		//���Ƶ÷֡��ؿ�����ʣ������
		TCHAR	szPoint[13];
		SetTextColor(hdcBuffer, RGB(0, 0, 0));
		SetBkMode(hdcBuffer, TRANSPARENT);
		TextOut(hdcBuffer, WNDWIDTH - 110, 30, szPoint, wsprintf(szPoint, _T("����:%d"), m_gameStatus.totalPoint));
		
		if (m_gameStatus.totalDist <=SCALE1)
			TextOut(hdcBuffer, WNDWIDTH - 110, 15, szPoint, wsprintf(szPoint, _T("��һ��")));
		else if (m_gameStatus.totalDist <= SCALE2)
			TextOut(hdcBuffer, WNDWIDTH - 110, 15, szPoint, wsprintf(szPoint, _T("�ڶ���")));
		else if (m_gameStatus.totalDist <= SCALE3)
			TextOut(hdcBuffer, WNDWIDTH - 110, 15, szPoint, wsprintf(szPoint, _T("������")));
		else
			TextOut(hdcBuffer, WNDWIDTH - 110, 15, szPoint, wsprintf(szPoint, _T("Boss��")));
		
		TextOut(hdcBuffer, WNDWIDTH - 110, 45, szPoint, wsprintf(szPoint, _T("ʣ������: %d"), life));
		break;
	case 3:
		//��������
		SelectObject(hdcBmp, m_hDeadBmp);
		BitBlt(hdcBuffer, 0, 0, WNDWIDTH, WNDHEIGHT,
			hdcBmp, 0, 0, SRCCOPY);

		TCHAR   szFinalPoint[13];
		SetTextColor(hdcBuffer, RGB(255, 255, 255));
		SetBkMode(hdcBuffer, TRANSPARENT);
		TextOut(hdcBuffer, 100, 45, szFinalPoint, wsprintf(szFinalPoint, _T("���շ���:%d"), m_gameStatus.totalPoint));
		break;
	case 4:
		//�����������ˡ�
		SelectObject(hdcBmp, m_hBoss1Bmp);
		BitBlt(hdcBuffer, 0, 0, WNDWIDTH, WNDHEIGHT,
			hdcBmp, 0, 0, SRCCOPY);
		break;
	case 5:
		//������þ��ˡ�
		SelectObject(hdcBmp, m_hBoss2Bmp);
		BitBlt(hdcBuffer, 0, 0, WNDWIDTH, WNDHEIGHT,
			hdcBmp, 0, 0, SRCCOPY);
		break;
	case 6:
		//��׷�����ɡ�
		SelectObject(hdcBmp, m_hBoss3Bmp);
		BitBlt(hdcBuffer, 0, 0, WNDWIDTH, WNDHEIGHT,
			hdcBmp, 0, 0, SRCCOPY);
		break;
	case 7:
		//����������뿪��
		SelectObject(hdcBmp, m_hBoss4Bmp);
		BitBlt(hdcBuffer, 0, 0, WNDWIDTH, WNDHEIGHT,
			hdcBmp, 0, 0, SRCCOPY);
		break;
	case 8:
		//�ȵϰ���ͷ
		SelectObject(hdcBmp, m_hBoss5Bmp);
		BitBlt(hdcBuffer, 0, 0, WNDWIDTH, WNDHEIGHT,
			hdcBmp, 0, 0, SRCCOPY);
		break;
	case 10:
		//���㵽����˭����
		SelectObject(hdcBmp, m_hBoss6Bmp);
		BitBlt(hdcBuffer, 0, 0, WNDWIDTH, WNDHEIGHT,
			hdcBmp, 0, 0, SRCCOPY);
		break;
	case 11:
		//����²���ô��
		SelectObject(hdcBmp, m_hBoss7Bmp);
		BitBlt(hdcBuffer, 0, 0, WNDWIDTH, WNDHEIGHT,
			hdcBmp, 0, 0, SRCCOPY);
		break;
	case 12:
		//������
		SelectObject(hdcBmp, m_hBoss8Bmp);
		BitBlt(hdcBuffer, 0, 0, WNDWIDTH, WNDHEIGHT,
			hdcBmp, 0, 0, SRCCOPY);
		break;
	case 13:
		//�ڶ������½���
		SelectObject(hdcBmp, m_hStoryEndBmp);
		BitBlt(hdcBuffer, 0, 0, WNDWIDTH, WNDHEIGHT,
			hdcBmp, 0, 0, SRCCOPY);
		break;
	case 14:
		//��������
		SelectObject(hdcBmp, m_hEndBmp);
		BitBlt(hdcBuffer, 0, 0, WNDWIDTH, WNDHEIGHT,
			hdcBmp, 0, 0, SRCCOPY);

		TCHAR   szFinalPoint0[13];
		SetTextColor(hdcBuffer, RGB(255, 255, 255));
		SetBkMode(hdcBuffer, TRANSPARENT);
		TextOut(hdcBuffer, 100, 45, szFinalPoint0, wsprintf(szFinalPoint0, _T("���շ���:%d"), m_gameStatus.totalPoint));
		break;
	case 15:
		//����ֵΪ3
		SelectObject(hdcBmp, m_hLife3Bmp);
		BitBlt(hdcBuffer, 0, 0, WNDWIDTH, WNDHEIGHT,
			hdcBmp, 0, 0, SRCCOPY);
		break;
	case 16:
		//����ֵΪ2
		SelectObject(hdcBmp, m_hLife2Bmp);
		BitBlt(hdcBuffer, 0, 0, WNDWIDTH, WNDHEIGHT,
			hdcBmp, 0, 0, SRCCOPY);
		break;
	case 17:
		//����ֵΪ1
		SelectObject(hdcBmp, m_hLife1Bmp);
		BitBlt(hdcBuffer, 0, 0, WNDWIDTH, WNDHEIGHT,
			hdcBmp, 0, 0, SRCCOPY);
		break;
	default:
		break;
	}

	//������е���Ϣ���Ƶ���Ļ��
	BitBlt(hdc, 0, 0, WNDWIDTH, WNDHEIGHT, hdcBuffer, 0, 0, SRCCOPY);

	//������Դ��ռ���ڴ�
	DeleteObject(cptBmp);
	DeleteDC(hdcBuffer);
	DeleteDC(hdcBmp);

	//��������
	EndPaint(hWnd, &ps);
}

Hero CreateHero(LONG posX, LONG posY, LONG sizeX, LONG sizeY, HBITMAP hBmp, int curFrameIndex, int maxFrameSize)
{
	Hero hero;
	hero.hBmp = hBmp;
	hero.pos.x = posX;
	hero.pos.y = posY;
	hero.size.cx = sizeX;
	hero.size.cy = sizeY;
	hero.curFrameIndex = curFrameIndex;
	hero.maxFrameSize = maxFrameSize;
	return hero;
}

GameStatus CreateGameStatus(LONG posX, LONG posY, LONG sizeX, LONG sizeY, HBITMAP hBmp)
{
	GameStatus gameStatus;
	gameStatus.pos.x = posX;
	gameStatus.pos.y = posY;
	gameStatus.size.cx = sizeX;
	gameStatus.size.cy = sizeY;
	gameStatus.hBmp = hBmp;
	gameStatus.isPaused = false;
	gameStatus.situation = 0;
	gameStatus.totalDist = 0;
	gameStatus.totalPoint = 0;
	return gameStatus;
}

Terrian CreateTerrian(LONG posX, LONG posY, LONG sizeX, LONG sizeY, HBITMAP hStepBmp, int stepHeight)
{
	Terrian terrian;
	terrian.pos.x = posX;
	terrian.pos.y = posY;
	terrian.size.cx = sizeX;
	terrian.size.cy = sizeY;
	terrian.hStepBmp = hStepBmp;
	terrian.stepWidth = sizeX;
	terrian.stepHeight = STEP_SIZE_Y;
	return terrian;
}

VOID TimerUpdate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	int i, items;
	//�Ѷ��ж�
	if (m_gameStatus.totalDist < SCALE1)
		difficulty = 3;
	else if (m_gameStatus.totalDist < SCALE2)
		difficulty = 4;
	else if (m_gameStatus.totalDist < SCALE3)
		difficulty = 5;
	
	//��ؼ��
	if (BeBorn()>=0)
	{
		HeroUpdate();
	}

	//��������Ϸ״̬����ء�׷�����Ե��߼��
	for (i = 1; i <= difficulty; i++)
	{
		TerrianUpdate();
		GameStatusUpdate();
		DownTest(hWnd);
		ChaseTest();
		RightCollision();
		items = Eating();
		switch (items)
		{
		case 1:
			m_gameStatus.totalPoint += 521 * (difficulty - 2);
			break;
		case 2:
			life++;
			break;
		default:
			break;
		}
	}

	//�������
	if (m_hero.pos.y >= 430 || m_hero.pos.x + m_hero.size.cx <= 0)
		Dead(hWnd);

	InvalidateRect(hWnd, NULL, FALSE);
}

VOID HeroUpdate()
{
	//���¶���
	++m_hero.curFrameIndex;
	m_hero.curFrameIndex = m_hero.curFrameIndex >= (m_hero.maxFrameSize - 1) ? 0 : m_hero.curFrameIndex;
}

VOID TerrianUpdate()
{
	int k, i, j;
	
	//����ˢ��
	for (k = 0; k < MAX_TERRIAN_NUM; ++k)
	{
		m_terrian[k].pos.x -= 2;
	}
	
	//һ����󳬹��������ʼ��һ�����
	for (k = 0; k < MAX_TERRIAN_NUM; ++k)
	{
		if (m_terrian[k].pos.x + m_terrian[k].size.cx <= -4 * m_terrian[k].size.cx)
		{
			Terrian	temp_terrian[5];
			RandTerrian();
			int temp[5];
			int heart_num = 0;
			for (i = 0; i < MAX_TERRIAN_NUM; i++)
			{
				if (m_terrian[i].items == 2)
				{
					heart_num = 1;
					break;
				}
			}
			
			for (j = 0; j < 5; j++)
				temp[j] = terriantype[terriansituation][j];
			for (j = 0; j < 5;j++)
			{
				if (temp[j] == 0)
					temp[j] = 10;
				temp_terrian[j] = CreateTerrian((MAX_TERRIAN_NUM - 5 + j) * 64, 28 + 64 * temp[j], STEP_SIZE_X, STEP_SIZE_Y, m_hStepBmp, STEP_SIZE_Y);
				
				//�������
				seed = ((seed + 1) * temp_terrian[j].pos.y * (unsigned)time(NULL)) % 100000000;
				srand(seed);
				if (rand() % 5 == 0)
					temp_terrian[j].items = 1;
				else
					temp_terrian[j].items = 0;
				seed = ((seed + 1) * temp_terrian[j].pos.y * (unsigned)time(NULL)) % 100000000;
				srand(seed);
				if(life != 3)
					if(temp_terrian[j].items == 0)
						if (rand() % 250 == 0)
							if (heart_num == 0)
							{
								temp_terrian[j].items = 2;
								heart_num++;
							}
			}

			//����ʱ���ηŵ���ʽ������
			for (i = k, j = 0; i <= k + 4; i++, j++)
			{
				if (i >= MAX_TERRIAN_NUM)
				{
					m_terrian[i - MAX_TERRIAN_NUM].pos.x = temp_terrian[j].pos.x;
					m_terrian[i - MAX_TERRIAN_NUM].pos.y = temp_terrian[j].pos.y;
					m_terrian[i - MAX_TERRIAN_NUM].items = temp_terrian[j].items;
				}
				else
				{
					m_terrian[i].pos.x = temp_terrian[j].pos.x;
					m_terrian[i].pos.y = temp_terrian[j].pos.y;
					m_terrian[i].items = temp_terrian[j].items;
				}

			}
		}
	}
}

VOID GameStatusUpdate()
{
	//��Ϸ״̬ˢ��
	++m_gameStatus.totalDist;
	m_gameStatus.totalPoint += difficulty;
}

BOOL Startd(POINT ptMouse)
{
	RECT rStart;

	rStart.left = 586;
	rStart.top = 303;
	rStart.right = 660;
	rStart.bottom = 332;

	return PtInRect(&rStart, ptMouse);
}

BOOL StoryStartd(POINT ptMouse)
{
	RECT rStoryStart;

	rStoryStart.left = 598;
	rStoryStart.top = 334;
	rStoryStart.right = 651;
	rStoryStart.bottom = 353;

	return PtInRect(&rStoryStart, ptMouse);
}

BOOL Restart(POINT ptMouse)
{
	RECT rRestart;

	rRestart.left = 509;
	rRestart.top = 361;
	rRestart.right = 650;
	rRestart.bottom = 378;

	return PtInRect(&rRestart, ptMouse);
}

BOOL Paused(POINT ptMouse)
{
	RECT rPause;

	rPause.left = m_gameStatus.pos.x;
	rPause.top = m_gameStatus.pos.y;
	rPause.right = m_gameStatus.pos.x + m_gameStatus.size.cx;
	rPause.bottom = m_gameStatus.pos.y + m_gameStatus.size.cy;

	return PtInRect(&rPause, ptMouse);
}

BOOL Continued(POINT ptMouse)
{
	RECT rPause;

	rPause.left = 0;
	rPause.top = 0;
	rPause.right = 720;
	rPause.bottom = 430;

	return PtInRect(&rPause, ptMouse);
}

int BeBorn()
{
	int i;
	for (i = 0; i < MAX_TERRIAN_NUM; i++)
	{
		if(m_hero.pos.x + m_hero.size.cx > m_terrian[i].pos.x && m_hero.pos.x < m_terrian[i].pos.x + m_terrian[i].size.cx)
			if (m_hero.pos.y + m_hero.size.cy >= m_terrian[i].pos.y)
				break;
	}
	if (i == MAX_TERRIAN_NUM)
		return -1;
	else
		return i;
}

void RightCollision()
{
	int i;
	for (i = 0; i < MAX_TERRIAN_NUM; i++)
	{
		if (m_hero.pos.y + m_hero.size.cy <= m_terrian[i].pos.y)
			continue;
		else if (m_hero.pos.x + m_hero.size.cx>m_terrian[i].pos.x - 2 && m_hero.pos.x + m_hero.size.cx <= m_terrian[i].pos.x)
		{
			m_hero.pos.x -= 2;
			break;
		}
	}
}

void ChaseTest()
{
	int i, type;
	type = 0;
	if (m_hero.pos.x < 200)
	{
		type = 1;
		for (i = 0; i < MAX_TERRIAN_NUM; i++)
		{
			if (m_hero.pos.y + m_hero.size.cy <= m_terrian[i].pos.y)
				continue;
			else if (m_hero.pos.x + m_hero.size.cx>m_terrian[i].pos.x - 2 && m_hero.pos.x + m_hero.size.cx <= m_terrian[i].pos.x)
			{
				type = 0;
				break;
			}
		}
	}
	if (type == 1)
	{
		m_hero.pos.x += 1;
	}
}

void DownTest(HWND hWnd)
{
	if (BeBorn() == -1 && jump_status == 0)
	{
		if (down_status==0)
		{
			down_status++;
			speed_jump = 0;
			m_hero.curFrameIndex = 5;
			SetTimer(hWnd, DOWN_TIMER, TIMER_ELAPSE, NULL);
		}
	}
}

int Eating()
{
	int i, j;
	j = 0;
	for (i = 0; i < MAX_TERRIAN_NUM; i++)
	{
		switch (m_terrian[i].items)
		{
		case 1:
			if (m_hero.pos.x + m_hero.size.cx > m_terrian[i].pos.x + 6)
				if (m_hero.pos.x < m_terrian[i].pos.x + m_terrian[i].size.cx - 6)
					if (m_hero.pos.y + m_hero.size.cy > m_terrian[i].pos.y - 34)
					{
						j = m_terrian[i].items;
						m_terrian[i].items = 0;
					}
			break;
		case 2:
			if(m_hero.pos.x + m_hero.size.cx > m_terrian[i].pos.x + 21)
				if(m_hero.pos.x < m_terrian[i].pos.x + m_terrian[i].size.cx - 21)
					if (m_hero.pos.y + m_hero.size.cy > m_terrian[i].pos.y - 40)
					{
						j = m_terrian[i].items;
						m_terrian[i].items = 0;
					}
			break;
		default:
			break;
		}
	}
	return j;
}

void Dead(HWND hWnd)
{
	if (jump_status != 0)
		KillTimer(hWnd, JUMP_TIMER);
	else if (down_status == 1)
		KillTimer(hWnd, DOWN_TIMER);
	KillTimer(hWnd, TIMER_ID);
	PlaySound((LPCWSTR)IDR_DEAD, NULL, SND_RESOURCE | SND_ASYNC);
	m_gameStatus.totalPoint = (int)(m_gameStatus.totalPoint / 2);
	SetTimer(hWnd, LIGHTING_TIMER, LIGHTING_ELAPSE, NULL);
}

void RandTerrian()
{
	srand((unsigned)time(NULL));
	switch (terriansituation)
	{
	case 4:
		if (m_gameStatus.totalDist <= SCALE1)
		{
			switch (rand() % 2)
			{
			case 0:
				terriansituation = 10;
				break;
			case 1:
				terriansituation = 13;
				break;
			}
		}
		else
		{
			switch (rand() % 3)
			{
			case 0:
				terriansituation = 7;
				break;
			case 1:
				terriansituation = 10;
				break;
			case 2:
				terriansituation = 13;
				break;
			}
		}
		break;
	case 5:
		if (m_gameStatus.totalDist <= SCALE1)
		{
			switch (rand() % 2)
			{
			case 0:
				terriansituation = 11;
				break;
			case 1:
				terriansituation = 14;
				break;
			}
		}
		else
		{
			switch (rand() % 3)
			{
			case 0:
				terriansituation = 8;
				break;
			case 1:
				terriansituation = 11;
				break;
			case 2:
				terriansituation = 14;
				break;
			}
		}
		break;
	case 6:
		if (m_gameStatus.totalDist <=SCALE1)
			terriansituation = 12;
		else
		{
			switch (rand() % 2)
			{
			case 0:
				terriansituation = 9;
				break;
			case 1:
				terriansituation = 12;
				break;
			}
		}
		break;
	case 1:
	case 7:
	case 10:
	case 15:
	case 17:
	case 21:
		switch (rand() % 7)
		{
		case 0:
			terriansituation = 1;
			break;
		case 1:
			terriansituation = 4;
			break;
		case 2:
			terriansituation = 7;
			break;
		case 3:
			terriansituation = 10;
			break;
		case 4:
			terriansituation = 13;
			break;
		case 5:
			terriansituation = 15;
			break;
		case 6:
			terriansituation = 21;
			break;
		default:
			break;
		}
		break;
	case 2:
	case 8:
	case 11:
	case 13:
	case 16:
	case 18:
	case 19:
	case 22:
		switch (rand() % 9)
		{
		case 0:
			terriansituation = 2;
			break;
		case 1:
			terriansituation = 5;
			break;
		case 2:
			terriansituation = 8;
			break;
		case 3:
			terriansituation = 11;
			break;
		case 4:
			terriansituation = 14;
			break;
		case 5:
			terriansituation = 16;
			break;
		case 6:
			terriansituation = 17;
			break;
		case 7:
			terriansituation = 19;
			break;
		case 8:
			terriansituation = 22;
			break;
		default:
			break;
		}
		break;
	case 3:
	case 9:
	case 12:
	case 14:
	case 20:
	case 23:
		switch (rand() % 7)
		{
		case 0:
			terriansituation = 3;
			break;
		case 1:
			terriansituation = 6;
			break;
		case 2:
			terriansituation = 9;
			break;
		case 3:
			terriansituation = 12;
			break;
		case 4:
			terriansituation = 18;
			break;
		case 5:
			terriansituation = 20;
			break;
		case 6:
			terriansituation = 23;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

void BossStart(HWND hWnd)
{
	if (jump_status != 0)
		KillTimer(hWnd, JUMP_TIMER);
	else if (down_status == 1)
		KillTimer(hWnd, DOWN_TIMER);
	KillTimer(hWnd, TIMER_ID);
	if (m_gameStatus.situation == 2)
	{
		m_gameStatus.situation = 4;
		difficulty = 6;
		PlaySound((LPCWSTR)IDR_BOSS, NULL, SND_RESOURCE | SND_ASYNC | SND_LOOP);
	}
	else if (m_gameStatus.situation == 9)
	{
		m_gameStatus.situation = 10;
		PlaySound((LPCWSTR)IDR_ENDING, NULL, SND_RESOURCE | SND_ASYNC | SND_LOOP);
	}
	InvalidateRect(hWnd, NULL, FALSE);
}

VOID KeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	//TODO
	if ((m_gameStatus.situation == 2 || m_gameStatus.situation == 9) && m_gameStatus.isPaused == 0)
	{
		switch (wParam)
		{
		case VK_UP:
			if (jump_status != 2)
			{
				speed_jump = 12;
				if (jump_status == 0)
				{
					jump_status++;
					SetTimer(hWnd, JUMP_TIMER, TIMER_ELAPSE, NULL);
					m_hero.curFrameIndex = 2;
				}
				else if (jump_status == 1)
				{
					jump_status++;
					m_hero.curFrameIndex = 2;
				}
			}
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		default:
			break;
		}
	}
}

VOID LButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	POINT ptMouse;
	ptMouse.x = LOWORD(lParam);
	ptMouse.y = HIWORD(lParam);

	switch (m_gameStatus.situation)
	{
	case 0:
		//����ڿ�ʼ������˿�ʼ��Ϸ
		if (Startd(ptMouse))
		{
			m_gameStatus.situation = 1;
			InvalidateRect(hWnd, NULL, FALSE);
		}
		break;
	case 1:
		//����ڹ��½����׶ε���GO
		if (StoryStartd(ptMouse))
		{
			m_gameStatus.situation = 15;
			SetTimer(hWnd, PICTURE_TIMER, PICTURE_ELAPSE, NULL);
			InvalidateRect(hWnd, NULL, FALSE);
		}
		break;
	case 2:
	case 9:
		//�������˼�������ͣͼ��
		if (Paused(ptMouse))
		{
			if (!m_gameStatus.isPaused)
			{
				if (jump_status != 0)
					KillTimer(hWnd, JUMP_TIMER);
				else if (down_status == 1)
					KillTimer(hWnd, DOWN_TIMER);
				KillTimer(hWnd, TIMER_ID);
				m_gameStatus.isPaused = TRUE;
			}
			else
			{
				if (jump_status != 0)
					SetTimer(hWnd, JUMP_TIMER, TIMER_ELAPSE, NULL);
				else if (down_status == 1)
					SetTimer(hWnd, DOWN_TIMER, TIMER_ELAPSE, NULL);
				SetTimer(hWnd, TIMER_ID, TIMER_ELAPSE, NULL);
				m_gameStatus.isPaused = FALSE;
			}
			InvalidateRect(hWnd, NULL, FALSE);
		}
		break;
	case 3:
		//����������������GO,ִ�����¿�ʼ
		if (Restart(ptMouse))
		{
			//��ʼ����ť
			//����Ӣ�ۡ�����
			m_hero = CreateHero(-52, 170, HERO_SIZE_X, HERO_SIZE_Y, m_hHeroBmp, 0, HERO_MAX_FRAME_NUM);
			//��������
			int k;
			for (k = 0; k < MAX_TERRIAN_NUM; ++k)
			{
				m_terrian[k] = CreateTerrian(k * 64, 220, STEP_SIZE_X, STEP_SIZE_Y, m_hStepBmp, STEP_SIZE_Y);
				m_terrian[k].items = 0;
			}
			//������Ϸ״̬
			m_gameStatus = CreateGameStatus(0, 0, GAME_STATUS_SIZE_X, GAME_STATUS_SIZE_Y, m_hGameStatusBmp);
			//����״̬
			jump_status = 0;
			down_status = 0;
			terriansituation = 11;
			life = 3;

			m_gameStatus.situation = 15;
			SetTimer(hWnd, PICTURE_TIMER, PICTURE_ELAPSE, NULL);
			InvalidateRect(hWnd, NULL, FALSE);
		}
		break;
	case 4:
	case 5:
	case 6:
	case 7:
		//�����PPT��ӳ�׶ε���PPT��������λ�ã�������ӳ
		if (Continued(ptMouse))
		{
			m_gameStatus.situation++;
			InvalidateRect(hWnd, NULL, FALSE);
		}
		break;
	case 8:
		//����ڵ�����PPT�����Ļ����׶Σ�����boss�ؿ�
		if (Continued(ptMouse))
		{
			m_hero = CreateHero(-52, 170, HERO_SIZE_X, HERO_SIZE_Y, m_hHeroBmp, 0, HERO_MAX_FRAME_NUM);
			//��������
			int k;
			for (k = 0; k < MAX_TERRIAN_NUM; ++k)
			{
				m_terrian[k] = CreateTerrian(k * 64, 220, STEP_SIZE_X, STEP_SIZE_Y, m_hStepBmp, STEP_SIZE_Y);
				m_terrian[k].items = 0;
			}
			//����״̬
			jump_status = 0;
			down_status = 0;
			terriansituation = 11;

			m_gameStatus.situation = 9;
			SetTimer(hWnd, TIMER_ID, TIMER_ELAPSE, NULL);
			InvalidateRect(hWnd, NULL, FALSE);
		}
		break;
	case 10:
	case 11:
	case 12:
	case 13:
		//���Boss�أ�����PPT��ӳ
		if (Continued(ptMouse))
		{
			m_gameStatus.situation++;
			InvalidateRect(hWnd, NULL, FALSE);
		}
		break;
	case 14:
		//TheEnd���棬����ر�
		if (Continued(ptMouse))
		{
			SendMessage(hWnd, WM_CLOSE, 0, 0);
		}
		break;
	default:
		break;
	}
}
