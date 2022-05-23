# Microsoft Developer Studio Generated NMAKE File, Based on instnp.dsp
!IF "$(CFG)" == ""
CFG=instnp - Win32 Release
!MESSAGE 構成が指定されていません。ﾃﾞﾌｫﾙﾄの instnp - Win32 Release を設定します。
!ENDIF 

!IF "$(CFG)" != "instnp - Win32 Release" && "$(CFG)" != "instnp - Win32 Debug"
!MESSAGE 指定された ﾋﾞﾙﾄﾞ ﾓｰﾄﾞ "$(CFG)" は正しくありません。
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "instnp.mak" CFG="instnp - Win32 Release"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "instnp - Win32 Release" ("Win32 (x86) Application" 用)
!MESSAGE "instnp - Win32 Debug" ("Win32 (x86) Application" 用)
!MESSAGE 
!ERROR 無効な構成が指定されています。
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "instnp - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\Release\instnp.exe"


CLEAN :
	-@erase "$(INTDIR)\Dialog.obj"
	-@erase "$(INTDIR)\DlgClass.obj"
	-@erase "$(INTDIR)\Instdrv.obj"
	-@erase "$(INTDIR)\instnp.res"
	-@erase "$(INTDIR)\Main.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\Release\instnp.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\misc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\instnp.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
RSC_PROJ=/l 0x411 /fo"$(INTDIR)\instnp.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\instnp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\instnp.pdb" /machine:I386 /out:"..\Release\instnp.exe" 
LINK32_OBJS= \
	"$(INTDIR)\Dialog.obj" \
	"$(INTDIR)\DlgClass.obj" \
	"$(INTDIR)\Instdrv.obj" \
	"$(INTDIR)\Main.obj" \
	"$(INTDIR)\instnp.res"

"..\Release\instnp.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "instnp - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\instnp.exe"


CLEAN :
	-@erase "$(INTDIR)\Dialog.obj"
	-@erase "$(INTDIR)\DlgClass.obj"
	-@erase "$(INTDIR)\Instdrv.obj"
	-@erase "$(INTDIR)\instnp.res"
	-@erase "$(INTDIR)\Main.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\instnp.exe"
	-@erase "$(OUTDIR)\instnp.ilk"
	-@erase "$(OUTDIR)\instnp.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\misc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\instnp.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
RSC_PROJ=/l 0x411 /fo"$(INTDIR)\instnp.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\instnp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\instnp.pdb" /debug /machine:I386 /out:"$(OUTDIR)\instnp.exe" 
LINK32_OBJS= \
	"$(INTDIR)\Dialog.obj" \
	"$(INTDIR)\DlgClass.obj" \
	"$(INTDIR)\Instdrv.obj" \
	"$(INTDIR)\Main.obj" \
	"$(INTDIR)\instnp.res"

"$(OUTDIR)\instnp.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("instnp.dep")
!INCLUDE "instnp.dep"
!ELSE 
!MESSAGE Warning: cannot find "instnp.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "instnp - Win32 Release" || "$(CFG)" == "instnp - Win32 Debug"
SOURCE=.\Dialog.cpp

"$(INTDIR)\Dialog.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=..\misc\DlgClass.cpp

"$(INTDIR)\DlgClass.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Instdrv.c

"$(INTDIR)\Instdrv.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\instnp.rc

"$(INTDIR)\instnp.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\Main.cpp

"$(INTDIR)\Main.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

