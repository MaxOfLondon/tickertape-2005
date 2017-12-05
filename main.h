#include <windows.h>
#include <windowsx.h>
#include <process.h>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <math.h>
#include <commctrl.h>
#include <tchar.h>
#include <assert.h>



// app specific constants
#define SCREEN_WIDTH GetSystemMetrics(SM_CXSCREEN) //screen width
#define MsgBox(a) MessageBox(NULL, (a), "Message", 0)


#define DEBUG 1

#if DEBUG
	#define DbgMsgBox(a) MessageBox(NULL, (a), "DEBUG Message", 0)
#else
	#define DbgMsgBox(a)
#endif



#define TPM_VERPOSANIMATION 0x1000L
#define _SECOND 10000000
#define APPBAR_CALLBACK 	WM_USER + 1010
#define APPBAR_REPAINT 		WM_USER + 1011
#define IDC_LSTOUTAGES_REFRESH WM_USER + 1012
#define ACCESS_DENIED 5
#define MAX_LOADSTRING 100


#define getrandom( min, max ) (( rand() % (int)((( max ) + 1 ) - ( min ))) + ( min ))



struct _AppGlobals {
		UINT 			uTapeScreenSide;						// screen edge (defaults to ABE_TOP)
		BOOL 			bTimerRunning;							// Timer running state
		UINT 			uElapse; 										// time delay
		UINT    	upTimerId; 									// timer ID
		DOUBLE 		dMsgScrStep;								// message scrolling step in pixels
		DOUBLE 		dMsgScrPosX;								// position X of message on the screen
		DOUBLE 		dMsgScrPosY;								// position Y of message on the screen
		char*			szMsgFilepath;
		char*			szMsgSepacer;
		LONG 			lBgColor;										// tape color
		HMENU			hPopupMenu;
};

typedef _AppGlobals APPGLOBALS;
typedef _AppGlobals* PAPPGLOBALS;


		class _EditMsgsDlg {
				public:
					_EditMsgsDlg() {
       		};
       		~_EditMsgsDlg() {
       	  };

      		UINT	_IDC_CHKNMC;
      		UINT  _IDC_CHKTIME;
      		UINT  _IDC_RBNOUTAGESNO;
      		UINT  _IDC_RBNOUTAGESYES;
      		std::string _IDC_EDTTIME;
      		std::string _IDC_EDTNMC;
      		std::string _IDC_EDTNOOUTAGES;
    };
    
typedef _EditMsgsDlg EDITMSGDLG;
typedef _EditMsgsDlg* PEDITMSGDLG;


		class _UpdateDlg {
			public:
     		_UpdateDlg() {};
     		~_UpdateDlg() {};
     		
     		UINT	_IDC_RBNONGOING;
     		UINT	_IDC_RBNRESTORED;
     		UINT 	_IDC_CHKETA;
     		UINT	_IDC_CHKETR;
     		BOOL	__ENABLE_IDC_EDTRESTORED;
     		std::string _IDC_EDTSTART;
     		std::string	_IDC_EDTRESTORED;
     		std::string	_IDC_EDTETA;
     		std::string	_IDC_EDTETR;
     		std::string	_IDC_EDTUPDATE;
     		std::string _IDC_EDTTT;
     		
     		UINT	uRecordId;
 		};
 		
typedef _UpdateDlg UPDATEDLG;
typedef _UpdateDlg* PUPDATEDLG;

class _AppFormsGlobals{
  public:
    _AppFormsGlobals() {
      this->pEditMsgsDlg = new EDITMSGDLG;
      this->pUpdateDlg = new UPDATEDLG;
    };
    ~_AppFormsGlobals() {
      delete(pEditMsgsDlg);
      delete(pUpdateDlg);
    }

  PEDITMSGDLG pEditMsgsDlg;
  PUPDATEDLG pUpdateDlg;
};



typedef _AppFormsGlobals APPFORMSGLOBALS;
typedef _AppFormsGlobals* PAPPFORMSGLOBALS;


std::string Int2Str(int i) {
  char str[20];
  sprintf(str,"%d", i);
  return std::string(str);
}


struct MESSAGE {
  std::string msg;
  COLORREF rgb;
  int length;
};


class CUpdate {
    
  public:
    CUpdate(){
      this->sTicket = "";
    	this->sStart = "";
    	this->sRestored = "";
    	this->sNote = "";
    	this->sEta = "";
    	this->sEtr = "";
    	this->bRestored = FALSE;
    	this->bShowEta = TRUE;
    	this->bShowEtr = TRUE;
    };
    
