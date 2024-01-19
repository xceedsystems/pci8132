/****************************************************************************

                            Driver.h

    PCI8132 Driver specfic UIOT structures and adjunct to the network node

*****************************************************************************/



#ifndef __DRIVER_H__
#define __DRIVER_H__


// "PCI8132" id num. Make sure this id is unique across all VLC C-modules and drivers.
#define DriverPCI8132       0x5e7f8d49L


/*
    Version 01.0002
    Bump this version # every time DRIVER_INST, DEVICE_INST or
    DEVICE_IO structs have changed. 
    This will force old projects to be recompiled before execution. 
*/
#define PCI8132VERS         0x00010002L



//  Network config struct id
#define NETCONFIG_ID        DriverPCI8132


/* 
    Network config struct version
    Bump this version # every time NETCONFIG got new fields. 
    NetPass1() will force drivers in old projects to be reconfigured. 
    If old fields and NETCONFIG size are preserved, 
    configuration in old fields will be kept. 
*/
#define NETCONFIG_VERS      0x00010002L



// Device  config struct id
#define DEVCONFIG_ID        0x12345678L    



/*
    Device  config struct version
    Bump this version # every time DEVCONFIG got new fields. 
    NetPass1() will force devices in old projects to be reconfigured. 
    If old fields and DEVCONFIG size are preserved, 
    configuration in old fields will be kept. 
*/
#define DEVCONFIG_VERS      0x00010002L     


// load this value in DRIVER_INST for rt checking
#define RT3_SENTINEL        0x55667788L


// max 4 networks can be controlled by 1 PC
#define  MAX_DRV_INSTANCES          4       



#include  "errors.h"

#ifndef APSTUDIO_READONLY_SYMBOLS


#ifndef VLCPORT_H__
#include "vlcport.h"
#pragma warning( disable: 4244 )
#endif

#ifndef DATASEG_H__
#include "dataseg.h"
#endif


/*****************************************************************************************

    This file keeps all necessary definitions needed for our pci8132 driver.
    This is a driver build for didactical purposes. It can be used as a starting point 
    when we need to create a new real driver.
    The PCI8132 driver assumes we have an IO network. 
    The network is controlled by a card and can have up to MAX_DEVICES devices. 
    The card is accesible by a dual port ram and a port address.
    The DRP is DPR_TOTAL_SIZE bytes large and contains a control space, an input space and
    an output space.
    To keep things simple, our devices have all the same size: 8 bytes. 
    They are mapped in the DPR IO spaces based on their network address: device 0's input
    area can be found at the beginning of the DPR input space, device 1's input area is 
    8 bytes farther...  The same mechanism works for devices' output points. 
    In order to see input tags changing, we have to use an external application 
    that writes in the DPR input space.  We can also use the POKE special function to write there. 
    When generating the driver we can change DPR_OUTPUT_OFF to match DPR_INPUT_OFF. 
    Input space will then overlap the output space, so in the VLC project all input tags 
    will be animated by their correspondent output tags.    

    Configuring the driver:
    1.  Choose a DPR address.
    2.  Choose a port address (didactic purpose only: will not be used)
    3.  Choose an interrupt level (didactic purpose only: the interrupt routine does nothing)
    4.  Skip HW tests. We may want to control the thoroughness of initial hw tests.
    5.  Simulate:  if on, there will be no attempt to touch the hardware.
    6.  Watchdog:  if on, the card's watchdog must be cyclicly kicked at run time.
    7.  Cyclic Input Read:  if on,  we update UIOT input image every Input() call.
                            if off, we have to rely on some hardware features telling us if 
                                    any input changed.
    
    Configuring devices:
    1.  Choose a link address (0 ... 127). This determines allocation in the DPR IO space
    2.  Critical:  if on, this device must be active at Online()    
    

    There are 5 different driver models we are studying:
    Model1:     No special functions at all.    (Simulate)
    Model2:     Only sync s.f. No background task.  (PID, Utility, ...)
    Model3:     Sequential async s.f. processing:  Pend & Done lists. (ex. ABKTX, MTL, ...)
                    DRIVER_INST needs  MarkTime.
    Model4:     Simoultaneous async s.f. processing: Pend, Run, Done lists (ex. DHPLUS, ...)
                    The hw supports commands with reply. 
                    New commands can be launched while others are waiting for their replies.
    Model5:     Paralel sequential s.f. processing. 
                    The hw supports a fixed # of channels that can accept commands.
                    Commands executed on different channels can run in paralel.
                    Commands executed on a channel are performed on a FIFO basis.
                    Pend[], Done lists    (DATALOG)
                    DRIVER_INST needs  MarkTime[].
    

    Here is an example for model 3:    
    
*****************************************************************************************/



