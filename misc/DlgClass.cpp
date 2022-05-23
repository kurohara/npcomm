/***********************************************************************
    DlgClass.cpp
	 a symple dialog class without MFC.

	Copyright (C) 1999 by Hiroyoshi Kurohara
		All Rights Reserved

	This file is part of NPCOMM transport DLL.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

************************************************************************/
#include <Windows.h>
#include "DlgClass.h"

BOOL CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

aDialog::aDialog()
{
	pMesMap = NULL;
}

aDialog::aDialog(int id, HWND parent)
{
	iDD = id;
	hWndParent = parent;
	pMesMap = NULL;
}

aDialog::~aDialog()
{
}

HWND aDialog::GetDlgItem(int id)
{
	return ::GetDlgItem(hDlg, id);
}

int aDialog::DoModal(HINSTANCE  hInstance)
{
	return ::DialogBoxParam(hInstance, MAKEINTRESOURCE(iDD), hWndParent, (DLGPROC)DlgProc, (LPARAM) this);
}

int aDialog::Create(HINSTANCE  hInstance)
{
	hDlg = ::CreateDialogParam(hInstance, MAKEINTRESOURCE(iDD), hWndParent, (DLGPROC)DlgProc, (LPARAM) this);
	return (int) hDlg;
}

int aDialog::OnCommand(WPARAM wParam, LPARAM lParam)
{
	MessageMap *pMap = (MessageMap *) pMesMap;

	while (pMap->message != 0) {
		if (pMap->message == WM_COMMAND &&
			(pMap->cid == LOWORD(wParam) || pMap->cid == 0) &&
			(pMap->n_kind == HIWORD(wParam) || pMap->n_kind == 0)) {

			if (pMap->handler != NULL) {
				(this->*pMap->handler)(wParam, lParam);
			}
			return TRUE;
		}
		pMap ++;
	}
	return FALSE;

}

BOOL CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	aDialog *pDialog;
	MessageMap *pMap;
	// reminder
	// WM_SETFONT

	if (uMsg == WM_INITDIALOG) {
		SetWindowLong(hDlg, DWL_USER, lParam);
		pDialog = (aDialog *) lParam;
		pDialog->hDlg = hDlg;
	} else {
		pDialog = (aDialog *) GetWindowLong(hDlg, DWL_USER);
	}
	if (pDialog == NULL) return FALSE;
	pMap = (MessageMap *) pDialog->pMesMap;


	if (pMap == NULL) {
		return FALSE;
	}
	if (uMsg == WM_COMMAND) {
		return pDialog->OnCommand(wParam, lParam);
	}
	while (pMap->message != 0) {
		if (pMap->message == uMsg &&
			(pMap->cid == LOWORD(wParam) || pMap->cid == 0) &&
			(pMap->n_kind == HIWORD(wParam) || pMap->n_kind == 0)) {

			if (pMap->handler != NULL) {
				(pDialog->*pMap->handler)(wParam, lParam);
			}
			if (uMsg == WM_QUERYENDSESSION)
				return FALSE;
			return TRUE;
		}
		pMap ++;
	}

	return FALSE;

}

