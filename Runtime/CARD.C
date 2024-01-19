/********************************************************************

                                Card.c

    Interface specific code. 
    This file only should touch the hardware.

*********************************************************************/


#include "stdafx.h"

#include <rt.h>
#include <string.h>     // strlen()
#include <stdio.h>      // sprintf()

#include "vlcport.h"
#include "dcflat.h"     // EROP()
#include "driver.h"     /* SEMAPHORE */
#include "errors.h"     /* IDS_RT_DP_RW_TEST                     */
#include "auxrut.h"     /* StartTimeout(), IsTimeout(), EROP     */
#include "pci8132.h"   /* DUAL_PORT                             */
#include "card.h"       /* Init()                                */
#include "pci_8132.h"
//#include "8134def.h"

//static UINT32 INTarray[MAX_PCI_CARDS *2];
//static double compData0[MAX_PCI_CARDS *2], compCnt0[MAX_PCI_CARDS *2];
//static double compData1[MAX_PCI_CARDS *2], compCnt1[MAX_PCI_CARDS *2];

static int AxisCompEnable[MAX_PCI_CARDS *2] ;

static int CompTable[100];
//static int CompCnt[MAX_PCI_CARDS *2];


/******************* Card specific  Functions  *******************************/


/******************* Initialization  *******************************/


static int TestAndFill(UINT8* pc, const int Size, const int test, const int fill)   /* test == 1  --> no test */
{
    int i  = 0;
    for(; i < Size;  *pc++ = fill, i++)
    {
        int c = *pc & 255;
        if(test != 1  &&  test != c)
        {
            EROP("Ram Error.  Address %p, is 0x%02x, and should be 0x%02x", pc, c, test, 0);
            return IDS_PCI8132_HW_TEST;
        }
    }
    return SUCCESS;
}


int  Init( LPDRIVER_INST pNet, P_ERR_PARAM const lpErrors)
{
    int rc = SUCCESS;

    return rc;
}



/****************************************************************************************
    IN:     pName   --> pointer to the device user name
            Address --> device's network address
            pBuf    --> pointer to the destination buffer
            szBuf   --> size of the destination buffer

    OUT:    *pBuf   <-- "Address xx (usr_name)".  
    Note:   The device user name may be truncated!
*/
static void LoadDeviceName( char* pName, UINT16 Address, char* pBuf, size_t szBuf )
{
    if( szBuf && (pBuf != NULL) )
    {
        char* format = "Address %d";

        *pBuf = '\0';

        if( szBuf > (strlen(format)+3) )    /* Address may need more digits */
        {
            size_t  len;

            sprintf(pBuf, format, Address & 0xffff);

            len = strlen( pBuf ); 

            if( pName && ((szBuf - len) > 10) )     /* if we still have 10 free bytes  */
            {
                strcat(pBuf, " (");
                len += 2;
                strncat( pBuf, pName, szBuf-len-2 );
                *(pBuf + szBuf - 2) = '\0';
                strcat( pBuf, ")" );
            }
        }
    }
}



int  TestConfig( LPDRIVER_INST const pNet, P_ERR_PARAM const lpErrors )
{
    int rc = SUCCESS;

    LPDEVICE_INST pDevice = (LPDEVICE_INST)pNet->pDeviceList;
        
    for( ; pDevice->Type && (rc == SUCCESS); pDevice++ )
    {
        
		// TO DO:
		DUAL_PORT* const dp = NULL; // (DUAL_PORT*)pNet->pDpr;     /* pointer to the dualport */

        pDevice->bPresent = 1;

        /*
        Check pDevice. 
        if( the device is not on the network )
            pDevice->bPresent = 0;
        */

		/*
		printf( "TO DO File=%s, line=%d \n" __FILE__, __LINE__ );
        
        if( !pDevice->bPresent)
        {
            LoadDeviceName( pDevice->pName, pDevice->Address, lpErrors->Param3, sizeof(lpErrors->Param3) );
            rc = IDS_PCI3TIER_DEVICE_OFFLINE; 
        }
		*/
    }

    return rc;
}


/********************* runtime specific card access functions ********************/


