/*********************************************************************

                        points.cpp

**********************************************************************/


#include "stdafx.h"

#include <windows.h>
#include <windowsx.h>

#include "drvrsym.h"
#include "resource.h"       // IDS_ERR_...
#include "driver.h"         // PCI3TIER driver specifics
#include "goodies.h"        // interface to goodies.cpp
#include "points.h"         // interface to points.cpp



/*********************************************************************************************/
/****************************** Usefull PCI3TIER classes *************************************/



/*********************************************************************************************/
/******************************** CIOPoint Implementation ************************************/

CIOPoint::CIOPoint( HDriverSym* pPntSym, UINT16 PointOffset )
{
    m_pPnt   = pPntSym;
    m_pName  = m_pPnt->GetName();                 // user's name for this I/O tag;
    m_Size   = GetPointSize( m_pPnt->GetType() );
    m_Offset = PointOffset;                       // point offset in the device's space, in bits

    DebugString( "creating point %s\n", m_pName );
} 

CIOPoint::~CIOPoint()  
{
    DebugString( "removing point %s\n", m_pName );
    delete m_pPnt; 
}

UINT16 CIOPoint::GetPointSize( SymType St )
{
    UINT16 Size = 0;

    switch(St)
    {
        case stBoolean:     Size = 1;   break;      //bit of byte
        case stUint8:
        case stSint8:
        case stByteBoolean: Size = 8;   break;      //One byte wide but only bit 0 used.
        case stUint16:
        case stSint16:      Size = 16;  break;
        case stUint32:   
        case stSint32:      Size = 32;  break;
    }                

    return Size;
}


// ensure "this" can live with "pPoint"
int CIOPoint::Check( CIOPoint* /* pPoint */)
{
    int rc = SUCCESS;
    // Overlapping points are legal. 
    // The VLC warning is enough. 
    return rc; 
}


void CIOPoint::UpdateOffsets( UINT32 UiotOffset )
{
    m_pPnt->PutDataOffset( UiotOffset + (m_Offset / 8));  // UIOT point offset in bytes
    m_pPnt->PutBit( (UINT16)(m_Offset & 7) );             // and relative offset in bits
    ListUpdate( m_pPnt);
}

/****************************** End of CIOPoint Implementation *********************************/
/*********************************************************************************************/




/*********************************************************************************************/
/**************************** CIOPointArray Implementation ***********************************/

CIOPointArray::CIOPointArray( LPCSTR pDeviceName ) 
{
    SetSize( 0, 32 );
    m_pDevName = pDeviceName;
    m_UiotSize = 0;
    DebugString( "creating CIOPointArray for %s\n", m_pDevName );
}

CIOPointArray::~CIOPointArray()  
{ 
    DebugString( "removing CIOPointArray\n", "" );
    for(int i = 0; i < GetSize(); i++ )
        delete GetAt( i );
    RemoveAll(); 
}


int CIOPointArray::Check( CIOPoint* pPoint )
{
    int rc = SUCCESS;
    
    if( rc == SUCCESS )
        if( !IsIncluded( pPoint->m_Offset, pPoint->m_Size, 0, m_UiotSize * 8) )
        {
            rc = IDS_CP_POINT_OUTSIDE;
            Erop( rc, pPoint->m_pName, m_pDevName, "", "");
        }


    if( rc == SUCCESS )
    {
        for( int index = 0; index < GetSize(); index++ )
        {
            CIOPoint* pPnt = GetAt(index);
            int     rc1  = pPoint->Check( pPnt );

            if( rc1 != SUCCESS )
            {
                Erop( rc1, pPoint->m_pName, pPnt->m_pName, m_pDevName, "");
                rc = FAILURE;
            }
        }
    }

    return rc; 
}



//*************************************************************************************
// Called in NetPass2 when we have real UIOT offsets for Input and Output areas
// Also increment the PrgInput or PrgOutput global offset. 
void CIOPointArray::UpdateOffsets( UINT32& PrgOfs )
{
    for( int index = 0; index < GetSize(); index++ )
    {
        CIOPoint* pPoint = GetAt(index);
        pPoint->UpdateOffsets( PrgOfs );
    }

    PrgOfs += m_UiotSize;   // increment the 'input/output' offset with the size of the device
}



/************************* End of CIOPointArray Implementation *******************************/
/*********************************************************************************************/




/************************** End of Usefull PCI3TIER classes **********************************/
/*********************************************************************************************/


