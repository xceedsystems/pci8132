/*********************************************************************

                        network.cpp

**********************************************************************/


#include "stdafx.h"

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>          // sprintf()

#include "drvrsym.h"
#include "drvrio.h"         // HStreamAbs
#include "resource.h"
#include "driver.h"         // PCI3TIER driver specifics
#include "goodies.h"        // interface to goodies.cpp
#include "network.h"        // interface to network.cpp




/*********************************************************************************************/
/****************************** Usefull PCI3TIER classes *************************************/




/*********************************************************************************************/
/****************************** CNetwork Implementation **************************************/

CNetwork::CNetwork( LPCSTR pNetName )
    : m_arrDevice( pNetName ) 
{
    m_pName             = pNetName;
    m_uConfigTotal      = 0;
    m_uInputTotal       = 0;
    m_uOutputTotal      = 0;
    m_uNameTotal        = 0;
    m_uDeviceCnt        = 0;
    m_uDeviceInstTotal  = 0;
    memset( &m_NetInst, 0, sizeof(DRIVER_INST) );
    DebugString( "creating network %s\n", m_pName );
}

CNetwork::~CNetwork()
{
    DebugString( "removing network  %s\n", m_pName );
}



int CNetwork::IsDevice( UINT32 DevType )
{
    int bIsDevice = FALSE; 

    switch(DevType)
    {
		case DEVICE_CARD_POS:
        case DEVICE_CARD_IO_STATUS1:
        case DEVICE_CARD_INT_STATUS1:
		case DEVICE_1W_IANDO:

                           bIsDevice = TRUE;
            
    }

    return bIsDevice;
}


// make sure  m_NetInst  can safely be used at run time
int CNetwork::NetValid()
{
    int rc = SUCCESS;

	UINT16 uIndex = m_NetInst.PciIndex;		// 1-based
    UINT16 uCount = m_NetInst.PciCount;		// 0 --> no checking

    if( !uIndex || (uIndex > MAX_DRV_INSTANCES) )
    {
        Erop( IDS_INVALID_PCI_INDEX, m_pName, "", "", uIndex );
        rc = FAILURE;
    }

    if( uCount )
    {
        if( uCount > MAX_DRV_INSTANCES )
        {
            Erop( IDS_INVALID_PCI_COUNT, m_pName, "", "", uCount );
            rc = FAILURE;
        }

        if( uIndex > uCount )
        {
            Erop( IDS_PCI_INDEX_TOO_BIG, m_pName, "", uIndex, uCount );
            rc = FAILURE;
        }
    }
    
    return rc;
}


// test if the driver is configured and load config info into DRIVER_INST
int CNetwork::LoadNetwork()
{
    NETCONFIG    NetConfig;     // where config data is loaded from symdb

    int rc = ListConfigGet( NetConfig );

    if( rc == SUCCESS )
    {
        m_NetInst.PciIndex    = NetConfig.PciIndex;
        m_NetInst.PciCount    = NetConfig.PciCount;
        m_NetInst.NumChannels = NetConfig.NumChannels;
        m_NetInst.bSimulate   = NetConfig.bSimulate; 
        m_NetInst.Sentinel    = RT3_SENTINEL;
    }
    else
    {
        rc = IDS_CP_DRIVER_NOT_CONFIGURED;
        Erop( rc, m_pName, "", "", "" );
    }

    return rc;
}


int CNetwork::LoadDevice( CDevice* pNewDevice )
{
    int rc = pNewDevice->Load();

	// Verify that each device is 'unique'
	if( rc == SUCCESS )
		rc = m_arrDevice.Check( pNewDevice );

	if( rc == SUCCESS )
    {
		// Add the Device so that later tests don't mysteriously fail.
        m_arrDevice.Add( pNewDevice );     //InsertInOrder( pNewDevice );
		m_uNameTotal += strlen( pNewDevice->m_pName ) + 1;
		m_uDeviceCnt++;

		/* Reserve UIOT input space only if there are defined tags	*/
		m_uInputTotal  += pNewDevice->GetUiotInputSize(); 

		/* Reserve UIOT output space only if there are defined tags	*/
		m_uOutputTotal += pNewDevice->GetUiotOutputSize();  
    }

    return rc;
}


