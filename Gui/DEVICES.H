/*********************************************************************

                        devices.h

**********************************************************************/


#ifndef __DEVICES_H__
#define __DEVICES_H__


#include "points.h"



/****************************************************************************************/
/**************************** Usefull PCI3TIER classes **********************************/



class CDevice : public CObject, public CDrvIntf
{
    private:
        CIOPointArray   m_arrInput;
        CIOPointArray   m_arrOutput;
        HDriverSym*     m_pDev;

    public:
        LPCSTR        m_pName;        // this name is stored on the heap in the *m_pDev block
        DEVICE_INST   m_DeviceInst;
                        
    private:
        int             DevValid();
        int             LoadDevice();
        UINT16          InputSizer();
        UINT16          OutputSizer();
        CIOPointArray*  GetPointArray( UINT16 Point, SymAccess Access, UINT16& PointOffset );

        int  ListConfigGet( DEVCONFIG& Devconfig );
        BOOL ListStart();

    public:
        int     Load( );
        int     Check( CDevice* pDevice );
        void    UpdateOffsets(  UINT32& ofsNames, 
                                UINT32& PrgInputOfs, 
                                UINT32& PrgOutputOfs, 
                                UINT32  IODelta );

        // Accessing elements
        UINT16  GetInputSize();         // Device  input size
        UINT16  GetOutputSize();        // Device output size
        UINT16  GetUiotInputSize();     // Device  input size or 0 if no  input tags defined
        UINT16  GetUiotOutputSize();    // Device output size or 0 if no output tags defined

        CDevice( HDriverSym* pDevSym );
        virtual ~CDevice();

        void AssertValid() const { return; }
};


/******************************************************************************/

class CDeviceArray : public CPtrArray, public CDrvIntf
{   
    private:
        LPCSTR m_pNetName;

    public:
        int  Check( CDevice* pDevice );
        void UpdateOffsets( UINT32& ofsNames, 
                            UINT32& PrgInputOfs, 
                            UINT32& PrgOutputOfs, 
                            UINT32  IODelta );
        
        // Accessing elements
        CDevice*  GetAt(int nIndex) const  {return (CDevice* )CPtrArray::GetAt(nIndex);}
        CDevice*& ElementAt(int nIndex)    {return (CDevice*&)CPtrArray::ElementAt(nIndex);}

        // overloaded operator helpers
        CDevice*  operator[](int nIndex) const {return GetAt(nIndex);}
        CDevice*& operator[](int nIndex)       {return ElementAt(nIndex);}

        CDeviceArray( LPCSTR pNetName );
        virtual ~CDeviceArray();
};



/************************** End of Usefull PCI3TIER classes **********************************/
/*********************************************************************************************/


#endif      /* __DEVICES_H__  */


