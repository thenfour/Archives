// that one flash game called myhouse... this is a minimal clone
#include <windows.h>
#include <vector>
#include <algorithm>
#include <tchar.h>

#define IDC_RESET 1000
#define ARENA_WIDTH 18
#define ARENA_HEIGHT 14

typedef std::pair<long , long > coord2d_t;
long g_nLevel;
#define MAXLEVEL 8
namespace cells
{
    typedef long  cell_t;
    const cell_t fire = 0;
    const cell_t ground = 1;
    const cell_t home = 2;
    const cell_t stone = 3;
};
HWND g_hWnd;

template<long  Tx, long  Ty>
class arena
{
public:
    const long _x;
    const long _y;

    arena() : _x(Tx), _y(Ty) { }

    const cells::cell_t& get(const coord2d_t& xy)
    {
        return m_arena.at(XYToV(xy));
    }
    void set(const coord2d_t& xy, const cells::cell_t& val)
    {
        long i = XYToV(xy);
        if(i >= (long)m_arena.size())
        {
            m_arena.resize(i+1, cells::ground);
        }
        m_arena[i] = val;
    }
private:
    long XYToV(const coord2d_t& xy){ return (xy.first * Tx) + xy.second; }
    std::vector<cells::cell_t> m_arena;
};


std::vector<coord2d_t> g_players;
arena<ARENA_WIDTH,ARENA_HEIGHT> g_arena;

HBRUSH g_hFire;
HBRUSH g_hHome;
HBRUSH g_hGround;
HBRUSH g_hStone;
HBRUSH g_hPlayer;


const long CELLWIDTH = 30;
const long CELLHEIGHT = 30;


template<long Tx>
void SetupArenaRow(long y, cells::cell_t v[Tx])
{
    long x;
    for(x=0;x<Tx;x++)
    {
        g_arena.set(coord2d_t(x, y), v[x]);
    }
}


template<long Tx>
void SetRow(const char sz[Tx], cells::cell_t dat[Tx])
{
    long x;
    for(x=0;x<Tx;x++)
    {
        switch(sz[x])
        {
        case 'f':
            dat[x] = cells::fire;
            break;
        case 's':
            dat[x] = cells::stone;
            break;
        case 'h':
            dat[x] = cells::home;
            break;
        case '.':
        case 'p':
            dat[x] = cells::ground;
            break;
        }
    }
}