int CNetwork::Load()
{
    int rc = LoadNetwork();

    if( rc == SUCCESS )
        rc = NetValid();

    if( ListStart() )    /* scan all devices */
    {
        do 
        {
            HDriverSym* pDev = ListGet();
			UINT16 DevType   = pDev->GetId();

			if( IsDevice( DevType ) )
			{
                CDevice* pNewDevice = new CDevice( pDev );
                rc = LoadDevice( pNewDevice );
		
				if( rc != SUCCESS )
                    delete pNewDevice;     // also deletes  *pDev
            }
            else
            {
                Erop( IDS_CP_UNKNOWN_DEVICE, m_pName, "", "", DevType );
                rc = FAILURE;
            }

        } while( ListNext());
    }
    ListEnd();

    DebugString( "CNetwork::Load(). Name=%s, rc=%d\n", m_pName, rc );
    
    return rc;
}


int CNetwork::Check( CNetwork* pNetwork )      // makes sure "this" can live with pNetwork
{
    int rc = SUCCESS;

    UINT16 uIndexThis = m_NetInst.PciIndex;
    UINT16 uIndexThat = pNetwork->m_NetInst.PciIndex;

    if( uIndexThis == uIndexThat )      // Same PCI index?
    {
        char buf[64];
        sprintf( buf, "%d", uIndexThis );
        Erop( IDS_CP_SAME_PCI_INDEX, m_pName, pNetwork->m_pName, buf, 0 );
        rc = FAILURE;
    }

    UINT16 uCountThis = m_NetInst.PciCount;
    UINT16 uCountThat = pNetwork->m_NetInst.PciCount;

    if( uCountThis && uCountThat && ( uCountThis != uCountThat ))   // Different # of expected PCI cards?
    {
        char BufThis[64];
        char BufThat[64];
        sprintf( BufThis, "%d", uCountThis );
        sprintf( BufThat, "%d", uCountThat );
        Erop( IDS_CP_INCONSISTENT_PCI_COUNT, m_pName, pNetwork->m_pName, BufThis, BufThat );
        rc = FAILURE;
    }


    return rc; 
}


void CNetwork::ReportSizes( UINT32& rConfigTotal, UINT32& rInputTotal, UINT32& rOutputTotal )
{
    m_uDeviceInstTotal = (m_uDeviceCnt + 1) * sizeof( DEVICE_INST );

    /*   The config block passed to the runtime will contain:
     *   1.  our DRIVER_INST struct
     *   2.  a DEVICE_INST array with an element for each defined device + a terminator element
     *   3.  the zero terminated names of all defined devices
     *   4.  RT3_SENTINEL, for validity checking.
     */

    m_uConfigTotal = sizeof(DRIVER_INST) - sizeof( NETWORK     ) + 
                    (m_uDeviceCnt + 1)   * sizeof( DEVICE_INST ) +
                    m_uNameTotal                                 +
                    sizeof( UINT32 );       // RT3_SENTINEL

    rConfigTotal = m_uConfigTotal;
    rInputTotal  = m_uInputTotal;
    rOutputTotal = m_uOutputTotal;
}


// called in NetPass2 when we have real UIOT offsets for Config, Input and Output areas
void CNetwork::UpdateOffsets( UINT32 CfgOfs, UINT32 PrgInputOfs, UINT32 PrgOutputOfs, UINT32 IODelta )
{
    UINT32 ofsDeviceInst = CfgOfs  + sizeof(DRIVER_INST) - sizeof( NETWORK);
    UINT32 ofsNames      = ofsDeviceInst  + m_uDeviceInstTotal;
    UINT32 SentinelOfs   = ofsNames       + m_uNameTotal;

    m_NetInst.ofsDeviceList = ofsDeviceInst;
    m_NetInst.nDevices      = m_uDeviceCnt;     // tell the runtime how many Devices are in the list

    m_arrDevice.UpdateOffsets( ofsNames, PrgInputOfs, PrgOutputOfs, IODelta );

    m_NetInst.ofsSentinel   = SentinelOfs;       // offset to RT3_SENTINEL
}



