/*********************************************************************

                        devices.cpp

**********************************************************************/


#include "stdafx.h"

#include <windows.h>
#include <windowsx.h>
#include <ctype.h>

#include "drvrsym.h"
#include "resource.h"       // IDS_ERR_...
#include "driver.h"         // PCI3TIER driver specifics
#include "goodies.h"        // interface to goodies.cpp
#include "devices.h"        // interface to devices.cpp



/*********************************************************************************************/
/****************************** Usefull PCI3TIER classes *************************************/



/*********************************************************************************************/
/******************************* CDevice Implementation **************************************/

CDevice::CDevice( HDriverSym* pDevSym )
    : m_arrInput( pDevSym->GetName() ), m_arrOutput( pDevSym->GetName() )
{
    m_pDev  = pDevSym;
    m_pName = pDevSym->GetName();
    memset( &m_DeviceInst, 0, sizeof(DEVICE_INST) );
    m_DeviceInst.Type = (UINT16)m_pDev->GetId();

    DebugString( "creating device %s\n", m_pName );
}

CDevice::~CDevice()
{
    DebugString( "removing device  %s\n", m_pName );
    delete m_pDev; 
}


CIOPointArray* CDevice::GetPointArray( UINT16 Point, SymAccess Access, UINT16& PointOffset )
{
    // Point        is the point id as defined in the RCD file. 
    // PointOffset  is the point offset (in bits) in the device space. 
    // PointOffset  is calculated using Point:  PointOffset = function( Point ); 
    // In our simple case: PointOffset = Point;
    
    CIOPointArray* pPointArray = NULL; 

    if( Access == saInput)
    {
        PointOffset = Point;
        pPointArray = &m_arrInput;
    }

    if( Access == saOutput)
    {
        PointOffset = Point;
        pPointArray = &m_arrOutput;
    }

    return  pPointArray;
}



// makes sure  m_DeviceInst  is can safely be used at run time
int CDevice::DevValid( )
{
    int rc = SUCCESS;
    return rc;
}



// test if the device is configured and load config info into DEVICE_INST
int CDevice::LoadDevice()
{
    DEVCONFIG   DeviceCfg;  // where config data is loaded from symdb

    int rc = ListConfigGet( DeviceCfg );

    if( rc == SUCCESS )
    {
        m_DeviceInst.Address    = DeviceCfg.Address ;
        m_DeviceInst.Gain		= DeviceCfg.Gain;
		m_DeviceInst.Bipolar	= DeviceCfg.Bipolar;
        m_DeviceInst.Sentinel   = RT3_SENTINEL;

        m_DeviceInst.Input.Size  = InputSizer();               // let runtime know how large is the  input image of this device
        m_DeviceInst.Output.Size = OutputSizer();              // let runtime know how large is the output image of this device

        m_arrInput.m_UiotSize  = m_DeviceInst.Input.Size;      // let the CIOPointArray know how large is
        m_arrOutput.m_UiotSize = m_DeviceInst.Output.Size;     // the area where points can be defined
    }
    else
    {
        rc = IDS_CP_DEVICE_NOT_CONFIGURED;
        Erop( rc, m_pName, "", "", "" );
    }


    return rc;
}




