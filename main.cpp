#define _WIN32_WINNT 0x0500
#include "main.h"
#include "monitor.h"
#include "resource.h"
#include "HelperFns.h"


const LONG ULW_ALPHA = 0x02;

// FORWARD REFERENCES
//
VOID InitAppGlobals();	// initialises application global vars
LRESULT CALLBACK WndTapeProc (HWND, UINT, WPARAM, LPARAM); // tape event handler
LRESULT CALLBACK ColorSelectorProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ColorSelectBtnProc (HWND, UINT, WPARAM, LPARAM);

BOOL CALLBACK OptionsDlgProc(HWND, UINT, WPARAM, LPARAM); // options dialog event handler

BOOL CALLBACK AccessDlgProc(HWND, UINT, WPARAM, LPARAM);  // no access dialog event handler


BOOL CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);  // about dialog event handler
BOOL AboutDlg_OnClose(HWND);
BOOL AboutDlg_OnInitDialog(HWND, HWND, LPARAM);
BOOL AboutDlg_OnDestroy(HWND);
BOOL AboutDlg_OnCommand(HWND, INT, HWND, UINT);


BOOL CALLBACK EditMsgsDlgProc(HWND, UINT, WPARAM, LPARAM); // edit messages dialog event handler
BOOL EditMsgsDlg_OnClose(HWND);
BOOL EditMsgsDlg_OnInitDialog(HWND, HWND, LPARAM);
BOOL EditMsgsDlg_OnDestroy(HWND);
BOOL EditMsgsDlg_OnCommand(HWND, INT, HWND, UINT);
VOID EditMsgsDlg_UpdatePrewiew(HWND);

BOOL CALLBACK UpdateDlgProc(HWND, UINT, WPARAM, LPARAM); // edit message dialog event handler
BOOL UpdateDlg_OnClose(HWND);
BOOL UpdateDlg_OnInitDialog(HWND, HWND, LPARAM);
BOOL UpdateDlg_OnDestroy(HWND);
BOOL UpdateDlg_OnCommand(HWND, INT, HWND, UINT);



BOOL RegisterAccessBar(HWND, BOOL);
VOID CALLBACK AppBarQuerySetPos(UINT, LPRECT, PAPPBARDATA);
VOID AppBarCallback(HWND, UINT, LPARAM);
VOID CALLBACK AppBarPosChanged(PAPPBARDATA pabd);
VOID MonitorCallback(VOID);
VOID StartTimer(LONG delay);
std::string GetCurrentTimeStr(VOID);
std::string GetCurrentTimeStrShort(VOID);
BOOL hasFileAccess(VOID);
BOOL ReadFromFile(char*);

void DrawSkin(HDC hdc);



// GLOBAL VARIABLES
//
PAPPGLOBALS g_pApp;
PAPPFORMSGLOBALS g_pAppFrm;

APPBARDATA g_Abd;
Monitor g_pFileMonitor;
HINSTANCE g_hInstance = 0;

INT g_iTextLength;
BOOL g_bShowClock = TRUE;
//std::vector<MESSAGE> g_vMsgs;

std::vector<CMessage> g_vMessages;
std::vector<CMessage> g_vTmpMessages;
CUpdateHeader g_vHeader;
HBITMAP g_hBmpUp;
HBITMAP g_hBmpDn;
HBITMAP g_hBmpSkin;
HBITMAP g_hBmpSkinMask;
HBITMAP g_hBmpUpMask;
HBITMAP g_hBmpDnMask;




HBITMAP g_hBmpFrameLeftMask;
HBITMAP g_hBmpFrameLeft;
HBITMAP g_hBmpFrameMiddleMask;
HBITMAP g_hBmpFrameMiddle;
HBITMAP g_hBmpFrameRightMask;
HBITMAP g_hBmpFrameRight;
HDC          hDCSkin;
HDC          hDCSkinMask;






COLORREF g_Colors[] = { RGB(0x99,0xB3,0xFF), RGB(0xB3,0x99,0xFF), RGB(0xE6,0x99,0xFF), RGB(0xFF,0x99,0xE6),
											RGB(0x99,0xE6,0xFF), RGB(0x5C,0x85,0xFF),	RGB(0x1F,0x57,0xFF), RGB(0xFF,0x99,0xB3),
											RGB(0x99,0xFF,0xE6), RGB(0xFF,0xC7,0x1F), RGB(0xFF,0xD6,0x5C), RGB(0xFF,0xB3,0x99),
											RGB(0x99,0xFF,0xB3), RGB(0xB3,0xFF,0x99), RGB(0xE6,0xFF,0x99), RGB(0xFF,0xE6,0x99)
           };

DWORD g_crItems[] = {
	RGB(  0,   0,   0),	RGB( 64,   0,   0),	RGB(255,   0,  51),	RGB(128,  64,  64),	RGB(204, 255, 153),	RGB(255, 128, 128),
	RGB(255, 255, 128),	RGB(255, 255,   0),	RGB(255, 128,  64),	RGB(255, 128,   0),	RGB(128,  64,   0),	RGB(128, 128,   0),
	RGB(128, 128,  64),	RGB(  0,  64,   0),	RGB(  0, 128,   0),	RGB(  0, 255,   0),	RGB(128, 255,   0),	RGB(128, 255, 128),
	RGB(  0, 255, 128),	RGB(  0, 255,  64),	RGB(  0, 128, 128),	RGB(  0, 128,  64),	RGB(  0,  64,  64),	RGB(128, 128, 128),
	RGB( 64, 128, 128),	RGB(  0,   0, 128),	RGB(  0,   0, 255),	RGB(  0,  64, 128),	RGB(  0, 255, 255), RGB(128, 255, 255),
	RGB(  0, 128, 255),	RGB(  0, 128, 192),	RGB(128, 128, 255),	RGB(  0,   0, 160),	RGB(  0,   0,  64),	RGB(192, 192, 192),
	RGB( 64,   0,  64),	RGB( 64,   0,  64),	RGB(128,   0, 128),	RGB(128,   0,  64),	RGB(128, 128, 192),	RGB(255, 128, 192),
	RGB(255, 128, 255),	RGB(255,   0, 255), RGB(255,   0, 128),	RGB(128,   0, 255), RGB( 64,   0, 128),	RGB(255, 255, 255),
};


char szClassName[] = "TickerTape ver 2.0";


// InitAppGlobals() - Initialises global variables
//
VOID InitAppGlobals() {

  g_pApp = new APPGLOBALS;
  g_pApp->uTapeScreenSide = ABE_TOP;
  g_pApp->bTimerRunning = FALSE;
  g_pApp->uElapse = 9; //9ms
  g_pApp->upTimerId = 0;
  g_pApp->dMsgScrStep = 1.0;
  g_pApp->dMsgScrPosX = SCREEN_WIDTH;
  g_pApp->dMsgScrPosY = 2;
  g_pApp->szMsgFilepath = "messages.txt";
  g_pApp->szMsgSepacer = "    ";
  
  g_pApp->lBgColor = 0;
  g_pApp->hPopupMenu = (HMENU) MAKEINTRESOURCE(IDR_POPUP);
  
  
  g_pAppFrm = new APPFORMSGLOBALS;
  
  PEDITMSGDLG g_pEditMsgsDlg = new EDITMSGDLG;
  g_pAppFrm->pEditMsgsDlg = g_pEditMsgsDlg;
  
  
  g_pAppFrm->pEditMsgsDlg->_IDC_CHKNMC = BST_CHECKED;
  g_pAppFrm->pEditMsgsDlg->_IDC_CHKTIME = BST_CHECKED;
  g_pAppFrm->pEditMsgsDlg->_IDC_RBNOUTAGESNO = BST_CHECKED;
  g_pAppFrm->pEditMsgsDlg->_IDC_RBNOUTAGESYES = BST_UNCHECKED;
  
  PUPDATEDLG g_pUpdateDlg = new UPDATEDLG;
  g_pAppFrm->pUpdateDlg = g_pUpdateDlg;
  //delete g_pUpdateDlg;
  
  g_pAppFrm->pUpdateDlg->_IDC_RBNONGOING = BST_CHECKED;
  g_pAppFrm->pUpdateDlg->_IDC_RBNRESTORED = BST_UNCHECKED;
  g_pAppFrm->pUpdateDlg->_IDC_CHKETA = BST_UNCHECKED;
  g_pAppFrm->pUpdateDlg->_IDC_CHKETR = BST_CHECKED;


   g_pAppFrm->pUpdateDlg->_IDC_EDTSTART = "00:00";
   g_pAppFrm->pUpdateDlg->_IDC_EDTRESTORED = "00:00";
   g_pAppFrm->pUpdateDlg->_IDC_EDTETA = "00:00";;
   g_pAppFrm->pUpdateDlg->_IDC_EDTETR = "00:00";;
   g_pAppFrm->pUpdateDlg->_IDC_EDTUPDATE = "...";
}

/*******************************************************************************/
// MonitorCallback() - callback function for file monitor
// Called by Monitor object when change to the file is detected
//
VOID MonitorCallback(VOID) {
  ReadFromFile(g_pApp->szMsgFilepath);
  
}

BOOL ReadFromFile(char* szFilePath) {

	CUpdate* tmp;
  CMessage* pMsg;
	
  std::ifstream in(szFilePath, std::ios::binary|std::ios::in);
  if (in.good()) {
  		g_vHeader.deserialize(in);
  		
  		g_vMessages.clear();
  		tmp = new CUpdate();
  		while (tmp->deserialize(in)) {
  			if (in.fail()) MsgBox("Error reading input file!");
        pMsg = new CMessage();
  			pMsg->pUpdate = tmp;
  			pMsg->oRgb =g_crItems[(tmp->bRestored?4:2)]; //red
//TODO: ADD COLOR
      	g_vMessages.push_back(*pMsg);
      	tmp = new CUpdate();
      }
      delete tmp;
      
      
      
      if (g_vMessages.empty()) {
        g_pAppFrm->pEditMsgsDlg->_IDC_RBNOUTAGESNO = BST_CHECKED;
        g_pAppFrm->pEditMsgsDlg->_IDC_RBNOUTAGESYES = BST_UNCHECKED;
      } else {
      	g_pAppFrm->pEditMsgsDlg->_IDC_RBNOUTAGESNO = BST_UNCHECKED;
        g_pAppFrm->pEditMsgsDlg->_IDC_RBNOUTAGESYES = BST_CHECKED;
      }
      
  } else {
    MsgBox("Error reading input file!");
  }
}



BOOL SaveToFile(char* szFilePath) {

 std::vector<CMessage>::iterator itMsg;
 
 // open the file for append
 std::ofstream out(szFilePath, std::ios::binary|std::ios::trunc|std::ios::out);

 if (out.good()) {
 		// save header
 		g_vHeader.serialize(out);
 		for (itMsg = g_vMessages.begin(); itMsg != g_vMessages.end(); itMsg++)
 			(*itMsg).pUpdate->serialize(out);
 		out.close();
 }
 
	return TRUE;
}

