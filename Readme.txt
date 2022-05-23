------------------------------------------------------------------------------
               NPCOMM ver 2.0.3
    Copyright (C) 1999-2003 by Hiroyoshi Kurohara
        All Rights Reserved
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

------------------------------------------------------------------------------

This is the binary/source distribution of Npcomm ver 2.0.
The files of binary distribution are all located at BIN directory.
You can simply copy these files to any place and use it.
Usage is similar to NPCOMM ver 1.5 so you may refer to help file of 
NPCOMM ver 1.5 to know the detail of this program.

Source distributions are divided to several directories,
  inc      -- header files for npcomm executable.
  instnp   -- npcomm driver installer for windows NT.
  npcomm   -- npcomm main executalbe.
  PortServ -- PortServ server program
  TcpShare -- Transport module used by npcomm.exe for PortShare function.
  Telnet   -- Transport module used by npcomm.exe for Telnet function.
  XpInc    -- header files for Transport modules.

 1. source code of some driver files(following) are not offered yet.
      pcom95.dll
      pcomm32*.vxd
      pcomnt.dll
      pserial.sys
    Because these files does not have heavy impact to the behavior of
    NPCOMM application, you don't need to use these source so soon.
    These sources affects only to quality.
    Now I'm preparing to distribute these source code, please wait.
 
 2. You need Microsoft Visual C++ 6.0 to use these source code.
 3. The license of source code distribution is GPL or Artistic, you can
    select one of these at your needs.
    Please refer to the file "Copying" and "Artistic".

------------------------------------------------------------------------------
Change History
2004/2/2
  The absolete path strings in project file(*.dsp) has been removed.
  Now PortServ can create .ini file automatically.

2003/1/15
  Startup code bug(around old registration key check code) fixed.

2003/1/13
  First source release with,
  1. removed registration key check codes.
  2. added license terms.

