/***********************************************************************
    Streams.h
    Uniformed Data Stream Framework.

	Copyright (C) 1999 by Hiroyoshi Kurohara
		All Rights Reserved

  	This file is part of TELNET XP module(transport DLL for NPCOMM).

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

/*

 It may looks alike STREAMS(A.K.A AT&T UNIX kernel technology).
 This U.D.S.F have no asynchronous message processing, and also no link.
 just have call chain.
 */ 
#ifndef _STREAMS_H
#define _STREAMS_H

#include <Windows.h>

typedef struct STMessageRec {
	union {
		int broadcastMessage;
		int anyMessage;
		struct { // sequenced for Intel CPU
			int messageId:16;
			int signature:16;
		} individMessage;
	} type;
	void *pData;
} STMessage;

// Well known "broadcast messages"
#define STMESSAGE_OPEN	0x0001
#define STMESSAGE_CLOSE	0x0002

#define STMESSAGE_CONNECT 0x0003
#define STMESSAGE_DISCONNECT 0x0004
#define STMESSAGE_DISCONNECTED 0x0005

// Well known "individual message"
#define STIMESSAGE_ENABLE		1
#define STIMESSAGE_SETPARAMS	2
#define STIMESSAGE_CLEARPARAMS	3
#define STIMESSAGE_ADDPARAM		5
#define STIMESSAGE_WK_END	STIMESSAGE_ADDPARAM
#define INDIVIDUAL_MESSAGE(muid, messageId) (muid << 16 | messageId) // make module unique message id from well known message
/////

class AnyStream
{
protected:
	AnyStream *pUpper, *pLower;
public:
	AnyStream()
	{
		pUpper = pLower = NULL;
	};
	AnyStream(AnyStream *upper, AnyStream *lower)
	{
		pUpper = upper; pLower = lower;
	};
	virtual ~AnyStream() {}
	void push(AnyStream *pSt)
	{
		if (pLower == this) {
			pLower = pSt;
			pUpper = pSt;
			if (pSt != NULL) {
				pSt->pLower = this;
				pSt->pUpper = this;
			}
		} else {
			if (pLower != NULL)
				pLower->pUpper = pSt;
			pSt->pUpper = this;
			pSt->pLower = pLower;
			pLower = pSt;
		}
	};
	AnyStream *pop()
	{
		AnyStream *pMod = pLower;
		if (pLower != NULL)
			pLower = pLower->pLower;
		if (pLower != NULL)
			pLower->pUpper = this;
		if (pMod != NULL) {
			pMod->pLower = NULL;
			pMod->pUpper = NULL;
		}
		return pMod;
	};
	AnyStream *getUpper()
	{
		return pUpper;
	};
	AnyStream *getLower()
	{
		return pLower;
	};
};

class STModule : public AnyStream
{
protected:
	// stream message utilities
	int WriteToUpper(int size, char *pBuf)
	{
//		STModule *pUpper = (STModule *) getUpper();
		STModule *pUpper = (STModule *) this->pUpper;
		if (pUpper)
			return pUpper->DataFromLower(size, pBuf);
		return -1;
	};
	int WriteToLower(int size, char *pBuf)
	{
//		STModule *pLower = (STModule *) getLower();
		STModule *pLower = (STModule *) this->pLower;
		if (pLower)
			return pLower->DataFromUpper(size, pBuf);
		return -1;
	};
	int ControlUpper(STMessage *pMes)
	{
//		STModule *pUpper = (STModule *) getUpper();
		STModule *pUpper = (STModule *) this->pUpper;
		if (pUpper)
			return pUpper->ControlFromLower(pMes);
		return FALSE;
	};
	int ControlLower(STMessage *pMes)
	{
//		STModule *pLower = (STModule *) getLower();
		STModule *pLower = (STModule *) this->pLower;
		if (pLower)
			return pLower->ControlFromUpper(pMes);
		return FALSE;
	};
public:
	STModule() : AnyStream() {};
	STModule(STModule *upper, STModule *lower) : AnyStream(upper, lower) {};
	virtual ~STModule() {}
	// stream interfaces
	virtual int DataFromUpper(int size, char *pBuf) = 0;
	virtual int DataFromLower(int size, char *pBuf) = 0;
	virtual int ControlFromUpper(STMessage *pMes) = 0;
	virtual int ControlFromLower(STMessage *pMes) = 0;
};

class STDriver : public STModule
{
	//////////////////////////////////////////////////////
	int DataFromUpper(int size, char *pBuf) //// bottom end
	{
		return WriteToMedia(size, pBuf);
	}
	int DataFromLower(int size, char *pBuf) //// top end
	{
		return ReadDataCB(size, pBuf);
	}
	int ControlFromUpper(STMessage *pMes)  //// bottom end
	{
		return ControlMedia(pMes);
	}
	int ControlFromLower(STMessage *pMes)  //// top end
	{
		return MessageCB(pMes);
	}
public:
	STDriver() : STModule(this, this) {} // Compiler says something? never mind.
	// APIs for driver user routines
	int WriteData(int size, char *pData)
	{
		return WriteToLower(size, pData);
	};
	int Control(STMessage *pMes)
	{
		return ControlLower(pMes);
	};

	///////////////////////////////////////////////////
	// interfaces
	virtual int ReadDataCB(int size, char *pData) = 0; // top end callback
	virtual int MessageCB(STMessage *pMes) = 0;        // top end callback
	virtual int WriteToMedia(int size, char *pData) = 0; // bottom end driver
	virtual int ControlMedia(STMessage *pMes) = 0; // bottom end driver
	//
	// driver implementer should write virtual ISR(inturrupt service routine).
	// It will call WriteToUpper() (for data), ControlUpper() (for control).
};


//////////////////////////////////////////////////////////////

#endif _STREAMS_H