/*******************************************************************************/

// getCurrentTimeStr() returns formatted current UTC time
//
std::string GetCurrentTimeStr() {
  tm* ptr;
  time_t tm;
  char dateTime[24];
  tm=time(NULL);
  strftime(dateTime,100,"%a, %d/%m/%y %H:%M:%S UTC",gmtime(&tm));
  return dateTime;
}

std::string GetCurrentTimeStrShort() {
  tm* ptr;
  time_t tm;
  char dateTime[24];
  TCHAR* pdate;
  tm=time(NULL);
  strftime(dateTime,100,"%H:%M UTC ",gmtime(&tm));
  return dateTime;
}


/*******************************************************************************/

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nCmdShow) {
    HWND hWnd;               /* This is the handle for our window */
    HWND hWndSkin;
    HWND hWndCloseButton;		// the close button
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */
    RECT rc;

    


    /* The Window structure */
    wincl.hInstance = hThisInstance;
    
    
    //#init globals
    InitAppGlobals();
    g_hBmpUp = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_BMPUP));
    g_hBmpDn = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_BMPDN));
    g_hBmpUpMask = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_BMPMASKUP));
    g_hBmpDnMask = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_BMPMASKDN));
    g_hBmpFrameLeftMask = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_BMPFRAMELEFTMASK));
    g_hBmpFrameLeft = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_BMPFRAMELEFT));
    g_hBmpFrameMiddleMask = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_BMPFRAMEMIDDLEMASK));
    g_hBmpFrameMiddle = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_BMPFRAMEMIDDLE));
    g_hBmpFrameRightMask = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_BMPFRAMERIGHTMASK));
    g_hBmpFrameRight = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_BMPFRAMERIGHT));
    
    	//DbgMsgBox((Int2Str(bm.bmHeight)).c_str());
    
    
    TCHAR szBuffer[MAX_LOADSTRING];
    LoadString(hThisInstance, IDS_NMC, szBuffer, sizeof(szBuffer)/sizeof(szBuffer[0]));
	  g_pAppFrm->pEditMsgsDlg->_IDC_EDTNMC = szBuffer;
	  LoadString(hThisInstance, IDS_OUTAGE, szBuffer, sizeof(szBuffer)/sizeof(szBuffer[0]));
	  g_pAppFrm->pEditMsgsDlg->_IDC_EDTNOOUTAGES = szBuffer;
	  g_pAppFrm->pEditMsgsDlg->_IDC_EDTTIME = GetCurrentTimeStrShort();
	  //#end init globals
	  
	  
	  
	  
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WndTapeProc;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* icon and mouse-pointer */
    wincl.hIcon = (HICON)LoadImage(hThisInstance, MAKEINTRESOURCE(IDI_MYICON), IMAGE_ICON, 32, 32, LR_SHARED);
    wincl.hIconSm = (HICON)LoadImage(hThisInstance, MAKEINTRESOURCE(IDI_MYICON), IMAGE_ICON, 16, 16, LR_SHARED);

    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL; //MAKEINTRESOURCE(IDR_POPUP);                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    wincl.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);


    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hWnd = CreateWindowEx (
            WS_EX_TOOLWINDOW |
           	WS_EX_CLIENTEDGE,                    /*Extended possibilites for variation */
           	szClassName,         /* Classname */
           	"TickerTape ver 2.2",       /* Title Text */
           	WS_POPUP | WS_VISIBLE,
           	//WS_POPUP | WS_SYSMENU,/*|WS_DLGFRAME, /* default window */
           	0,       /* Windows decides the position */
           	0,       /* where the window ends up on the screen */
           	SCREEN_WIDTH,                 /* The programs width */
           	24,                 /* and height in pixels */
           	HWND_DESKTOP,        /* The window is a child-window to desktop */
           	NULL,                /* No menu */
           	hThisInstance,       /* Program Instance handler */
           	NULL                 /* No Window Creation data */
    );

     hWndCloseButton = CreateWindowEx(
    				0,
    				"BUTTON",
    				(const CHAR *) "X\0",
    				WS_VISIBLE | WS_CHILD | BS_CENTER | BS_VCENTER | BS_FLAT | BS_PUSHBUTTON,
    				SCREEN_WIDTH - 15,
        		0,
        		10,
        		10,
        		hWnd,
        		(HMENU) IDM_CLOSE,
        		hThisInstance,
        		NULL
   		);



    //:TOCHECK: test if it is necessary to set default font
    //
    HGDIOBJ hObj = GetStockObject(DEFAULT_GUI_FONT);
    SendMessage(hWnd, WM_SETFONT,(WPARAM)hObj, true);


    BOOL success = RegisterAccessBar(hWnd, TRUE);
    if (!success) {
    	MessageBox(NULL, "Registration failed", "Error" ,MB_ICONSTOP);
    	return 0;
    }
    GetWindowRect(hWnd, &rc);
    AppBarQuerySetPos(ABE_TOP, &rc, &g_Abd);
    
    SetTimer(hWnd, g_pApp->upTimerId, g_pApp->uElapse, NULL);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // setup file monitoring
    g_pFileMonitor.setCallbackFunction(&MonitorCallback);
    
    
    //:TOCHECK: Provide file instead of directory
    g_pFileMonitor.start(".\\");

    while (GetMessage (&messages, NULL, 0, 0)) {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}

/*******************************************************************************/

LRESULT CALLBACK WndTapeProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {

 		HDC hdc, hDCBtn;
 		static HMENU hMenu;
 		char str[100];
 		int wmId, wmEvent;
 		POINT pt;
 		RECT rc;
 		int tpos;


    HDC          hdcMem;
    HDC          hDcArrow;
    HDC          hDcArrowMask;
    
    
    int streachX;
    
    BLENDFUNCTION blend;

    
    
    BITMAP 			 bm;
    POINT      	 ptSize;
    HBITMAP      hbmMem;
		HANDLE       hOld;
		PAINTSTRUCT  ps;
		//HDC          hdc;

		HFONT	    	 hFont;
		HFONT        hOldFont;
		HBRUSH 			 hBrush;
		RECT 				 rec;

 		int offset;
    SIZE size;
    std::string sMsg;
    std::vector<CMessage>::iterator it;
 		
 

    switch (message)                  /* handle the messages */
    {
      	case WM_CREATE:
         		// read the message
         		MonitorCallback();
         		hMenu = LoadMenu((HINSTANCE) GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_POPUP));
         		hMenu = GetSubMenu(hMenu, 0);
         		break;
        // app specific
        case WM_CONTEXTMENU:
        //case WM_RBUTTONUP:
          	 GetWindowRect(hwnd, &rc);
             TrackPopupMenu(hMenu, TPM_RIGHTBUTTON|TPM_VERPOSANIMATION, LOWORD(lParam), rc.bottom, 0, hwnd, NULL);
             break;
    		case APPBAR_CALLBACK:
 				 		AppBarCallback(hwnd, (UINT) wParam, lParam);
 				 		return DefWindowProc (hwnd, message, wParam, lParam);
        		break;
        // window specific
        case WM_TIMER:
          	if (g_pApp->dMsgScrPosX < - (g_iTextLength))
           		g_pApp->dMsgScrPosX = SCREEN_WIDTH - 12;
          	g_pApp->dMsgScrPosX -= g_pApp->dMsgScrStep;
            InvalidateRect(hwnd, NULL, FALSE);
            break;
        case WM_ERASEBKGND:
            break;
        case WM_ACTIVATE:
          	GetClientRect(hwnd, &rec);
            SendMessage(hwnd, WM_SIZE, (WPARAM) SIZE_RESTORED, MAKELPARAM(rec.right - rec.left, rec.bottom - rec.top));
          	break;
        case WM_SIZE:
        		break;
        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);
    				hdcMem = CreateCompatibleDC(hdc);
    				hbmMem = CreateCompatibleBitmap(hdc, ps.rcPaint.right,  ps.rcPaint.bottom);
    				hOld   = SelectObject(hdcMem, hbmMem);

        		rec.left = SCREEN_WIDTH - 10;
        		rec.top = 10;
        		rec.right = SCREEN_WIDTH;
        		rec.bottom = ps.rcPaint.bottom;

    				hFont = CreateFont(13, 8, 0, 0,
                           FW_SEMIBOLD, 0, 0, 0,
                           DEFAULT_CHARSET, OUT_TT_PRECIS,
                           CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                           OUT_OUTLINE_PRECIS | FF_MODERN,
                           "Tahoma");

            hOldFont = (HFONT) SelectObject(hdcMem, hFont); // store old font
    				SetBkMode( hdcMem, TRANSPARENT);


    				hBrush = CreateSolidBrush(g_pApp->lBgColor);
    				FillRect(hdcMem,&ps.rcPaint,hBrush);

    				
    				offset = 0;