//************   1 byte alignment typedef structures !!!   ************

#pragma BYTE_ALIGN(_SPECIAL_INST)
typedef  struct _SPECIAL_INST*  LPSPECIAL_INST_;
typedef                 UINT8*  LPUINT8;

typedef  selector   SEMAPHORE;

typedef  UINT32     UIOTREF2CHAR;
typedef  UINT32     UIOTREF2SINT8;
typedef  UINT32     UIOTREF2UINT8;
typedef  UINT32     UIOTREF2SINT16;
typedef  UINT32     UIOTREF2UINT16;
typedef  UINT32     UIOTREF2SINT32;
typedef  UINT32     UIOTREF2UINT32;
typedef  UINT32     UIOTREF2DOUBLE;
typedef  UINT32     UIOTREF2VOID;


#pragma BYTE_ALIGN(_LINKED_LIST)
typedef struct _LINKED_LIST 
{
    LPSPECIAL_INST_     pHead;      // Pointer to the first element in the linked list
    LPSPECIAL_INST_     pTail;      // Pointer to the last element in the linked list
    SEMAPHORE           Semaphore;  // Semaphore that locks the SPECIAL_INST list
    UINT16              uCounter;   // How many items are enqueued here
} LINKED_LIST, *LPLINKED_LIST; 
#pragma BYTE_NORMAL()


#pragma BYTE_ALIGN(_PTBUFFER)
typedef struct _PTBUFFER 
{
    UIOTREF2VOID    Offset;  
    UINT32          Size;           // Use PTBUFFER type for PT_BUFFERs
} PTBUFFER, * LPPTBUFFER;           // Its size is 8 bytes
#pragma BYTE_NORMAL()


#pragma BYTE_ALIGN(_TASK)
typedef struct _TASK 
{
    UINT16      hTask;          // background/interrupt task handle
    SEMAPHORE   Semaphore;      // Where the background task waits
    void*       pStack;         // Pointer to the stack allocated to the task
    UINT16      bBusy;          // True if Special I/O Task is working on packet, used during shutdown
    UINT16      Error;          // error code for the task's init sequence
    void*       IrqThunk;       // pointer to the interrupt routine
    UINT16      level;          // irmx encoded IRQ
    UINT16      align;
} TASK, * LPTASK; 
#pragma BYTE_NORMAL()





#pragma BYTE_ALIGN(_DEVICE_IO)  // 1 byte alignment
typedef struct _DEVICE_IO       // Specifies the UIOT offset and the size for each device
{
    void*        pSrc;          // DPR  offset/pointer for input devices || UIOT offset/pointer for output devices
    void*        pDst;          // UIOT offset/pointer for input devices || DPR  offset/pointer for output devices
    UIOTREF2VOID ofsUiot;       // ofsDst for input devices or ofsSrc for output devices
    UINT16       Size;          // device input or output size.  Never 0 !!!
    UINT16       bUsed;         // If no I/O tags defined in the UIOT, skip it
} DEVICE_IO, *LPDEVICE_IO;            
#pragma BYTE_NORMAL()


#pragma BYTE_ALIGN(_DEVICE_INST)         // 1 byte alignment
typedef struct _DEVICE_INST
{
    UIOTREF2VOID ofsName;       // UIOT offset to the device name generated at compile time
    UINT16       Address;       // device's network address
    UINT16       Type;          // DEVICE_1W_INPUT, ... Never 0 !!!
    UINT16       Gain;	
	UINT16       Bipolar;
    UINT16       bPresent;      // if 1 --> device was online when load and go
    char*        pName;         // Usable UIOT pointer to the device name generated at runtime based on ofsName.
	DEVICE_IO	 Input;			// how to handle the input image
	DEVICE_IO	 Output;		// how to handle the output image
	UINT32       Sentinel;
} DEVICE_INST, *LPDEVICE_INST;
#pragma BYTE_NORMAL()




