
#ifndef PINCFG_H
#define	PINCFG_H
#include "pin_map.h"


#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

	#define		AO0					PAout(5)
	#define		AI0					PAin(6)
	#define		AI1					PAin(7)
	
	#define		LED_RUN				PBout(11)
	#define		LED_LINK			PBout(10)
	
	#define		ADDR7				PBin(9)
	#define		ADDR6				PEin(0)
    #define		ADDR5 				PEin(1)
	#define		ADDR4				PEin(2)
	#define		ADDR3				PEin(3)
	#define		ADDR2				PEin(4)
	#define		ADDR1				PEin(5)
	#define		ADDR0				PEin(6)
	
	#define		RS485_TX			PCout(10)
	#define		RS485_RX			PCin(11)
	#define		RS485_DE			PDout(3)
	#define		RS485_nRE			PDout(4)

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	

