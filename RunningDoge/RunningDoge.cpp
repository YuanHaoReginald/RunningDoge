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
		WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, //设置窗口样式，不可改变大小，不可最大化
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
		//初始化
		Init(hWnd, wParam, lParam);
		break;
	case WM_PAINT:
		//绘制
		Render(hWnd);
		break;
	case WM_KEYDOWN:
		//键盘按下事件
		KeyDown(hWnd, wParam, lParam);
		break;
	case WM_LBUTTONDOWN:
		//左鼠标事件
		LButtonDown(hWnd, wParam, lParam);
		break;
	case WM_TIMER:
		//定时器事件
		switch (wParam)
		{
		case TIMER_ID:
			//正常游戏状态计时器
			if (m_gameStatus.totalDist > SCALE3 && m_gameStatus.situation == 2)
				BossStart(hWnd);
			if (m_gameStatus.totalDist > SCALE4 && m_gameStatus.situation == 9)
				BossStart(hWnd);


			TimerUpdate(hWnd, wParam, lParam);
			break;
		case JUMP_TIMER:
			//跳跃计时器
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
			//下落计时器
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
			//生命图片计时器：显示1秒
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
				//创建地形
				int k;
				for (k = 0; k < MAX_TERRIAN_NUM; ++k)
				{
					m_terrian[k] = CreateTerrian(k * 64, 220, STEP_SIZE_X, STEP_SIZE_Y, m_hStepBmp, STEP_SIZE_Y);
					m_terrian[k].items = 0;
				}
				//人物状态
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
				//依据难度选择音乐
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
			//闪电状态变化及结束判断
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
		//窗口关闭
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
	//加载开始界面位图
	m_hStartBmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_START));
	//加载故事讲述位图
	m_hStoryStartBmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_STORY_START));
	//加载背景位图
	m_hBackgroundBmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_BACKGROUND));
	m_hBossBackgroundBmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_BACKGROUNDBOSS));
	//加载英雄位图
	m_hHeroBmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_HERODOGE_GRAY));
	//加载游戏状态位图
	m_hGameStatusBmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_GAME_STATUS));
	//加载地形块位图
	m_hStepBmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_STEP));
	//加载死亡画面
	m_hDeadBmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_DEAD));
	//加载BOSS关卡PPT及结束界面
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
	//加载生命图片
	m_hLife1Bmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_LIFE1));
	m_hLife2Bmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_LIFE2));
	m_hLife3Bmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_LIFE3));
	//加载道具图片
	m_hBoneBmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_BONE));
	m_hHeartBmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_HEART));
	//加载闪电图片
	m_hLightingBmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_LIGHTING));
	//创建英雄、建筑
	m_hero = CreateHero(-52, 170, HERO_SIZE_X, HERO_SIZE_Y, m_hHeroBmp, 0, HERO_MAX_FRAME_NUM);
	//创建地形
	int k;
	for (k = 0; k < MAX_TERRIAN_NUM; ++k)
	{
		m_terrian[k] = CreateTerrian(k * 64, 220, STEP_SIZE_X, STEP_SIZE_Y, m_hStepBmp, STEP_SIZE_Y);
		m_terrian[k].items = 0;
	}
	//创建游戏状态
	m_gameStatus = CreateGameStatus(0, 0, GAME_STATUS_SIZE_X, GAME_STATUS_SIZE_Y, m_hGameStatusBmp);

	//人物状态
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
	//开始绘制
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
		//开始界面
		SelectObject(hdcBmp, m_hStartBmp);
		BitBlt(hdcBuffer, 0, 0, WNDWIDTH, WNDHEIGHT,
			hdcBmp, 0, 0, SRCCOPY);
		break;
	case 1:
		//第一个故事讲述界面
		SelectObject(hdcBmp, m_hStoryStartBmp);
		BitBlt(hdcBuffer, 0, 0, WNDWIDTH, WNDHEIGHT,
			hdcBmp, 0, 0, SRCCOPY);
		break;
	case 2: //第一阶段游戏
	case 9: //第二阶段游戏
		//绘制背景到缓存
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

		//绘制地形
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

		//死亡状态时绘制闪电
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

		//绘制Hero到缓存
		SelectObject(hdcBmp, m_hero.hBmp);
		TransparentBlt(
			hdcBuffer, m_hero.pos.x, m_hero.pos.y,
			m_hero.size.cx, m_hero.size.cy,
			hdcBmp, 0, m_hero.size.cy * m_hero.curFrameIndex, m_hero.size.cx, m_hero.size.cy,
			RGB(255, 255, 255)
			);

		//绘制游戏状态
		SelectObject(hdcBmp, m_gameStatus.hBmp);
		TransparentBlt(hdcBuffer, m_gameStatus.pos.x, m_gameStatus.pos.y, m_gameStatus.size.cx, m_gameStatus.size.cy,
			hdcBmp, 0, m_gameStatus.size.cy * m_gameStatus.isPaused,
			m_gameStatus.size.cx, m_gameStatus.size.cy, RGB(255, 255, 255));

		//绘制得分、关卡数、剩余生命
		TCHAR	szPoint[13];
		SetTextColor(hdcBuffer, RGB(0, 0, 0));
		SetBkMode(hdcBuffer, TRANSPARENT);
		TextOut(hdcBuffer, WNDWIDTH - 110, 30, szPoint, wsprintf(szPoint, _T("分数:%d"), m_gameStatus.totalPoint));
		
		if (m_gameStatus.totalDist <=SCALE1)
			TextOut(hdcBuffer, WNDWIDTH - 110, 15, szPoint, wsprintf(szPoint, _T("第一关")));
		else if (m_gameStatus.totalDist <= SCALE2)
			TextOut(hdcBuffer, WNDWIDTH - 110, 15, szPoint, wsprintf(szPoint, _T("第二关")));
		else if (m_gameStatus.totalDist <= SCALE3)
			TextOut(hdcBuffer, WNDWIDTH - 110, 15, szPoint, wsprintf(szPoint, _T("第三关")));
		else
			TextOut(hdcBuffer, WNDWIDTH - 110, 15, szPoint, wsprintf(szPoint, _T("Boss关")));
		
		TextOut(hdcBuffer, WNDWIDTH - 110, 45, szPoint, wsprintf(szPoint, _T("剩余生命: %d"), life));
		break;
	case 3:
		//死亡界面
		SelectObject(hdcBmp, m_hDeadBmp);
		BitBlt(hdcBuffer, 0, 0, WNDWIDTH, WNDHEIGHT,
			hdcBmp, 0, 0, SRCCOPY);

		TCHAR   szFinalPoint[13];
		SetTextColor(hdcBuffer, RGB(255, 255, 255));
		SetBkMode(hdcBuffer, TRANSPARENT);
		TextOut(hdcBuffer, 100, 45, szFinalPoint, wsprintf(szFinalPoint, _T("最终分数:%d"), m_gameStatus.totalPoint));
		break;
	case 4:
		//“你终于来了”
		SelectObject(hdcBmp, m_hBoss1Bmp);
		BitBlt(hdcBuffer, 0, 0, WNDWIDTH, WNDHEIGHT,
			hdcBmp, 0, 0, SRCCOPY);
		break;
	case 5:
		//“等你好久了”
		SelectObject(hdcBmp, m_hBoss2Bmp);
		BitBlt(hdcBuffer, 0, 0, WNDWIDTH, WNDHEIGHT,
			hdcBmp, 0, 0, SRCCOPY);
		break;
	case 6:
		//“追过来吧”
		SelectObject(hdcBmp, m_hBoss3Bmp);
		BitBlt(hdcBuffer, 0, 0, WNDWIDTH, WNDHEIGHT,
			hdcBmp, 0, 0, SRCCOPY);
		break;
	case 7:
		//“如果你想离开”
		SelectObject(hdcBmp, m_hBoss4Bmp);
		BitBlt(hdcBuffer, 0, 0, WNDWIDTH, WNDHEIGHT,
			hdcBmp, 0, 0, SRCCOPY);
		break;
	case 8:
		//尤迪安回头
		SelectObject(hdcBmp, m_hBoss5Bmp);
		BitBlt(hdcBuffer, 0, 0, WNDWIDTH, WNDHEIGHT,
			hdcBmp, 0, 0, SRCCOPY);
		break;
	case 10:
		//“你到底是谁？”
		SelectObject(hdcBmp, m_hBoss6Bmp);
		BitBlt(hdcBuffer, 0, 0, WNDWIDTH, WNDHEIGHT,
			hdcBmp, 0, 0, SRCCOPY);
		break;
	case 11:
		//“你猜不到么”
		SelectObject(hdcBmp, m_hBoss7Bmp);
		BitBlt(hdcBuffer, 0, 0, WNDWIDTH, WNDHEIGHT,
			hdcBmp, 0, 0, SRCCOPY);
		break;
	case 12:
		//狗望天
		SelectObject(hdcBmp, m_hBoss8Bmp);
		BitBlt(hdcBuffer, 0, 0, WNDWIDTH, WNDHEIGHT,
			hdcBmp, 0, 0, SRCCOPY);
		break;
	case 13:
		//第二个故事讲述
		SelectObject(hdcBmp, m_hStoryEndBmp);
		BitBlt(hdcBuffer, 0, 0, WNDWIDTH, WNDHEIGHT,
			hdcBmp, 0, 0, SRCCOPY);
		break;
	case 14:
		//结束界面
		SelectObject(hdcBmp, m_hEndBmp);
		BitBlt(hdcBuffer, 0, 0, WNDWIDTH, WNDHEIGHT,
			hdcBmp, 0, 0, SRCCOPY);

		TCHAR   szFinalPoint0[13];
		SetTextColor(hdcBuffer, RGB(255, 255, 255));
		SetBkMode(hdcBuffer, TRANSPARENT);
		TextOut(hdcBuffer, 100, 45, szFinalPoint0, wsprintf(szFinalPoint0, _T("最终分数:%d"), m_gameStatus.totalPoint));
		break;
	case 15:
		//生命值为3
		SelectObject(hdcBmp, m_hLife3Bmp);
		BitBlt(hdcBuffer, 0, 0, WNDWIDTH, WNDHEIGHT,
			hdcBmp, 0, 0, SRCCOPY);
		break;
	case 16:
		//生命值为2
		SelectObject(hdcBmp, m_hLife2Bmp);
		BitBlt(hdcBuffer, 0, 0, WNDWIDTH, WNDHEIGHT,
			hdcBmp, 0, 0, SRCCOPY);
		break;
	case 17:
		//生命值为1
		SelectObject(hdcBmp, m_hLife1Bmp);
		BitBlt(hdcBuffer, 0, 0, WNDWIDTH, WNDHEIGHT,
			hdcBmp, 0, 0, SRCCOPY);
		break;
	default:
		break;
	}

	//最后将所有的信息绘制到屏幕上
	BitBlt(hdc, 0, 0, WNDWIDTH, WNDHEIGHT, hdcBuffer, 0, 0, SRCCOPY);

	//回收资源所占的内存
	DeleteObject(cptBmp);
	DeleteDC(hdcBuffer);
	DeleteDC(hdcBmp);

	//结束绘制
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
	//难度判定
	if (m_gameStatus.totalDist < SCALE1)
		difficulty = 3;
	else if (m_gameStatus.totalDist < SCALE2)
		difficulty = 4;
	else if (m_gameStatus.totalDist < SCALE3)
		difficulty = 5;
	
	//落地检测
	if (BeBorn()>=0)
	{
		HeroUpdate();
	}

	//建筑、游戏状态、落地、追及、吃道具检测
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

	//死亡检测
	if (m_hero.pos.y >= 430 || m_hero.pos.x + m_hero.size.cx <= 0)
		Dead(hWnd);

	InvalidateRect(hWnd, NULL, FALSE);
}

