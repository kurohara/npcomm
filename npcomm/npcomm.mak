# Microsoft Developer Studio Generated NMAKE File, Based on npcomm.dsp
!IF "$(CFG)" == ""
CFG=npcomm - Win32 Release
!MESSAGE 構成が指定されていません。ﾃﾞﾌｫﾙﾄの npcomm - Win32 Release を設定します。
!ENDIF 

!IF "$(CFG)" != "npcomm - Win32 Release" && "$(CFG)" != "npcomm - Win32 Debug"
!MESSAGE 指定された ﾋﾞﾙﾄﾞ ﾓｰﾄﾞ "$(CFG)" は正しくありません。
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "npcomm.mak" CFG="npcomm - Win32 Release"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "npcomm - Win32 Release" ("Win32 (x86) Application" 用)
!MESSAGE "npcomm - Win32 Debug" ("Win32 (x86) Application" 用)
!MESSAGE 
!ERROR 無効な構成が指定されています。
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "npcomm - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\Release\npcomm.exe"


CLEAN :
	-@erase "$(INTDIR)\DialogUI.obj"
	-@erase "$(INTDIR)\DlgClass.obj"
	-@erase "$(INTDIR)\Main.obj"
	-@erase "$(INTDIR)\main.res"
	-@erase "$(INTDIR)\V2Mgr.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\Release\npcomm.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /Od /I "..\inc" /I "..\misc" /I "..\xpinc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
RSC_PROJ=/l 0x411 /fo"$(INTDIR)\main.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\npcomm.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\npcomm.pdb" /machine:I386 /out:"..\Release\npcomm.exe" 
LINK32_OBJS= \
	"$(INTDIR)\DialogUI.obj" \
	"$(INTDIR)\DlgClass.obj" \
	"$(INTDIR)\Main.obj" \
	"$(INTDIR)\V2Mgr.obj" \
	"$(INTDIR)\main.res"

"..\Release\npcomm.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "npcomm - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\npcomm.exe" "$(OUTDIR)\npcomm.bsc"


CLEAN :
	-@erase "$(INTDIR)\DialogUI.obj"
	-@erase "$(INTDIR)\DialogUI.sbr"
	-@erase "$(INTDIR)\DlgClass.obj"
	-@erase "$(INTDIR)\DlgClass.sbr"
	-@erase "$(INTDIR)\Main.obj"
	-@erase "$(INTDIR)\main.res"
	-@erase "$(INTDIR)\Main.sbr"
	-@erase "$(INTDIR)\V2Mgr.obj"
	-@erase "$(INTDIR)\V2Mgr.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\npcomm.bsc"
	-@erase "$(OUTDIR)\npcomm.exe"
	-@erase "$(OUTDIR)\npcomm.ilk"
	-@erase "$(OUTDIR)\npcomm.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\inc" /I "..\misc" /I "..\xpinc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\npcomm.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
RSC_PROJ=/l 0x411 /fo"$(INTDIR)\main.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\npcomm.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\DialogUI.sbr" \
	"$(INTDIR)\DlgClass.sbr" \
	"$(INTDIR)\Main.sbr" \
	"$(INTDIR)\V2Mgr.sbr"

"$(OUTDIR)\npcomm.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\npcomm.pdb" /debug /machine:I386 /out:"$(OUTDIR)\npcomm.exe" 
LINK32_OBJS= \
	"$(INTDIR)\DialogUI.obj" \
	"$(INTDIR)\DlgClass.obj" \
	"$(INTDIR)\Main.obj" \
	"$(INTDIR)\V2Mgr.obj" \
	"$(INTDIR)\main.res"

"$(OUTDIR)\npcomm.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("npcomm.dep")
!INCLUDE "npcomm.dep"
!ELSE 
!MESSAGE Warning: cannot find "npcomm.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "npcomm - Win32 Release" || "$(CFG)" == "npcomm - Win32 Debug"
SOURCE=.\DialogUI.cpp

!IF  "$(CFG)" == "npcomm - Win32 Release"


"$(INTDIR)\DialogUI.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "npcomm - Win32 Debug"


"$(INTDIR)\DialogUI.obj"	"$(INTDIR)\DialogUI.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=..\misc\DlgClass.cpp

!IF  "$(CFG)" == "npcomm - Win32 Release"


"$(INTDIR)\DlgClass.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "npcomm - Win32 Debug"


"$(INTDIR)\DlgClass.obj"	"$(INTDIR)\DlgClass.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Main.cpp

!IF  "$(CFG)" == "npcomm - Win32 Release"


"$(INTDIR)\Main.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "npcomm - Win32 Debug"


"$(INTDIR)\Main.obj"	"$(INTDIR)\Main.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\main.rc

"$(INTDIR)\main.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\V2Mgr.cpp

!IF  "$(CFG)" == "npcomm - Win32 Release"


"$(INTDIR)\V2Mgr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "npcomm - Win32 Debug"


"$(INTDIR)\V2Mgr.obj"	"$(INTDIR)\V2Mgr.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 


!ENDIF 

