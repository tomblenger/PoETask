#pragma once

void	Initialize();
void	__stdcall MainTimer();
void	PressKeyAlternate(WCHAR wKey);
void	PressKeyDwon(WCHAR wKey);
void	SetMouseCursor(int nX, int nY);
void	ClickDone(int nX, int nY);
void	Click(int nX, int nY, BOOL bMoveCursor, BOOL bDelayClick);
void	RClickDone(int nX, int nY);
void	RClick(int nX, int nY, BOOL bMoveCursor, BOOL bDelayClick);
void	MClickDone(int nX, int nY);
void	MClick(int nX, int nY, BOOL bMoveCursor, BOOL bDelayClick);
void	DumpStatus();
void	Test3();
void	Test5();
void	Test6();
void	Test7();
void	Test8();
void	DumpStack(DWORD64 dwRSP, DWORD64 dwDepth);
