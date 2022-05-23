# Microsoft Developer Studio Generated NMAKE File, Based on TcpShare.dsp
!IF "$(CFG)" == ""
CFG=TcpShare - Win32 Release
!MESSAGE 構成が指定されていません。ﾃﾞﾌｫﾙﾄの TcpShare - Win32 Release を設定します。
!ENDIF 

!IF "$(CFG)" != "TcpShare - Win32 Release" && "$(CFG)" != "TcpShare - Win32 Debug"
!MESSAGE 指定された ﾋﾞﾙﾄﾞ ﾓｰﾄﾞ "$(CFG)" は正しくありません。
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "TcpShare.mak" CFG="TcpShare - Win32 Release"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "TcpShare - Win32 Release" ("Win32 (x86) Dynamic-Link Library" 用)
!MESSAGE "TcpShare - Win32 Debug" ("Win32 (x86) Dynamic-Link Library" 用)
!MESSAGE 
!ERROR 無効な構成が指定されています。
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "TcpShare - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\Release\TcpShare.xpm"


CLEAN :
	-@erase "$(INTDIR)\DlgClass.obj"
	-@erase "$(INTDIR)\LogDrv.obj"
	-@erase "$(INTDIR)\Sock.obj"
	-@erase "$(INTDIR)\TcpShare.obj"
	-@erase "$(INTDIR)\TcpShare.res"
	-@erase "$(INTDIR)\TcpShDlg.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\TcpShare.exp"
	-@erase "$(OUTDIR)\TcpShare.lib"
	-@erase "..\Release\TcpShare.xpm"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "..\xpinc" /I "..\portserv" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\TcpShare.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
RSC_PROJ=/l 0x411 /fo"$(INTDIR)\TcpShare.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\TcpShare.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib wsock32.lib comctl32.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\TcpShare.pdb" /machine:I386 /out:"..\Release\TcpShare.xpm" /implib:"$(OUTDIR)\TcpShare.lib" 
LINK32_OBJS= \
	"$(INTDIR)\DlgClass.obj" \
	"$(INTDIR)\LogDrv.obj" \
	"$(INTDIR)\Sock.obj" \
	"$(INTDIR)\TcpShare.obj" \
	"$(INTDIR)\TcpShDlg.obj" \
	"$(INTDIR)\TcpShare.res"

"..\Release\TcpShare.xpm" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "TcpShare - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\TcpShare.xpm"


CLEAN :
	-@erase "$(INTDIR)\DlgClass.obj"
	-@erase "$(INTDIR)\LogDrv.obj"
	-@erase "$(INTDIR)\Sock.obj"
	-@erase "$(INTDIR)\TcpShare.obj"
	-@erase "$(INTDIR)\TcpShare.res"
	-@erase "$(INTDIR)\TcpShDlg.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\TcpShare.exp"
	-@erase "$(OUTDIR)\TcpShare.ilk"
	-@erase "$(OUTDIR)\TcpShare.lib"
	-@erase "$(OUTDIR)\TcpShare.pdb"
	-@erase "$(OUTDIR)\TcpShare.xpm"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\xpinc" /I "..\portserv" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\TcpShare.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
RSC_PROJ=/l 0x411 /fo"$(INTDIR)\TcpShare.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\TcpShare.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib wsock32.lib comctl32.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\TcpShare.pdb" /debug /machine:I386 /out:"$(OUTDIR)\TcpShare.xpm" /implib:"$(OUTDIR)\TcpShare.lib" 
LINK32_OBJS= \
	"$(INTDIR)\DlgClass.obj" \
	"$(INTDIR)\LogDrv.obj" \
	"$(INTDIR)\Sock.obj" \
	"$(INTDIR)\TcpShare.obj" \
	"$(INTDIR)\TcpShDlg.obj" \
	"$(INTDIR)\TcpShare.res"

"$(OUTDIR)\TcpShare.xpm" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("TcpShare.dep")
!INCLUDE "TcpShare.dep"
!ELSE 
!MESSAGE Warning: cannot find "TcpShare.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "TcpShare - Win32 Release" || "$(CFG)" == "TcpShare - Win32 Debug"
SOURCE=.\DlgClass.cpp

"$(INTDIR)\DlgClass.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=..\PortServ\LogDrv.cpp

"$(INTDIR)\LogDrv.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Sock.cpp

"$(INTDIR)\Sock.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\TcpShare.cpp

"$(INTDIR)\TcpShare.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\TcpShare.rc

"$(INTDIR)\TcpShare.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\TcpShDlg.cpp

"$(INTDIR)\TcpShDlg.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