    CUpdate(std::string _sTicket, std::string _sStart, std::string _sRestored,
    				std::string	_sNote,	std::string _sEta,
        		std::string _sEtr, BOOL	_bRestored,
        		BOOL _bShowEta, BOOL _bShowEtr) {
      this->sTicket = _sTicket;
    	this->sStart = _sStart;
    	this->sRestored = _sRestored;
    	this->sNote = _sNote;
    	this->sEta = _sEta;
    	this->sEtr = _sEtr;
    	this->bRestored = _bRestored;
    	this->bShowEta = _bShowEta;
    	this->bShowEtr = _bShowEtr;
   	};
   	
   	CUpdate(const CUpdate& rhs) {
   			  sTicket = std::string(rhs.sTicket);
   			  sStart = std::string(rhs.sStart);
   			  sRestored = std::string(rhs.sRestored);
        	sNote = std::string(rhs.sNote);
        	sEta = std::string(rhs.sEta);
        	sEtr = std::string(rhs.sEtr);
        	bRestored = rhs.bRestored;
        	bShowEta = rhs.bShowEta;
        	bShowEtr = rhs.bShowEtr;
   	}
   	
   	~CUpdate() {
   	};
   	
   	CUpdate& operator=(const CUpdate& rhs) {
    	if (this != &rhs) {
   			  sTicket = std::string(rhs.sTicket);
   			  sStart = std::string(rhs.sStart);
   			  sRestored = std::string(rhs.sRestored);
        	sNote = std::string(rhs.sNote);
        	sEta = std::string(rhs.sEta);
        	sEtr = std::string(rhs.sEtr);
        	bRestored = rhs.bRestored;
        	bShowEta = rhs.bShowEta;
        	bShowEtr = rhs.bShowEtr;
       }
  	   return *this;
   	}
   	
   	std::string toString() {
   		std::string s;
   		s = "TT" + sTicket + " (" + sStart + " - " + sRestored + ") " +  sNote + " " \
     		+ (bShowEta?(" ETA: " + sEta):"") + (bShowEtr?(" ETR: " + sEtr):"");
      return s;
   	};
   	
   	std::string toStringShort() {
   		std::string s;
   		s = "TT" + sTicket + " (" + sStart + " - " + sRestored + ") " +  getNoteShort() + " " \
     		+ (bShowEta?(" ETA: " + sEta):"") + (bShowEtr?(" ETR: " + sEtr):"");
      return s;
   	};
   	