SetTextColor(hdcMem, RGB(51,153,255));
    				// show clock
    				if (g_bShowClock) {
          			sMsg = GetCurrentTimeStr() + g_pApp->szMsgSepacer;
          			TextOut (hdcMem, (DWORD)(floor((float)g_pApp->dMsgScrPosX+0.5)+offset), (DWORD) g_pApp->dMsgScrPosY, sMsg.c_str(), lstrlen(sMsg.c_str())) ;
              	GetTextExtentPoint32(hdcMem, sMsg.c_str(), lstrlen(sMsg.c_str()), &size);
              	offset+= size.cx; // store X position of clock offset
    		    }
    				
    				// display messages
    				
    				// first header
    				if (&g_vHeader != NULL) {
            SetTextColor(hdcMem, RGB(0,204,255));
    						sMsg = g_vHeader.toString() + g_pApp->szMsgSepacer;
          			TextOut (hdcMem, (DWORD)(floor((float)g_pApp->dMsgScrPosX+0.5)+offset), (DWORD) g_pApp->dMsgScrPosY, sMsg.c_str(), lstrlen(sMsg.c_str())) ;
              	GetTextExtentPoint32(hdcMem, sMsg.c_str(), lstrlen(sMsg.c_str()), &size);
              	offset+= size.cx; // store X position of clock offset
    		    }

    		    // then all messages
    		    // if there are messages

    		    if (!g_vMessages.empty()) {
    		        for(it = g_vMessages.begin(); it != g_vMessages.end();it++) {

                    // draw arrows
                    hDcArrow = CreateCompatibleDC(hdc);
                    hDcArrowMask = CreateCompatibleDC(hdc);
                    
                    SetBkColor(hdcMem, RGB(255, 255, 255));      // 1s --> 0xFFFFFF
                    SetTextColor(hdcMem, RGB(0, 0, 0));          // 0s --> 0x000000

                    if ( (*it).pUpdate->bRestored) {
                    		SelectObject(hDcArrowMask, g_hBmpUpMask);
                    		SelectObject(hDcArrow, g_hBmpUp);
                    } else {
                    		SelectObject(hDcArrowMask, g_hBmpDnMask);
                    		SelectObject(hDcArrow, g_hBmpDn);
                    }
                    
                    GetObject(g_hBmpUpMask,sizeof(BITMAP),(LPVOID)&bm); // Get the description struct of the bitmap.

                     BitBlt(hdcMem,
                       (DWORD)(floor((float)g_pApp->dMsgScrPosX+0.5)+offset),
                       (DWORD) g_pApp->dMsgScrPosY,
                       bm.bmWidth,
                       bm.bmHeight,
                       hDcArrow,
                       0,
                       0,
                       SRCINVERT);
                    
                    BitBlt(hdcMem,
                       (DWORD)(floor((float)g_pApp->dMsgScrPosX+0.5)+offset),
                       (DWORD) g_pApp->dMsgScrPosY,
                       bm.bmWidth,
                       bm.bmHeight,
                       hDcArrowMask,
                       0,
                       0,
                       SRCAND);

                     BitBlt(hdcMem,
                       (DWORD)(floor((float)g_pApp->dMsgScrPosX+0.5)+offset),
                       (DWORD) g_pApp->dMsgScrPosY,
                       bm.bmWidth,
                       bm.bmHeight,
                       hDcArrow,
                       0,
                       0,
                       SRCINVERT);
                       

                       
                     DeleteDC    (hDcArrow);
                     DeleteDC    (hDcArrowMask);
                     offset+= bm.bmWidth+2;

              			SetTextColor(hdcMem, (*it).oRgb);
              			
              			sMsg = (*it).pUpdate->toString() + g_pApp->szMsgSepacer;
              			TextOut (hdcMem,
              				   (DWORD)(floor((float)g_pApp->dMsgScrPosX+0.5)+offset),
              				   (DWORD) g_pApp->dMsgScrPosY,
                   		   sMsg.c_str(),
                   		   lstrlen(sMsg.c_str()));
            		    GetTextExtentPoint32(hdcMem, sMsg.c_str(), lstrlen(sMsg.c_str()), &size);
            		    offset+= size.cx;
            		    //it++;
            		} // end for
            } else {
            		SetTextColor(hdcMem, g_crItems[4]);
            		sMsg = g_pAppFrm->pEditMsgsDlg->_IDC_EDTNOOUTAGES;
            		TextOut (hdcMem,
              				   (DWORD)(floor((float)g_pApp->dMsgScrPosX+0.5)+offset),
              				   (DWORD) g_pApp->dMsgScrPosY,
                   		   sMsg.c_str(),
                   		   lstrlen(sMsg.c_str()));
        		    GetTextExtentPoint32(hdcMem, sMsg.c_str(), lstrlen(sMsg.c_str()), &size);
            		offset+= size.cx;
            		//#TODO: Display: No major outages on the network or error
            }

            g_iTextLength = offset; // store entire message length
            
            // Add frame to make it look nice
/*
            hDCSkin = CreateCompatibleDC(hdc);
            hDCSkinMask = CreateCompatibleDC(hdc);
            SelectObject(hDCSkin, g_hBmpSkin);
            SelectObject(hDCSkinMask, g_hBmpSkinMask);
            
            
            GetObject(g_hBmpSkin, sizeof(BITMAP),(LPVOID)&bm); // Get the description struct of the bitmap.

            BitBlt(hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, hDCSkin, 0, 0, SRCINVERT);
            BitBlt(hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, hDCSkinMask, 0, 0, SRCAND);
            BitBlt(hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, hDCSkin, 0, 0, SRCINVERT);


          	DeleteDC(hDCSkin);
          	DeleteDC(hDCSkinMask);
*/

            // flash buffer to screen - in two stages to eliminate close button flickering
            
            
            
            DrawSkin(hDCSkin);
            DoAlphaBlend(hdcMem , 0,0,SCREEN_WIDTH,24,
				            hDCSkin,0,0,100,24,70);

    				BitBlt(hdc, 0, 0, ps.rcPaint.right-10,  ps.rcPaint.bottom, hdcMem, 0, 0, SRCCOPY);
    				BitBlt(hdc, ps.rcPaint.right-10, 10, 10,  ps.rcPaint.bottom-ps.rcPaint.top-10, hdcMem, ps.rcPaint.right-10, 10, SRCCOPY);

            SelectObject(hdcMem, hOldFont); // restore old font
    				DeleteObject(hFont);
    				DeleteObject(hbmMem);
    				DeleteObject(hBrush);
    				DeleteDC    (hdcMem);
            EndPaint(hwnd, &ps);
            
    				break;
        case WM_COMMAND:
            wmId    = LOWORD(wParam);
            wmEvent = HIWORD(wParam);
            switch (wmId) {
              case IDM_OPTIONS:
              	DialogBox((HINSTANCE)GetModuleHandle(NULL), (LPCTSTR)(ID_DLGOPIONS), hwnd, OptionsDlgProc);
              	break;
              case IDM_BACKGROUND:
                //:TOCHECK: swapped hInstance with GetModuleHandle(NULL)
                DialogBox((HINSTANCE)GetModuleHandle(NULL), (LPCTSTR)IDD_BACKGROUND, hwnd, (DLGPROC) ColorSelectorProc);
                break;
              case IDM_ABOUT:
                DialogBox((HINSTANCE)GetModuleHandle(NULL), (LPCTSTR)(ID_ABOUT), hwnd, AboutDlgProc);
                break;
              case IDM_UPDATE:
              	SendMessage (hwnd, WM_LBUTTONDBLCLK, 0, 0);
                return TRUE;
              case IDM_CLOSE:
              case IDM_EXIT:
              case IDC_BTNOK:
              	DestroyWindow(hwnd);
                return 0;
             	default:
                return DefWindowProc(hwnd, message, wParam, lParam);
            }
            break;
        case WM_LBUTTONDBLCLK:
          	if (hasFileAccess())
            	DialogBox((HINSTANCE)GetModuleHandle(NULL), (LPCTSTR)(IDD_EDITMSGS), hwnd, EditMsgsDlgProc );
            else
            	DialogBox((HINSTANCE)GetModuleHandle(NULL), (LPCTSTR)(IDD_NOACCESS), hwnd, AccessDlgProc );
            break;
        case WM_DESTROY:
          	g_pFileMonitor.stop();
          	//delete(g_pFileMonitor);
        		KillTimer(hwnd,g_pApp->upTimerId);
        		g_vMessages.clear();
        		g_vTmpMessages.clear();
          	RegisterAccessBar(hwnd, FALSE);
          	//DeleteDC(doubleBuff.back_dc);
          	//ReleaseDC(doubleBuff.win_hwnd,doubleBuff.win_dc);
          	//memset(&doubleBuff,0,sizeof(SDBuffer));
          	
          	
          	//delete g_pEditMsgsDlg;
          	//delete(g_pAppFrm->pEditMsgsDlg);
          	//delete (g_pAppFrm->pEditMsgsDlg);
          	//delete (g_pAppFrm->pUpdateDlg);
          	delete (g_pAppFrm);
          	delete (g_pApp);
          	delete &g_vHeader;
          	
          	
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}


/******************************************************************************/

// RegisterAccessBar - registers or unregisters an appbar.
// Returns TRUE if successful, or FALSE otherwise.
// hwndAccessBar - handle to the appbar
// fRegister - register and unregister flag
//
// Global variables
//     g_uSide - screen edge (defaults to ABE_TOP)
//     g_fAppRegistered - flag indicating whether the bar is registered

BOOL RegisterAccessBar(HWND hwndAccessBar, BOOL fRegister){
    // Specify the structure size and handle to the appbar.
    APPBARDATA abd;
    abd.cbSize = sizeof(APPBARDATA);
    abd.hWnd = hwndAccessBar;

    if (fRegister) {

        // Provide an identifier for notification messages.
        abd.uCallbackMessage = APPBAR_CALLBACK;

        // Register the appbar.
        if (!SHAppBarMessage(ABM_NEW, &abd))
            return FALSE;
        g_pApp->uTapeScreenSide = ABE_TOP;       // default edge
    } else {
        // Unregister the appbar.
        SHAppBarMessage(ABM_REMOVE, &abd);
    }
    return TRUE;
}

/******************************************************************************/


// AppBarQuerySetPos - sets the size and position of an appbar.
// uEdge - screen edge to which the appbar is to be anchored
// lprc - current bounding rectangle of the appbar
// pabd - address of the APPBARDATA structure with the hWnd and
//     cbSize members filled
//typedef struct _AppBarData {
//    DWORD cbSize;
//    HWND hWnd;
//    UINT uCallbackMessage;
//    UINT uEdge;
//    RECT rc;
//    LPARAM lParam;
//} APPBARDATA, *PAPPBARDATA;

void PASCAL AppBarQuerySetPos(UINT uEdge, LPRECT lprc, PAPPBARDATA pabd)
{
    int iHeight = 0;
    int iWidth = 0;

    pabd->rc = *lprc;
    pabd->uEdge = uEdge;

    // Copy the screen coordinates of the appbar's bounding
    // rectangle into the APPBARDATA structure.
    if ((uEdge == ABE_LEFT) ||
            (uEdge == ABE_RIGHT)) {

        iWidth = pabd->rc.right - pabd->rc.left;
        pabd->rc.top = 0;
        pabd->rc.bottom = GetSystemMetrics(SM_CYSCREEN);

    } else {

        iHeight = pabd->rc.bottom - pabd->rc.top;
        pabd->rc.left = 0;
        pabd->rc.right = GetSystemMetrics(SM_CXSCREEN);

    }

    // Query the system for an approved size and position.
    SHAppBarMessage(ABM_QUERYPOS, pabd);


    // Adjust the rectangle, depending on the edge to which the
    // appbar is anchored.
    switch (uEdge) {

        case ABE_LEFT:
            pabd->rc.right = pabd->rc.left + iWidth;
            break;

        case ABE_RIGHT:
            pabd->rc.left = pabd->rc.right - iWidth;
            break;

        case ABE_TOP:
            pabd->rc.bottom = pabd->rc.top + iHeight;
            break;

        case ABE_BOTTOM:
            pabd->rc.top = pabd->rc.bottom - iHeight;
            break;

    }

    // Pass the final bounding rectangle to the system.
    SHAppBarMessage(ABM_SETPOS, pabd);

    // Move and size the appbar so that it conforms to the
    // bounding rectangle passed to the system.
    MoveWindow(pabd->hWnd, pabd->rc.left, pabd->rc.top,
        pabd->rc.right - pabd->rc.left,
        pabd->rc.bottom - pabd->rc.top, TRUE);

}

/******************************************************************************/

// AppBarCallback - processes notification messages sent by the system.
// hwndAccessBar - handle to the appbar
// uNotifyMsg - identifier of the notification message
// lParam - message parameter

