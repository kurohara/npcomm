# Microsoft Developer Studio Generated NMAKE File, Based on PortServ.dsp
!IF "$(CFG)" == ""
CFG=PortServ - Win32 Release
!MESSAGE 構成が指定されていません。ﾃﾞﾌｫﾙﾄの PortServ - Win32 Release を設定します。
!ENDIF 

!IF "$(CFG)" != "PortServ - Win32 Release" && "$(CFG)" != "PortServ - Win32 Debug"
!MESSAGE 指定された ﾋﾞﾙﾄﾞ ﾓｰﾄﾞ "$(CFG)" は正しくありません。
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "PortServ.mak" CFG="PortServ - Win32 Release"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "PortServ - Win32 Release" ("Win32 (x86) Application" 用)
!MESSAGE "PortServ - Win32 Debug" ("Win32 (x86) Application" 用)
!MESSAGE 
!ERROR 無効な構成が指定されています。
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "PortServ - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\Release\PortServ.exe"


CLEAN :
	-@erase "$(INTDIR)\CommDrv.obj"
	-@erase "$(INTDIR)\CreatDev.obj"
	-@erase "$(INTDIR)\DCBUtil.obj"
	-@erase "$(INTDIR)\DlgClass.obj"
	-@erase "$(INTDIR)\IERegist.obj"
	-@erase "$(INTDIR)\LogDrv.obj"
	-@erase "$(INTDIR)\Main.obj"
	-@erase "$(INTDIR)\PortServ.res"
	-@erase "$(INTDIR)\PSDialog.obj"
	-@erase "$(INTDIR)\Sock.obj"
	-@erase "$(INTDIR)\TcpMgr.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\Release\PortServ.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "..\xpinc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\PortServ.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x411 /fo"$(INTDIR)\PortServ.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\PortServ.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib th32.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\PortServ.pdb" /machine:I386 /out:"..\Release\PortServ.exe" 
LINK32_OBJS= \
	"$(INTDIR)\CommDrv.obj" \
	"$(INTDIR)\CreatDev.obj" \
	"$(INTDIR)\DCBUtil.obj" \
	"$(INTDIR)\DlgClass.obj" \
	"$(INTDIR)\IERegist.obj" \
	"$(INTDIR)\LogDrv.obj" \
	"$(INTDIR)\Main.obj" \
	"$(INTDIR)\PSDialog.obj" \
	"$(INTDIR)\Sock.obj" \
	"$(INTDIR)\TcpMgr.obj" \
	"$(INTDIR)\PortServ.res"

"..\Release\PortServ.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "PortServ - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\PortServ.exe"


CLEAN :
	-@erase "$(INTDIR)\CommDrv.obj"
	-@erase "$(INTDIR)\CreatDev.obj"
	-@erase "$(INTDIR)\DCBUtil.obj"
	-@erase "$(INTDIR)\DlgClass.obj"
	-@erase "$(INTDIR)\IERegist.obj"
	-@erase "$(INTDIR)\LogDrv.obj"
	-@erase "$(INTDIR)\Main.obj"
	-@erase "$(INTDIR)\PortServ.res"
	-@erase "$(INTDIR)\PSDialog.obj"
	-@erase "$(INTDIR)\Sock.obj"
	-@erase "$(INTDIR)\TcpMgr.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\PortServ.exe"
	-@erase "$(OUTDIR)\PortServ.ilk"
	-@erase "$(OUTDIR)\PortServ.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\xpinc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x411 /fo"$(INTDIR)\PortServ.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\PortServ.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib th32.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\PortServ.pdb" /debug /machine:I386 /out:"$(OUTDIR)\PortServ.exe" 
LINK32_OBJS= \
	"$(INTDIR)\CommDrv.obj" \
	"$(INTDIR)\CreatDev.obj" \
	"$(INTDIR)\DCBUtil.obj" \
	"$(INTDIR)\DlgClass.obj" \
	"$(INTDIR)\IERegist.obj" \
	"$(INTDIR)\LogDrv.obj" \
	"$(INTDIR)\Main.obj" \
	"$(INTDIR)\PSDialog.obj" \
	"$(INTDIR)\Sock.obj" \
	"$(INTDIR)\TcpMgr.obj" \
	"$(INTDIR)\PortServ.res"

"$(OUTDIR)\PortServ.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("PortServ.dep")
!INCLUDE "PortServ.dep"
!ELSE 
!MESSAGE Warning: cannot find "PortServ.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "PortServ - Win32 Release" || "$(CFG)" == "PortServ - Win32 Debug"
SOURCE=.\CommDrv.cpp

"$(INTDIR)\CommDrv.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\CreatDev.cpp

"$(INTDIR)\CreatDev.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\DCBUtil.c

"$(INTDIR)\DCBUtil.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\DlgClass.cpp

"$(INTDIR)\DlgClass.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\IERegist.c

"$(INTDIR)\IERegist.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\LogDrv.cpp

"$(INTDIR)\LogDrv.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Main.cpp

"$(INTDIR)\Main.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\PortServ.rc

"$(INTDIR)\PortServ.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\PSDialog.cpp

"$(INTDIR)\PSDialog.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Sock.cpp

"$(INTDIR)\Sock.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\TcpMgr.cpp

"$(INTDIR)\TcpMgr.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