int CDevice::Load( )
{
    int rc = LoadDevice();

    if(rc == SUCCESS)
        rc = DevValid();

    if( rc == SUCCESS )
    {
        if( ListStart())    /* scan all points */
        {
            do
            {
                int rc1 = SUCCESS;
                
                HDriverSym* pPnt     = ListGet();
                SymAccess   Access   = pPnt->GetAccess();     // saInput, saOutput
                UINT16      Point    = pPnt->GetPoint();      // point offset in bits   (0xffff for strobe out)
                LPCSTR      pPntName = pPnt->GetName();       // user's name for that I/O tag
        
                UINT16         PointOffset = 0;
                CIOPointArray* pPointArray = GetPointArray( Point, Access, PointOffset );

                if( pPointArray != NULL )
                {
                    CIOPoint* pNewPoint = new CIOPoint( pPnt, PointOffset );

                    rc1 = pPointArray->Check( pNewPoint );

                    if( rc1 == SUCCESS )
                        pPointArray->Add( pNewPoint );
                    else
                        delete pNewPoint;               // also deletes *pPnt 
                }
                else
                {
                    rc1 = IDS_CP_INVALID_POINT;
                    Erop( rc1, pPntName, m_pName, "", "");
                    delete pPnt;
                }

                if( rc1 != SUCCESS )
                    rc = FAILURE;

            } while( ListNext());
        }
        ListEnd();

		//  If there are any input points defined here
        if( m_arrInput.GetSize() )
            m_DeviceInst.Input.bUsed = 1;   // set the 'used' flag
        else  
            m_arrInput.m_UiotSize =  0;     // otherwise do not reserve UIOT space

		// If there are any output points defined here
        if( m_arrOutput.GetSize() )
            m_DeviceInst.Output.bUsed = 1;  // set the 'used' flag
        else 
            m_arrOutput.m_UiotSize =  0;    // otherwise do not reserve UIOT space
    }

    DebugString( "CDevice::Load(). Name=%s, rc=%d\n", m_pName, rc );
    
    return rc;
}



UINT16 CDevice::InputSizer()
{
    UINT16 Size = 0;

    switch( m_DeviceInst.Type )
    {
        case DEVICE_CARD_POS:
                    Size = sizeof(UINT32)*1;
					break;
        case DEVICE_CARD_IO_STATUS1:
					Size = sizeof(UINT16)*2;
					break;

        case DEVICE_CARD_INT_STATUS1:
					Size = sizeof(UINT32)*1;
					break;

		case DEVICE_1W_IANDO:
					Size=sizeof(UINT8)*2;
    }
    return Size;
}


UINT16 CDevice::OutputSizer()
{
    UINT16 Size = 0;


    switch( m_DeviceInst.Type )
    {


        case DEVICE_CARD_INT_STATUS1:
					Size = sizeof(UINT32)*1;
					break;
		case DEVICE_1W_IANDO:
					Size=sizeof(UINT8)*2;

	}


    return Size;
}


// if there are input tags defined --> returns the device input size
// else --> returns 0
UINT16 CDevice::GetUiotInputSize()  
{
    return m_arrInput.m_UiotSize;
}


// if there are output tags defined --> returns the device output size
// else --> returns 0
UINT16 CDevice::GetUiotOutputSize()
{
    return m_arrOutput.m_UiotSize;
}


UINT16 CDevice::GetInputSize()         // Device  input size
{
    return m_DeviceInst.Input.Size;
}


UINT16 CDevice::GetOutputSize()        // Device output size
{
    return m_DeviceInst.Output.Size;
}


int CDevice::Check( CDevice* pDevice )      // makes sure "this" can live with pDevice
{
    int rc = SUCCESS;

    if( m_DeviceInst.Address == pDevice->m_DeviceInst.Address )
   {
//     rc = IDS_CP_SAME_DEVICE_ADDRESS;
		rc = SUCCESS;
	}

    return rc; 
}


//*************************************************************************************
// Called in NetPass2 when we have real UIOT offsets for Config, Input and Output areas
// Also increment the Name, PrgInput and PrgOutput global offsets. 
void CDevice::UpdateOffsets( UINT32& ofsNames, UINT32& PrgInputOfs, UINT32& PrgOutputOfs, UINT32 IODelta )
{
	UINT32 PrgOfs;

	m_DeviceInst.ofsName = ofsNames;
    
    // increment the 'name' offset
    ofsNames += strlen( m_pName ) + 1;

	// Set the offset for the input image
	{
		PrgOfs = 0;

		// If there are any input tags defined here
		if( m_arrInput.m_UiotSize )
		{
			// store the associated offset in the drv image
			m_DeviceInst.Input.ofsUiot = PrgInputOfs + IODelta;
            if( !PrgOfs )
                PrgOfs = PrgInputOfs;
            // map all input points here, and increment.
			m_arrInput.UpdateOffsets( PrgInputOfs );
		}

	    // The following is for diagnostic purposes only. 
	    // Record in the map file where the input image of this device 
        // is mapped in the prg area
        m_pDev->PutInputOffset( PrgOfs );
	}

	// Set the offset for the output image
	{
		PrgOfs = 0;

		// If there are any output tags defined here
		if( m_arrOutput.m_UiotSize ) 
		{
			// store the associated offset in the drv image
			m_DeviceInst.Output.ofsUiot = PrgOutputOfs + IODelta;
            if( !PrgOfs )
                PrgOfs = PrgOutputOfs;
            // map all output points here, and increment.
			m_arrOutput.UpdateOffsets( PrgOutputOfs );
		}

	    // The following is for diagnostic purposes only. 
	    // Record in the map file where the input image of this device 
        // is mapped in the prg area
	    m_pDev->PutOutputOffset( PrgOfs );
	}

    // The following is a validity check only. 
    // If used, the compiler ensures the values reported by PutInputOffset and PutOutputOffset
    // are inside the progarm images of the driver. 
	ListUpdate( m_pDev );
}