void AppBarCallback(HWND hwndAccessBar, UINT uNotifyMsg,
    LPARAM lParam)
{
    APPBARDATA abd;
    UINT uState;

    abd.cbSize = sizeof(abd);
    abd.hWnd = hwndAccessBar;

    switch (uNotifyMsg) {
        case ABN_STATECHANGE:

            // Check to see if the taskbar's always-on-top state has
            // changed and, if it has, change the appbar's state
            // accordingly.
            uState = SHAppBarMessage(ABM_GETSTATE, &abd);

            SetWindowPos(hwndAccessBar,
                (ABS_ALWAYSONTOP & uState) ? HWND_TOPMOST : HWND_BOTTOM,
                0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

            break;

        case ABN_FULLSCREENAPP:

            // A full-screen application has started, or the last full-
            // screen application has closed. Set the appbar's
            // z-order appropriately.
            if (lParam) {

                SetWindowPos(hwndAccessBar,
                    (ABS_ALWAYSONTOP & uState) ? HWND_TOPMOST : HWND_BOTTOM,
                    0, 0, 0, 0,
                    SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

            }

			else {

                uState = SHAppBarMessage(ABM_GETSTATE, &abd);

                if (uState & ABS_ALWAYSONTOP)
                    SetWindowPos(hwndAccessBar, HWND_TOPMOST,
                        0, 0, 0, 0,
                        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

	        }

        case ABN_POSCHANGED:

            // The taskbar or another appbar has changed its
            // size or position.
            AppBarPosChanged(&abd);
            break;

    }
}

/******************************************************************************/


// AppBarPosChanged - adjusts the appbar's size and position.
// pabd - address of an APPBARDATA structure that contains information
//     used to adjust the size and position

void PASCAL AppBarPosChanged(PAPPBARDATA pabd)
{
    RECT rc;
    RECT rcWindow;
    int iHeight;
    int iWidth;

    rc.top = 0;
    rc.left = 0;
    rc.right = GetSystemMetrics(SM_CXSCREEN);
    rc.bottom = GetSystemMetrics(SM_CYSCREEN);

    GetWindowRect(pabd->hWnd, &rcWindow);
    iHeight = rcWindow.bottom - rcWindow.top;
    iWidth = rcWindow.right - rcWindow.left;

    switch (g_pApp->uTapeScreenSide) {

        case ABE_TOP:
            rc.bottom = rc.top + iHeight;
            break;

        case ABE_BOTTOM:
            rc.top = rc.bottom - iHeight;
            break;

        case ABE_LEFT:
            rc.right = rc.left + iWidth;
            break;

        case ABE_RIGHT:
            rc.left = rc.right - iWidth;
            break;
        }

        AppBarQuerySetPos(g_pApp->uTapeScreenSide, &rc, pabd);
}

/******************************************************************************/

BOOL AboutDlg_OnClose(HWND hwnd) {
  EndDialog(hwnd, 0);
  return TRUE;
}

/******************************************************************************/

BOOL AboutDlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) {
    HWND hImgCanvas;
  	HBITMAP hBmp;
    hImgCanvas = GetDlgItem(hwnd,IDC_STATICIMG);
    hBmp = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_BITMAPABOUT));
    SendMessage(hImgCanvas,(UINT)STM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBmp);
    // Remember to delete bitmap on dialogDestroy to avoid GDI leak
    DeleteObject(hImgCanvas);
    return TRUE;
}

/******************************************************************************/

BOOL AboutDlg_OnDestroy(HWND hwnd) {
    HWND hImgCanvas;
  	HBITMAP hBmp;
    hImgCanvas = GetDlgItem(hwnd,IDC_STATICIMG);
    hBmp = (HBITMAP) SendMessage(hImgCanvas,(UINT)STM_GETIMAGE,(WPARAM)IMAGE_BITMAP,0);
    DeleteObject(hBmp);
    DeleteObject(hImgCanvas);
    return TRUE;
}

/******************************************************************************/

BOOL AboutDlg_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) {
		switch (id) {
        case IDCOK:
            EndDialog (hwnd, 0);
  	        return TRUE;
  	    default:
  	    	  return FALSE;
 		}
}


/******************************************************************************/

BOOL CALLBACK AboutDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {

  int wmId, wmEvent;
  
  switch (message) {
    HANDLE_MSG (hwnd, WM_CLOSE, 			AboutDlg_OnClose);
    HANDLE_MSG (hwnd, WM_INITDIALOG, 	AboutDlg_OnInitDialog);
    HANDLE_MSG (hwnd, WM_DESTROY, 		AboutDlg_OnDestroy);
    HANDLE_MSG (hwnd, WM_COMMAND,			AboutDlg_OnCommand);
   default:
     return FALSE;
  }
}


/******************************************************************************/



BOOL CALLBACK OptionsDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {

  int wmId, wmEvent;
  char str[20];
  DWORD dwPos;    // current position of slider


  switch (message) {
    case WM_INITDIALOG:
      dwPos = (DWORD) (g_pApp->dMsgScrStep*50) ;
      SendMessage(GetDlgItem(hwnd,IDC_TRACKBARSPEED), TBM_SETPOS,(WPARAM) TRUE, (LPARAM) dwPos);
      SetWindowLong (GetDlgItem(hwnd,IDC_BTNBACKGROUND),GWL_WNDPROC,(LONG)ColorSelectBtnProc);
      return TRUE;
    case WM_COMMAND:
    	wmId    = LOWORD(wParam);
      wmEvent = HIWORD(wParam);
      switch (wmId) {
  	      case IDC_BTNOK:
  	      	EndDialog (hwnd, 0);
  	      	return TRUE;
  	      case IDC_BTNCANCEL:
  	      	EndDialog (hwnd, 0);
  	      	return TRUE;
  	      case IDC_BTNBACKGROUND:
           DialogBox((HINSTANCE)GetModuleHandle(NULL), (LPCTSTR)IDD_BACKGROUND, hwnd, (DLGPROC) ColorSelectorProc);
           return TRUE;
  	      default:
           return FALSE;
  	  }
  	  break;
   case WM_HSCROLL:
    	wmId    = GetDlgCtrlID((HWND)lParam);//GetDlgItem(hwnd,lParam);
      wmEvent = LOWORD(wParam);

//      sprintf(str,"%d", wmEvent);
//      eout  << str<< endl;
      switch (wmId) {
      	case IDC_TRACKBARSPEED:
            switch (wmEvent) {
          		case SB_THUMBPOSITION: // scroll “thumb” released
              case SB_THUMBTRACK: // scroll “thumb” pressed
              case SB_PAGEDOWN: // click page right
              case SB_PAGEUP: // click page left
              	  dwPos = SendMessage((HWND)lParam, TBM_GETPOS, 0, 0);
              	  if (dwPos > 100)
              	  	  SendMessage((HWND)lParam, TBM_SETPOS,(WPARAM) TRUE, (LPARAM) 100);
                  else if (dwPos < 0)
                      SendMessage((HWND)lParam, TBM_SETPOS,(WPARAM) TRUE, (LPARAM) 0);
                  dwPos = SendMessage((HWND)lParam, TBM_GETPOS, 0, 0);
                  g_pApp->dMsgScrStep = dwPos*0.02;
                      break;
        	    default:
        	    	return FALSE;
        	  }
        	  break;
        default:
          return FALSE;
      }
      return TRUE;
   default:
     return FALSE;
  }
}



/******************************************************************************/

LRESULT CALLBACK ColorSelectBtnProc (HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam) {

	HDC 				 hdc;
	PAINTSTRUCT  ps;
  RECT         rc;
	HBRUSH			 hBrush;
	SIZE  			 size;
	HFONT        hFont, hOldFont;
	char T[50] = "Tape";
  int XCtr, YCtr, x, y, w, h;


	static bool bPressed;

	switch (msg) {
   case WM_CREATE:
     bPressed = FALSE;
     break;
   case WM_PAINT:
     GetClientRect(hwnd,&rc);
     hdc = BeginPaint(hwnd, &ps);
     DrawEdge(hdc,&ps.rcPaint, (bPressed?EDGE_SUNKEN:EDGE_RAISED),BF_RECT);
     InflateRect(&rc, -2, -2);
     DrawEdge(hdc,&ps.rcPaint, EDGE_SUNKEN,BF_SOFT);
     InflateRect(&rc, -3, -3);
     //DrawFocusRect(hdc,&rc);
     //InflateRect(&rc, -2, -2);

     hBrush = CreateSolidBrush(g_pApp->lBgColor);
     FillRect(hdc,&rc,hBrush);
     //FrameRect(hdc,&ps.rcPaint, (HBRUSH) GetStockObject(BLACK_BRUSH));
     x=rc.left;
	   y=rc.top;
		 w=rc.right-rc.left;
		 h=rc.bottom-rc.top;
		 XCtr=(rc.left+rc.right)/2;
		 YCtr=(rc.top+rc.bottom)/2;
		 GetTextExtentPoint32(hdc,T,strlen(T),&size);
     SetBkMode(hdc,TRANSPARENT);
/*  	 hFont = CreateFont(10, 6, 0, 0,
                           FW_BOLD, FALSE, FALSE, FALSE,
                           ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                           CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                           OUT_OUTLINE_PRECIS | FF_ROMAN,
                           "Tahoma");

      hOldFont = (HFONT) SelectObject(hdc, hFont); // store old font
*/
      SetTextColor(hdc,(~g_pApp->lBgColor)&0x00ffffff);
				TextOut(hdc,XCtr-(size.cx/2)+1,YCtr-(size.cy/2),T,strlen(T));



     EndPaint(hwnd, &ps);
//     SelectObject(hdc, hOldFont); // restore old font
//   	 DeleteObject(hFont);
     DeleteObject(hBrush);
     return 0;
   case WM_LBUTTONDOWN:
     bPressed = TRUE;
     InvalidateRect(hwnd,NULL, TRUE);
     break;
   case WM_LBUTTONUP:
     bPressed = FALSE;
     InvalidateRect(hwnd,NULL, TRUE);
 		 DialogBox((HINSTANCE)GetModuleHandle(NULL), (LPCTSTR)IDD_BACKGROUND, hwnd, (DLGPROC) ColorSelectorProc);
     break;
   default:
     return DefWindowProc (hwnd, msg, wParam, lParam);
   }
}

/*******************************************************************************/


