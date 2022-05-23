/***********************************************************************
    CommDrv.cpp
	Communication port driver class for PortShare server program.

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
// CommDrv.cpp

// Some Windows application does kind of framing by using 
// EV_RXFLAG event.
// 
//  software emulation of EV_RXFLAG.
// #define SOFT_EVENTCHAR
//

#include <Windows.h>
#include <stdio.h>
#include "CommDrv.h"
#include "DCBUtil.h"


CommDriver::CommDriver()
{
	pRxBuf = rxBuf
#ifdef NOCOPY
		+ SIZE_PREP
#endif
		;
	timeout = 10000;
}

CommDriver::~CommDriver()
{
}

int CommDriver::SetUpStream(SerialUpStream *pUp)
{
	pUpStream = pUp;
	return TRUE;
}

int CommDriver::Connect(char *hostName, int progNum, void *exData)
{
	return TRUE;
}

int CommDriver::Disconnect()
{
	return TRUE;
}

int CommDriver::OpenPort(char *portName, CommConfig *pConfig, int *modemStat)
{
	char commPortName[32];
	COMMPROP lProp;
	COMMTIMEOUTS cmto;
	unsigned long errValue;

	sprintf(commPortName, "\\\\.\\%s", portName);
	hPort = CreateFile(commPortName, GENERIC_READ | GENERIC_WRITE,
			0, 
			NULL, 
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL | 
			FILE_FLAG_OVERLAPPED, 
			NULL );

	if (hPort == INVALID_HANDLE_VALUE)
		return FALSE;

	GetCommProperties(hPort, &lProp);
	SetupComm(hPort, MAXSIZE_RXD, MAXSIZE_TXD);
	PurgeComm(hPort, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	ClearCommError(hPort, &errValue, NULL);

	// timeout setting
	GetCommTimeouts(hPort, &cmto);
	cmto.ReadIntervalTimeout = MAXDWORD;
	cmto.ReadTotalTimeoutConstant = 0;
	cmto.ReadTotalTimeoutMultiplier = 0;
#if 1
	cmto.WriteTotalTimeoutMultiplier = 0;
	cmto.WriteTotalTimeoutConstant = 0;
#else
	cmto.WriteTotalTimeoutMultiplier = 0;
	cmto.WriteTotalTimeoutConstant = 1000;
#endif
	SetCommTimeouts(hPort, &cmto);
	//
	// set initial DCB to port
//	BuildCommDCB("baud=19200 parity=N data=8 stop=1", &curDCB);
//	SetCommState(hPort, &curDCB);
	//
	GetCommState(hPort, &curDCB);
	_DCB2CommConf(&curDCB, pConfig);
	GetCommModemStatus(hPort, (unsigned long *) modemStat);

	int eventMask = EV_BREAK |
			EV_CTS |
			EV_DSR |
			EV_ERR |
			EV_RING |
			EV_RLSD |
			EV_RXCHAR |
#ifndef SOFT_EVENTCHAR
			EV_RXFLAG |
#endif
			EV_TXEMPTY ;

	SetCommMask(hPort, eventMask);

	bClosed = FALSE;

	memset((char *) &ovlWrite, 0, sizeof (ovlWrite));
	memset((char *) &ovlRecv, 0, sizeof (ovlRecv));
	memset((char *) &ovlEvent, 0, sizeof (ovlRecv));
	ovlWrite.hEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
	ovlRecv.hEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
	ovlEvent.hEvent = CreateEvent(NULL, TRUE, TRUE, NULL);

	return TRUE;
}

int CommDriver::ClosePort()
{
	CloseHandle(ovlWrite.hEvent);
	CloseHandle(ovlRecv.hEvent);
	CloseHandle(ovlEvent.hEvent);
	bClosed = TRUE;
	CloseHandle(hPort);
	return TRUE;
}

int CommDriver::SetConfig(CommConfig *pConfig)
{
	DCB dcb;
	int ret;
	_CommConf2DCB(pConfig, &dcb);
	curDCB = dcb;
#ifdef SOFT_EVENTCHAR
	if (dcb.EvtChar != 0) {
		dcb.EvtChar = 0;
	}
#endif
	ret = SetCommState(hPort, &dcb);
	return TRUE;
}

int CommDriver::TxData(int size, char *pData)
{
	unsigned long sizeWritten, rsize;
	int err;

	pUpStream->TxCharNotify();

	if (!GetOverlappedResult(hPort, &ovlWrite, &sizeWritten, FALSE)){
		err = GetLastError();
		if (err == ERROR_IO_INCOMPLETE) {
			err = WaitForSingleObject(ovlWrite.hEvent, INFINITE);
		} else {
			return FALSE;
		}
	}
	if (!WriteFile(hPort, pData, size, &rsize, &ovlWrite)) {
		err = GetLastError();
		if (err != ERROR_IO_PENDING) {
			return FALSE;
		}
	}

	return TRUE;
}

int CommDriver::TxEMD(short data)
{
	int ret;
	ret = TransmitCommChar(hPort, (char) data);
	return TRUE;
}

int CommDriver::PortCtrl(int control)
{
	int ret;
	ret = EscapeCommFunction(hPort, control);
	return TRUE;
}

//
int CommDriver::SetWindowNotify(_HWND hWnd, int messageId)
{
	return FALSE;
}

int CommDriver::ClearWindowNotify(_HWND hWnd)
{
	return FALSE;
}

// this will cause calling callback functions
int CommDriver::Recv()
{
	int sizeRead;
	int err;
	COMSTAT errStat;
	unsigned long errValue;
	unsigned long sizeXfered;
	int bWait = FALSE;

	if (GetOverlappedResult(hPort, &ovlEvent, &sizeXfered, FALSE)) {
		if (WaitCommEvent(hPort, &evt, &ovlEvent)) {
		} else {
			err = GetLastError();
			if (err == ERROR_IO_PENDING)
				bWait = TRUE;
			else
				return RECV_FAIL;
		}
	} else {
		err = GetLastError();
		if (err == ERROR_IO_INCOMPLETE)
			bWait = TRUE;
		else
			return RECV_FAIL;
	}
	if (bWait) {
		err =  WaitForSingleObject(ovlEvent.hEvent, timeout);
		if (err == WAIT_TIMEOUT) {
			// 
#if 0
			if (ClearCommError(hPort, &errValue, &errStat)) {
				if (errStat.cbInQue == 0) {
					return TRUE;
				}
				evt = EV_RXCHAR;
			} else {
				return RECV_FAIL;
			}
#else
			return RECV_TIMEOUT;
#endif
		}
	}

	if (evt & EV_RXCHAR) {
		sizeRead = _readData(pRxBuf, MAXSIZE_RXD);
		if (sizeRead > 0) {
#ifdef SOFT_EVENTCHAR
			if (curDCB.EvtChar != 0) {
				int i, cnt = 0;
				char *pBuf = pRxBuf;
				for (i=0;i<sizeRead;i++) {
					if (pBuf[cnt++] == curDCB.EvtChar) {
						pUpStream->WriteToUp(cnt, pBuf);
						pUpStream->EventCharNotify();
						pBuf = pBuf + cnt;
						cnt = 0;
					}
				}
				if (cnt > 0) {
					pUpStream->WriteToUp(cnt, pBuf);
				}
			} else {
				pUpStream->WriteToUp(sizeRead, pRxBuf);
			}
#else
			pUpStream->WriteToUp(sizeRead, pRxBuf);
#endif
		}
	}
	if (evt & EV_BREAK) {
		pUpStream->LineBreakNotify();
		ClearCommError(hPort, &errValue, &errStat);
	}
	if (evt & (EV_CTS | EV_DSR | EV_RING | EV_RLSD)) {
		unsigned long mstat;
		err = GetCommModemStatus(hPort, &mstat);
		//
		pUpStream->ModemEventNotify(
			evt & (EV_CTS | EV_DSR | EV_RING | EV_RLSD),
			(int) mstat); 
	}
	if (evt & EV_ERR) {
		ClearCommError(hPort, &errValue, &errStat);
		pUpStream->LineErrorNotify(errValue);
	}
	if (evt & EV_TXEMPTY) {
		if (bClosed)
			return RECV_FAIL;
		pUpStream->TxEmptyNotify();
	}
#ifndef SOFT_EVENTCHAR
	if (evt & EV_RXFLAG) {
		pUpStream->EventCharNotify();
	}
#endif

	return RECV_SUCCESS;
}

int CommDriver::_readData(char *pBuf, int maxSize)
{
	int sizeToRead = maxSize;
	unsigned long sizeRead;
	int err;
	COMSTAT errStat;
	unsigned long errValue;

	err = ReadFile(hPort, pBuf,	sizeToRead, &sizeRead, &ovlRecv);
	if (!err) {
		err = GetLastError();
		if (err != ERROR_IO_PENDING)
			return 0;

		err = WaitForSingleObject(ovlRecv.hEvent, timeout);
		if (err == WAIT_TIMEOUT) {
			ClearCommError(hPort, &errValue, &errStat);
		}
		GetOverlappedResult(hPort, &ovlRecv, &sizeRead, TRUE);

	}
	return sizeRead;
}
