/***********************************************************************
    DCBUtil.c
	Communication port configuration convert utility for PortShare 
	server program.

    Copyright (C) 1999-2003 by Hiroyoshi Kurohara
        All Rights Reserved

    This file is part of PortShare server program for Windows.

************************************************************************/
/*
   $Date: $ 
   $Revision: $
 */
/*
    This program is free software; you can redistribute it and/or modify
    it under the terms of either
        a) the GNU General Public License as published by the Free Software
           Foundation; either version 2 of the License, or (at your option)
           any later version.
        b) the "Artistic License" .

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License or Artistic License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    You should also have received a copy of the "Artistic License" 
    (which is the file named "Artistic") with this bunch of source codes.
    if not, you can request it to copyright holder of this program.

 */
// DCBUtil.c

#include "DCBUtil.h"

void _CommConf2DCB(CommConfig *pSConf, DCB *pDCB)
{
	memset((char *) pDCB, 0, sizeof (DCB));
	pDCB->DCBlength = sizeof (DCB);
	pDCB->BaudRate = pSConf->BaudRate;

	pDCB->fBinary = 1;
	pDCB->fParity = pSConf->fParity;
	pDCB->fOutxCtsFlow = pSConf->fOutXCTSFlow;
	pDCB->fOutxDsrFlow = pSConf->fOutXDSRFlow;

	pDCB->fDtrControl = DTR_CONTROL_ENABLE;
	pDCB->fDtrControl = (pSConf->fDTRDisable 
			? DTR_CONTROL_DISABLE  : (pSConf->fDTRFlow
				? DTR_CONTROL_HANDSHAKE : DTR_CONTROL_ENABLE)
			);
//
	pDCB->fDsrSensitivity = pSConf->fDsrSensitivity;
	pDCB->fTXContinueOnXoff = pSConf->fTxContinueOnXoff;
	pDCB->fOutX = pSConf->fOutX;
	pDCB->fInX = pSConf->fInX;
	pDCB->fErrorChar = pSConf->fPErrChar;
	pDCB->fNull = pSConf->fNullStrip;
	//
	pDCB->fRtsControl = RTS_CONTROL_ENABLE;
#ifdef RTSTOGGLE
	if (pSConf->fRTSToggle) {
		pDCB->fRtsControl = RTS_CONTROL_TOGGLE;
	}
#endif
	if (pSConf->fRTSFlow)
		pDCB->fRtsControl = RTS_CONTROL_HANDSHAKE;
	if (pSConf->fRTSEnable)
		pDCB->fRtsControl = RTS_CONTROL_ENABLE;
	if (pSConf->fRTSDisable)
		pDCB->fRtsControl = RTS_CONTROL_DISABLE;

	pDCB->fAbortOnError = pSConf->fAbortOnError;

	pDCB->XonLim = (WORD) pSConf->XonLim;
	pDCB->XoffLim = (WORD) pSConf->XoffLim;

	pDCB->ByteSize = pSConf->ByteSize;
	pDCB->Parity = pSConf->Parity;
	pDCB->StopBits = pSConf->StopBits;
	pDCB->ErrorChar = pSConf->ErrorChar;
	pDCB->EofChar = pSConf->EofChar;
	pDCB->EvtChar = pSConf->EvtChar;
	if (pSConf->XonChar != pSConf->XoffChar) {
		pDCB->XonChar = pSConf->XonChar;
		pDCB->XoffChar = pSConf->XoffChar;
	}

	pDCB->wReserved = 0;

}

void _DCB2CommConf(DCB *pDCB, CommConfig *pSConf)
{
	memset((char *) pSConf, 0, sizeof (CommConfig));
	pSConf->Size = sizeof (CommConfig);
	// Version is set to 0

	pSConf->BaudRate = pDCB->BaudRate;
	pSConf->fBinary = 1;
	pSConf->fParity = pDCB->fParity;
	pSConf->fOutXCTSFlow = pDCB->fOutxCtsFlow;
	pSConf->fOutXDSRFlow = pDCB->fOutxDsrFlow;

	if (pDCB->fDtrControl == DTR_CONTROL_DISABLE)
		pSConf->fDTRDisable = 1;
	if (pDCB->fDtrControl == DTR_CONTROL_HANDSHAKE)
		pSConf->fDTRFlow = 1;
	if (pDCB->fDtrControl ==  DTR_CONTROL_ENABLE)
		pSConf->fDtrEnable = 1;

	pSConf->fDsrSensitivity = pDCB->fDsrSensitivity;
	pSConf->fTxContinueOnXoff = pDCB->fTXContinueOnXoff;
	pSConf->fOutX = pDCB->fOutX;
	pSConf->fInX = pDCB->fInX;
	pSConf->fPErrChar = pDCB->fErrorChar;
	pSConf->fNullStrip = pDCB->fNull;

	if (pDCB->fRtsControl == RTS_CONTROL_DISABLE)
		pSConf->fRTSDisable = 1;
	if (pDCB->fRtsControl == RTS_CONTROL_TOGGLE)
		pSConf->fRTSToggle = 1;
	if (pDCB->fRtsControl == RTS_CONTROL_HANDSHAKE)
		pSConf->fRTSFlow = 1;
	if (pDCB->fRtsControl == RTS_CONTROL_ENABLE)
		pSConf->fRTSEnable = 1;
	// for debug

	pSConf->fAbortOnError = pDCB->fAbortOnError;
	pSConf->XonLim = pDCB->XonLim;
	pSConf->XoffLim = pDCB->XoffLim;
	pSConf->ByteSize = pDCB->ByteSize;
	pSConf->Parity = pDCB->Parity;
	pSConf->StopBits = pDCB->StopBits;
	pSConf->XonChar = pDCB->XonChar;
	pSConf->XoffChar = pDCB->XoffChar;
	pSConf->ErrorChar = pDCB->ErrorChar;
	pSConf->EofChar = pDCB->EofChar;
	pSConf->EvtChar = pDCB->EvtChar;
}