LRESULT CALLBACK ColorSelectorProc(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		int nColor;
		for (nColor = 0; nColor < sizeof(g_crItems)/sizeof(DWORD); nColor++)
		{
			SendDlgItemMessage(hdlg, IDC_BACKGROUND, LB_ADDSTRING, nColor, (LPARAM) "");
			SendDlgItemMessage(hdlg, IDC_BACKGROUND, LB_SETITEMDATA , nColor, (LPARAM) g_crItems[nColor]);
			if (g_pApp->lBgColor == g_crItems[nColor])
				SendDlgItemMessage(hdlg, IDC_BACKGROUND, LB_SETCURSEL, nColor, 0);
		}
		return TRUE;

	case WM_MEASUREITEM:
		RECT rc;
		LPMEASUREITEMSTRUCT lpmis;
		lpmis = (LPMEASUREITEMSTRUCT) lParam;
		GetWindowRect(GetDlgItem(hdlg, lpmis->CtlID), &rc);
		lpmis->itemHeight = (rc.bottom-rc.top)/6;
		lpmis->itemWidth = (rc.right-rc.left)/8;
		break;

	case WM_CTLCOLORLISTBOX:
		return (LRESULT) CreateSolidBrush(GetSysColor(COLOR_3DFACE));

	case WM_DRAWITEM:
		HDC hdc;
		COLORREF	cr;
		HBRUSH		hbrush;

		DRAWITEMSTRUCT * pdis;

		pdis = (DRAWITEMSTRUCT *)lParam;
		hdc = pdis->hDC;
		rc = pdis->rcItem;

		// Transparent.
		SetBkMode(hdc,TRANSPARENT);

		// NULL object
		if (pdis->itemID == -1) return 0;

		switch (pdis->itemAction)
		{
		case ODA_DRAWENTIRE:
			switch (pdis->CtlID)
			{
			case IDC_BACKGROUND:
				rc = pdis->rcItem;
				cr = (COLORREF) pdis->itemData;
				InflateRect(&rc, -3, -3);
				hbrush = CreateSolidBrush((COLORREF)cr);
				FillRect(hdc, &rc, hbrush);
				DeleteObject(hbrush);
				FrameRect(hdc, &rc, (HBRUSH) GetStockObject(GRAY_BRUSH));
				break;
			}
			// *** FALL THROUGH ***
		case ODA_SELECT:
			rc = pdis->rcItem;
			if (pdis->itemState & ODS_SELECTED)
			{
				rc.bottom --;
				rc.right --;
				// Draw the lighted side.
				HPEN hpen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNSHADOW));
				HPEN holdPen = (HPEN)SelectObject(hdc, hpen);
				MoveToEx(hdc, rc.left, rc.bottom, NULL);
				LineTo(hdc, rc.left, rc.top);
				LineTo(hdc, rc.right, rc.top);
				SelectObject(hdc, holdPen);
				DeleteObject(hpen);
				// Draw the darkened side.
				hpen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNHIGHLIGHT));
				holdPen = (HPEN)SelectObject(hdc, hpen);
				LineTo(hdc, rc.right, rc.bottom);
				LineTo(hdc, rc.left, rc.bottom);
				SelectObject(hdc, holdPen);
				DeleteObject(hpen);
			}
			else
			{
				hbrush = CreateSolidBrush(GetSysColor(COLOR_3DFACE));
				FrameRect(hdc, &rc, hbrush);
				DeleteObject(hbrush);
			}
			break;
		case ODA_FOCUS:
			rc = pdis->rcItem;
			InflateRect(&rc, -2, -2);
			DrawFocusRect(hdc, &rc);
			break;
		default:
			break;
		}
		return true;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			int nItem;
			nItem = SendDlgItemMessage(hdlg, IDC_BACKGROUND, LB_GETCURSEL, 0, 0L);
			g_pApp->lBgColor = SendDlgItemMessage(hdlg, IDC_BACKGROUND, LB_GETITEMDATA, nItem, 0L);
			EndDialog(hdlg, LOWORD(wParam));
			InvalidateRect(GetParent(hdlg), NULL, true);
			return TRUE;
		}
		else if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hdlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
    return FALSE;
}



/******************************************************************************/

BOOL AccessDlg_OnClose(HWND hwnd) {
  EndDialog(hwnd, 0);
  return TRUE;
}

/******************************************************************************/

BOOL AccessDlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) {
  	HBITMAP hBmp;
    hBmp = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_NO));
    SendDlgItemMessage(hwnd, IDC_IMG1, (UINT)STM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBmp);    return TRUE;
    return TRUE;
}

/******************************************************************************/

BOOL AccessDlg_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) {
		switch (id) {
        case IDC_OK:
            EndDialog (hwnd, 0);
  	        return TRUE;
  	    default:
  	    	  return FALSE;
 		}
}

/******************************************************************************/

BOOL AccessDlg_OnDestroy(HWND hwnd) {
    HWND hImgCanvas;
  	HBITMAP hBmp;
    hImgCanvas = GetDlgItem(hwnd,IDC_IMG1);
    hBmp = (HBITMAP) SendMessage(hImgCanvas,(UINT)STM_GETIMAGE,(WPARAM)IDC_IMG1,0);
    DeleteObject(hBmp);
    DeleteObject(hImgCanvas);
    return TRUE;
}

/******************************************************************************/

BOOL CALLBACK AccessDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	switch (message){
    HANDLE_MSG (hwnd, WM_INITDIALOG,    AccessDlg_OnInitDialog);
    case WM_LBUTTONDBLCLK:
        HBITMAP hBmp;
        HWND hImgCanvas;
		    hBmp = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_NO2));
		    SendDlgItemMessage(hwnd, IDC_IMG1, (UINT)STM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBmp);
		    Sleep(500);
		    hBmp = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_NO));
		    SendDlgItemMessage(hwnd, IDC_IMG1, (UINT)STM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBmp);
		    return TRUE;
    HANDLE_MSG (hwnd, WM_COMMAND,    AccessDlg_OnCommand);
    HANDLE_MSG (hwnd, WM_DESTROY,    AccessDlg_OnDestroy);
	}
	return FALSE;
}

/******************************************************************************/
//
//	EDITMSGSDLG DIALOG
//
/******************************************************************************/



BOOL EditMsgsDlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) {

		std::vector<CMessage>::iterator itSrc;
		
		
		ReadFromFile(g_pApp->szMsgFilepath);
    // replicate messages to g_vTmpMessages
    g_vTmpMessages.clear();
	  for (itSrc = g_vMessages.begin(); itSrc != g_vMessages.end(); itSrc++) {
        //g_vTmpMessages.push_back(*(new CMessage(*itSrc)));
        g_vTmpMessages.push_back(*itSrc);
    }
    
		CheckDlgButton(hwnd, IDC_CHKTIME, g_pAppFrm->pEditMsgsDlg->_IDC_CHKTIME);
  	if (BST_CHECKED == g_pAppFrm->pEditMsgsDlg->_IDC_CHKTIME) {
     		g_pAppFrm->pEditMsgsDlg->_IDC_EDTTIME = GetCurrentTimeStrShort();
  			Edit_Enable(GetDlgItem(hwnd, IDC_EDTTIME), (BST_CHECKED == g_pAppFrm->pEditMsgsDlg->_IDC_CHKTIME)?FALSE:TRUE);
		} 
		
		SetDlgItemText(hwnd, IDC_EDTTIME, g_pAppFrm->pEditMsgsDlg->_IDC_EDTTIME.c_str());
		CheckDlgButton(hwnd, IDC_CHKNMC, g_pAppFrm->pEditMsgsDlg->_IDC_CHKNMC);
		SetDlgItemText(hwnd, IDC_EDTNMC, g_pAppFrm->pEditMsgsDlg->_IDC_EDTNMC.c_str());
		Edit_Enable(GetDlgItem(hwnd, IDC_EDTNMC), (BST_CHECKED == g_pAppFrm->pEditMsgsDlg->_IDC_CHKNMC)?FALSE:TRUE);

		EditMsgsDlg_UpdatePrewiew(hwnd);

		CheckDlgButton(hwnd, IDC_RBNOUTAGESNO, g_pAppFrm->pEditMsgsDlg->_IDC_RBNOUTAGESNO);
		CheckDlgButton(hwnd, IDC_RBNOUTAGESYES, g_pAppFrm->pEditMsgsDlg->_IDC_RBNOUTAGESYES);
		
  	Button_Enable(GetDlgItem(hwnd, IDC_EDIT), (BST_CHECKED == g_pAppFrm->pEditMsgsDlg->_IDC_RBNOUTAGESNO)?FALSE:TRUE);
  	Button_Enable(GetDlgItem(hwnd, IDC_NEW), (BST_CHECKED == g_pAppFrm->pEditMsgsDlg->_IDC_RBNOUTAGESNO)?FALSE:TRUE);
  	Button_Enable(GetDlgItem(hwnd, IDC_DELETE), (BST_CHECKED == g_pAppFrm->pEditMsgsDlg->_IDC_RBNOUTAGESNO)?FALSE:TRUE);
  	Edit_Enable(GetDlgItem(hwnd, IDC_EDTNOOUTAGES), (BST_CHECKED == g_pAppFrm->pEditMsgsDlg->_IDC_RBNOUTAGESNO)?TRUE:FALSE);
  	SetDlgItemText(hwnd, IDC_EDTNOOUTAGES, g_pAppFrm->pEditMsgsDlg->_IDC_EDTNOOUTAGES.c_str());
    SendMessage(hwnd, IDC_LSTOUTAGES_REFRESH,0,0);
    ListBox_Enable(GetDlgItem(hwnd, IDC_LSTOUTAGES), (BST_CHECKED == g_pAppFrm->pEditMsgsDlg->_IDC_RBNOUTAGESNO)?FALSE:TRUE);
    return TRUE;
}

BOOL EditMsgsDlg_OnClose(HWND hwnd) {

    EndDialog(hwnd, 0);
    return TRUE;
}

/******************************************************************************/

BOOL EditMsgsDlg_OnDestroy(HWND hwnd) {

    return TRUE;
}

/******************************************************************************/

