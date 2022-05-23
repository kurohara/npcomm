// pcommdrv.h
#ifndef _PCOMMDRV_H
#define _PCOMMDRV_H

#include "PComConf.h"
#include "PCommMgr.h"

class PCommDevice
{
public:
	virtual ~PCommDevice() {}
	int PCommVersion;
	int IRQ;
	int IOBase;
	char PortName[32];
	char PortDriverNum[32];
	int maxSendSize;
	//
	HANDLE hDevice;
	void *DeviceData;
	//
	virtual int DeviceInitialize(char *ComName, int index) = 0;
	virtual void DeviceRelease(void) = 0;
	//
	virtual void ChangeCommName(void) = 0;
	//
	virtual int GetPCommCommand(int Timeout, int *Command, int *DataSize, void *Data) = 0;
	enum { PCA_OK, PCA_ERROR } ;
	virtual int CommandAck(int Kind) = 0;
	//
	enum { NK_TXDATA, NK_RXDATA, NK_EVENT } ;
	virtual int Notify(int Kind, EventData *pE) = 0;
	virtual int WriteData(int Size, char *pBuf) = 0;
	virtual int SetCommConfig(CommConfig *pDcb) = 0;
	virtual int SetModemStat(int modemStat) = 0;
	virtual int Disconnect(void) = 0;
};

// manager control commands for driver modules
#define MQC_NUMAPPS		1


#endif _PCOMMDRV_H