void SetupLevel(long nLevel)
{
    cells::cell_t f = cells::fire;
    cells::cell_t g = cells::ground;
    cells::cell_t h = cells::home;
    cells::cell_t s = cells::stone;
    cells::cell_t x0[ARENA_WIDTH];
    cells::cell_t x1[ARENA_WIDTH];
    cells::cell_t x2[ARENA_WIDTH];
    cells::cell_t x3[ARENA_WIDTH];
    cells::cell_t x4[ARENA_WIDTH];
    cells::cell_t x5[ARENA_WIDTH];
    cells::cell_t x6[ARENA_WIDTH];
    cells::cell_t x7[ARENA_WIDTH];
    cells::cell_t x8[ARENA_WIDTH];
    cells::cell_t x9[ARENA_WIDTH];
    cells::cell_t xA[ARENA_WIDTH];
    cells::cell_t xB[ARENA_WIDTH];
    cells::cell_t xC[ARENA_WIDTH];
    cells::cell_t xD[ARENA_WIDTH];

    g_players.clear();

    switch(nLevel)
    {
    case 1:
        {
            SetRow<18>("ffffffffffffffffff", x0);
            SetRow<18>("ffffffffffffffffff", x1);
            SetRow<18>("ff..............ff", x2);
            SetRow<18>("ff..............ff", x3);
            SetRow<18>("ff..............ff", x4);
            SetRow<18>("ff....h....h....ff", x5);
            SetRow<18>("ff..............ff", x6);
            SetRow<18>("ff....ssssss....ff", x7);
            SetRow<18>("ff..............ff", x8);
            SetRow<18>("ff..............ff", x9);
            SetRow<18>("ff..............ff", xA);
            SetRow<18>("ff..............ff", xB);
            SetRow<18>("ffffffffffffffffff", xC);
            SetRow<18>("ffffffffffffffffff", xD);

            g_players.push_back(coord2d_t(6,10));
            g_players.push_back(coord2d_t(11,10));
            break;
        }
    case 2:
        {
            SetRow<18>("ssssssssssssssssss", x0);
            SetRow<18>("f.f............f.f", x1);
            SetRow<18>("f.f............f.f", x2);
            SetRow<18>("f.f............f.f", x3);
            SetRow<18>("f.f...h....h...f.f", x4);
            SetRow<18>("f.f.....ff.....f.f", x5);
            SetRow<18>("f.f.....ss.....f.f", x6);
            SetRow<18>("f.f............f.f", x7);
            SetRow<18>("f.f...h....h...f.f", x8);
            SetRow<18>("f.f............f.f", x9);
            SetRow<18>("f.f............f.f", xA);
            SetRow<18>("f.f............f.f", xB);
            SetRow<18>("f.f....pppp....f.f", xC);
            SetRow<18>("ssssssssssssssssss", xD);

            g_players.push_back(coord2d_t(7,12));
            g_players.push_back(coord2d_t(8,12));
            g_players.push_back(coord2d_t(9,12));
            g_players.push_back(coord2d_t(10,12));
            break;
        }
    case 3:
        {///////////////          11111111
            ////////////012345678901234567
            SetRow<18>("ssssssssssssssssss", x0);
            SetRow<18>("s........f.......s", x1);
            SetRow<18>("s................s", x2);
            SetRow<18>("s................s", x3);
            SetRow<18>("s.......pp.......s", x4);
            SetRow<18>("s.......f........s", x5);
            SetRow<18>("sfff.p..ss..p.fffs", x6);
            SetRow<18>("sfffhhhh..hhhhfffs", x7);
            SetRow<18>("sfff.p..ss..p.fffs", x8);
            SetRow<18>("s........f.......s", x9);
            SetRow<18>("s.......pp.......s", xA);
            SetRow<18>("s................s", xB);
            SetRow<18>("s.......f........s", xC);
            SetRow<18>("ssssssssssssssssss", xD);

            g_players.push_back(coord2d_t(8,4));
            g_players.push_back(coord2d_t(9,4));
            g_players.push_back(coord2d_t(8,10));
            g_players.push_back(coord2d_t(9,10));

            g_players.push_back(coord2d_t(5,6));
            g_players.push_back(coord2d_t(5,8));
            g_players.push_back(coord2d_t(12,6));
            g_players.push_back(coord2d_t(12,8));
            break;
        }
    case 4:
        {///////////////          11111111
            ////////////012345678901234567
            SetRow<18>("ssssssssssssssssss", x0);
            SetRow<18>("s................s", x1);
            SetRow<18>("s..s.h......h.s..s", x2);
            SetRow<18>("s................s", x3);
            SetRow<18>("s..f....p.....fh.s", x4);
            SetRow<18>("s................s", x5);
            SetRow<18>("s................s", x6);
            SetRow<18>("s.......ssh......s", x7);
            SetRow<18>("s.....p..........s", x8);
            SetRow<18>("s.f.............ps", x9);
            SetRow<18>("sp...............s", xA);
            SetRow<18>("s................s", xB);
            SetRow<18>("s................s", xC);
            SetRow<18>("ssssssssssssssssss", xD);

            g_players.push_back(coord2d_t(8,4));
            g_players.push_back(coord2d_t(6,8));
            g_players.push_back(coord2d_t(1,10));
            g_players.push_back(coord2d_t(16,9));
            break;
        }
    case 5:
        {///////////////          11111111
            ////////////012345678901234567
            SetRow<18>("ssssssssssssssssss", x0);
            SetRow<18>("s................s", x1);
            SetRow<18>("s................s", x2);
            SetRow<18>("s.....sssss......s", x3);
            SetRow<18>("s....s.....s.....s", x4);
            SetRow<18>("s...s..s.s..s....s", x5);
            SetRow<18>("s...s..f.f.....p.s", x6);
            SetRow<18>("s...s.h...h.s..s.s", x7);
            SetRow<18>("sff.s..hhh..s..sss", x8);
            SetRow<18>("sff..s.....s...sss", x9);
            SetRow<18>("sf....s.sss....s.s", xA);
            SetRow<18>("sf.............s.s", xB);
            SetRow<18>("sf...p.p.p.p..s..s", xC);
            SetRow<18>("ssssssssssssssssss", xD);

            g_players.push_back(coord2d_t(15,6));
            g_players.push_back(coord2d_t(5,12));
            g_players.push_back(coord2d_t(7,12));
            g_players.push_back(coord2d_t(9,12));
            g_players.push_back(coord2d_t(11,12));
            break;
        }
    case 6:
        {///////////////          11111111
            ////////////012345678901234567
            SetRow<18>("ssssssssssssssssss", x0);
            SetRow<18>("s................s", x1);
            SetRow<18>("s........f.......s", x2);
            SetRow<18>("s......sss.......s", x3);
            SetRow<18>("s......sfhs.f....s", x4);
            SetRow<18>("s......shhs.f....s", x5);
            SetRow<18>("s......s.s.......s", x6);
            SetRow<18>("s......f.........s", x7);
            SetRow<18>("s....fff.........s", x8);
            SetRow<18>("s....p...f.......s", x9);
            SetRow<18>("s...pp....f......s", xA);
            SetRow<18>("s..........f.....s", xB);
            SetRow<18>("s................s", xC);
            SetRow<18>("ssssssssssssssssss", xD);

            g_players.push_back(coord2d_t(5,9));
            g_players.push_back(coord2d_t(5,10));
            g_players.push_back(coord2d_t(4,11));
            break;
        }
    case 7:
        {///////////////          11111111
            ////////////012345678901234567
            SetRow<18>("ssssssssssssssssss", x0);
            SetRow<18>("s..............hhs", x1);
            SetRow<18>("s..............hhs", x2);
            SetRow<18>("s..............hhs", x3);
            SetRow<18>("s..............hhs", x4);
            SetRow<18>("s..............hhs", x5);
            SetRow<18>("s.......ss.....hhs", x6);
            SetRow<18>("s.......ss.....hhs", x7);
            SetRow<18>("s..............hhs", x8);
            SetRow<18>("s..............hhs", x9);
            SetRow<18>("s..............hhs", xA);
            SetRow<18>("s..............hhs", xB);
            SetRow<18>("s..............hhs", xC);
            SetRow<18>("ssssssssssssssssss", xD);

            g_players.push_back(coord2d_t(1,1));
            g_players.push_back(coord2d_t(1,2));
            g_players.push_back(coord2d_t(1,3));
            g_players.push_back(coord2d_t(1,4));
            g_players.push_back(coord2d_t(1,5));
            g_players.push_back(coord2d_t(1,6));
            g_players.push_back(coord2d_t(1,7));
            g_players.push_back(coord2d_t(1,8));
            g_players.push_back(coord2d_t(1,9));
            g_players.push_back(coord2d_t(1,10));
            g_players.push_back(coord2d_t(1,11));
            g_players.push_back(coord2d_t(1,12));
            g_players.push_back(coord2d_t(2,1));
            g_players.push_back(coord2d_t(2,2));
            g_players.push_back(coord2d_t(2,3));
            g_players.push_back(coord2d_t(2,4));
            g_players.push_back(coord2d_t(2,5));
            g_players.push_back(coord2d_t(2,6));
            g_players.push_back(coord2d_t(2,7));
            g_players.push_back(coord2d_t(2,8));
            g_players.push_back(coord2d_t(2,9));
            g_players.push_back(coord2d_t(2,10));
            g_players.push_back(coord2d_t(2,11));
            g_players.push_back(coord2d_t(2,12));
            break;
        }
    case 8:
        {///////////////          11111111
            ////////////012345678901234567
            SetRow<18>("ssssssssssssssssss", x0);
            SetRow<18>("s................s", x1);
            SetRow<18>("s.......hh.......s", x2);
            SetRow<18>("s................s", x3);
            SetRow<18>("s......h..h......s", x4);
            SetRow<18>("s....h..ss..h....s", x5);
            SetRow<18>("s....h..ss..h....s", x6);
            SetRow<18>("s......h..h......s", x7);
            SetRow<18>("s................s", x8);
            SetRow<18>("s.......hh.......s", x9);
            SetRow<18>("s................s", xA);
            SetRow<18>("s.....pppppp.....s", xB);
            SetRow<18>("s.....pppppp.....s", xC);
            SetRow<18>("ssssssssssssssssss", xD);

            g_players.push_back(coord2d_t(6,11));
            g_players.push_back(coord2d_t(7,11));
            g_players.push_back(coord2d_t(8,11));
            g_players.push_back(coord2d_t(9,11));
            g_players.push_back(coord2d_t(10,11));
            g_players.push_back(coord2d_t(11,11));
            g_players.push_back(coord2d_t(6,12));
            g_players.push_back(coord2d_t(7,12));
            g_players.push_back(coord2d_t(8,12));
            g_players.push_back(coord2d_t(9,12));
            g_players.push_back(coord2d_t(10,12));
            g_players.push_back(coord2d_t(11,12));

            break;
        }
    }

    SetupArenaRow<18>(0, x0);
    SetupArenaRow<18>(1, x1);
    SetupArenaRow<18>(2, x2);
    SetupArenaRow<18>(3, x3);
    SetupArenaRow<18>(4, x4);
    SetupArenaRow<18>(5, x5);
    SetupArenaRow<18>(6, x6);
    SetupArenaRow<18>(7, x7);
    SetupArenaRow<18>(8, x8);
    SetupArenaRow<18>(9, x9);
    SetupArenaRow<18>(10, xA);
    SetupArenaRow<18>(11, xB);
    SetupArenaRow<18>(12, xC);
    SetupArenaRow<18>(13, xD);
}