int	DoCollect( LPDRIVER_INST pNet, LPSPECIAL_INST pData)
{
    int     rc       = SUCCESS;
//	int		channel;
//	UINT16	*ChanBuff[16];
//	UINT16	NumSamples = pData->Work.paramCommand.NumSamps;
	UINT16* pResult = BuildUiotPointer( pData->Work.paramHeader.ofsResult );
   
//	for (channel = 0; (channel < 16) && (rc == SUCCESS); channel++)
//	{
//		LPPTBUFFER pRBuffer = &pData->Work.paramCommand.Buffers[channel];

//		printf("Channel %d  Length %d  Offset %d\n", 
//			channel, pRBuffer->Size, pRBuffer->Offset);

//		ChanBuff[channel] = BuildUiotPointer( pRBuffer->Offset );
//		if( pRBuffer->Size > NumSamples)
//		{
//			rc = IDS_PCI3TIER_READ_SIZE;
//		}
//	}

	// At this time, ChanBuf[i] is a pointer to the buffer area for channel i.
	// Insert your code here.
	*pResult = rc;
	return  (rc);
}


int ADlinkReadIO( LPDEVICE_INST const pDevice, int bn, VOID *Dest )
{

    int     rc       = SUCCESS;
	U16		StsAxis, cRes;
	UINT16 ValueGet16;
	UINT16	AxisNum; //	IntAxisNum;
	double	ValuePos ;

	float	ValuePos32;
	

 switch(pDevice->Type ) 

    {

	case  DEVICE_1W_IANDO:

	
	cRes= _8132_DI(bn, Dest) ;
	break;

        case DEVICE_CARD_POS:
		//0~3
		AxisNum = (bn * 2) + pDevice->Address;

		rc= _8132_get_position(AxisNum, &ValuePos);
		ValuePos32=ValuePos;	// conver to 32 bits

		*((UINT32  volatile*)Dest) = (UINT32)ValuePos;


///hardware need encoder, but i loop back pos into here
///so encoder is not needed

 		if (AxisCompEnable[AxisNum]==1) 
		rc=	_8132_Set_CompCnt(AxisNum,ValuePos);
		
		
		break;

		case DEVICE_CARD_IO_STATUS1: /// motion status and motion done
		AxisNum = (bn * 2) + pDevice->Address;

//	rc=_8132_Get_CompData(AxisNum,&compData0[AxisNum]);
//	rc= _8132_Get_CompCnt(AxisNum,&compCnt0[AxisNum]);

		rc= _8132_get_io_status(AxisNum, &ValueGet16);

		*((UINT16  volatile*)Dest) = (UINT16) ValueGet16;

		ValueGet16= _8132_motion_done(AxisNum);
		*((UINT16  volatile*)Dest+1) = (UINT16) ValueGet16;
			
		break;

		case DEVICE_CARD_INT_STATUS1:
		AxisNum = (bn * 2) + pDevice->Address;

		///	rc =	_8132_Get_CompSts(AxisNum, &StsAxis);

		rc =	_8132_Get_CompSts(bn, &StsAxis);

	    *((UINT32  volatile*)Dest) = (UINT32) StsAxis;
//		INTarray[AxisNum] = (UINT32) StsAxis;

		break;            
    }




	return  (rc);

}


