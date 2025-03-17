#pragma once

#include "Common.h"

void DetectConnectedArea(BYTE *pbIn, BYTE *pbOut, int nStartX, int nStartY, int nWidth, int nHeight, BYTE nBk);
void CalcRawPath(BYTE *pbIn, int nStartX, int nStartY, int nEndX, int nEndY, int nWidth, int nHeight, BYTE nBk, int *pnPointsNum, IVector *pPoints, bool roundScan = false);
bool IsConnected(BYTE *pbIn, int nWidth, int nHeight, BYTE nBk, IVector pt1, IVector pt2);
void CalcMidStep(BYTE *pbIn, int nWidth, int nHeight, BYTE nBk, int nStep, IVector pt1, IVector pt2, IVector *pPointMid);
void CalcRefinePath(BYTE *pbIn, int nWidth, int nHeight, BYTE nBk, int nStep, int *pnPointsNum, IVector *pPoints);
void CalcScanPath(BYTE *pbIn, int nStartX, int nStartY, int nWidth, int nHeight, BYTE nBk, int nStep, int nPassedRange, int *pnPointsNum, IVector *pPoints);
void CalcScanPath2(BYTE *pbIn, int nStartX, int nStartY, int nWidth, int nHeight, BYTE nBk, int nStep, int nPassedRange, int *pnPointsNum, IVector *pPoints);
