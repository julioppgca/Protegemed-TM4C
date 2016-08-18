#ifndef __NETWORK_CONFIG_H__
#define __NETWORK_CONFIG_H__

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif


#include <ptgmed_inc/system.h>

//*****************************************************************************
//
// Defines.
//
//*****************************************************************************

#define TCPPACKETSIZE 	256
#define NUMTCPWORKERS 	3
#define TCPPORT 		1000
#define TCPHANDLERSTACK 1024

//*****************************************************************************
//
// Prototypes.
//
//*****************************************************************************

extern Void tcpWorker(UArg arg0, UArg arg1);
extern Void tcpHandler(UArg arg0, UArg arg1);
extern void netOpenHook(void);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __NETWORK_CONFIG_H__