void AdLinkMotion( const LPDRIVER_INST pNet, SPECIAL_INST* const pData )
{
    int rc = SUCCESS ;

	UINT16 bn;
		
    SPECIAL_INST_COMMAND* const pUser = &pData->User.paramCommand;

    UINT16* pResult = BuildUiotPointer( pUser->Header.ofsResult );
 	*pResult=rc;	


	bn=(((pNet->PciIndex) -1) *2) + pUser->Address; // axis number

 switch(pUser->Function  ) 
    {

	
	case	START_AA_MOVE:
	*pResult= _8132_start_a_move(bn, pUser->fPos , pUser->stVel, pUser->maxVel, pUser->Accl);
	break;
	case	START_A_MOVE:
	*pResult=	_8132_start_a_move(bn, pUser->fPos, pUser->stVel, pUser->maxVel, pUser->Accl);
		break;
	case	A_MOVE:
	*pResult=	_8132_a_move(bn, pUser->fPos, pUser->stVel, pUser->maxVel, pUser->Accl);
		break;
	case	START_R_MOVE:
	*pResult=	_8132_start_r_move(bn, pUser->fPos, pUser->stVel, pUser->maxVel, pUser->Accl);
		break;
	case	R_MOVE:
	*pResult=	_8132_r_move(bn, pUser->fPos, pUser->stVel, pUser->maxVel, pUser->Accl);
		break;
	case	START_T_MOVE: 
	*pResult=	_8132_start_t_move(bn, pUser->fPos, pUser->stVel, pUser->maxVel, pUser->Accl, pUser->TDeccl);
	
		break;
	case	T_MOVE:
	*pResult=	_8132_t_move(bn, pUser->fPos, pUser->stVel, pUser->maxVel, pUser->Accl, pUser->TDeccl);
		break;
	case	START_TA_MOVE:
	*pResult=	_8132_start_ta_move(bn, pUser->fPos, pUser->stVel, pUser->maxVel, pUser->Accl, pUser->TDeccl);
		break;
	case	TA_MOVE:
	*pResult=	_8132_ta_move(bn, pUser->fPos, pUser->stVel, pUser->maxVel, pUser->Accl, pUser->TDeccl);
		break;
	case	START_S_MOVE:
	*pResult=	_8132_start_s_move(bn, pUser->fPos, pUser->stVel, pUser->maxVel, pUser->T1Accl, pUser->TSAccl);
		break;
	case	S_MOVE:
	*pResult=	_8132_s_move(bn, pUser->fPos, pUser->stVel, pUser->maxVel, pUser->T1Accl, pUser->TSAccl);
		break;
	case	START_RS_MOVE:
	*pResult=	_8132_start_rs_move(bn, pUser->fPos, pUser->stVel, pUser->maxVel, pUser->T1Accl, pUser->TSAccl);
		break;
	case	RS_MOVE:
	*pResult=	_8132_rs_move(bn, pUser->fPos, pUser->stVel, pUser->maxVel, pUser->T1Accl, pUser->TSAccl);
		break;
	case	START_TAS_MOVE:
	*pResult=	_8132_start_tas_move(bn, pUser->fPos, pUser->stVel, pUser->maxVel, pUser->T1Accl, pUser->TSAccl,pUser->T1Deccl,pUser->TSDeccl);
		break;
	case	TAS_MOVE:
	*pResult=	_8132_tas_move(bn, pUser->fPos, pUser->stVel, pUser->maxVel, pUser->T1Accl, pUser->TSAccl,pUser->T1Deccl,pUser->TSDeccl);
		break;
	case	V_MOVE:
	*pResult=	_8132_v_move(bn, pUser->stVel, pUser->maxVel, pUser->Accl);
		break;
	case	SV_MOVE:
	*pResult=	_8132_sv_move(bn, pUser->stVel, pUser->maxVel, pUser->T1Accl, pUser->TSAccl);
		break;
	case	HOMEMOVE:
	*pResult=	_8132_home_move(bn, pUser->stVel,pUser->maxVel,pUser->Accl);
	
		break;

	case	V_CHANGE:
	*pResult=	_8132_v_change(bn, pUser->maxVel, pUser->Accl);
		break;
	case	MANU_MOVE:
	*pResult=	_8132_manu_move(bn, pUser->fPos);
		break;

	}

}

