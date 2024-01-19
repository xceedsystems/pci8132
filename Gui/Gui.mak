# Microsoft Developer Studio Generated NMAKE File, Based on Gui.dsp
!IF "$(CFG)" == ""
CFG=Gui - Win32 Release
!MESSAGE No configuration specified. Defaulting to Gui - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "Gui - Win32 Release" && "$(CFG)" != "Gui - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Gui.mak" CFG="Gui - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Gui - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Gui - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Gui - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "..\..\..\Driver 6_X\Bin\Pci3tier.io3" "$(OUTDIR)\Gui.bsc"


CLEAN :
	-@erase "$(INTDIR)\DEVICES.OBJ"
	-@erase "$(INTDIR)\DEVICES.SBR"
	-@erase "$(INTDIR)\GOODIES.OBJ"
	-@erase "$(INTDIR)\GOODIES.SBR"
	-@erase "$(INTDIR)\MFCSTUFF.OBJ"
	-@erase "$(INTDIR)\MFCSTUFF.SBR"
	-@erase "$(INTDIR)\NETWORK.OBJ"
	-@erase "$(INTDIR)\NETWORK.SBR"
	-@erase "$(INTDIR)\PCI3TIER.OBJ"
	-@erase "$(INTDIR)\PCI3TIER.res"
	-@erase "$(INTDIR)\PCI3TIER.SBR"
	-@erase "$(INTDIR)\POINTS.OBJ"
	-@erase "$(INTDIR)\POINTS.SBR"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Gui.bsc"
	-@erase "$(OUTDIR)\Pci3tier.exp"
	-@erase "$(OUTDIR)\Pci3tier.lib"
	-@erase "$(OUTDIR)\Pci3tier.map"
	-@erase "$(OUTDIR)\Pci3tier.pdb"
	-@erase "..\..\..\Driver 6_X\Bin\Pci3tier.io3"
	-@erase "..\..\inc\SYMDB.tlb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W4 /Zi /O2 /I "..\inc" /I "..\..\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "STRICT" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Gui.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\PCI3TIER.res" /i "..\inc" /i "..\..\inc" /d "NDEBUG" /d "WIN32" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Gui.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\DEVICES.SBR" \
	"$(INTDIR)\GOODIES.SBR" \
	"$(INTDIR)\PCI3TIER.SBR" \
	"$(INTDIR)\MFCSTUFF.SBR" \
	"$(INTDIR)\NETWORK.SBR" \
	"$(INTDIR)\POINTS.SBR"

"$(OUTDIR)\Gui.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=oldnames.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\Pci3tier.pdb" /map:"$(INTDIR)\Pci3tier.map" /debug /debugtype:both /machine:IX86 /out:"\Program Files\VLC\Driver 6_X\Bin\Pci3tier.io3" /implib:"$(OUTDIR)\Pci3tier.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\DEVICES.OBJ" \
	"$(INTDIR)\GOODIES.OBJ" \
	"$(INTDIR)\PCI3TIER.OBJ" \
	"$(INTDIR)\MFCSTUFF.OBJ" \
	"$(INTDIR)\NETWORK.OBJ" \
	"$(INTDIR)\POINTS.OBJ" \
	"$(INTDIR)\PCI3TIER.res"

"..\..\..\Driver 6_X\Bin\Pci3tier.io3" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Gui - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\Pci3tier.dll" "$(OUTDIR)\Gui.bsc"


CLEAN :
	-@erase "$(INTDIR)\DEVICES.OBJ"
	-@erase "$(INTDIR)\DEVICES.SBR"
	-@erase "$(INTDIR)\GOODIES.OBJ"
	-@erase "$(INTDIR)\GOODIES.SBR"
	-@erase "$(INTDIR)\MFCSTUFF.OBJ"
	-@erase "$(INTDIR)\MFCSTUFF.SBR"
	-@erase "$(INTDIR)\NETWORK.OBJ"
	-@erase "$(INTDIR)\NETWORK.SBR"
	-@erase "$(INTDIR)\PCI3TIER.OBJ"
	-@erase "$(INTDIR)\PCI3TIER.res"
	-@erase "$(INTDIR)\PCI3TIER.SBR"
	-@erase "$(INTDIR)\POINTS.OBJ"
	-@erase "$(INTDIR)\POINTS.SBR"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Gui.bsc"
	-@erase "$(OUTDIR)\Pci3tier.dll"
	-@erase "$(OUTDIR)\Pci3tier.exp"
	-@erase "$(OUTDIR)\Pci3tier.ilk"
	-@erase "$(OUTDIR)\Pci3tier.lib"
	-@erase "$(OUTDIR)\Pci3tier.pdb"
	-@erase "..\..\inc\SYMDB.tlb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W4 /Gm /ZI /Od /I "..\inc" /I "..\..\inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "STRICT" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Gui.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\PCI3TIER.res" /i "..\inc" /i "..\..\inc" /d "_DEBUG" /d "WIN32" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Gui.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\DEVICES.SBR" \
	"$(INTDIR)\GOODIES.SBR" \
	"$(INTDIR)\PCI3TIER.SBR" \
	"$(INTDIR)\MFCSTUFF.SBR" \
	"$(INTDIR)\NETWORK.SBR" \
	"$(INTDIR)\POINTS.SBR"

