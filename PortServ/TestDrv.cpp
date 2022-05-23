/***********************************************************************
    TestDrv.cpp
	Test module as Communication port driver class for PortShare 
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
#include <Windows.h>
#include <stdio.h>
#include "TestDrv.h"
#include "DCBUtil.h"

char *commandStrings[] = 
			  { "$LIST$\r", "$STARTECHO$\r", "$STOPECHO$\r", "$SHOWDCB$\r", "$SHOWMODEMSTAT$\r", "$SETDISCONNECT$\r"} ;
CommandsScan::CommandsScan()
{
	int i;
	for (i=0;i<ATCOMMAND;i++) {
		commands[i] = new StringScan(commandStrings[i]);
	}
	commands[ATCOMMAND] = new MCScan;

}

CommandsScan::~CommandsScan()
{
	int i;
	for (i=0;i<ATDIAL;i++) {
		delete commands[i];
	}
}

int CommandsScan::scan(int cnext)
{
	int i, matched = -1;
	int res;

	for (i=0;i<ATDIAL;i++) {
		res = commands[i]->iscan(cnext);
		if (res) {
			matched = i;
			break;
		}
	}
	if (matched==ATCOMMAND) {
		if (((MCScan *)commands[ATCOMMAND])->atX == (int) 'D')
			matched = ATDIAL;
	}
	return matched;
}

TestDriver::TestDriver()
{
	pRxBuf = rxBuf
#ifdef NOCOPY
		+ SIZE_PREP
#endif
		;
	hEndEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	bLineConnected = FALSE;
	bEcho = TRUE;
}

TestDriver::~TestDriver()
{
	CloseHandle(hEndEvent);
}

int TestDriver::SetUpStream(SerialUpStream *pUp)
{
	pUpStream = pUp;
	return TRUE;
}

int TestDriver::Connect(char *hostName, int progNum, void *exData)
{
	return TRUE;
}

int TestDriver::Disconnect()
{
	return TRUE;
}

int TestDriver::OpenPort(char *portName, CommConfig *pConfig, int *modemStat)
{
	DCB dcbPort;

	BuildCommDCB("baud=19200 parity=N data=8 stop=1", &dcbPort);
	_DCB2CommConf(&dcbPort, pConfig);
	*modemStat = curModemStat = MS_DSR_ON | MS_CTS_ON;
	ResetEvent(hEndEvent);
	return TRUE;
}

int TestDriver::ClosePort()
{
	SetEvent(hEndEvent);
	return TRUE;
}

int TestDriver::SetConfig(CommConfig *pConfig)
{
	_CommConf2DCB(pConfig, &curDCB);
	return TRUE;
}

int TestDriver::TxData(int size, char *pData)
{
	unsigned long rsize = 0;
	char *pBuf = pRxBuf;
	char *pResultOK = "OK\r\n";
	char *pResultConnected = "CONNECT\r\n";
	int cid;

	pUpStream->TxCharNotify();

	while (size > 0) {
		*pBuf = *pData;
		size--;
		rsize++;
		if (rsize >= MAXSIZE_RXD) {
			if (bEcho) pUpStream->WriteToUp(rsize, pRxBuf);
			pBuf = pRxBuf;
			rsize = 0;
		}
		if ((cid = commands.scan(*pData)) < 0) {
		} else {
			if (rsize > 0) {
				if (bEcho) pUpStream->WriteToUp(rsize, pRxBuf);
				pBuf = pRxBuf;
				rsize = 0;
			}
			switch (cid) {
			case LIST:
				_showList();
				break;
			case STARTECHO:
				bEcho = TRUE;
				break;
			case STOPECHO:
				bEcho = FALSE;
				break;
			case SHOWDCB:
				_showDCB();
				break;
			case SHOWMODEMSTAT:
				_showModemStat();
				break;
			case SETDISCONNECT:
				bLineConnected = FALSE;
				curModemStat &= ~MS_RLSD_ON;
				pUpStream->ModemEventNotify(EV_RLSD, (int) curModemStat);
				break;
			case ATCOMMAND:
				pUpStream->WriteToUp(strlen(pResultOK), pResultOK);
				break;
			case ATDIAL:
				bLineConnected = TRUE;
				pUpStream->WriteToUp(strlen(pResultConnected), pResultConnected);
				curModemStat |= MS_RLSD_ON;
				pUpStream->ModemEventNotify(EV_RLSD, (int) curModemStat); 
				break;
			default:
				break;
			}
		}
		pBuf++;
		pData++;
	}
	if (rsize > 0) {
		if (bEcho) pUpStream->WriteToUp(rsize, pRxBuf);
	}

	return TRUE;
}

int TestDriver::TxEMD(short data)
{
	lastSentChar = data;
	return TRUE;
}

int TestDriver::PortCtrl(int control)
{
	switch (control) {
	case CLRDTR:
		curModemStat &= ~MS_DSR_ON;
		pUpStream->ModemEventNotify(EV_DSR,	(int) curModemStat); 
		break;
	case CLRRTS:
		curModemStat &= ~MS_CTS_ON;
		pUpStream->ModemEventNotify(EV_CTS,	(int) curModemStat);
		break;
	case SETDTR:
		curModemStat |= MS_DSR_ON;
		pUpStream->ModemEventNotify(EV_DSR,	(int) curModemStat);
		break;
	case SETRTS:
		curModemStat |= MS_CTS_ON;
		pUpStream->ModemEventNotify(EV_CTS,	(int) curModemStat); 
		break;
	case SETXOFF:
		break;
	case SETXON:
		break;
	case SETBREAK:
		break;
	case CLRBREAK:
		break;
	}
	return TRUE;
}

//
int TestDriver::SetWindowNotify(HWND hWnd, int messageId)
{
	return FALSE;
}

int TestDriver::ClearWindowNotify(HWND hWnd)
{
	return FALSE;
}

// this will cause calling callback functions
int TestDriver::Recv()
{
	int ret;
	ret = WaitForSingleObject(hEndEvent, timeout);
	if (ret == WAIT_TIMEOUT)
		return TRUE;
	return FALSE;
}

void TestDriver::_showDCB()
{
	char outBuf[MAX_PATH];

	if (pUpStream == NULL)
		return;

	sprintf(outBuf, "\r   Starting DCB setting log :\r");
	pUpStream->WriteToUp(strlen(outBuf), outBuf);
	sprintf(outBuf, "BaudRate : %d\r", curDCB.BaudRate);
	pUpStream->WriteToUp(strlen(outBuf), outBuf);
	sprintf(outBuf, "fBinary : %d\r", curDCB.fBinary);
	pUpStream->WriteToUp(strlen(outBuf), outBuf);
	sprintf(outBuf, "fParity : %d\r", curDCB.fParity);
	pUpStream->WriteToUp(strlen(outBuf), outBuf);
	sprintf(outBuf, "fOutxCtsFlow :%d\r", curDCB.fOutxCtsFlow);
	pUpStream->WriteToUp(strlen(outBuf), outBuf);
	sprintf(outBuf, "fOutxDsrFlow :%d\r", curDCB.fOutxDsrFlow);
	pUpStream->WriteToUp(strlen(outBuf), outBuf);
	sprintf(outBuf, "fDtrControl :%d\r", curDCB.fDtrControl);
	pUpStream->WriteToUp(strlen(outBuf), outBuf);
	sprintf(outBuf, "fDsrSensitivity :%d\r", curDCB.fDsrSensitivity);
	pUpStream->WriteToUp(strlen(outBuf), outBuf);
	sprintf(outBuf, "fTXContinueOnXoff :%d\r", curDCB.fTXContinueOnXoff);
	pUpStream->WriteToUp(strlen(outBuf), outBuf);
	sprintf(outBuf, "fOutX :%d\r", curDCB.fOutX);
	pUpStream->WriteToUp(strlen(outBuf), outBuf);
	sprintf(outBuf, "fInX :%d\r", curDCB.fInX);
	pUpStream->WriteToUp(strlen(outBuf), outBuf);
	sprintf(outBuf, "fErrorChar :%d\r", curDCB.fErrorChar);
	pUpStream->WriteToUp(strlen(outBuf), outBuf);
	sprintf(outBuf, "fNull :%d\r\r\r", curDCB.fNull);
	pUpStream->WriteToUp(strlen(outBuf), outBuf);
	sprintf(outBuf, "fRtsControl :%d\r", curDCB.fRtsControl);
	pUpStream->WriteToUp(strlen(outBuf), outBuf);
	sprintf(outBuf, "fAbortOnError :%d\r", curDCB.fAbortOnError);
	pUpStream->WriteToUp(strlen(outBuf), outBuf);
	sprintf(outBuf, "XonLim :%d\r", curDCB.XonLim);
	pUpStream->WriteToUp(strlen(outBuf), outBuf);
	sprintf(outBuf, "XoffLim :%d\r", curDCB.XoffLim);
	pUpStream->WriteToUp(strlen(outBuf), outBuf);
	sprintf(outBuf, "ByteSize :%d\r", curDCB.ByteSize);
	pUpStream->WriteToUp(strlen(outBuf), outBuf);
	sprintf(outBuf, "Parity :%d\r", curDCB.Parity);
	pUpStream->WriteToUp(strlen(outBuf), outBuf);
	sprintf(outBuf, "StopBits :%d\r", curDCB.StopBits);
	pUpStream->WriteToUp(strlen(outBuf), outBuf);
	sprintf(outBuf, "XonChar :%x\r", curDCB.XonChar);
	pUpStream->WriteToUp(strlen(outBuf), outBuf);
	sprintf(outBuf, "XoffChar :%x\r", curDCB.XoffChar);
	pUpStream->WriteToUp(strlen(outBuf), outBuf);
	sprintf(outBuf, "ErrorChar :%d\r", curDCB.ErrorChar);
	pUpStream->WriteToUp(strlen(outBuf), outBuf);
	sprintf(outBuf, "EofChar :%d\r", curDCB.EofChar);
	pUpStream->WriteToUp(strlen(outBuf), outBuf);
	sprintf(outBuf, "EvtChar :%d\r", curDCB.EvtChar);
	pUpStream->WriteToUp(strlen(outBuf), outBuf);
	sprintf(outBuf, "   End of current DCB setting\r");
	pUpStream->WriteToUp(strlen(outBuf), outBuf);
}

void TestDriver::_showModemStat()
{
	char outBuf[MAX_PATH];
	if (pUpStream == NULL)
		return;

	if (curModemStat & MS_CTS_ON) {
		strcpy(outBuf, "\rCTS On   ");
	} else {
		strcpy(outBuf, "\rCTS Off   ");
	}
	if (curModemStat & MS_DSR_ON) {
		strcat(outBuf, "DSR On   ");
	} else {
		strcat(outBuf, "DSR Off   ");
	}
	if (curModemStat & MS_RING_ON) {
		strcat(outBuf, "RING On   ");
	} else {
		strcat(outBuf, "RING Off   ");
	}
	if (curModemStat & MS_RLSD_ON) {
		strcat(outBuf, "RLSD On   \r");
	} else {
		strcat(outBuf, "RLSD Off   \r");
	}

	pUpStream->WriteToUp(strlen(outBuf), outBuf);


}

void TestDriver::_showList()
{
	int i;
	for (i=0;i<ATCOMMAND;i++) {
		pUpStream->WriteToUp(strlen(commandStrings[i]), commandStrings[i]);
	}
}