#pragma BYTE_ALIGN(_DRIVER_INST) 
typedef struct _DRIVER_INST 
{
    NETWORK Net;

        // Compile-time Static variables.  This structure maps to UIOT

    UIOTREF2VOID    ofsDeviceList;      // Where the DEVICE_INST list starts.
    UINT32          nDevices;           // how many Devices are in the list
	UINT16			PciIndex;			// 1...8  which board
	UINT16			PciCount;			// 0...8  how many boards expected in the system. 0--> skip test.
	UINT32          PhyAddr;            // BAR0 read from the PCI card
    UINT16          NumChannels;        // 
    UINT16          bSimulate;          // =0 --> interface card must be present
    
        // Run-time Dynamic Variables
    LPDEVICE_INST   pDeviceList;        // Where the DEVICE_INST list starts.

	void* 			pDpr;               // virtual pointer built based on BAR0
    UINT16          bFirstCycle;        // Set by OnLine(), reset by Output(). Read by Input() and Output()
    UINT16          bGoOffLine;         // Tell all the bkg functions to shutdown

    LINKED_LIST     Pend;               // Pointer to the linked list of pending functions
    LINKED_LIST     Done;               // Pointer to the linked list of done  functions

    TASK            BackgroundTask;     // controls for the background task
    TASK            InterruptTask;      // controls for the interrupt task

    UIOTREF2UINT32  ofsSentinel;        // 0x55667788 - display this value using Soft Scope to check corrct map
    UINT32          Sentinel;           // 0x55667788 - display this value using Soft Scope to check corrct map

} DRIVER_INST, *LPDRIVER_INST;    
#pragma BYTE_NORMAL() 


#pragma BYTE_ALIGN( _SPECIAL_INST_HEADER )      // Must be first block in all paremeter blocks
typedef struct _SPECIAL_INST_HEADER
{       // Compile-time Static variables.  This structure maps to .rcd descrition
                                    // off, sz, ob.sz
    UINT16          FunctionId;     //  0    2   2L   PT_CONST  --> UINT16, _SIZE 2L
    UINT16          align;          //  2    2
    UIOTREF2UINT16  ofsStatus;      //  4    4   2L   PT_REF    --> tag's offset in the UIOT
    UIOTREF2UINT16  ofsResult;      //  8    4   2L   PT_REF    --> tag's offset in the UIOT
} SPECIAL_INST_HEADER;              //      12 == sizeof( SPECIAL_INST_HEADER )
#pragma BYTE_NORMAL()
/*
    Note: beacuse all functions have an Id field and a return status, we can standardize them 
    at offsets 0 and 4. This is especially helpful when using customized parameter structures 
    to better match function particularities and to save memory. 
*/


#pragma BYTE_ALIGN( _SPECIAL_INST_COMMAND ) 
typedef struct _SPECIAL_INST_COMMAND
{       // Compile-time Static variables.  This structure maps to .rcd descrition
                                        // off, sz, ob.sz
    SPECIAL_INST_HEADER Header;         //  0   12        the header must always be first
    UINT16              Address;        // 12    2   2L   PT_VALUE, PT_DEVICE --> UINT32
	UINT16				Spare0;			// 14	 2
    UINT16				Function;		// 16    2   2L   PT_REF    --> tag's offset in the UIOT
	UINT16				Spare1;			// 18	 2
	UINT32				Spare2;			// 20	 4

  //  double		fPos;			// 24    8   8L   PT_BUFFER --> tag's offset in the UIOT
	double			fPos;			// 24    8   8L   PT_BUFFER --> tag's offset in the UIOT
	double      stVel;
	double      maxVel;
	double      Accl;
	double      Deccl;
	double      TAccl;
	double      TDeccl;
	double      T1Accl;
	double      T1Deccl;
	double      TSAccl;
	double      TSDeccl;
//	UIOTREF2DOUBLE      Buffer[4];

} SPECIAL_INST_COMMAND;                 //     152 == sizeof( SPECIAL_INST_COMMAND )
#pragma BYTE_NORMAL()


