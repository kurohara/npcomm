/***********************************************************************
    DCBUtil.h
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
// DCBUtil.h
#ifndef _DCBUTIL_H
#define _DCBUTIL_H

#include <Windows.h>
#include "PComConf.h"

#ifdef __cplusplus
extern "C" {
#endif

void _CommConf2DCB(CommConfig *pSConf, DCB *pDCB);
void _DCB2CommConf(DCB *pDCB, CommConfig *pSConf);

#ifdef __cplusplus
}
#endif

#endif _DCBUTIL_H