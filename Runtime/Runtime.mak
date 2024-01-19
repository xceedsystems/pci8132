# Microsoft Developer Studio Generated NMAKE File, Based on Runtime.dsp
!IF "$(CFG)" == ""
CFG=Runtime - Win32 Release
!MESSAGE No configuration specified. Defaulting to Runtime - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "Runtime - Win32 Release" && "$(CFG)" != "Runtime - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Runtime.mak" CFG="Runtime - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Runtime - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Runtime - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Runtime - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "..\..\..\Driver 6_X\Bin\Jobs\Pci3tier.rt3" "$(OUTDIR)\Runtime.bsc"


CLEAN :
	-@erase "$(INTDIR)\auxrut.obj"
	-@erase "$(INTDIR)\auxrut.sbr"
	-@erase "$(INTDIR)\Card.obj"
	-@erase "$(INTDIR)\Card.sbr"
	-@erase "$(INTDIR)\Pci3tier.obj"
	-@erase "$(INTDIR)\Pci3tier.sbr"
	-@erase "$(INTDIR)\PCISTUFF.OBJ"
	-@erase "$(INTDIR)\PCISTUFF.SBR"
	-@erase "$(INTDIR)\task.obj"
	-@erase "$(INTDIR)\task.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\VERSION.res"
	-@erase "$(OUTDIR)\Pci3tier.map"
	-@erase "$(OUTDIR)\Pci3tier.pdb"
	-@erase "$(OUTDIR)\Runtime.bsc"
	-@erase "..\..\..\Driver 6_X\Bin\Jobs\Pci3tier.rt3"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /Z7 /Od /X /I "..\inc" /I "..\..\inc" /I "..\..\intime\inc" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_WIN32" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Runtime.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\VERSION.res" /i "..\inc" /i "..\..\inc" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Runtime.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\auxrut.sbr" \
	"$(INTDIR)\Card.sbr" \
	"$(INTDIR)\Pci3tier.sbr" \
	"$(INTDIR)\PCISTUFF.SBR" \
	"$(INTDIR)\task.sbr"

"$(OUTDIR)\Runtime.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=RtFlat.lib DcFlat.lib CsFlat.lib cstart32.obj rt.lib ciff3m.lib pcibus.lib /nologo /version:21076.20052 /stack:0x100000,0x2000 /subsystem:console /incremental:no /pdb:"$(OUTDIR)\Pci3tier.pdb" /map:"$(INTDIR)\Pci3tier.map" /debug /debugtype:both /machine:I386 /nodefaultlib /out:"\Program Files\VLC\Driver 6_X\Bin\Jobs\Pci3tier.rt3" /pdbtype:sept /libpath:"..\..\lib" /libpath:"..\..\intime\lib" /heap:0x4000000,0x4000 
LINK32_OBJS= \
	"$(INTDIR)\auxrut.obj" \
	"$(INTDIR)\Card.obj" \
	"$(INTDIR)\Pci3tier.obj" \
	"$(INTDIR)\PCISTUFF.OBJ" \
	"$(INTDIR)\task.obj" \
	"$(INTDIR)\VERSION.res"

"..\..\..\Driver 6_X\Bin\Jobs\Pci3tier.rt3" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Runtime - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\Pci3tier.rta" "$(OUTDIR)\Runtime.bsc"


CLEAN :
	-@erase "$(INTDIR)\auxrut.obj"
	-@erase "$(INTDIR)\auxrut.sbr"
	-@erase "$(INTDIR)\Card.obj"
	-@erase "$(INTDIR)\Card.sbr"
	-@erase "$(INTDIR)\Pci3tier.obj"
	-@erase "$(INTDIR)\Pci3tier.sbr"
	-@erase "$(INTDIR)\PCISTUFF.OBJ"
	-@erase "$(INTDIR)\PCISTUFF.SBR"
	-@erase "$(INTDIR)\task.obj"
	-@erase "$(INTDIR)\task.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\VERSION.res"
	-@erase "$(OUTDIR)\Pci3tier.rta"
	-@erase "$(OUTDIR)\Runtime.bsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /GX /Z7 /Od /X /I "..\inc" /I "..\..\inc" /I "..\..\intime\inc" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_WIN32" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Runtime.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\VERSION.res" /i "..\inc" /i "..\..\inc" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Runtime.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\auxrut.sbr" \
	"$(INTDIR)\Card.sbr" \
	"$(INTDIR)\Pci3tier.sbr" \
	"$(INTDIR)\PCISTUFF.SBR" \
	"$(INTDIR)\task.sbr"

"$(OUTDIR)\Runtime.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=RtFlat.lib DcFlat.lib CsFlat.lib cstart32.obj rt.lib ciff3m.lib pcibus.lib /nologo /version:21076.20052 /stack:0x100000,0x2000 /subsystem:console /pdb:none /debug /machine:I386 /nodefaultlib /out:"$(OUTDIR)\Pci3tier.rta" /libpath:"..\..\lib" /libpath:"..\..\intime\lib" /heap:0x4000000,0x4000 
LINK32_OBJS= \
	"$(INTDIR)\auxrut.obj" \
	"$(INTDIR)\Card.obj" \
	"$(INTDIR)\Pci3tier.obj" \
	"$(INTDIR)\PCISTUFF.OBJ" \
	"$(INTDIR)\task.obj" \
	"$(INTDIR)\VERSION.res"

"$(OUTDIR)\Pci3tier.rta" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("Runtime.dep")
!INCLUDE "Runtime.dep"
!ELSE 
!MESSAGE Warning: cannot find "Runtime.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Runtime - Win32 Release" || "$(CFG)" == "Runtime - Win32 Debug"
SOURCE=.\auxrut.c

"$(INTDIR)\auxrut.obj"	"$(INTDIR)\auxrut.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Card.c

"$(INTDIR)\Card.obj"	"$(INTDIR)\Card.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Pci3tier.c

"$(INTDIR)\Pci3tier.obj"	"$(INTDIR)\Pci3tier.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\PCISTUFF.C

"$(INTDIR)\PCISTUFF.OBJ"	"$(INTDIR)\PCISTUFF.SBR" : $(SOURCE) "$(INTDIR)"


SOURCE=.\task.c

"$(INTDIR)\task.obj"	"$(INTDIR)\task.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\VERSION.RC

"$(INTDIR)\VERSION.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