VOID HeroUpdate()
{
	//更新动作
	++m_hero.curFrameIndex;
	m_hero.curFrameIndex = m_hero.curFrameIndex >= (m_hero.maxFrameSize - 1) ? 0 : m_hero.curFrameIndex;
}

VOID TerrianUpdate()
{
	int k, i, j;
	
	//常规刷新
	for (k = 0; k < MAX_TERRIAN_NUM; ++k)
	{
		m_terrian[k].pos.x -= 2;
	}
	
	//一旦落后超过五个。开始下一轮随机
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
				
				//道具随机
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

			//将临时地形放到正式地形中
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
	//游戏状态刷新
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
		//如果在开始界面点了开始游戏
		if (Startd(ptMouse))
		{
			m_gameStatus.situation = 1;
			InvalidateRect(hWnd, NULL, FALSE);
		}
		break;
	case 1:
		//如果在故事讲述阶段点了GO
		if (StoryStartd(ptMouse))
		{
			m_gameStatus.situation = 15;
			SetTimer(hWnd, PICTURE_TIMER, PICTURE_ELAPSE, NULL);
			InvalidateRect(hWnd, NULL, FALSE);
		}
		break;
	case 2:
	case 9:
		//如果点击了继续或暂停图标
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
		//如果在死亡界面点了GO,执行重新开始
		if (Restart(ptMouse))
		{
			//初始化按钮
			//创建英雄、建筑
			m_hero = CreateHero(-52, 170, HERO_SIZE_X, HERO_SIZE_Y, m_hHeroBmp, 0, HERO_MAX_FRAME_NUM);
			//创建地形
			int k;
			for (k = 0; k < MAX_TERRIAN_NUM; ++k)
			{
				m_terrian[k] = CreateTerrian(k * 64, 220, STEP_SIZE_X, STEP_SIZE_Y, m_hStepBmp, STEP_SIZE_Y);
				m_terrian[k].items = 0;
			}
			//创建游戏状态
			m_gameStatus = CreateGameStatus(0, 0, GAME_STATUS_SIZE_X, GAME_STATUS_SIZE_Y, m_hGameStatusBmp);
			//人物状态
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
		//如果在PPT放映阶段点了PPT窗口任意位置，继续放映
		if (Continued(ptMouse))
		{
			m_gameStatus.situation++;
			InvalidateRect(hWnd, NULL, FALSE);
		}
		break;
	case 8:
		//如果在第五张PPT点击屏幕任意阶段，进入boss关卡
		if (Continued(ptMouse))
		{
			m_hero = CreateHero(-52, 170, HERO_SIZE_X, HERO_SIZE_Y, m_hHeroBmp, 0, HERO_MAX_FRAME_NUM);
			//创建地形
			int k;
			for (k = 0; k < MAX_TERRIAN_NUM; ++k)
			{
				m_terrian[k] = CreateTerrian(k * 64, 220, STEP_SIZE_X, STEP_SIZE_Y, m_hStepBmp, STEP_SIZE_Y);
				m_terrian[k].items = 0;
			}
			//人物状态
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
		//完成Boss关，最终PPT放映
		if (Continued(ptMouse))
		{
			m_gameStatus.situation++;
			InvalidateRect(hWnd, NULL, FALSE);
		}
		break;
	case 14:
		//TheEnd界面，点击关闭
		if (Continued(ptMouse))
		{
			SendMessage(hWnd, WM_CLOSE, 0, 0);
		}
		break;
	default:
		break;
	}
}
