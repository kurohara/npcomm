# Microsoft Developer Studio Generated NMAKE File, Format Version 40001
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=Telnet - Win32 Debug
!MESSAGE 構成が指定されていません。ﾃﾞﾌｫﾙﾄの Telnet - Win32 Debug を設定します。
!ENDIF 

!IF "$(CFG)" != "Telnet - Win32 Release" && "$(CFG)" != "Telnet - Win32 Debug"
!MESSAGE 指定された ﾋﾞﾙﾄﾞ ﾓｰﾄﾞ "$(CFG)" は正しくありません。
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛ 'CFG' を定義することによって
!MESSAGE NMAKE 実行時にﾋﾞﾙﾄﾞ ﾓｰﾄﾞを指定できます。例えば:
!MESSAGE 
!MESSAGE NMAKE /f "Telnet.mak" CFG="Telnet - Win32 Debug"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "Telnet - Win32 Release" ("Win32 (x86) Dynamic-Link Library" 用)
!MESSAGE "Telnet - Win32 Debug" ("Win32 (x86) Dynamic-Link Library" 用)
!MESSAGE 
!ERROR 無効な構成が指定されています。
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "Telnet - Win32 Debug"
MTL=mktyplib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Telnet - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\Telnet.xpm"

CLEAN : 
	-@erase "..\Release\Telnet.xpm"
	-@erase ".\Release\STSock.obj"
	-@erase ".\Release\ProxProc.obj"
	-@erase ".\Release\DlgClass.obj"
	-@erase ".\Release\TelProto.obj"
	-@erase ".\Release\TelnetDlg.obj"
	-@erase ".\Release\VirModem.obj"
	-@erase ".\Release\Telnet.obj"
	-@erase ".\Release\Telnet.res"
	-@erase ".\Release\Telnet.lib"
	-@erase ".\Release\Telnet.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\xpinc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "..\xpinc" /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /Fp"$(INTDIR)/Telnet.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
RSC_PROJ=/l 0x411 /fo"$(INTDIR)/Telnet.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Telnet.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib shell32.lib wsock32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"..\Release/Telnet.xpm"
LINK32_FLAGS=kernel32.lib user32.lib shell32.lib wsock32.lib /nologo\
 /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)/Telnet.pdb"\
 /machine:I386 /out:"..\Release/Telnet.xpm" /implib:"$(OUTDIR)/Telnet.lib" 
LINK32_OBJS= \
	"$(INTDIR)/STSock.obj" \
	"$(INTDIR)/ProxProc.obj" \
	"$(INTDIR)/DlgClass.obj" \
	"$(INTDIR)/TelProto.obj" \
	"$(INTDIR)/TelnetDlg.obj" \
	"$(INTDIR)/VirModem.obj" \
	"$(INTDIR)/Telnet.obj" \
	"$(INTDIR)/Telnet.res"

"$(OUTDIR)\Telnet.xpm" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Telnet - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\Telnet.xpm"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\vc40.idb"
	-@erase ".\Debug\Telnet.xpm"
	-@erase ".\Debug\Telnet.obj"
	-@erase ".\Debug\DlgClass.obj"
	-@erase ".\Debug\TelProto.obj"
	-@erase ".\Debug\ProxProc.obj"
	-@erase ".\Debug\VirModem.obj"
	-@erase ".\Debug\TelnetDlg.obj"
	-@erase ".\Debug\STSock.obj"
	-@erase ".\Debug\Telnet.res"
	-@erase ".\Debug\Telnet.ilk"
	-@erase ".\Debug\Telnet.lib"
	-@erase ".\Debug\Telnet.exp"
	-@erase ".\Debug\Telnet.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\xpinc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\xpinc" /D "WIN32" /D "_DEBUG"\
 /D "_WINDOWS" /Fp"$(INTDIR)/Telnet.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
RSC_PROJ=/l 0x411 /fo"$(INTDIR)/Telnet.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Telnet.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib shell32.lib wsock32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"Debug/Telnet.xpm"
LINK32_FLAGS=kernel32.lib user32.lib shell32.lib wsock32.lib /nologo\
 /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)/Telnet.pdb" /debug\
 /machine:I386 /out:"$(OUTDIR)/Telnet.xpm" /implib:"$(OUTDIR)/Telnet.lib" 
LINK32_OBJS= \
	"$(INTDIR)/Telnet.obj" \
	"$(INTDIR)/DlgClass.obj" \
	"$(INTDIR)/TelProto.obj" \
	"$(INTDIR)/ProxProc.obj" \
	"$(INTDIR)/VirModem.obj" \
	"$(INTDIR)/TelnetDlg.obj" \
	"$(INTDIR)/STSock.obj" \
	"$(INTDIR)/Telnet.res"

"$(OUTDIR)\Telnet.xpm" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "Telnet - Win32 Release"
# Name "Telnet - Win32 Debug"

!IF  "$(CFG)" == "Telnet - Win32 Release"

!ELSEIF  "$(CFG)" == "Telnet - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Telnet.rc
DEP_RSC_TELNE=\
	".\res\bitmap2.bmp"\
	