class MovePlayerUp
{
public:
    static coord2d_t getnewpos(const coord2d_t& p)
    {
        return coord2d_t(p.first, p.second-1);
    }
    // sort by Y asc.
    static bool sort_pred(const coord2d_t& elem1, const coord2d_t& elem2)
    {
        if(elem1.second == elem2.second) return (elem1.first < elem2.first);
        return elem1.second < elem2.second;
    }
};

class MovePlayerDown
{
public:
    static coord2d_t getnewpos(const coord2d_t& p)
    {
        return coord2d_t(p.first, p.second+1);
    }
    // sort by Y desc.
    static bool sort_pred(const coord2d_t& elem1, const coord2d_t& elem2)
    {
        if(elem1.second == elem2.second) return (elem1.first < elem2.first);
        return elem1.second > elem2.second;
    }
};

class MovePlayerLeft
{
public:
    static coord2d_t getnewpos(const coord2d_t& p)
    {
        return coord2d_t(p.first-1, p.second);
    }
    static bool sort_pred(const coord2d_t& elem1, const coord2d_t& elem2)
    {
        if(elem1.first == elem2.first) return (elem1.second < elem2.second);
        return elem1.first < elem2.first;
    }
};

class MovePlayerRight
{
public:
    static coord2d_t getnewpos(const coord2d_t& p)
    {
        return coord2d_t(p.first+1, p.second);
    }
    static bool sort_pred(const coord2d_t& elem1, const coord2d_t& elem2)
    {
        if(elem1.first == elem2.first) return (elem1.second < elem2.second);
        return elem1.first > elem2.first;
    }
};


