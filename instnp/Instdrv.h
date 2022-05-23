// instdrv.h
/*
   $Date: $ 
   $Revision: $
 */
/*
    Copyright (C) 1999-2003 Hiroyoshi Kurohara all rights reserved.
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
#ifndef _INSTDRV_H
#define _INSTDRV_H

typedef struct PPortInfoRec {
	int portNum;
	char pcomName[64];
	char dpcomName[64];
} PPortInfo;

typedef struct drvInfoRec {
	int nPCOMM;
	PPortInfo pinfo[9];
} drvInfo;

#ifdef __cplusplus
extern "C" {
#endif

int SetParameters(drvInfo *pInfo);
int GetParameters(drvInfo *pInfo);
int GetLastComId(void);
int GetDriverStatus(int *bRunning, char *pPath, int *bAuto);
int InstDrv(int autostart);
int DeinstDrv(void);
int StartDrv(void);
int StopDrv(void);
int CheckDriverFile(void);
int SetDriverPath(char *pDir);
int GetDriverFileName(char *pName);

#ifdef __cplusplus
}
#endif

#endif _INSTDRV_H