void AdLinkSetGet( const LPDRIVER_INST pNet, SPECIAL_INST* const pData )
{
    int rc = SUCCESS ;
	UINT16 bn;
//	F64 PosRead;		
    SPECIAL_INST_SETGET* pUser = &pData->User.paramSetGet;

    double* pGetValue = BuildUiotPointer( pUser->GetValue  );

    UINT16* pResult = BuildUiotPointer( pUser->Header.ofsResult );
	*pResult=rc;	



	bn=(((pNet->PciIndex) -1) *2) + pUser->Address; // axis number
	

 switch(pUser->Function  ) 
    {
	case	STARTMOTION:
	*pResult= _8132_start_motion();
	*pGetValue = (double) *pResult;

	break;
	case	STOPMOTION:
	*pResult=_8132_stop_motion();
	*pGetValue = (double) *pResult;

	break;
	case	V_STOP: 
	*pResult=_8132_v_stop(bn,pUser->SetValue );
	*pGetValue = (double) *pResult;

	break;
	case	AXISMOTIONDONE:
	*pResult=_8132_motion_done(bn);
	*pGetValue = (double) *pResult;

	break;
	case	WAITAXISDONE:
	*pResult=_8132_wait_for_done(bn);
	*pGetValue = (double) *pResult;

	break;
	case	SETMAXVEL:
	*pResult=_8132_fix_max_speed(bn, pUser->SetValue );
	*pGetValue = (double) *pResult;

	break;
	case	GETPOS:
	*pResult=_8132_get_position(bn,  pGetValue);
	
	break;
	case	SETPOS:
	*pResult=_8132_set_position(bn, pUser->SetValue );
	*pGetValue = (double) *pResult;

	break;
	case	GETCOMMAND:
	*pResult=_8132_get_command(bn, pGetValue );
	break;
	case	SETCOMMAND:
	*pResult=_8132_set_command(bn,  pUser->SetValue);
	*pGetValue = (double) *pResult;

	break;

	case SETMOVERATIO:
	*pResult=_8132_set_move_ratio(bn, pUser->SetValue);
	*pGetValue = (double) *pResult;

	break;

	}


}


void AdLinkOthers( const LPDRIVER_INST pNet, SPECIAL_INST* const pData )
{
    int rc = SUCCESS ;
	int bn;
	float	ValuePos32;

    SPECIAL_INST_OTHERS* pUser = &pData->User.paramOthers;

    int* pGetValue = BuildUiotPointer( pUser->GetValue  );

    UINT16* pResult = BuildUiotPointer( pUser->Header.ofsResult );
	*pResult=rc;	

//	*pGetValue=*pGetValue+1;

	bn=(((pNet->PciIndex) -1) *2) + pUser->Address; // axis number
	

 switch(pUser->Function  ) 
    {
	case	SETHOMECONFIG:
	*pResult= _8132_set_home_config(bn, pUser->SetValue1, pUser->SetValue2, pUser->SetValue3, pUser->SetValue3);
	*pGetValue = (int) *pResult;
		break;

		case	SETSVONOFF:
	*pResult =_8132_Set_SVON(bn, pUser->SetValue1);
	*pGetValue = (int) *pResult;
		break;

		case	SETMAN_IPMODE:
	*pResult = _8132_set_manu_iptmode(bn, pUser->SetValue1, pUser->SetValue2);
	*pGetValue = (int) *pResult;
		break;

		case	SET_PLS_OUTMODE:

	*pResult = _8132_set_pls_outmode(bn,pUser->SetValue1);
	*pGetValue = (int) *pResult;
		break;
		case	SET_PLS_INMODE:

	*pResult = _8132_set_pls_iptmode(bn,pUser->SetValue1);
	*pGetValue = (int) *pResult;
		break;
		case	SET_CNTMODE:
	*pResult = _8132_set_cnt_src(bn,pUser->SetValue1);
	*pGetValue = (int) *pResult;
		break;
		case	SETALARM_LOGIC:
	*pResult = _8132_set_alm_logic(bn,pUser->SetValue1,pUser->SetValue2);
	*pGetValue = (int) *pResult;
		break;
		case	SETIN_LOGIC:

	*pResult = _8132_set_inp_logic(bn,pUser->SetValue1,pUser->SetValue2);
	*pGetValue = (int) *pResult;
		break;

		case	SETINTZERO:   ///clear interrupt

//		INTarray[bn] = 	INTarray[bn] & pUser->SetValue1;
//		*pGetValue = INTarray[bn] ; //(int) *pResult;
		break;
 }


}



