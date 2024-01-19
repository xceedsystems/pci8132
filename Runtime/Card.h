/***************************************************************

                Card.h             

   This file contains the interface to the manufacturer code

****************************************************************/


#ifndef  __CARD_H__
#define  __CARD_H__

int Init( void* const dp, P_ERR_PARAM const lpErrors);
int TestConfig( LPDRIVER_INST const pNet, P_ERR_PARAM const lpErrors);
int	DoCollect( LPDRIVER_INST pNet, LPSPECIAL_INST pData);
int ADlinkReadIO( LPDEVICE_INST const pDevice, int bn, VOID *Dest);

void AdLinkMotion(  const LPDRIVER_INST pNet, SPECIAL_INST* const pData );
void AdLinkSetGet(  const LPDRIVER_INST pNet, SPECIAL_INST* const pData );
void AdLinkOthers(  const LPDRIVER_INST pNet, SPECIAL_INST* const pData );
void AdLinkOnFly(  const LPDRIVER_INST pNet, SPECIAL_INST* const pData );


#endif      /* __CARD_H__ */

