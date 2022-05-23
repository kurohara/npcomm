/***********************************************************************

  PComConf.h
   Serial port configuration structure definition for NPCOMM.
   This file is part of NPCOMM transport module interface.
   $Date: 1999/03/21 10:30:47 $ 
   $Revision: 1.2 $

   Copyright (C) 1999 by Hiroyoshi Kurohara
		All Rights Reserved

Permission to use, copy and distribute of this source code
is hereby granted, provided that the above copyright notice appear 
in all copies and no modification is applied to this file..

Hiroyoshi Kurohara disclaims all warranties with regard to this 
software, including all implied warranties of merchantability and 
fitness.
In no event shall Hiroyoshi Kurohara be liable for any special, 
indirect or consequential damages or any damages whatsoever resulting 
from loss of use, data or profits, whether in an action of contract, 
negligence or other tortious action, arising out of or in connection 
with the use or performance of this software.
************************************************************************/

#ifndef _PCOMCONF_H
#define _PCOMCONF_H

#pragma pack (1)
// Serial configuration structure.

typedef struct CommConfigRec {
	DWORD Size;					// size of this structure
	DWORD BaudRate ;			// Baudrate
	// flags
	DWORD fBinary:1;			// is binary mode, always set
	DWORD fRTSDisable:1;		// disable RTS
	DWORD fParity:1;			// enable parity check
	DWORD fOutXCTSFlow:1;		// enable CTS flow control
	DWORD fOutXDSRFlow:1;		// enable DSR flow control
	DWORD fEnqAck:1;			// enable ENQ/ACK software handshaking
	DWORD fEtxAck:1;			// enable ETX/ACK software handshaking
	DWORD fDTRDisable:1;		// disable DTR
	DWORD fOutX:1;				// enable output software flow control
	DWORD fInX:1;				// enable input software flow control
	DWORD fPErrChar:1;			// enable parity error replacement
	DWORD fNullStrip:1;			// enable null stripping(obsolete)
	DWORD fCharEvent:1;			// make event by specified character
	DWORD fDTRFlow:1;			// enable DTR input flow control
	DWORD fRTSFlow:1;			// enable RTS output flow control
	DWORD fReserved:1;
	DWORD fDsrSensitivity:1;	// act sensitive about DSR
	DWORD fTxContinueOnXoff:1;	// continue pended transmission after Xoff sent
	DWORD fDtrEnable:1;			// enable DTR
	DWORD fAbortOnError:1;		// abort all reads and writes on error
	DWORD fRTSEnable:1;			// enable RTS
	DWORD fRTSToggle:1;			// toggle RTS line by characters in tx queue
	DWORD pad:10;
	//
	DWORD XonLim;			// Transmit X-ON threshold
	DWORD XoffLim;			// Transmit X-OFF threshold
	WORD wReserved;			// reserved
	BYTE ByteSize;			// bitlength (4-8)
	BYTE Parity;			// parity 0-4=None,Odd,Even,Mark,Space
	BYTE StopBits;			// 0,1,2 = 1, 1.5, 2
	char XonChar;			// Tx and Rx X-ON character
	char XoffChar;			// Tx and Rx X-OFF character
	char ErrorChar;			// Parity error replacement char
	char EofChar;			// End of Input character
	char EvtChar;			// special event character
	BYTE bPad[2];			// reserved
	DWORD dPad[4];			// not used
} CommConfig;

#pragma pack ()

#endif _PCOMCONF_H