BOOL EditMsgsDlg_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) {

		LRESULT checkState;
		TCHAR szBuffer[MAX_LOADSTRING];
		int nCurrSel;
		int nCounter;
		int ret;
		

		std::vector<CUpdate>::iterator itUpd;
		std::vector<CMessage>::iterator itSrc;
		
		switch (id) {
        case IDC_CANCEL:
          		EndDialog (hwnd, 0);
          		return TRUE;
		    case IDC_SAVEOK:
	   				//copy temp g_vTmpMessages to g_vMessages
	   				g_vMessages.clear();
	   				for (itSrc = g_vTmpMessages.begin(); itSrc != g_vTmpMessages.end(); itSrc++)
    		  	   		g_vMessages.push_back(*itSrc);
    		  	   		//g_vMessages.push_back(*(new CMessage(*itSrc)));
    		  	
         		g_pAppFrm->pEditMsgsDlg->_IDC_CHKTIME = SendDlgItemMessage(hwnd,IDC_CHKTIME,BM_GETCHECK,0,0);
            g_pAppFrm->pEditMsgsDlg->_IDC_CHKNMC = SendDlgItemMessage(hwnd,IDC_CHKNMC,BM_GETCHECK,0,0);
            if (g_vMessages.empty()) {
                g_pAppFrm->pEditMsgsDlg->_IDC_RBNOUTAGESNO = BST_CHECKED;
                g_pAppFrm->pEditMsgsDlg->_IDC_RBNOUTAGESYES = BST_UNCHECKED;
            } else {
                g_pAppFrm->pEditMsgsDlg->_IDC_RBNOUTAGESNO = BST_UNCHECKED;
                g_pAppFrm->pEditMsgsDlg->_IDC_RBNOUTAGESYES = BST_CHECKED;
            }
       
            GetDlgItemText(hwnd, IDC_EDTNMC, szBuffer, sizeof(szBuffer)/sizeof(szBuffer[0]));
            g_pAppFrm->pEditMsgsDlg->_IDC_EDTNMC = szBuffer;
            GetDlgItemText(hwnd, IDC_EDTTIME, szBuffer, sizeof(szBuffer)/sizeof(szBuffer[0]));
            g_pAppFrm->pEditMsgsDlg->_IDC_EDTTIME = szBuffer;
            GetDlgItemText(hwnd, IDC_EDTNOOUTAGES, szBuffer, sizeof(szBuffer)/sizeof(szBuffer[0]));
            g_pAppFrm->pEditMsgsDlg->_IDC_EDTNOOUTAGES = szBuffer;

            //update header
            g_vHeader.sStart = g_pAppFrm->pEditMsgsDlg->_IDC_EDTTIME;
            g_vHeader.sNote = g_pAppFrm->pEditMsgsDlg->_IDC_EDTNMC;
            
            // if 'no outages' is selected clear messages and save file
            if (BST_CHECKED == g_pAppFrm->pEditMsgsDlg->_IDC_RBNOUTAGESNO) {
              	g_vMessages.clear();
              	g_vTmpMessages.clear();
            }
            // write messages to file
            SaveToFile(g_pApp->szMsgFilepath);
            EndDialog (hwnd, 0);
  	        return TRUE;
  	    case IDC_NEW:
    	    	ret = DialogBoxParam((HINSTANCE)GetModuleHandle(NULL), (LPCTSTR)IDD_UPDATE, hwnd, (DLGPROC) UpdateDlgProc, (LPARAM) -1);
    	    	if (1 == ret) {
            	  SendMessage(hwnd, IDC_LSTOUTAGES_REFRESH,0, (LPARAM) g_pAppFrm->pUpdateDlg->uRecordId);
            } else {
                //operation cancelled
                // reload g_vTmpMessages with g_vMessages
  	   				  g_vTmpMessages.clear();
	   				    for (itSrc = g_vMessages.begin(); itSrc != g_vMessages.end(); itSrc++)
    		  	   		  g_vTmpMessages.push_back(*itSrc);
    		  	   		  //g_vTmpMessages.push_back(*(new CMessage(*itSrc)));
            }
            return TRUE;
        case IDC_EDIT:
            ret = DialogBoxParam((HINSTANCE)GetModuleHandle(NULL), (LPCTSTR)IDD_UPDATE, hwnd, (DLGPROC) UpdateDlgProc, (LPARAM) ListBox_GetCurSel(GetDlgItem(hwnd, IDC_LSTOUTAGES)));
            if (1 == ret) {
                SendMessage(hwnd, IDC_LSTOUTAGES_REFRESH,0, (LPARAM) g_pAppFrm->pUpdateDlg->uRecordId);
            } else {
                //operation cancelled
                // reload g_vTmpMessages with g_vMessages
  	   				  g_vTmpMessages.clear();
	   				    for (itSrc = g_vMessages.begin(); itSrc != g_vMessages.end(); itSrc++)
    		  	   		  g_vTmpMessages.push_back(*itSrc);
    		  	   		  //g_vTmpMessages.push_back(*(new CMessage(*itSrc)));
            }
            return TRUE;
        case IDC_DELETE:
          	nCurrSel = ListBox_GetCurSel(GetDlgItem(hwnd, IDC_LSTOUTAGES));
          	if (nCurrSel>-1) {
              g_vTmpMessages.erase(g_vTmpMessages.begin()+nCurrSel);
             	//ListBox_DeleteString(GetDlgItem(hwnd, IDC_LSTOUTAGES),nCurrSel);
             	g_pAppFrm->pUpdateDlg->uRecordId = nCurrSel-1;
              SendMessage(hwnd, IDC_LSTOUTAGES_REFRESH,0, (LPARAM) g_pAppFrm->pUpdateDlg->uRecordId);
            }
            return TRUE;
  	    case IDC_CHKTIME:
         		if(codeNotify == BN_CLICKED) {
         			checkState = SendDlgItemMessage(hwnd,IDC_CHKTIME,BM_GETCHECK,0,0);
         			//g_pAppFrm->pEditMsgsDlg->_IDC_CHKTIME = (BST_CHECKED == checkState?BST_UNCHECKED:BST_CHECKED);
            	if (BST_CHECKED == checkState) {
             			SetDlgItemText(hwnd, IDC_EDTTIME, (GetCurrentTimeStrShort()).c_str());
             	}
         			Edit_Enable(GetDlgItem(hwnd, IDC_EDTTIME), (BST_CHECKED == checkState)?FALSE:TRUE);
         			return TRUE;
         	  } else {
              return FALSE;
            }
  	    case IDC_CHKNMC:
         		if(codeNotify == BN_CLICKED) {
              checkState = SendDlgItemMessage(hwnd,IDC_CHKNMC,BM_GETCHECK,0,0);
              g_pAppFrm->pEditMsgsDlg->_IDC_CHKNMC = (BST_CHECKED == checkState?BST_UNCHECKED:BST_CHECKED);
              if (BST_CHECKED == checkState) {
              		LoadString(GetModuleHandle(NULL), IDS_NMC, szBuffer, sizeof(szBuffer)/sizeof(szBuffer[0]));
              		SetDlgItemText(hwnd, IDC_EDTNMC, szBuffer);
              }
         			Edit_Enable(GetDlgItem(hwnd, IDC_EDTNMC), (BST_CHECKED == checkState)?FALSE:TRUE);
         			return TRUE;
         	  } else {
              return FALSE;
            }
        case IDC_EDTNMC:
          	if(codeNotify == EN_CHANGE) {
          		EditMsgsDlg_UpdatePrewiew(hwnd);
          		return TRUE;
            } else {
            	return FALSE;
            }
        case IDC_EDTTIME:
          	if(codeNotify == EN_CHANGE) {
          		EditMsgsDlg_UpdatePrewiew(hwnd);
          		return TRUE;
            } else {
            	return FALSE;
            }
        case IDC_RBNOUTAGESNO:
        		if (codeNotify == BN_CLICKED) {
            		checkState = SendDlgItemMessage(hwnd, IDC_RBNOUTAGESNO,BM_GETCHECK,0,0);
           			if (BST_CHECKED == checkState) {
                    	Edit_Enable(GetDlgItem(hwnd, IDC_EDTNOOUTAGES), TRUE);
                    	ListBox_Enable(GetDlgItem(hwnd, IDC_LSTOUTAGES), FALSE);
                    	Button_Enable(GetDlgItem(hwnd, IDC_EDIT), FALSE);
                    	Button_Enable(GetDlgItem(hwnd, IDC_NEW), FALSE);
                    	Button_Enable(GetDlgItem(hwnd, IDC_DELETE), FALSE);
                    	
                    	SetDlgItemText(hwnd, IDC_EDTNOOUTAGES, g_pAppFrm->pEditMsgsDlg->_IDC_EDTNOOUTAGES.c_str());
                    	
                }
                return TRUE;
            } else {
             		return FALSE;
            }
        case IDC_RBNOUTAGESYES:
        		if (codeNotify == BN_CLICKED) {
            		checkState = SendDlgItemMessage(hwnd, IDC_RBNOUTAGESYES,BM_GETCHECK,0,0);
           			if (BST_CHECKED == checkState) {
                    	Edit_Enable(GetDlgItem(hwnd, IDC_EDTNOOUTAGES), FALSE);
                    	ListBox_Enable(GetDlgItem(hwnd, IDC_LSTOUTAGES), TRUE);
                    	Button_Enable(GetDlgItem(hwnd, IDC_EDIT), TRUE);
                    	Button_Enable(GetDlgItem(hwnd, IDC_NEW), TRUE);
                    	Button_Enable(GetDlgItem(hwnd, IDC_DELETE), TRUE);
                    	
                    	LoadString(GetModuleHandle(NULL), IDS_OUTAGE, szBuffer, sizeof(szBuffer)/sizeof(szBuffer[0]));
                    	SetDlgItemText(hwnd, IDC_EDTNOOUTAGES, szBuffer);
                    	g_pAppFrm->pEditMsgsDlg->_IDC_EDTNOOUTAGES = szBuffer;
                }
                return TRUE;
            } else {
             		return FALSE;
            }
        case IDC_LSTOUTAGES:
     	      if (LBN_DBLCLK == LOWORD(codeNotify)) {
     	      		SendMessage(GetDlgItem(hwnd, IDC_EDIT), WM_LBUTTONDOWN, (WPARAM) 0, (LPARAM) 0 );
     	      		SendMessage(GetDlgItem(hwnd, IDC_EDIT), WM_LBUTTONUP, (WPARAM) 0, (LPARAM) 0 );
     	      		return TRUE;
     	      }
            return FALSE;
  	    default:
  	    	  return FALSE;
 		}
}






/******************************************************************************/

BOOL CALLBACK EditMsgsDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){

	std::vector<CMessage>::iterator it;
	int tpos;
	
	switch (message){
    HANDLE_MSG (hwnd, WM_INITDIALOG, 	EditMsgsDlg_OnInitDialog);
    HANDLE_MSG (hwnd, WM_CLOSE, 			EditMsgsDlg_OnClose);
    HANDLE_MSG (hwnd, WM_DESTROY, 		EditMsgsDlg_OnDestroy);
    HANDLE_MSG (hwnd, WM_COMMAND,			EditMsgsDlg_OnCommand);
    
    // process custom messages
    case IDC_LSTOUTAGES_REFRESH:
      	 ListBox_ResetContent(GetDlgItem(hwnd, IDC_LSTOUTAGES));
      	 for (it = g_vTmpMessages.begin(); it != g_vTmpMessages.end(); it++) {
      	 			ListBox_AddString(GetDlgItem(hwnd, IDC_LSTOUTAGES),  (*it).pUpdate->toStringShort().c_str());
         }
      	 SetFocus(GetDlgItem(hwnd, IDC_LSTOUTAGES));
      	 ListBox_SetCurSel(GetDlgItem(hwnd, IDC_LSTOUTAGES), lParam);
      	 //char str[100];
      	 //sprintf(str,"%l", lParam);
         //MessageBox(NULL,str , "",0);
         //if (InSendMessage()) ReplyMessage(TRUE);
         return TRUE;
    }
	return FALSE;
}

