/************************************************************************
  LogDrv.cpp

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
#include "LogDrv.h"
#include <time.h>

#define CURRENTTICK (GetTickCount() - tickDisp)

static void printConfig(FILE *fp, CommConfig *pConf)
{
	fprintf(fp, "%d \
/%x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x /\
%d %d %d %d %d /%x %x %x %x %x\n",
		pConf->BaudRate,
		pConf->fRTSDisable,
		pConf->fParity,
		pConf->fOutXCTSFlow,
		pConf->fOutXDSRFlow,
		pConf->fEnqAck,
		pConf->fEtxAck,
		pConf->fDTRDisable,
		pConf->fOutX,
		pConf->fInX,
		pConf->fPErrChar,
		pConf->fNullStrip,
		pConf->fCharEvent,
		pConf->fDTRFlow,
		pConf->fRTSFlow,
		pConf->fDsrSensitivity,
		pConf->fTxContinueOnXoff,
		pConf->fDtrEnable,
		pConf->fAbortOnError,
		pConf->fRTSEnable,
		pConf->fRTSToggle,
		pConf->XonLim,
		pConf->XoffLim,
		pConf->ByteSize,
		pConf->Parity,
		pConf->StopBits,
		pConf->XonChar,
		pConf->XoffChar,
		pConf->ErrorChar,
		pConf->EofChar,
		pConf->EvtChar
		);
}

LogDrv::LogDrv(SerialDrive *pDown, char *logFile)
{
	pDownStream = pDown;

	time_t ltime;

	logFp = fopen(logFile, "a+b");

	tickDisp = GetTickCount();

	time(&ltime);
	fprintf(logFp, "creation time %s\n", ctime(&ltime));

}

LogDrv::~LogDrv()
{
	time_t ltime;
	time(&ltime);
	fprintf(logFp, "deletion time %s\n", ctime(&ltime));
	fclose(logFp);

	delete pDownStream;
}

int LogDrv::WriteToUp(int size, char *pData)
{
	char pbuf[32];
	int copylen = size > 31 ? 31 : size;

	strncpy(pbuf, pData, copylen);
	pbuf[copylen] = '\0';
	fprintf(logFp, "WriteToUp %d : %d : %s\n", CURRENTTICK, size, pbuf);
	
	return pUpStream->WriteToUp(size, pData);
}

int LogDrv::LineBreakNotify()
{
	fprintf(logFp, "LineBreakNotify %d\n", CURRENTTICK);
	return pUpStream->LineBreakNotify();
}

int LogDrv::LineErrorNotify(int errStat)
{
	fprintf(logFp, "LineErrorNotify %d : %x\n", CURRENTTICK, errStat);

	return pUpStream->LineErrorNotify(errStat);
}

int LogDrv::EventCharNotify()
{
	fprintf(logFp, "EventCharNotify %d\n", CURRENTTICK);

	return pUpStream->EventCharNotify();
}

int LogDrv::TxEmptyNotify()
{
	fprintf(logFp, "TxEmptyNotify %d\n", CURRENTTICK);

	return pUpStream->EventCharNotify();
}

int LogDrv::TxCharNotify()
{
	fprintf(logFp, "TxCharNotify %d\n", CURRENTTICK);

	return pUpStream->TxCharNotify();
}

int LogDrv::ModemEventNotify(int eventMask, int modemStat)
{
	fprintf(logFp, "ModemEventNotify %d : %x %x\n", CURRENTTICK, eventMask, modemStat);

	return pUpStream->ModemEventNotify(eventMask, modemStat);
}

int LogDrv::TxFlowOff(int bOff)
{
	fprintf(logFp, "TxFlowOff %d : %x\n", CURRENTTICK, bOff);

	return pUpStream->TxFlowOff(bOff);
}

int LogDrv::ShutdownUp()
{
	fprintf(logFp, "ShutdownUp %d\n", CURRENTTICK);

	return pUpStream->ShutdownUp();
}

//////////////////////////////////////////////////
// SerialDrive implementation
int LogDrv::SetUpStream(SerialUpStream *pUp)
{
	pUpStream = pUp;
	return pDownStream->SetUpStream(this);
}

// communication
int LogDrv::Connect(char *hostName, int progNum, void *exData)
{
	fprintf(logFp, "Connect %d : %s %d %x\n", CURRENTTICK, hostName, progNum, exData);

	return pDownStream->Connect(hostName, progNum, exData);
}

int LogDrv::Disconnect()
{
	fprintf(logFp, "Disconnect %d\n", CURRENTTICK);
	return pDownStream->Disconnect();
}

int LogDrv::OpenPort(char *portName, CommConfig *pConfig, int *modemStat)
{
	int rval;
	fprintf(logFp, "OpenPort %d :", CURRENTTICK);
	rval = pDownStream->OpenPort(portName, pConfig, modemStat);
	printConfig(logFp, pConfig);
	fprintf(logFp, "%x\n", *modemStat);
	return rval;
}

int LogDrv::ClosePort()
{
	fprintf(logFp, "ClosePort %d\n", CURRENTTICK);
	fflush(logFp);
	return pDownStream->ClosePort();
}

int LogDrv::SetConfig(CommConfig *pConfig)
{
	fprintf(logFp, "SetConfig %d : ", CURRENTTICK);
	printConfig(logFp, pConfig);
	return pDownStream->SetConfig(pConfig);
}

int LogDrv::TxData(int size, char *pData)
{
	char pbuf[32];
	int copylen = size > 31 ? 31 : size;
	strncpy(pbuf, pData, 31);
	pbuf[copylen] = '\0';
	fprintf(logFp, "TxData %d : %d : %s\n", CURRENTTICK, size, pbuf);

	return pDownStream->TxData(size, pData);
}

int LogDrv::TxEMD(short data)
{
	fprintf(logFp, "TxEMD %d : %x\n", CURRENTTICK, data);

	return pDownStream->TxEMD(data);
}

int LogDrv::PortCtrl(int control)
{
	fprintf(logFp, "PortCtrl %d : %x\n", CURRENTTICK, control);
	return pDownStream->PortCtrl(control);
}

int LogDrv::SetWindowNotify(_HWND hWnd, int messageId)
{

	return pDownStream->SetWindowNotify(hWnd, messageId);
}

int LogDrv::ClearWindowNotify(_HWND hWnd)
{
	return pDownStream->ClearWindowNotify(hWnd);
}

int LogDrv::Recv()
{
	pDownStream->timeout = timeout;
	return pDownStream->Recv();
}