#pragma BYTE_ALIGN( _SPECIAL_INST_OTHERS ) 
typedef struct _SPECIAL_INST_OTHERS
{       // Compile-time Static variables.  This structure maps to .rcd descrition
                                        // off, sz, ob.sz
    SPECIAL_INST_HEADER Header;         //  0   12        the header must always be first
    UINT16              Address;        // 12    2   2L   PT_VALUE, PT_DEVICE --> UINT32
    UINT16				Function;		// 14	 2		

    UINT32		GetValue;			//16
	UINT32		SetValue1;			//20
	UINT32		SetValue2;			//24
	UINT32		SetValue3;			//28
	UINT32		SetValue4;			//32
	UIOTREF2SINT32		SetValue5;			//36
	

} SPECIAL_INST_OTHERS;                 //    44
#pragma BYTE_NORMAL()






#pragma BYTE_ALIGN( _SPECIAL_INST_SETGET ) 
typedef struct _SPECIAL_INST_SETGET
{       // Compile-time Static variables.  This structure maps to .rcd descrition
                                        // off, sz, ob.sz
    SPECIAL_INST_HEADER Header;         //  0   12        the header must always be first
    UINT16              Address;        // 12    2   2L   PT_VALUE, PT_DEVICE --> UINT32
	UINT16				Spare0;			// 14	 2
    UINT16				Function;		// 16    2   2L   PT_REF    --> tag's offset in the UIOT
	UINT16				Spare1;			// 18	 2
	UINT32				Spare2;			// 20	 4

    double		SetValue;			// 24    8   8L   PT_BUFFER --> tag's offset in the UIOT
	UIOTREF2DOUBLE      GetValue;
	

} SPECIAL_INST_SETGET;                 //     152 == sizeof( SPECIAL_INST_COMMAND )
#pragma BYTE_NORMAL()



#pragma BYTE_ALIGN(_SPECIAL_INST_PORT)  // we may have substitutes for SPECIAL_INST_PARAM
typedef struct _SPECIAL_INST_PORT
{       // Compile-time Static variables.  This structure maps to .rcd descrition
                                        // off, sz, ob.sz
    SPECIAL_INST_HEADER Header;         //  0   12        the header must always be first
    UINT16              Address;        // 12    2   2L   PT_VALUE  --> UINT16, _SIZE 2L
    UINT16              Length;         // 14    2   2L   PT_VALUE  --> UINT16, _SIZE 2L
    UIOTREF2UINT16      ofsInValue;     // 16    4   2L   PT_REF    --> tag's offset in the UIOT 
    UINT16              OutValue;       // 20    2   2L   PT_VALUE  --> UINT16, _SIZE 2L
    UINT16              align;          // 22    2   2L   PT_VALUE  --> UINT16, _SIZE 2L
} SPECIAL_INST_PORT;                    //      24 == sizeof( SPECIAL_INST_PORT )
#pragma BYTE_NORMAL()

typedef union _SPECIAL_INST_PARAM
{       // Compile-time Static variables.  This structure maps to .rcd descrition
                                        // off, sz
    SPECIAL_INST_HEADER  paramHeader;   //  0   12
    SPECIAL_INST_COMMAND paramCommand;  //  0   48
    SPECIAL_INST_PORT    paramPort;     //  0   24
	SPECIAL_INST_SETGET  paramSetGet;
	SPECIAL_INST_OTHERS  paramOthers;

} SPECIAL_INST_PARAM;                   //      48 == sizeof(SPECIAL_INST_PARAM)


typedef struct _SPECIAL_INST
{       // Compile-time Static variables.  This structure maps to .rcd descrition
                                        // off,  sz
    SPECIAL_INST_PARAM  User;           //   0   48
    SPECIAL_INST_PARAM  Work;           //  48   48

        // generic, same for all drivers having asyncronous special functions
    UINT32                MarkTime;     //  96    4  when this s.f. must be complete
    SINT16                Status;       // 100    2
    UINT16                Busy;         // 102    2    
    struct _SPECIAL_INST* pNext;        // 104    4

} SPECIAL_INST, *LPSPECIAL_INST;        //      108 == sizeof( SPECIAL_INST )

