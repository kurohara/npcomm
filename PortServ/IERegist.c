/***********************************************************************
    IERegist.c
	"AutoDial" registry utility for PortShare server program.

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
// IERegist.c
#include <Windows.h>

int ChangeAutoDial(BOOL bAuto)
{
	unsigned int oldValue, newValue;
	int sizeValue;
	int typeValue;
	HKEY kAutoDial;
	int ret;

	ret = RegOpenKeyEx(HKEY_CURRENT_USER,
		"Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings",
		0,
		KEY_ALL_ACCESS,
		&kAutoDial);

	if (ret != ERROR_SUCCESS) {
		return 0;
	}

	if (bAuto)
		newValue = 1;
	else
		newValue = 0;

	typeValue = 0;
	sizeValue = sizeof(oldValue);
	ret = RegQueryValueEx(kAutoDial, "EnableAutodial", NULL,
		&typeValue, (char *) &oldValue, &sizeValue);

	ret = RegSetValueEx(kAutoDial, "EnableAutodial", 0,
		REG_DWORD, (char *) &newValue, sizeof (newValue));

	RegCloseKey(kAutoDial);

	return oldValue != 0 ? 1 : 0;
}