// Called in NetPass2 to write config data in the config area
// The # of written bytes must match the ConfigTotal value declared in NetPass1
void CNetwork::WriteConfigData( HStreamAbs& rFileHandle )
{
    // Write the config block as defined by 'ReportSizes'
    // The config block will be read by the runtime

    UINT32 uConfigTotal = 0;

    {
        char*  pCfg  = (char*) & m_NetInst + sizeof(NETWORK);
        UINT16 szCfg = sizeof(DRIVER_INST) - sizeof(NETWORK);
        rFileHandle.Write( pCfg, szCfg );
        uConfigTotal += szCfg;
    }

    for( int DevIndex = 0; DevIndex < m_arrDevice.GetSize() ; DevIndex++ )
    {
        DEVICE_INST* pDeviceInst = &m_arrDevice[ DevIndex ]->m_DeviceInst;
        rFileHandle.Write( pDeviceInst, sizeof( DEVICE_INST ) );
        uConfigTotal += sizeof( DEVICE_INST );
    }
    {
        DEVICE_INST Terminator;
        memset( &Terminator, 0, sizeof(DEVICE_INST) );
        rFileHandle.Write( &Terminator, sizeof( DEVICE_INST ) );
        uConfigTotal += sizeof( DEVICE_INST );
    }


    for( DevIndex = 0; DevIndex < m_arrDevice.GetSize() ; DevIndex++ )
    {
        LPCSTR pName  = m_arrDevice[ DevIndex ]->m_pName;
        size_t szName = strlen( pName );
        rFileHandle.Write( pName, szName );
        char Terminator = 0;
        rFileHandle.Write( &Terminator, sizeof( char ) );
        uConfigTotal += szName + 1;
    }

    {
        UINT32 RT3Sentinel = RT3_SENTINEL;
        rFileHandle.Write( &RT3Sentinel, sizeof( UINT32 ) );
        uConfigTotal += sizeof( UINT32 );
    }

    ASSERT( uConfigTotal == m_uConfigTotal );
	if( uConfigTotal != m_uConfigTotal )
	{
        // we have a bug here !!!
		Erop( IDS_CP_SOFTWARE_FAULT, "uConfigTotal != m_uConfigTotal", "", "", "" );
	}
}



int  CNetwork::ListConfigGet( NETCONFIG& Netconfig )
{
    int       rc = SUCCESS;
    UINT16    Size;

    memset( &Netconfig, 0, sizeof(NETCONFIG) );
    
    rc = ListNetworkConfigGet( m_pName, &Netconfig, sizeof(NETCONFIG), Size );

    if( rc == SUCCESS )
    {
        if( Size != sizeof(NETCONFIG) )
            rc = FAILURE;
    }

    if( rc == SUCCESS )
    {
        if( Netconfig.NetcfgId        == NETCONFIG_ID               &&  
            Netconfig.NetcfgVersMajor == ( NETCONFIG_VERS >> 16 )   &&  
            Netconfig.NetcfgVersMinor >= ( NETCONFIG_VERS & 0xffff )
          )
        {
            ;   // SUCCESS
        }
        else
            rc = FAILURE;
    }

    return rc;
}


BOOL CNetwork::ListStart()
{
    return ListNetworkStart( m_pName );
}



/************************************************************************************/

CNetworkArray::CNetworkArray( DRIVER_KEY NetKey, LPCSTR ProjPath ) 
{
    SetSize( 0, MAX_DRV_INSTANCES );
    DebugString( "Creating CNetworkArray. Path=%s\n", ProjPath);
    m_NetKey   = NetKey;    // PCI3TIER_DRIVER may be useful when more than 1 driver described in the rcd file
    m_ProjPath = ProjPath;  // full path to the project directory
}

CNetworkArray::~CNetworkArray()  
{ 
    DebugString( "removing CNetworkArray\n", "");
    for(int i = 0; i < GetSize(); i++ )
        delete GetAt( i );
    RemoveAll(); 
}


int CNetworkArray::Check( CNetwork* pNetwork )
{
    int rc = SUCCESS;

    for(int i = 0; i < GetSize(); i++ )
    {
        CNetwork* pNet = GetAt( i );
        int       rc1  = pNetwork->Check( pNet );

        if( rc1 != SUCCESS )
            rc = FAILURE;
    }

    return rc; 
}


CNetwork*  CNetworkArray::GetNetwork( LPCSTR lpNetName )
{
    for(int i = 0; i < GetSize(); i++ )
    {
        CNetwork* pNetwork = GetAt( i );
        if( pNetwork->m_pName == lpNetName )
            return pNetwork;
    }

    return NULL;
}

/**************************** End of CNetwork Implementation *********************************/
/*********************************************************************************************/



/************************** End of Usefull PCI3TIER classes **********************************/
/*********************************************************************************************/