template<typename TMovement>
void MovePlayers()
{
    /*
        Move each player down 1 Y coordinate.  We have to also take into account
        obstacles and fire.
    */
    std::vector<coord2d_t>::iterator it;
    cells::cell_t c;
    
    // sort the players by position, so we scan through them in a logical fashion
    std::sort(g_players.begin(), g_players.end(), TMovement::sort_pred);

    bool bContinue = true;

    for(it=g_players.begin();it!=g_players.end();it++)
    {
        long x = it->first;
        long y = it->second;
        coord2d_t n = TMovement::getnewpos(*it);// proposed new position
        c = g_arena.get(n);// what is the cell of the player's NEW position
        // is it an obstacle
        switch(c)
        {
        case cells::fire:
            // CRASH BABY!
            RedrawWindow(g_hWnd, 0, 0, RDW_INVALIDATE);
            MessageBox(0, _T("Game Over Son!"), 0, MB_OK);
            SetupLevel(g_nLevel);
            RedrawWindow(g_hWnd, 0, 0, RDW_INVALIDATE);
            bContinue = false;
            break;
        case cells::stone:
            // cannot move.
            break;
        case cells::home:
        case cells::ground:
            bool bHit = false;

            // make sure nothing is already there (simple hit test)
            std::vector<coord2d_t>::iterator itcmp;
            for(itcmp=g_players.begin();itcmp!=g_players.end();itcmp++)
            {
                if((itcmp->first == n.first) && (itcmp->second == n.second))
                {
                    bHit = true;
                    break;
                }
            }

            if(!bHit)
            {
                *it = n;
            }

            break;
        }

        if(!bContinue) break;
    }

    // check if they won.
    bool bWon = true;
    for(it=g_players.begin();it!=g_players.end();it++)
    {
        c = g_arena.get(*it);
        if(c != cells::home)
        {
            bWon = false;
            break;
        }
    }

    if(bWon)
    {
        RedrawWindow(g_hWnd, 0, 0, RDW_INVALIDATE);
        MessageBox(0, _T("you win"), 0, MB_OK);
        if(g_nLevel == MAXLEVEL)
        {
            MessageBox(0, _T("YOU WON THE WHOLE DAMN GAME SON!"), 0, MB_OK);
        }
        else
        {
            g_nLevel ++;
            SetupLevel(g_nLevel);
        }
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
    case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
            case IDC_RESET:
                {
                    SetupLevel(g_nLevel);
                    SetFocus(hWnd);
                    InvalidateRect(hWnd, 0, FALSE);
                    return 0;
                }
            }
        }
    case WM_KEYDOWN:
        {
            switch(wParam)
            {
            case VK_UP:
                MovePlayers<MovePlayerUp>();
                InvalidateRect(hWnd, 0, FALSE);
                return 0;
            case VK_LEFT:
                MovePlayers<MovePlayerLeft>();
                InvalidateRect(hWnd, 0, FALSE);
                return 0;
            case VK_RIGHT:
                MovePlayers<MovePlayerRight>();
                InvalidateRect(hWnd, 0, FALSE);
                return 0;
            case VK_DOWN:
                MovePlayers<MovePlayerDown>();
                InvalidateRect(hWnd, 0, FALSE);
                return 0;
            }
            break;
        }
    case WM_CHAR:
        {
            int a = 0+4;
            int b = a+1;
            break;
        }
	case WM_PAINT:
        {
		    hdc = BeginPaint(hWnd, &ps);
            // render the arena.
            int x, y;
            for(y=0;y<g_arena._y;y++)
            {
                for(x=0;x<g_arena._x; x++)
                {
                    cells::cell_t c;
                    c = g_arena.get(coord2d_t(x,y));
                    RECT rc;
                    SetRect(&rc, x*CELLWIDTH,y*CELLHEIGHT,(x+1)*CELLWIDTH,(y+1)*CELLHEIGHT);
                    switch(c)
                    {
                    case cells::fire:
                        FillRect(ps.hdc, &rc, g_hFire);
                        break;
                    case cells::ground:
                        FillRect(ps.hdc, &rc, g_hGround);
                        break;
                    case cells::home:
                        FillRect(ps.hdc, &rc, g_hHome);
                        break;
                    case cells::stone:
                        FillRect(ps.hdc, &rc, g_hStone);
                        break;
                    }
                }
            }

            // Draw the players
            //
            std::vector<coord2d_t>::iterator it;
            SelectObject(ps.hdc, g_hPlayer);

            for(it=g_players.begin();it!=g_players.end();it++)
            {
                x = it->first;
                y = it->second;

                Ellipse(ps.hdc, x*CELLWIDTH, y*CELLHEIGHT, (x+1)*CELLWIDTH, (y+1)*CELLHEIGHT);
            }

    		EndPaint(hWnd, &ps);
    		break;
        }
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
    WNDCLASSEX wcex = {0};

    g_nLevel = 1;

    wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.hInstance		= hInstance;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= _T("myhouse_class");

	RegisterClassEx(&wcex);

    g_hFire = CreateSolidBrush(RGB(255,128,0));
    g_hHome = CreateSolidBrush(RGB(0,160,0));
    g_hGround = CreateSolidBrush(RGB(128,128,40));
    g_hPlayer = CreateSolidBrush(RGB(0,0,160));
    g_hStone = CreateSolidBrush(RGB(140,140,150));

    SetupLevel(1);

    g_hWnd = CreateWindow(_T("myhouse_class"), _T("My House"), WS_OVERLAPPEDWINDOW,
        0, 0, ARENA_WIDTH*CELLWIDTH+20, ARENA_HEIGHT*CELLHEIGHT+50, NULL, NULL, hInstance, NULL);

    // Create a button.
    CreateWindow(_T("BUTTON"), _T("Reset"), WS_CHILD | WS_VISIBLE,
        ARENA_WIDTH*CELLWIDTH-75,ARENA_HEIGHT*CELLHEIGHT, 75, 23, g_hWnd, (HMENU)IDC_RESET, hInstance, NULL);

    ShowWindow(g_hWnd, nCmdShow);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int) msg.wParam;
}