/*
Note1: This struct is declared 1 byte aligned on top of file. The struct description is 
       evaluated by the "Runtime" sub-project only.  
       The 'Gui' subproject evaluates the SPECIAL_INST parameter block as presented 
       by the FNC_... definitions. 

Note2: For a very simple function module,  SPECIAL_INST is sufficient.  
       Parameter fields can be described directly in SPECIAL_INST.  
       SPECIAL_INST_PARAM, SPECIAL_INST_PORT, SPECIAL_INST_COMMAND and SPECIAL_INST_HEADER 
       are optional.  They have been defined here only to show a more complex example. 

Note3: In order to save memory SPECIAL_INST can be used only for asynchronous special functions. 
       SPECIAL_INST_COMMAND, SPECIAL_INST_PORT, or even SPECIAL_INST_HEADER 
       will do the job for synchronous special functions. 
       Make sure the correct param block size is declared NET(DEV)_FUNC_TYPE paragraph (p#2).

Note4: Because asynchronous functions are executed concurenlty with the flowchart code, 
       it is safer to provide a copy of the parameter block, to be used by the background thread. 
       This is why we have introduced the 'User' and 'Work' areas. 
       'User' is the area marked by the compiler to be filled in every time a function 
       is called. When the function is posted for execution, 'User' is copied into 'Work' 
       and 'Work' is what the background sees.
       Make sure the fields in 'User' and 'Header' match the FNC_... definitions. 
       It is a good idea to have them both mapped at offset 0.

Note5: The Runtime Special() entry point offers a pointer to the associated SPECIAL_INST. 
       Depending on the FunctionId, the right parameter layout will be selected. 
       This can be implemented in 3 ways: 
       a. Define 1 layout only large enough to encompass all parameters needed by any function. 
       b. Define 1 layout for every function, and cast to the right one based on the FunctionId. 
       c. Define 1 layout for every function, store them into a union and select the right 
          union branch based on the FunctionId. 
       Our current implementation is a mixture of a. and c. and should be optimal 
       for consumed memory and code complexity. 
*/


#ifdef WINVER          // This is for MSVC compiler


#ifndef DRVRUTIL_H__
#include "drvrutil.h"   // SS_ZERO
#endif

// What we put into the database for network config

#pragma BYTE_ALIGN(_NETCONFIG)     // 1 byte alignment
typedef struct _NETCONFIG
{
    UINT32           NetcfgId;          //  0  NETCONFIG_ID
    UINT16           NetcfgVersMinor;   //  4  LOW(  NETCONFIG_VERS )
    UINT16           NetcfgVersMajor;   //  6  HIGH( NETCONFIG_VERS )
    UINT32           PciIndex;          //  8  1...8 max 8 boards in the system
    UINT16           PciCount;          // 12  0...8 if 0--> skip the count test
    UINT16           IrqLevel;          // 14  0 ... 15
    UINT16           BaudRate;          // 16  BAUDRATE_125, BAUDRATE_250, ...
    UINT16			 NumChannels;       // 18  8 or 16 channels

    UINT16           bSimulate;         // 20  =0 --> interface card must be present
    UINT16           bWatchdog;         // 22  =1 --> kick the watchdog
    UINT16           DprHWTests;        // 24  HWTEST_RW, HWTEST_OFF
    UINT16           InputRead;         // 26  INPUT_READ_CYCLIC, INPUT_READ_COS,
    
    UINT8            Reserved[128-28];  // 28  add new fields without changing NETCONFIG size
} NETCONFIG;                            // 128  == NET_CONFIG_SIZE == sizeof(NETCONFIG)
#pragma BYTE_NORMAL()