"$(INTDIR)\Telnet.res" : $(SOURCE) $(DEP_RSC_TELNE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\STSock.cpp
DEP_CPP_STSOC=\
	".\STSock.h"\
	".\Streams.h"\
	

"$(INTDIR)\STSock.obj" : $(SOURCE) $(DEP_CPP_STSOC) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Telnet.cpp

!IF  "$(CFG)" == "Telnet - Win32 Release"

DEP_CPP_TELNET=\
	".\..\xpinc\PCommMgr.h"\
	".\Telnet.h"\
	".\TelnetDlg.h"\
	".\..\xpinc\PCommXs.h"\
	".\..\xpinc\PComConf.h"\
	".\STSock.h"\
	".\VirModem.h"\
	".\Streams.h"\
	".\DlgClass.h"\
	".\..\xpinc\UICtrl.h"\
	".\Params.h"\
	

"$(INTDIR)\Telnet.obj" : $(SOURCE) $(DEP_CPP_TELNET) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Telnet - Win32 Debug"

DEP_CPP_TELNET=\
	".\..\xpinc\PCommMgr.h"\
	".\Telnet.h"\
	".\TelnetDlg.h"\
	".\..\xpinc\PCommXs.h"\
	".\..\xpinc\PComConf.h"\
	".\STSock.h"\
	".\VirModem.h"\
	".\TelProto.h"\
	".\ProxProc.h"\
	".\Params.h"\
	".\Streams.h"\
	".\DlgClass.h"\
	".\..\xpinc\UICtrl.h"\
	

"$(INTDIR)\Telnet.obj" : $(SOURCE) $(DEP_CPP_TELNET) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\TelnetDlg.cpp

!IF  "$(CFG)" == "Telnet - Win32 Release"

DEP_CPP_TELNETD=\
	".\Telnet.h"\
	".\TelnetDlg.h"\
	".\..\xpinc\PCommMgr.h"\
	".\..\xpinc\PCommXs.h"\
	".\STSock.h"\
	".\VirModem.h"\
	".\..\xpinc\PComConf.h"\
	".\Streams.h"\
	".\DlgClass.h"\
	".\..\xpinc\UICtrl.h"\
	".\Params.h"\
	

"$(INTDIR)\TelnetDlg.obj" : $(SOURCE) $(DEP_CPP_TELNETD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Telnet - Win32 Debug"

DEP_CPP_TELNETD=\
	".\Telnet.h"\
	".\TelnetDlg.h"\
	".\..\xpinc\PCommMgr.h"\
	".\..\xpinc\PCommXs.h"\
	".\STSock.h"\
	".\VirModem.h"\
	".\TelProto.h"\
	".\ProxProc.h"\
	".\Params.h"\
	".\..\xpinc\PComConf.h"\
	".\Streams.h"\
	".\DlgClass.h"\
	".\..\xpinc\UICtrl.h"\
	

"$(INTDIR)\TelnetDlg.obj" : $(SOURCE) $(DEP_CPP_TELNETD) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\TelProto.cpp

!IF  "$(CFG)" == "Telnet - Win32 Release"

DEP_CPP_TELPR=\
	".\Telnet.h"\
	".\STSock.h"\
	".\TelProto.h"\
	".\..\xpinc\PCommMgr.h"\
	".\..\xpinc\PCommXs.h"\
	".\VirModem.h"\
	".\..\xpinc\PComConf.h"\
	".\Streams.h"\
	

"$(INTDIR)\TelProto.obj" : $(SOURCE) $(DEP_CPP_TELPR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Telnet - Win32 Debug"

DEP_CPP_TELPR=\
	".\Telnet.h"\
	".\STSock.h"\
	".\TelProto.h"\
	".\..\xpinc\PCommMgr.h"\
	".\..\xpinc\PCommXs.h"\
	".\VirModem.h"\
	".\ProxProc.h"\
	".\Params.h"\
	".\TelnetDlg.h"\
	".\..\xpinc\PComConf.h"\
	".\Streams.h"\
	".\DlgClass.h"\
	".\..\xpinc\UICtrl.h"\
	

"$(INTDIR)\TelProto.obj" : $(SOURCE) $(DEP_CPP_TELPR) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ProxProc.cpp
DEP_CPP_PROXP=\
	".\ProxProc.h"\
	".\STSock.h"\
	".\Streams.h"\
	

"$(INTDIR)\ProxProc.obj" : $(SOURCE) $(DEP_CPP_PROXP) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\VirModem.cpp

!IF  "$(CFG)" == "Telnet - Win32 Release"

DEP_CPP_VIRMO=\
	".\VirModem.h"\
	

"$(INTDIR)\VirModem.obj" : $(SOURCE) $(DEP_CPP_VIRMO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Telnet - Win32 Debug"

DEP_CPP_VIRMO=\
	".\VirModem.h"\
	".\Streams.h"\
	".\TelProto.h"\
	".\STSock.h"\
	

"$(INTDIR)\VirModem.obj" : $(SOURCE) $(DEP_CPP_VIRMO) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DlgClass.cpp
DEP_CPP_DLGCL=\
	".\DlgClass.h"\
	

"$(INTDIR)\DlgClass.obj" : $(SOURCE) $(DEP_CPP_DLGCL) "$(INTDIR)"


# End Source File
# End Target
# End Project
################################################################################