void AdLinkOnFly( const LPDRIVER_INST pNet, SPECIAL_INST* const pData )
{
    int rc = SUCCESS ;
	int bn;
	int cardno;
	int i ;

	double	ValuePos32;

    SPECIAL_INST_OTHERS* pUser = &pData->User.paramOthers;

    int* pGetValue = BuildUiotPointer( pUser->GetValue  );

    UINT16* pResult = BuildUiotPointer( pUser->Header.ofsResult );
	*pResult=rc;	

	bn=(((pNet->PciIndex) -1) *2) + pUser->Address; // axis number
	cardno = (pNet->PciIndex) -1;

 switch(pUser->Function  ) 
    {
	case	SETCOMPHOME:
	*pResult=	_8132_Set_CompHome(bn);
		break;
	case	SETCOMPMODE:
	*pResult=	_8132_Set_CompMode(bn, pUser->SetValue1);
		break;

	case	SETCOMPCNT:
	*pResult=	_8132_Set_CompCnt(bn, pUser->SetValue1);
		break;
	case	SETCOMDATA:
	*pResult=	_8132_Set_CompData(bn, pUser->SetValue1);
		break;
	case	SETCOMPINT:

	*pResult=_8132_Set_CompInt(bn,pUser->SetValue1);
	AxisCompEnable[bn] = pUser->SetValue2;  /// para 2 for stepper without encoder only

	break;
	case	SETCOMPTABLE:
	*pResult=	_8132_Set_Comp_Table(bn, pUser->SetValue1);
		break;
	case	GETCOMPCNT:
	*pResult=	 _8132_Get_CompCnt(bn, &ValuePos32);

	*pGetValue=(UINT32)ValuePos32;

		break;

	case	GETCOMPSTS:
	*pResult=	_8132_Get_CompSts(cardno, (U16*) *pGetValue);
		break;
	

	case	GETCOMPDATA:

	*pResult=	 _8132_Get_CompData(bn, &ValuePos32);

		*pGetValue=(UINT32)ValuePos32;

		break;
 

	case	BUILDCOMPTABLE:
/*  for (i=0; i<= pUser->SetValue3;i++)
	  CompTable[i]= pUser->SetValue1 + ( pUser->SetValue2 * i);

	*pResult= _8132_set_cnt_src(bn, 1);
	*pResult= _8132_set_pls_iptmode(bn, 0);
_8132_Set_CompCnt(bn, pUser->SetValue1);

	_8132_Set_CompMode(bn,0);
_8132_Set_CompInt(bn,1);
_8132_Set_Comp_Table(bn,0);
////rintf("we are in BUILDCOMPTABLE %d  card :%d",bn,cardno); 
*/	
		break;
	
	case	BUILDCOMPFUNCT:

//	*pResult=	_8132_Set_CompCnt(bn, CompTable[pUser->SetValue1]);
///	*pResult= _8132_Build_Comp_Table(bn, CompTable ,i);
///	*pResult=  _8132_Build_Comp_Function(bn, pUser->SetValue1, pUser->SetValue2, pUser->SetValue3);
		
_8132_Set_CompCnt(bn, pUser->SetValue1);


////	*pResult=  _8132_Build_Comp_Function(bn, pUser->SetValue1, pUser->SetValue2, pUser->SetValue3);
	break;
 
 }

}


// U16    _8132_Get_CompCnt(U16 axis, double *act_pos);
// U16    _8132_Set_CompCnt(U16 axis, double cnt_value);
// U16    _8132_Set_CompMode(U16 axis, I16 comp_mode);
// U16    _8132_Set_CompData(U16 axis, double comp_data);
// U16    _8132_Get_CompData(U16 axis, double *comp_data);
// U16    _8132_Set_CompInt(U16 axis, U16 enable);
// U16    _8132_Set_CompHome(U16 axis);
//U16    _8132_Get_CompSts(U16 cardNo, U16 *Comp_Sts);
//U16    _8132_Build_Comp_Table(U16 axis, I32 *table, I16 Size);
//U16    _8132_Build_Comp_Function(U16 axis, I32 Start, I32 End, I32 Interval);
//U16    _8132_Set_Comp_Table(U16 axis, U16 Control);