   	/** Writes object to the stream
    */
    void serialize(std::ofstream& out){
    	int size;

    	// save sStart
    	size = sStart.size(); // get string length
    	out.write(reinterpret_cast<char *>(&size),sizeof(size));
    	out.write(sStart.c_str() ,size+1); // save string with '\0'
    	
    	//save sRestored
    	size = sRestored.size(); // get string length
    	out.write(reinterpret_cast<char *>(&size), sizeof(size));
    	out.write(sRestored.c_str(), size+1); // save string with '\0'
    	
    	//save sTicket
    	size = sTicket.size(); // get string length
    	out.write(reinterpret_cast<char *>(&size), sizeof(size));
    	out.write(sTicket.c_str(), size+1); // save string with '\0'

    	
    	//save sNote
    	size = sNote.size(); // get string length
    	out.write(reinterpret_cast<char *>(&size), sizeof(size));
    	out.write(sNote.c_str(), size+1); // save string with '\0'
    	
    	//save sEta
    	size = sEta.size(); // get string length
    	out.write(reinterpret_cast<char *>(&size), sizeof(size));
    	out.write(sEta.c_str(), size+1); // save string with '\0'
    	
    	//save sEtr
    	size = sEtr.size(); // get string length
    	out.write(reinterpret_cast<char *>(&size), sizeof(size));
    	out.write(sEtr.c_str(), size+1); // save string with '\0'
    	
    	//save bRestored
    	out.write(reinterpret_cast<char *>(&bRestored), sizeof(bRestored));

    	//save bShowEta
    	out.write(reinterpret_cast<char *>(&bShowEta), sizeof(bShowEta));
    	
    	//save bShowEtr
    	
    	out.write(reinterpret_cast<char *>(&bShowEtr), sizeof(bShowEtr));
    };
    
    
    BOOL deserialize(std::ifstream& in){

    	int len=0;
     	char* p=0;
     	
     	if (in.peek() == in.eof() ) return FALSE;
     	
     	// read sStart
     	in.read(reinterpret_cast<char *> (&len), sizeof(len));
     	p = new char[len+1]; // allocate temp buffer for string
     	in.read(p, len+1); 		// copy string to temp, including '\0'
     	sStart = p;						//copy temp to member
     	delete[] p;

      // read sRestored
     	in.read(reinterpret_cast<char *> (&len), sizeof(len));
     	p = new char [len+1]; // allocate temp buffer for string
     	in.read(p, len+1); 		// copy string to temp, including '\0'
     	sRestored = p;				//copy temp to member
     	delete[] p;

      // read sTicket
     	in.read(reinterpret_cast<char *> (&len), sizeof(len));
     	p = new char [len+1]; // allocate temp buffer for string
     	in.read(p, len+1); 		// copy string to temp, including '\0'
     	sTicket = p;						//copy temp to member
     	delete[] p;

      // read sNote
     	in.read(reinterpret_cast<char *> (&len), sizeof(len));
     	p = new char [len+1]; // allocate temp buffer for string
     	in.read(p, len+1); 		// copy string to temp, including '\0'
     	sNote = p;						//copy temp to member
     	delete[] p;
     	
      // read sEta
     	in.read(reinterpret_cast<char *> (&len), sizeof(len));
     	p = new char [len+1]; // allocate temp buffer for string
     	in.read(p, len+1); 		// copy string to temp, including '\0'
     	sEta = p;							//copy temp to member
     	delete[] p;
      
      // read sEtr
     	in.read(reinterpret_cast<char *> (&len), sizeof(len));
     	p = new char [len+1]; // allocate temp buffer for string
     	in.read(p, len+1); 		// copy string to temp, including '\0'
     	sEtr = p;							//copy temp to member
     	delete[] p;

      // read bRestored
      in.read(reinterpret_cast<char *> (&bRestored), sizeof(bRestored));
      
      // read bShowEta
      in.read(reinterpret_cast<char *> (&bShowEta), sizeof(bShowEta));
      
      // read bShowEtr
      in.read(reinterpret_cast<char *> (&bShowEtr), sizeof(bShowEtr));
      
      if (in.fail()) return FALSE;
      return TRUE;
    };

   	
  public:
    // properties
   	std::string sTicket;
  	std::string sStart;
  	std::string sRestored;
    std::string	sNote;
    std::string sEta;
    std::string sEtr;
    BOOL				bRestored;
    BOOL				bShowEta;
    BOOL				bShowEtr;
    
  private:
    std::string getNoteShort() {
      if (sNote.length() > 80) {
      		return (sNote.substr(0 , 80) + "...");
      } else {
          return sNote;
      }
    };
  };
    
class CUpdateHeader : public CUpdate {
	public:
   	CUpdateHeader(){};
    ~CUpdateHeader(){};
    
    std::string toString() {
   		return (sStart + sNote);
   	};

};

class CMessage {
  public:
    CMessage() {
      pUpdate = new CUpdate();
    };
    
    CMessage(const CMessage& rhs) {
      oRgb = rhs.oRgb;
      pUpdate = new CUpdate(*(rhs.pUpdate));
    };
    
    ~CMessage() {
      //if (pUpdate != NULL) delete pUpdate;
      //if (hBmp != NULL) delete hBmp;
    }
    
    CMessage& operator=(const CMessage& rhs){
    	if (this != &rhs) {
   			  delete pUpdate;
   			  pUpdate = new CUpdate();
   			  pUpdate = rhs.pUpdate;
        	oRgb = rhs.oRgb;
       }
  	   return *this;
    }
    
    std::string dump() {
      std::string s;
   		s = "TT: \"" + pUpdate->sTicket + "\"\n";
  	  s+= "Start: \"" + pUpdate->sStart + "\"\n";
  	  s+= "Restored: \"" + pUpdate->sRestored + "\"\n";
      s+= "Note: \"" + pUpdate->sNote + "\"\n";
      s+= "ETA: \"" + pUpdate->sEta + "\"\n";
      s+= "ETR: \"" + pUpdate->sEtr + "\"\n";
      s+= "Restored: " + std::string(pUpdate->bRestored?"true":"false") + "\n";
      s+= "Show ETA: " + std::string(pUpdate->bShowEta?"true":"false") + "\n";
      s+= "Show ETR: " + std::string(pUpdate->bShowEtr?"true":"false") + "\n";
      s+= "COLOR: " + Int2Str(oRgb) + "\n";
      return s;
    }
    
  public:
    //properties
    CUpdate* pUpdate;
    COLORREF oRgb;
    //HBITMAP hBmp;
};


