#ifndef __TEXTEDITBOX
#define __TEXTEDITBOX

#include "passvars.h"
#include "ddutil.h"
#include "ddrawlib.h"
#include "textlib.h"

#define TEXTENTRYBOX_HEIGHT		 50
#define TEXTENTRYBOX_WIDTH			200

#define TEXTENTRYBOX_TOP			(480/2)-(TEXTENTRYBOX_HEIGHT/2)
#define TEXTENTRYBOX_LEFT			(640/2)-(TEXTENTRYBOX_WIDTH/2)
#define TEXTENTRYBOX_BOTTOM		(480/2)+(TEXTENTRYBOX_HEIGHT/2)
#define TEXTENTRYBOX_RIGHT			(640/2)+(TEXTENTRYBOX_WIDTH/2)

void TextEntryBox(char String[100]);
int TextEditProc(PASSPROCVARS Params);

///////////////////////////////////////////
class TEXTEDITBOX{
	public:
   	TEXTEDITBOX();
      ~TEXTEDITBOX();

      LPDIRECTDRAWSURFACE EditBoxSurface;
      LPDIRECTDRAWSURFACE StorageSurface;

		int Info;
      void AddChar(char ch);
      void UpdateBox();
      void DeleteChar();
      void Enter();
      void Done();
      int ProcFunction(PASSPROCVARS Params);
		void GetInput(char Input[100]);

      char String[100];
		int CursorPosition;
      int StringLength;
   private:
};
///////////////////////////////////////////
#endif