#pragma BYTE_ALIGN(_DEVCONFIG)  // 1 byte alignment
typedef struct _DEVCONFIG
{                               // Byte Offset
    UINT32  NetcfgId;           //  0 NETCONFIG_ID
    UINT32  DevcfgId;           //  4 DEVCONFIG_ID
    UINT16  DevcfgVersMinor;    //  8 LOW(  DEVCONFIG_VERS )
    UINT16  DevcfgVersMajor;    // 10 HIGH( DEVCONFIG_VERS )
    UINT16  Address;            // 12 device's address on the link
    UINT16  Reserved0;          // 14 =1 --> this device must be present on the link
	UINT16  Gain;				// 16 Desired gain for input
	UINT16	Bipolar;			// 18 TRUE if this is a bipolar channel
    UINT8   Reserved[128-20];	// 20 add new fields without changing NETCONFIG size
} DEVCONFIG;                    // 128 == DEVCONFIG_SIZE == sizeof(DEVCONFIG)
#pragma BYTE_NORMAL()
/*
    Note: The reserved fields will be used for future developpment. 
    They ensure compatibility with projects generated by older versions of this driver.
*/


#endif      // WINVER


#endif      // ! APSTUDIO_READONLY_SYMBOLS

/* 
    Defines for .rcd file 
    Arithmetic expressions are allowed to define RC and RCD constants, 
    when  ONLY using + and -.  
    It is a good idea to have them encapsulated in ( ).
    Never use * and /.  The RC compiler silently ignores them.
*/


// SPECIAL_INST offsets & sizes
#define FNC_HD_FUNCTIONID           0L 
#define FNC_HD_FUNCTIONID_SIZE          2L      // PT_CONST  --> size 2L    
#define FNC_HD_STATUS               4L 
#define FNC_HD_STATUS_SIZE              2L      // PT_REF --> size of the object pointed to
#define FNC_HD_RESULT               8L 
#define FNC_HD_RESULT_SIZE              2L      // PT_REF --> size of the object pointed to

#define FNC_CM_AXIS_NO              12L    
#define FNC_CM_AXIS_NO_SIZE             2L      // PT_VALUE, PT_DEVICE  --> size 4L    
#define FNC_CM_FUN_SEL             16L    
#define FNC_CM_FUN_SEL_SIZE            2L      // PT_REF --> size of the object pointed to

#define FNC_CM_FINAL_POS             24L    
#define FNC_CM_FINAL_POS_SIZE            8L      // PT_VALUE  --> size 4L    
#define FNC_CM_ST_VEL              32L 
#define FNC_CM_ST_VEL_SIZE              8L      // PT_BUFFER --> size 8L
#define FNC_CM_MAX_VEL               40L 
#define FNC_CM_MAX_VEL_SIZE              8L      // PT_BUFFER --> size 8L
#define FNC_CM_ACCL               48L 
#define FNC_CM_ACCL_SIZE              8L      // PT_BUFFER --> size 8L
#define FNC_CM_DECCL               56L 
#define FNC_CM_DECCL_SIZE              8L      // PT_BUFFER --> size 8L
#define FNC_CM_TACCL               64L 
#define FNC_CM_TACCL_SIZE              8L      // PT_BUFFER --> size 8L
#define FNC_CM_TDECCL               72L 
#define FNC_CM_TDECCL_SIZE              8L     // PT_BUFFER --> size 8L
#define FNC_CM_T1ACCL               80L 
#define FNC_CM_T1ACCL_SIZE              8L      // PT_BUFFER --> size 8L
#define FNC_CM_T1DECCL              88L 
#define FNC_CM_T1DECCL_SIZE              8L      // PT_BUFFER --> size 8L
#define FNC_CM_TSACCL               96L 
#define FNC_CM_TSACCL_SIZE              8L      // PT_BUFFER --> size 8L
#define FNC_CM_TSDECCL               104L 
#define FNC_CM_TSDECCL_SIZE              8L      // PT_BUFFER --> size 8L

#define FNC_PO_ADDRESS              12L    
#define FNC_PO_ADDRESS_SIZE             2L      // PT_VALUE --> 2L
#define FNC_PO_LENGTH               14L    
#define FNC_PO_LENGTH_SIZE              2L      // PT_VALUE --> 2L
#define FNC_PO_IN_VALUE             16L 
#define FNC_PO_IN_VALUE_SIZE            2L      // PT_REF --> size of the object pointed to
#define FNC_PO_OUT_VALUE            20L    
#define FNC_PO_OUT_VALUE_SIZE           2L      // PT_VALUE --> 2L


