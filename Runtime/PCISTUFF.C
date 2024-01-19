/********************************************************************

                                PciStuff.c

    Interface specific code. 
    This file only should touch the hardware.

*********************************************************************/


#include "stdafx.h"

#include <rt.h>
#include <pcibus.h>     // 
#include <string.h>     // strlen()
#include <stdio.h>      // sprintf()
#include <stdlib.h>     // malloc()

#include "vlcport.h"
#include "dcflat.h"             // EROP()
#include "driver.h"             // SEMAPHORE
#include "errors.h"             // IDS_SYNERGETIC_HW_TEST
#include "auxrut.h"             // StartTimeout(), IsTimeout(), EROP
#include "pcistuff.h"           // Init()





/******************* Local definitions *******************************/



/******************* Initialization  *******************************/


/*
// ****************************************************************
//
//  PCI helper functions:
//
///////////////////////////////////////////////////////////////
//
//  Configuration registers in general and BARs in particular
//
*/

/* Make sure the configuration access parameters are valid */
static int CheckConfigRegister( UINT32 Offset, UINT32 Size, UINT8* pSizeCode )
{
    int rc = SUCCESS;

    if( Offset + Size > 256 )
        rc = IDS_ERR_PCI_OUTOF_IOIMAGE;
    else
    {
        switch( Size )
        {
            case 1:
                    *pSizeCode = T_BYTE;
                    break;
            case 2:
                    *pSizeCode = T_WORD;
                    break;
            case 4:
                    *pSizeCode = T_DWORD;
                    break;
            default:
                    *pSizeCode = 0;
                    rc = IDS_ERR_PCI_INVALID_REGSIZE;
        }
    }

    return rc;
}

static int WriteConfigRegister( PCIDEV* pPciDev, UINT32 Offset, UINT32 Size, UINT32 Value )
{
    UINT8 SizeCode;
    int rc = CheckConfigRegister( Offset, Size, &SizeCode );
    if( rc == SUCCESS )
        PciSetConfigRegister( pPciDev, (UINT8)Offset, SizeCode, (DWORD)Value );
    return rc;
}

static int ReadConfigRegister( PCIDEV* pPciDev, UINT32 Offset, UINT32 Size, UINT32* pValue )
{
    UINT8 SizeCode;
    int rc = CheckConfigRegister( Offset, Size, &SizeCode );
    if( rc == SUCCESS )
        *pValue = PciGetConfigRegister( pPciDev, (UINT8)Offset, SizeCode );
    return rc;
}



int vlcWriteConfigRegister( UINT32 PciBusNum, UINT32 PciSlotNum, UINT32 Offset, UINT32 Size, UINT32 Value )
{
    PCIDEV PciDev;
    memset( &PciDev, 0, sizeof( PCIDEV ) );
    PciDev.wBusNum    = (UINT16)PciBusNum;
    PciDev.wDeviceNum = (UINT16)PciSlotNum;
    return WriteConfigRegister( &PciDev, Offset, Size, Value );
}

int vlcReadConfigRegister( UINT32 PciBusNum, UINT32 PciSlotNum, UINT32 Offset, UINT32 Size, UINT32* pValue )
{
    PCIDEV PciDev;
    memset( &PciDev, 0, sizeof( PCIDEV ) );
    PciDev.wBusNum    = (UINT16)PciBusNum;
    PciDev.wDeviceNum = (UINT16)PciSlotNum;
    return ReadConfigRegister( &PciDev, Offset, Size, pValue );
}


static int ReadHeaderType( PCIDEV* pPciDev, UINT8* pHeaderType )
{
    UINT32 HeaderType;
    UINT32 Offset = 0x0e;
    int rc = ReadConfigRegister( pPciDev, Offset, 1, &HeaderType );
    if( rc == SUCCESS )
        *pHeaderType = (UINT8)HeaderType;
    return rc;
}

static int ReadBar( PCIDEV* pPciDev, int BarIndex, UINT32* pValue )
{
    UINT32 Offset = BarIndex * sizeof(UINT32) + 0x10;
    return ReadConfigRegister( pPciDev, Offset, 4, pValue );
}

static int WriteBar( PCIDEV* pPciDev, int BarIndex, UINT32 Value )
{
    UINT32 Offset = BarIndex * sizeof(UINT32) + 0x10;
    return WriteConfigRegister( pPciDev, Offset, 4, Value );
}



/*
///////////////////////////////////////////////////////////////
//
//  Find the configuration for the specified 
//  Vendor/Device/Function set.
//
*/

/* returns TRUE if the next index is found. */
static int vlcPciFindNextDeviceOrDuplicate( PCIDEV* pPciDev )
{
    pPciDev->wDeviceIndex++;
    return PciFindDevice( pPciDev );
}