int  CDevice::ListConfigGet( DEVCONFIG& Devconfig )
{
    int       rc = SUCCESS;
    UINT16    Size;

    memset( &Devconfig, 0, sizeof(DEVCONFIG) );
    
    rc = ListDeviceConfigGet( m_pName, &Devconfig, sizeof(DEVCONFIG), Size );

    if( rc == SUCCESS )
    {
        if( Size != sizeof(DEVCONFIG) )
            rc = FAILURE;
    }

    if( rc == SUCCESS )
    {
        if( Devconfig.NetcfgId        == NETCONFIG_ID               &&  
            Devconfig.DevcfgId        == DEVCONFIG_ID               &&  
            Devconfig.DevcfgVersMajor == ( DEVCONFIG_VERS >> 16 )   &&  
            Devconfig.DevcfgVersMinor >= ( DEVCONFIG_VERS & 0xffff )
          )
        {
            ;   // SUCCESS
        }
        else
            rc = FAILURE;
    }

    return rc;
}


BOOL CDevice::ListStart()
{
    return ListDeviceStart( m_pName );
}



/*************************************************************************/

CDeviceArray::CDeviceArray( LPCSTR pNetworkName ) 
{
    SetSize( 0, MAX_DEVICES );
    m_pNetName = pNetworkName;
    DebugString( "creating CDeviceArray for %s\n", m_pNetName );
}

CDeviceArray::~CDeviceArray()  
{ 
    DebugString( "removing CDeviceArray\n", "" );
    for(int i = 0; i < GetSize(); i++ )
        delete GetAt( i );
    RemoveAll(); 
}


// makes sure  pDevice can live with all previousely defined devices
int CDeviceArray::Check( CDevice* pDevice )
{
    int    rc    = SUCCESS;

    for( int DevIndex = 0; DevIndex < GetSize() && (rc == SUCCESS) ; DevIndex++ )
    {
        CDevice* pDev = GetAt( DevIndex );
        int      rc1  = pDevice->Check( pDev );

        if( rc1 != SUCCESS )
        {
            Erop( rc1, pDevice->m_pName, pDev->m_pName, m_pNetName, pDevice->m_DeviceInst.Address );
            rc = FAILURE;
        }
    }

    return rc; 
}


//*************************************************************************************
// Called in NetPass2 when we have real UIOT offsets for Config, Input and Output areas
// Also increment the Name, PrgInput and PrgOutput global offsets. 
void CDeviceArray::UpdateOffsets( UINT32& ofsNames, UINT32& PrgInputOfs, UINT32& PrgOutputOfs, UINT32 IODelta )
{
    for( int DeviceIndex = 0; DeviceIndex < GetSize() ; DeviceIndex++ )
    {
        CDevice* pDevice = GetAt( DeviceIndex );
        pDevice->UpdateOffsets( ofsNames, PrgInputOfs, PrgOutputOfs, IODelta );
    }
}


/***************************** End of CDevice Implementation *********************************/
/*********************************************************************************************/




/************************** End of Usefull PCI3TIER classes **********************************/
/*********************************************************************************************/