#define FNC_CM_AXIS_NO              12L    
#define FNC_CM_AXIS_NO_SIZE             2L      // PT_VALUE, PT_DEVICE  --> size 4L    
#define FNC_CM_FUN_SEL             16L    
#define FNC_CM_FUN_SEL_SIZE            2L      // PT_REF --> size of the object pointed to

#define FNC_CM_SET             24L    
#define FNC_CM_SET_SIZE            8L      // PT_VALUE  --> size 4L    
#define FNC_CM_GET             32L 
#define FNC_CM_GET_SIZE              8L      // PT_BUFFER --> size 8L



#define FNC_OT_AXIS_NO              12L    
#define FNC_OT_AXIS_NO_SIZE             2L      // PT_VALUE, PT_DEVICE  --> size 4L    
#define FNC_OT_FUN_SEL             14L    
#define FNC_OT_FUN_SEL_SIZE            2L      // PT_REF --> size of the object pointed to

#define FNC_OT_GET             16L 
#define FNC_OT_GET_SIZE              4L      // PT_BUFFER --> size 8L
#define FNC_OT_SET1             20L    
#define FNC_OT_SET_SIZE            4L      // PT_VALUE  --> size 4L    
#define FNC_OT_SET2             24L    
#define FNC_OT_SET3             28L    
#define FNC_OT_SET4             32L    
#define FNC_OT_SET5             36L    

#define FNC_OT_SET_SIZE5            8L      // PT_VALUE  --> size 4L    


#define FNC_HD_SPECIAL_INST_SIZE        12
#define FNC_CM_SPECIAL_INST_SIZE        112
#define FNC_SG_SPECIAL_INST_SIZE		48
#define FNC_PO_SPECIAL_INST_SIZE        24
#define FNC_OT_SPECIAL_INST_SIZE        48
#define FNC_SPECIAL_INST_SIZE           112


// NETCONFIG offsets & sizes
#define NET_ID                      0 
#define NET_ID_SIZE                     32 
#define NET_VERS                    4 
#define NET_VERS_SIZE                   32 
#define NET_PCI_INDEX               8 
#define NET_PCI_INDEX_SIZE              32 
#define NET_PCI_COUNT               12
#define NET_PCI_COUNT_SIZE              16 
#define NET_NUMCHANNELS             18 
#define NET_NUMCHANNELS_SIZE            16 
#define NET_SIMULATE                20 
#define NET_SIMULATE_SIZE               16 
#define NET_WATCHDOG                22 
#define NET_WATCHDOG_SIZE               16 
#define NET_HWTEST                  24 
#define NET_HWTEST_SIZE                 16 
#define NETCONFIG_SIZE              128 

// DEVICECONFIG offsets & sizes
#define DEV_DRVID                   0
#define DEV_DRVID_SIZE                  32
#define DEV_ID                      4
#define DEV_ID_SIZE                     32
#define DEV_VERS                    8
#define DEV_VERS_SIZE                   32
#define DEV_ADDRESS                 12 
#define DEV_ADDRESS_SIZE                16 
#define DEV_GAIN                    16 
#define DEV_GAIN_SIZE                   16 
#define DEV_BIPOLAR                 18 
#define DEV_BIPOLAR_SIZE                16 
#define DEVCONFIG_SIZE              128 


// This is for the demo only.  Replace it with the real settings. 
// Dual port ram layout
#define  DPR_CONTROL_OFF        0
#define  DPR_CONTROL_SIZE       2048
#define  DPR_INPUT_OFF          2048    // where the input image can be found in the dpr
#define  DPR_INPUT_SIZE         1024    // 1kbyte =  MAX_DEVICES * 8bytes input devices
#define  DPR_OUTPUT_OFF         3072    // where the output image can be found in the dpr
//#define  DPR_OUTPUT_OFF       2048    // for didactic purposes use 2048 --> outputs will be looped back in inputs
#define  DPR_OUTPUT_SIZE        1024    // 1kbyte =  MAX_DEVICES * 8bytes input devices
#define  DPR_TOTAL_SIZE         4096    // 4 kbytes
#define  MAX_DEVICES            16      // max 16 devices allowed by our didactical network