/* returns TRUE if the next different index is found. */
static int vlcPciFindNextDevice( PCIDEV* pPciDev )
{
    int bFound = 0;
    if( pPciDev->wFunction )
    {
        /* Multi-function device */
        bFound = vlcPciFindNextDeviceOrDuplicate( pPciDev );
    }
    else
    {
        /* Function 0. */
        UINT8 HeaderType;
        if( ReadHeaderType( pPciDev, &HeaderType ) == SUCCESS )
        {
            int bMultiFunctionDevice = HeaderType & 0x80;   /* see PCI specs for Header Type */

            if( bMultiFunctionDevice )
            {
                /* Multi-function device  */
                bFound = vlcPciFindNextDeviceOrDuplicate( pPciDev );
            }
            else
            {
	            /*
                    Single function device.  May show up as more dups, which we need to skip.
                    All duplicates share the same slot. 
                */
                UINT16 BusNum    = pPciDev->wBusNum;
	            UINT16 DeviceNum = pPciDev->wDeviceNum;
                int    bSameSlot;
                
                do
                {
                    bFound    = vlcPciFindNextDeviceOrDuplicate( pPciDev );
                    bSameSlot = ( ( BusNum == pPciDev->wBusNum ) && ( DeviceNum == pPciDev->wDeviceNum ) );
                } while( bFound && bSameSlot );     /* skip all duplicates */
            }
        }
    }
    
    return bFound;
}


/* returns TRUE if there is at least 1 device. */
static int vlcPciFindFirstDevice( PCIDEV* pPciDev, int VendorId, int DeviceId )
{
    memset( pPciDev, 0, sizeof(PCIDEV) );
    pPciDev->wVendorId = VendorId;
    pPciDev->wDeviceId = DeviceId;
    return PciFindDevice( pPciDev );
}




/* BoardIndex is 1 based */
/* called by InitPCI() */
static int FindPCIBoard( LPDRIVER_INST pNet, P_ERR_PARAM pErrors, PCIDEV* pPciDev )
{
    int rc = SUCCESS; 

    UINT16 const  BoardIndex         = pNet->PciIndex;	/* 1-based index,  key to our board. */
    UINT16 const  ExpectedBoardCount = pNet->PciCount;	/* 0 --> any # of boards.  For check purposes only. */
    UINT16        FoundBoardCount    = 0;               /* how many boards are pluged in this system        */

    if( rc == SUCCESS )
    {
        PCIDEV PciDev;

        /*
            TO DO: 
            Define here he VENDOR ID and the DEVICE ID  for our PCI card. 
        */

        const   int VendorId = 0x144a;  // TO DO: define here the VENDOR ID
        const   int DeviceId = 0x8132;  // TO DO: define here the DEVICE ID

        int     bDone  = FALSE;                          /* when to stop looking for new boards of this type. */
        int     bFound = vlcPciFindFirstDevice( &PciDev, VendorId, DeviceId );
        for( ;  bFound && !bDone ; bFound = vlcPciFindNextDevice( &PciDev ) )
        {
            if( ++FoundBoardCount == BoardIndex )
            {
                /* Found our board.  PciDev contains a handle to it. */
                if( !ExpectedBoardCount )   /* if no BoardCount check requested,                   */
                    bDone = TRUE;           /* no need to find the exact # of boards of this type. */

                memcpy( pPciDev, &PciDev, sizeof( PCIDEV ) );
            }
        }
    }

    if( rc == SUCCESS )
    {
        if( FoundBoardCount < BoardIndex )
        {
            /* Not enough boards available on this system */
            rc = IDS_ERR_PCI_CANNOT_FIND_PCIDEVICE;
            sprintf( pErrors->Param3, "%d", BoardIndex );
            sprintf( pErrors->Param4, "%d", FoundBoardCount );
        }
    }

    if( rc == SUCCESS )
    {
        if( ExpectedBoardCount && ( ExpectedBoardCount != FoundBoardCount ) )
        {
            /* BoardCount check requested and failed. */
            rc = IDS_ERR_PCI_BOARD_NO_MISMATCH;
            sprintf( pErrors->Param3, "%d", ExpectedBoardCount );
            sprintf( pErrors->Param4, "%d", FoundBoardCount    );
        }
    }

    return rc;
}





/* called by rtOpen() */
int InitPCI( LPDRIVER_INST pNet, P_ERR_PARAM pErrors )
{
    PCIDEV PciDev;                             /* work variable */
    int rc = FindPCIBoard( pNet, pErrors, &PciDev );

    if( rc == SUCCESS )
        pNet->PhyAddr = PciDev.dwBaseAddr[ 0 ] & 0xfffffff0;

    return rc;
}




