/******************************************************************************/

VOID EditMsgsDlg_UpdatePrewiew(HWND hwnd) {
		std::string s;
		char szTime[80];
		char szNMC[80];
		GetDlgItemText(hwnd, IDC_EDTTIME, szTime, 80);
		GetDlgItemText(hwnd, IDC_EDTNMC, szNMC, 80);
  	s =  szTime;
    s += szNMC;
		Static_SetText(GetDlgItem(hwnd, IDC_PREVIEW), s.c_str());
}


/******************************************************************************/

void UpdateDlg_Obj2Global(CUpdate* upd) {
  		g_pAppFrm->pUpdateDlg->_IDC_RBNONGOING = upd->bRestored?BST_UNCHECKED:BST_CHECKED;
   		g_pAppFrm->pUpdateDlg->_IDC_RBNRESTORED = upd->bRestored?BST_CHECKED:BST_UNCHECKED;
   		g_pAppFrm->pUpdateDlg->__ENABLE_IDC_EDTRESTORED = BST_CHECKED == upd->bRestored;
   		g_pAppFrm->pUpdateDlg->_IDC_CHKETA = upd->bShowEta?BST_CHECKED:BST_UNCHECKED;
   		g_pAppFrm->pUpdateDlg->_IDC_CHKETR = upd->bShowEtr?BST_CHECKED:BST_UNCHECKED;
   		g_pAppFrm->pUpdateDlg->_IDC_EDTSTART = upd->sStart.c_str();
   		g_pAppFrm->pUpdateDlg->_IDC_EDTRESTORED = upd->sRestored.c_str();
   		g_pAppFrm->pUpdateDlg->_IDC_EDTETA = upd->sEta;
   		g_pAppFrm->pUpdateDlg->_IDC_EDTETR = upd->sEtr;
   		g_pAppFrm->pUpdateDlg->_IDC_EDTUPDATE = upd->sNote.c_str();
   		g_pAppFrm->pUpdateDlg->_IDC_EDTTT = upd->sTicket.c_str();
}

void UpdateDlg_Global2Obj(CUpdate* upd) {
  		upd->bRestored = g_pAppFrm->pUpdateDlg->_IDC_RBNONGOING==BST_UNCHECKED;
   		upd->bRestored = g_pAppFrm->pUpdateDlg->_IDC_RBNRESTORED==BST_CHECKED;
   		upd->bShowEta = g_pAppFrm->pUpdateDlg->_IDC_CHKETA==BST_CHECKED;
   		upd->bShowEtr = g_pAppFrm->pUpdateDlg->_IDC_CHKETR==BST_CHECKED;
   		upd->sStart = g_pAppFrm->pUpdateDlg->_IDC_EDTSTART;
   		upd->sRestored = g_pAppFrm->pUpdateDlg->_IDC_EDTRESTORED;
   		upd->sEta = g_pAppFrm->pUpdateDlg->_IDC_EDTETA;
   		upd->sEtr = g_pAppFrm->pUpdateDlg->_IDC_EDTETR;
   		upd->sNote = g_pAppFrm->pUpdateDlg->_IDC_EDTUPDATE;
   		upd->sTicket = g_pAppFrm->pUpdateDlg->_IDC_EDTTT;
}

/******************************************************************************/

BOOL UpdateDlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) {

	TCHAR szBuffer[MAX_LOADSTRING];
  CUpdate* upd;
  std::vector<CMessage>::iterator itSrc;
  
  // copy all updates to g_vTmpMessages
  
//  g_vTmpMessages.clear();
//  for (itSrc = g_vMessages.begin(); itSrc != g_vMessages.end(); itSrc++)
//  	g_vTmpMessages.push_back(*itSrc);
  
  
	if (lParam == -1) {
    // add new record
    SetWindowText(hwnd, "New Entry");
    LoadString(GetModuleHandle(NULL), IDS_ONGOING, szBuffer, sizeof(szBuffer)/sizeof(szBuffer[0]));
		upd = new CUpdate;
		upd->sRestored = szBuffer;
   	UpdateDlg_Obj2Global(upd); // populate form globals
   	delete upd;
  } else {
    //load record from vector
    SetWindowText(hwnd, "Update Entry");
    CUpdate* upd = g_vTmpMessages.at(lParam).pUpdate;
    g_pAppFrm->pUpdateDlg->uRecordId = lParam; // store selected record  pos
    UpdateDlg_Obj2Global(upd); // populate form globals
  }
  
  
  // initialize form
	CheckDlgButton(hwnd, IDC_RBNONGOING, g_pAppFrm->pUpdateDlg->_IDC_RBNONGOING);
	CheckDlgButton(hwnd, IDC_RBNRESTORED, g_pAppFrm->pUpdateDlg->_IDC_RBNRESTORED);
	CheckDlgButton(hwnd, IDC_CHKETA, g_pAppFrm->pUpdateDlg->_IDC_CHKETA);
	CheckDlgButton(hwnd, IDC_CHKETR, g_pAppFrm->pUpdateDlg->_IDC_CHKETR);
	
  Edit_Enable(GetDlgItem(hwnd, IDC_EDTETA), (BST_CHECKED == g_pAppFrm->pUpdateDlg->_IDC_RBNRESTORED)?FALSE:TRUE);
  Edit_Enable(GetDlgItem(hwnd, IDC_EDTETR), (BST_CHECKED == g_pAppFrm->pUpdateDlg->_IDC_RBNRESTORED)?FALSE:TRUE);
  Button_Enable(GetDlgItem(hwnd, IDC_CHKETA), (g_pAppFrm->pUpdateDlg->_IDC_RBNRESTORED)?FALSE:TRUE);
  Button_Enable(GetDlgItem(hwnd, IDC_CHKETR), (g_pAppFrm->pUpdateDlg->_IDC_RBNRESTORED)?FALSE:TRUE);


	SetDlgItemText(hwnd, IDC_EDTSTART, g_pAppFrm->pUpdateDlg->_IDC_EDTSTART.c_str());
	SetDlgItemText(hwnd, IDC_EDTRESTORED, g_pAppFrm->pUpdateDlg->_IDC_EDTRESTORED.c_str());
	Edit_Enable(GetDlgItem(hwnd, IDC_EDTRESTORED), g_pAppFrm->pUpdateDlg->__ENABLE_IDC_EDTRESTORED);
  SetDlgItemText(hwnd, IDC_EDTETA, g_pAppFrm->pUpdateDlg->_IDC_EDTETA.c_str());
  SetDlgItemText(hwnd, IDC_EDTETR, g_pAppFrm->pUpdateDlg->_IDC_EDTETR.c_str());
	SetDlgItemText(hwnd, IDC_EDTUPDATE, g_pAppFrm->pUpdateDlg->_IDC_EDTUPDATE.c_str());
	SetDlgItemText(hwnd, IDC_EDTTT, g_pAppFrm->pUpdateDlg->_IDC_EDTTT.c_str());

  return TRUE;
}