/*
#define  DPADR_MIN              0x80000L
#define  DPADR_MAX              0xef000L
#define  DPADR_STEP             0x01000L    // 4 kbytes increments
#define  DPADR_DEFAULT          0xd0000L

#define  PORT_MIN               0x100
#define  PORT_MAX               0x3fc
#define  PORT_STEP              4           // 4 bytes increments
#define  PORT_DEFAULT           0x250

#define  NO_IRQ                 0L
#define  HWTEST_RW              1
#define  HWTEST_OFF             0
#define  INPUT_READ_COS         1
#define  INPUT_READ_CYCLIC      0

#define  BAUDRATE_125           1
#define  BAUDRATE_250           2
#define  BAUDRATE_500           3
*/


#define  DEVICE_CARD_POS            10      

#define  DEVICE_CARD_IO_STATUS0      20     
#define  DEVICE_CARD_IO_STATUS1      21     
#define  DEVICE_CARD_IO_STATUS2      22      
#define  DEVICE_CARD_IO_STATUS3      23      
#define  DEVICE_CARD_IO_STATUS4      24      

#define  DEVICE_CARD_INT_STATUS0     30      
#define  DEVICE_CARD_INT_STATUS1     31     
#define  DEVICE_CARD_INT_STATUS2     32     
#define  DEVICE_CARD_INT_STATUS3     33     
#define  DEVICE_CARD_INT_STATUS4     34

// io     
#define  DEVICE_1W_IANDO			 40
//#define  DEVICE_CARD_POS            10     



#define  DRVF_MOTION               2105    
#define  DRVF_SETGET               2106    
#define  DRVF_SETAXIS              2107
#define  DRVF_OTHERS               2108    
#define  DRVF_ONFLY                2109

#define  DEVICE_FUNC                2010    // special device functions ids
#define  DEVF_GET_DEVSTAT           2200    // functions at device level

#define  LIST_FUNCTION1				3001
#define  LIST_FUNCTION2				3002
#define  LIST_FUNCTION3				3003
#define  LIST_FUNCTION4				3004


// #define  MAX_LENGTH                 400


#define  START_AA_MOVE	100
#define  START_A_MOVE	101
#define  A_MOVE			102
#define  START_R_MOVE	103
#define  R_MOVE			104
#define  START_T_MOVE	105
#define  T_MOVE			106

#define  START_TA_MOVE	108
#define  TA_MOVE		109
#define  START_S_MOVE	110
#define  S_MOVE			111
#define  START_RS_MOVE	112
#define  RS_MOVE		113
#define  START_TAS_MOVE 114
#define  TAS_MOVE		115

#define  V_MOVE			120
#define  SV_MOVE		121
#define  MANU_MOVE		122
#define  V_CHANGE		200

#define  V_STOP			201

#define  AXISMOTIONDONE 202
#define  WAITAXISDONE	203


#define  SETMAXVEL		205
#define  GETPOS			206
#define  SETPOS			207
#define  GETCOMMAND		208
#define  SETCOMMAND		209

#define  SETHOMECONFIG	300
#define  HOMEMOVE		301

#define  STARTMOTION	302
#define  STOPMOTION		303

#define  SETMOVERATIO	304

#define  SETAXISCONFIG		400
#define  SETSVONOFF			401
#define	 SETMAN_IPMODE		402
#define	 SET_PLS_OUTMODE	403
#define  SET_PLS_INMODE		404
#define  SET_CNTMODE		405
#define  SETALARM_LOGIC		406
#define  SETIN_LOGIC		407
#define	 SETINTZERO			408


// On the fly setting


#define	 SETCOMPMODE	500
#define	 SETCOMPCNT		501
#define	 SETCOMDATA		502
#define	 SETCOMPINT 	503
#define	 SETCOMPHOME	504
#define	 SETCOMPTABLE	505

#define	 GETCOMPCNT		510
#define	 GETCOMPDATA	511
#define	 GETCOMPSTS		512

#define	 BUILDCOMPTABLE	520
#define	 BUILDCOMPFUNCT	521




#endif       // __DRIVER_H__ 