"$(OUTDIR)\Gui.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=oldnames.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\Pci3tier.pdb" /debug /machine:IX86 /out:"$(OUTDIR)\Pci3tier.dll" /implib:"$(OUTDIR)\Pci3tier.lib" 
LINK32_OBJS= \
	"$(INTDIR)\DEVICES.OBJ" \
	"$(INTDIR)\GOODIES.OBJ" \
	"$(INTDIR)\PCI3TIER.OBJ" \
	"$(INTDIR)\MFCSTUFF.OBJ" \
	"$(INTDIR)\NETWORK.OBJ" \
	"$(INTDIR)\POINTS.OBJ" \
	"$(INTDIR)\PCI3TIER.res"

"$(OUTDIR)\Pci3tier.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

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


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("Gui.dep")
!INCLUDE "Gui.dep"
!ELSE 
!MESSAGE Warning: cannot find "Gui.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Gui - Win32 Release" || "$(CFG)" == "Gui - Win32 Debug"
SOURCE=.\DEVICES.CPP

"$(INTDIR)\DEVICES.OBJ"	"$(INTDIR)\DEVICES.SBR" : $(SOURCE) "$(INTDIR)" "..\..\inc\SYMDB.tlb"


SOURCE=.\GOODIES.CPP

"$(INTDIR)\GOODIES.OBJ"	"$(INTDIR)\GOODIES.SBR" : $(SOURCE) "$(INTDIR)" "..\..\inc\SYMDB.tlb"


SOURCE=.\MFCSTUFF.CPP

"$(INTDIR)\MFCSTUFF.OBJ"	"$(INTDIR)\MFCSTUFF.SBR" : $(SOURCE) "$(INTDIR)"


SOURCE=.\NETWORK.CPP

"$(INTDIR)\NETWORK.OBJ"	"$(INTDIR)\NETWORK.SBR" : $(SOURCE) "$(INTDIR)" "..\..\inc\SYMDB.tlb"


SOURCE=.\PCI3TIER.CPP

"$(INTDIR)\PCI3TIER.OBJ"	"$(INTDIR)\PCI3TIER.SBR" : $(SOURCE) "$(INTDIR)" "..\..\inc\SYMDB.tlb"


SOURCE=.\POINTS.CPP

"$(INTDIR)\POINTS.OBJ"	"$(INTDIR)\POINTS.SBR" : $(SOURCE) "$(INTDIR)" "..\..\inc\SYMDB.tlb"


SOURCE=..\..\inc\SYMDB.ODL

!IF  "$(CFG)" == "Gui - Win32 Release"

MTL_SWITCHES=/nologo /D "NDEBUG" /tlb "..\..\inc\SYMDB.tlb" /mktyplib203 /win32 

"..\..\inc\SYMDB.tlb" : $(SOURCE) "$(OUTDIR)"
	$(MTL) @<<
  $(MTL_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Gui - Win32 Debug"

MTL_SWITCHES=/nologo /D "_DEBUG" /tlb "..\..\inc\SYMDB.tlb" /mktyplib203 /win32 

"..\..\inc\SYMDB.tlb" : $(SOURCE) "$(OUTDIR)"
	$(MTL) @<<
  $(MTL_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\PCI3TIER.RC

!IF  "$(CFG)" == "Gui - Win32 Release"


"$(INTDIR)\PCI3TIER.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\PCI3TIER.res" /i "..\inc" /i "..\..\inc" /i ".\Release" /d "NDEBUG" /d "WIN32" $(SOURCE)


!ELSEIF  "$(CFG)" == "Gui - Win32 Debug"


"$(INTDIR)\PCI3TIER.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\PCI3TIER.res" /i "..\inc" /i "..\..\inc" /i ".\Debug" /d "_DEBUG" /d "WIN32" $(SOURCE)


!ENDIF 


!ENDIF 