/******************************************************************************/
BOOL UpdateDlg_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) {

	LRESULT checkState;
	TCHAR szBuffer[MAX_LOADSTRING];
	TCHAR szNoteBuffer[MAX_LOADSTRING * 4];
	CMessage* pMsg;
	CUpdate* pUpd;
	std::vector<CMessage>::iterator itSrc;
	std::string errMsg;
	
	
	switch (id) {
   	case IDC_RBNONGOING:
			if(codeNotify == BN_CLICKED) {
     		checkState = SendDlgItemMessage(hwnd,IDC_RBNRESTORED,BM_GETCHECK,0,0);
     		LoadString(GetModuleHandle(NULL), IDS_ONGOING, szBuffer, sizeof(szBuffer)/sizeof(szBuffer[0]));
     		SetDlgItemText(hwnd, IDC_EDTRESTORED, szBuffer);
      	Edit_Enable(GetDlgItem(hwnd, IDC_EDTRESTORED), (BST_CHECKED == checkState)?TRUE:FALSE);
      	Edit_Enable(GetDlgItem(hwnd, IDC_EDTETA), (BST_CHECKED == checkState)?FALSE:TRUE);
      	Edit_Enable(GetDlgItem(hwnd, IDC_EDTETR), (BST_CHECKED == checkState)?FALSE:TRUE);
      	SetDlgItemText(hwnd, IDC_EDTETA, g_pAppFrm->pUpdateDlg->_IDC_EDTETA.c_str());
      	SetDlgItemText(hwnd, IDC_EDTETR, g_pAppFrm->pUpdateDlg->_IDC_EDTETR.c_str());
      	CheckDlgButton(hwnd, IDC_CHKETA, g_pAppFrm->pUpdateDlg->_IDC_CHKETA);
      	CheckDlgButton(hwnd, IDC_CHKETR, g_pAppFrm->pUpdateDlg->_IDC_CHKETR);
      	Button_Enable(GetDlgItem(hwnd, IDC_CHKETA), !checkState);
      	Button_Enable(GetDlgItem(hwnd, IDC_CHKETR), !checkState);
      	return TRUE;
      } else {
        return FALSE;
      }
		case IDC_RBNRESTORED:
			if(codeNotify == BN_CLICKED) {
     		checkState = SendDlgItemMessage(hwnd,IDC_RBNRESTORED,BM_GETCHECK,0,0);
      	Edit_Enable(GetDlgItem(hwnd, IDC_EDTRESTORED), (BST_CHECKED == checkState)?TRUE:FALSE);
      	SetDlgItemText(hwnd, IDC_EDTRESTORED, "");
      	Edit_Enable(GetDlgItem(hwnd, IDC_EDTETA), (BST_CHECKED == checkState)?FALSE:TRUE);
      	Edit_Enable(GetDlgItem(hwnd, IDC_EDTETR), (BST_CHECKED == checkState)?FALSE:TRUE);
      	SetDlgItemText(hwnd, IDC_EDTETA, "");
      	SetDlgItemText(hwnd, IDC_EDTETR, "");
      	CheckDlgButton(hwnd, IDC_CHKETA, (BST_CHECKED == checkState)?FALSE:TRUE);
      	CheckDlgButton(hwnd, IDC_CHKETR, (BST_CHECKED == checkState)?FALSE:TRUE);
      	Button_Enable(GetDlgItem(hwnd, IDC_CHKETA), !checkState);
      	Button_Enable(GetDlgItem(hwnd, IDC_CHKETR), !checkState);
      	return TRUE;
      } else {
        return FALSE;
      }
    case IDC_BTNOK:
    	g_pAppFrm->pUpdateDlg->_IDC_RBNONGOING = SendDlgItemMessage(hwnd,IDC_RBNONGOING,BM_GETCHECK,0,0);
    	g_pAppFrm->pUpdateDlg->_IDC_RBNRESTORED = SendDlgItemMessage(hwnd,IDC_RBNRESTORED,BM_GETCHECK,0,0);
    	g_pAppFrm->pUpdateDlg->_IDC_CHKETA = SendDlgItemMessage(hwnd,IDC_CHKETA,BM_GETCHECK,0,0);
    	g_pAppFrm->pUpdateDlg->_IDC_CHKETR = SendDlgItemMessage(hwnd,IDC_CHKETR,BM_GETCHECK,0,0);
      GetDlgItemText(hwnd, IDC_EDTETA, szBuffer, sizeof(szBuffer)/sizeof(szBuffer[0]));
      g_pAppFrm->pUpdateDlg->_IDC_EDTETA = szBuffer;
      GetDlgItemText(hwnd, IDC_EDTETR, szBuffer, sizeof(szBuffer)/sizeof(szBuffer[0]));
      g_pAppFrm->pUpdateDlg->_IDC_EDTETR = szBuffer;
      GetDlgItemText(hwnd, IDC_EDTUPDATE, szNoteBuffer, sizeof(szNoteBuffer)/sizeof(szNoteBuffer[0]));
      g_pAppFrm->pUpdateDlg->_IDC_EDTUPDATE = szNoteBuffer;
      GetDlgItemText(hwnd, IDC_EDTSTART, szBuffer, sizeof(szBuffer)/sizeof(szBuffer[0]));
      g_pAppFrm->pUpdateDlg->_IDC_EDTSTART = szBuffer;
      GetDlgItemText(hwnd, IDC_EDTRESTORED, szBuffer, sizeof(szBuffer)/sizeof(szBuffer[0]));
      g_pAppFrm->pUpdateDlg->_IDC_EDTRESTORED = szBuffer;
      GetDlgItemText(hwnd, IDC_EDTTT, szBuffer, sizeof(szBuffer)/sizeof(szBuffer[0]));
      g_pAppFrm->pUpdateDlg->_IDC_EDTTT = szBuffer;
      errMsg = "";
      // validation
      if ((BST_CHECKED == g_pAppFrm->pUpdateDlg->_IDC_RBNRESTORED) &&
      		(g_pAppFrm->pUpdateDlg->_IDC_EDTRESTORED.size() == 0))
      		errMsg = "- invalid \"Restored Time\"\n";
      if ((BST_CHECKED == g_pAppFrm->pUpdateDlg->_IDC_CHKETA) &&
      		(g_pAppFrm->pUpdateDlg->_IDC_EDTETA.size() == 0))
      		errMsg = "- invalid \"ETA Time\"\n";
      if ((BST_CHECKED == g_pAppFrm->pUpdateDlg->_IDC_CHKETR) &&
      		(g_pAppFrm->pUpdateDlg->_IDC_EDTETR.size() == 0))
      		errMsg = "- invalid \"ETR Time\"\n";
      
      if (errMsg.size() > 0) {
      		errMsg = "Following error(s) occured:\t\t\n\n" + errMsg;
      		errMsg+= "\nPlease correct and try again";
      		MessageBox(hwnd, errMsg.c_str(), "Error", MB_ICONERROR | MB_OK | MB_TOPMOST | MB_APPLMODAL | MB_SETFOREGROUND);
      		return TRUE;
      }
      
      GetWindowText(hwnd, szBuffer, sizeof(szBuffer)/sizeof(szBuffer[0]));
      if (strcmp(szBuffer, "New Entry") == 0) {
        // add new message
      		pMsg = new CMessage();
      		pMsg->oRgb =g_crItems[2];
      		UpdateDlg_Global2Obj(pMsg->pUpdate);
      		g_vTmpMessages.push_back(*pMsg);
      		g_pAppFrm->pUpdateDlg->uRecordId = g_vTmpMessages.size()-1; // record #
      } else {
      	// update existing message
      	UpdateDlg_Global2Obj((g_vTmpMessages.at(g_pAppFrm->pUpdateDlg->uRecordId)).pUpdate);
      }

    	EndDialog (hwnd, 1);
  	  return TRUE;
  	case IDC_BTNCANCEL:
			EndDialog (hwnd, 0);
  	  return TRUE;
    default:
      return FALSE;
  }
  return FALSE;
}

/******************************************************************************/

BOOL UpdateDlg_OnClose(HWND hwnd) {

    EndDialog(hwnd, 0);
    return TRUE;
}

/******************************************************************************/

BOOL UpdateDlg_OnDestroy(HWND hwnd) {
    return TRUE;
}


BOOL CALLBACK UpdateDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){

	switch (message){
    HANDLE_MSG (hwnd, WM_INITDIALOG, 	UpdateDlg_OnInitDialog);
    HANDLE_MSG (hwnd, WM_CLOSE, 			UpdateDlg_OnClose);
    HANDLE_MSG (hwnd, WM_DESTROY, 		UpdateDlg_OnDestroy);
    HANDLE_MSG (hwnd, WM_COMMAND,			UpdateDlg_OnCommand);
  }
	return FALSE;
}



/******************************************************************************/
//
//	HELPER FUNCTIONS
//
/******************************************************************************/

BOOL hasFileAccess() {
	HANDLE hFile;
	hFile = CreateFile(g_pApp->szMsgFilepath, GENERIC_READ|GENERIC_WRITE,
	FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
 	if(hFile == INVALID_HANDLE_VALUE)	return FALSE;
 	if (GetLastError() == ACCESS_DENIED) {
 		CloseHandle(hFile);
 		return FALSE;
  }
  CloseHandle(hFile);
	return TRUE;
}





void DrawSkin(HDC hdc) {
          	// used to draw frame into g_hBmpSkin and mask that will be BitBlted in WM_PAINT
          	// skin needs to be redrawn only if size changes
    HDC          hdcMem;
    HDC					 hDCFrameLeft;
    HDC					 hDCFrameMiddle;
    HDC					 hDCFrameRight;
    HDC					 hDCFrameLeftMask;
    HDC					 hDCFrameMiddleMask;
    HDC					 hDCFrameRightMask;
    int          offset, streachX;
    BITMAP       bm;
    
            if (hdc) {

              hdcMem = CreateCompatibleDC(hdc);


              hDCFrameLeft = CreateCompatibleDC(hdc);
              hDCFrameMiddle = CreateCompatibleDC(hdc);
              hDCFrameRight = CreateCompatibleDC(hdc);
              hDCFrameLeftMask = CreateCompatibleDC(hdc);
              hDCFrameMiddleMask = CreateCompatibleDC(hdc);
              hDCFrameRightMask = CreateCompatibleDC(hdc);
              hDCSkin = CreateCompatibleDC(hdc);
              hDCSkinMask = CreateCompatibleDC(hdc);

              SelectObject(hDCFrameLeftMask, g_hBmpFrameLeftMask);
              SelectObject(hDCFrameLeft, g_hBmpFrameLeft);
              SelectObject(hDCFrameMiddleMask, g_hBmpFrameMiddleMask);
              SelectObject(hDCFrameMiddle, g_hBmpFrameMiddle);
              SelectObject(hDCFrameRightMask, g_hBmpFrameRightMask);
              SelectObject(hDCFrameRight, g_hBmpFrameRight);

              if (NULL != g_hBmpSkin) {
                DeleteObject(g_hBmpSkin);
                g_hBmpSkin = NULL;
              }

              if (NULL != g_hBmpSkinMask) {
                DeleteObject(g_hBmpSkinMask);
                g_hBmpSkin = NULL;
              }


           		g_hBmpSkin = CreateCompatibleBitmap(hdc, SCREEN_WIDTH,  24);
           		g_hBmpSkinMask = CreateCompatibleBitmap(hdc, SCREEN_WIDTH,  24);

              SelectObject(hDCSkin, g_hBmpSkin);
              SelectObject(hDCSkinMask, g_hBmpSkinMask);



  /* frame */
              offset = 0;

              // left frame
              GetObject(g_hBmpFrameLeft, sizeof(BITMAP),(LPVOID)&bm); // Get the description struct of the bitmap.
              BitBlt(hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, hDCFrameLeft, 0, 0, SRCCOPY);

              offset += bm.bmWidth;

              // middle frame
              GetObject(g_hBmpFrameMiddle, sizeof(BITMAP),(LPVOID)&bm); // Get the description struct of the bitmap.
              streachX = SCREEN_WIDTH -10 - offset - bm.bmWidth;
              StretchBlt(hdcMem, offset, 0, streachX, 24, hDCFrameMiddle, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);

              offset += streachX;

              // right frame
              GetObject(g_hBmpFrameRight, sizeof(BITMAP),(LPVOID)&bm); // Get the description struct of the bitmap.
              BitBlt(hdcMem, offset, 0, bm.bmWidth, bm.bmHeight, hDCFrameRight, 0, 0, SRCCOPY);

              // copy skin from DC
              GetObject(g_hBmpSkin, sizeof(BITMAP),(LPVOID)&bm); // Get the description struct of the bitmap.
            	BitBlt(hDCSkin, 0, 0, SCREEN_WIDTH,  24, hdcMem, 0, 0, SRCCOPY);
  /* mask */
              offset = 0;

              // left frame
              GetObject(g_hBmpFrameLeftMask, sizeof(BITMAP),(LPVOID)&bm); // Get the description struct of the bitmap.
              BitBlt(hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, hDCFrameLeftMask, 0, 0, SRCCOPY);

              offset += bm.bmWidth;

              // middle frame
              GetObject(g_hBmpFrameMiddleMask, sizeof(BITMAP),(LPVOID)&bm); // Get the description struct of the bitmap.
              streachX = SCREEN_WIDTH-10 - offset - bm.bmWidth;
              StretchBlt(hdcMem, offset, 0, streachX, 24, hDCFrameMiddleMask, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);

              offset += streachX;

              // right frame
              GetObject(g_hBmpFrameRightMask, sizeof(BITMAP),(LPVOID)&bm); // Get the description struct of the bitmap.
              BitBlt(hdcMem, offset, 0, bm.bmWidth, bm.bmHeight, hDCFrameRightMask, 0, 0, SRCCOPY);

              // copy skin from DC
              GetObject(g_hBmpSkinMask, sizeof(BITMAP),(LPVOID)&bm); // Get the description struct of the bitmap.
            	BitBlt(hDCSkinMask, 0, 0, SCREEN_WIDTH,  24, hdcMem, 0, 0, SRCCOPY);



            	BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hDCSkin, 0, 0, SRCINVERT);
            	BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hDCSkinMask, 0, 0, SRCAND);
            	BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hDCSkin, 0, 0, SRCINVERT);


            	// clean up
              DeleteDC(hDCFrameLeft);
              DeleteDC(hDCFrameMiddle);
              DeleteDC(hDCFrameRight);
              DeleteDC(hDCFrameLeftMask);
              DeleteDC(hDCFrameMiddleMask);
              DeleteDC(hDCFrameRightMask);
            	//DeleteDC(hDCSkin);
            	//DeleteDC(hDCSkinMask);
            	DeleteDC(hdcMem);
            	
            }

}




