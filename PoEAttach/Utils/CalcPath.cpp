#include "stdafx.h"
#include <math.h>
#include "CalcPath.h"

void DetectConnectedArea(BYTE *pbIn, BYTE *pbOut, int nStartX, int nStartY, int nWidth, int nHeight, BYTE nBk)
{
	int nX, nY, nX2, nY2, nX3, nY3, k;
	int pnDirectX[8] = { -1, 0,   1, 1, 1, 0, -1, -1 };
	int pnDirectY[8] = { -1, -1, -1, 0, 1, 1,  1,  0 };

	if (nWidth <= 0 || nHeight <= 0)
		return;

	int nConnComponentNum = 0, nPointNum, nStackTop;
	int *pnStack = new int[nWidth*nHeight];
	BYTE *pbTemp = new BYTE[nWidth*nHeight];
	BYTE nSeed;
	int *pnConnComponents = new int[nWidth*nHeight];

	memcpy(pbTemp, pbIn, nWidth*nHeight);

	nX = nStartX;
	nY = nStartY;

	if (pbTemp[nY*nWidth + nX] == nBk)
		return;

	nSeed = pbTemp[nY*nWidth + nX];
	pbTemp[nY*nWidth + nX] = nBk;
	nPointNum = 1;
	nStackTop = 0;
	nX3 = nX; nY3 = nY;
	pnConnComponents[nY3*nWidth + nX3] = -1;

	while (1)
	{
		for (k = 0; k < 8; k++)
		{
			nX2 = nX3 + pnDirectX[k];
			nY2 = nY3 + pnDirectY[k];
			if (nX2 < 0 || nX2 >= nWidth)
				continue;
			if (nY2 < 0 || nY2 > nHeight)
				continue;
			if (pbTemp[nY2*nWidth + nX2] != nSeed)
				continue;

			pbTemp[nY2*nWidth + nX2] = nBk;
			nPointNum++;

			if (nStackTop < 0 || nStackTop >= nWidth * nHeight)
			{
				nStackTop = nStackTop;
			}
			pnStack[nStackTop] = nY2 * nWidth + nX2;
			nStackTop++;
		}

		nStackTop--;
		if (nStackTop < 0)
			break;
		nX2 = pnStack[nStackTop] % nWidth;
		nY2 = pnStack[nStackTop] / nWidth;
		pnConnComponents[nY2*nWidth + nX2] = nY3 * nWidth + nX3;
		nY3 = nY2; nX3 = nX2;
	}

	if (nPointNum)
	{
		memset(pbOut, nBk, nWidth*nHeight);
		k = nY3 * nWidth + nX3;
		nX2 = k % nWidth;
		nY2 = k / nWidth;
		pbOut[nY2*nWidth + nX2] = nSeed;
		while ((k = pnConnComponents[k]) != -1)
		{
			nX2 = k % nWidth;
			nY2 = k / nWidth;
			pbOut[nY2*nWidth + nX2] = nSeed;
		}
	}


	delete[] pnConnComponents;
	delete[] pnStack;
	delete[] pbTemp;
}

void CalcRawPath(BYTE *pbIn, int nStartX, int nStartY, int nEndX, int nEndY, int nWidth, int nHeight, BYTE nBk, int *pnPointsNum, IVector *pPoints, bool roundScan/* = false*/)
{
	int nX, nY, nX2, nY2, nX3, nY3, k;
	int pnDirectX[8] = { -1, 0,   1, 1, 1, 0, -1, -1 };
	int pnDirectY[8] = { -1, -1, -1, 0, 1, 1,  1,  0 };

	*pnPointsNum = 0;
	if (nWidth <= 0 || nHeight <= 0)
		return;

	int nConnComponentNum = 0, nPointNum, nStackTop;
	int *pnStack = new int[nWidth*nHeight];
	BYTE *pbTemp = new BYTE[nWidth*nHeight];
	BYTE nSeed;

	memcpy(pbTemp, pbIn, nWidth*nHeight);

	nX = nStartX;
	nY = nStartY;

	if (pbTemp[nY*nWidth + nX] == nBk)
		return;

	nSeed = pbTemp[nY*nWidth + nX];
	pbTemp[nY*nWidth + nX] = nBk;
	nPointNum = 1;
	nStackTop = 0;
	nX3 = nX; nY3 = nY;
	pPoints[0].x = nX;
	pPoints[0].y = nY;


	int nDiffX, nDiffY;
	int pnDir[8];
	while (1)
	{
		nDiffX = nEndX - nX3;
		nDiffY = nEndY - nY3;
		if (nDiffX < 0)
		{
			if (nDiffY < 0)
			{
				//pnDirectX[0] = 1; pnDirectX[1] = 0; pnDirectX[2] = 1; pnDirectX[3] = 1; pnDirectX[4] = -1; pnDirectX[5] = 0; pnDirectX[6] = 0; pnDirectX[7] = 0;
				//pnDirectY[0] = 1; pnDirectY[1] = 1; pnDirectY[2] = 0; pnDirectY[3] = -1; pnDirectY[4] = -1; pnDirectY[5] = 0; pnDirectY[6] = 0; pnDirectY[7] = 0;
				pnDir[0] = 0; pnDir[1] = 1; pnDir[2] = 7; pnDir[3] = 2; pnDir[4] = 6; pnDir[5] = 3; pnDir[6] = 5; pnDir[7] = 4;

			}
			else if (nDiffY > 0)
			{
				pnDir[0] = 6; pnDir[1] = 7; pnDir[2] = 5; pnDir[3] = 0; pnDir[4] = 4; pnDir[5] = 1; pnDir[6] = 3; pnDir[7] = 2;
			}
			else
			{
				pnDir[0] = 7; pnDir[1] = 0; pnDir[2] = 6; pnDir[3] = 1; pnDir[4] = 5; pnDir[5] = 2; pnDir[6] = 4; pnDir[7] = 3;
			}
		}
		else if (nDiffX > 0)
		{
			if (nDiffY < 0)
			{
				pnDir[0] = 2; pnDir[1] = 1; pnDir[2] = 3; pnDir[3] = 0; pnDir[4] = 4; pnDir[5] = 7; pnDir[6] = 5; pnDir[7] = 6;
			}
			else if (nDiffY > 0)
			{
				pnDir[0] = 4; pnDir[1] = 3; pnDir[2] = 5; pnDir[3] = 2; pnDir[4] = 6; pnDir[5] = 7; pnDir[6] = 1; pnDir[7] = 0;
			}
			else
			{
				pnDir[0] = 3; pnDir[1] = 2; pnDir[2] = 4; pnDir[3] = 1; pnDir[4] = 5; pnDir[5] = 0; pnDir[6] = 6; pnDir[7] = 7;
			}
		}
		else
		{
			if (nDiffY < 0)
			{
				pnDir[0] = 1; pnDir[1] = 0; pnDir[2] = 2; pnDir[3] = 7; pnDir[4] = 3; pnDir[5] = 6; pnDir[6] = 5; pnDir[7] = 4;
			}
			else if (nDiffY > 0)
			{
				pnDir[0] = 5; pnDir[1] = 6; pnDir[2] = 4; pnDir[3] = 7; pnDir[4] = 3; pnDir[5] = 0; pnDir[6] = 2; pnDir[7] = 1;
			}
			else
			{
				break;
			}
		}

		for (k = 7; k >= 0; k--)
			//for (k = 0; k < 8; k++)
		{
			nX2 = nX3 + pnDirectX[pnDir[k]];
			nY2 = nY3 + pnDirectY[pnDir[k]];
			if (nX2 < 0 || nX2 >= nWidth)
				continue;
			if (nY2 < 0 || nY2 > nHeight)
				continue;

			if (pbTemp[nY2*nWidth + nX2] != nSeed)
				continue;
			pbTemp[nY2*nWidth + nX2] = nBk;
			if (roundScan)
			{
				if (nX2 > 0 && nX2 < nWidth - 1 && nY2 > 0 && nY2 < nHeight - 1)
				{
					if (pbIn[(nY2 + 1)*nWidth + nX2] != nSeed && pbIn[nY2*nWidth + nX2 + 1] != nSeed)
						continue;
					if (pbIn[(nY2 - 1)*nWidth + nX2] != nSeed && pbIn[nY2*nWidth + nX2 + 1] != nSeed)
						continue;
					if (pbIn[(nY2 + 1)*nWidth + nX2] != nSeed && pbIn[nY2*nWidth + nX2 - 1] != nSeed)
						continue;
					if (pbIn[(nY2 - 1)*nWidth + nX2] != nSeed && pbIn[nY2*nWidth + nX2 - 1] != nSeed)
						continue;
				}
			}
			
			//if (pbIn[nY2*nWidth + nX3] != nSeed && pbIn[nY3*nWidth + nX2] != nSeed)
			//	continue;

			pPoints[nPointNum].x = nX2;
			pPoints[nPointNum].y = nY2;
			nPointNum++;

			if (nX2 == nEndX && nY2 == nEndY)
			{
				nStackTop = 0;
				break;
			}

			if (nStackTop < 0 || nStackTop >= nWidth * nHeight)
			{
				nStackTop = nStackTop;
			}
			pnStack[nStackTop] = nY2 * nWidth + nX2;
			nStackTop++;
		}

		nStackTop--;
		if (nStackTop < 0)
			break;
		nX2 = pnStack[nStackTop] % nWidth;
		nY2 = pnStack[nStackTop] / nWidth;
		nY3 = nY2; nX3 = nX2;
	}

	*pnPointsNum = nPointNum;

	delete[] pnStack;
	delete[] pbTemp;
}

bool IsConnected(BYTE *pbIn, int nWidth, int nHeight, BYTE nBk, IVector pt1, IVector pt2)
{
	int      dx, dy;
	int      p_value;
	int      inc_2dy;
	int      inc_2dydx;
	int      inc_value;
	int      ndx;
	int		 x1, x2, y1, y2;

	x1 = pt1.x;
	y1 = pt1.y;
	x2 = pt2.x;
	y2 = pt2.y;

	dx = abs(x2 - x1);
	dy = abs(y2 - y1);


	if (dy <= dx)
	{
		inc_2dy = 2 * dy;
		inc_2dydx = 2 * (dy - dx);

		if (x2 < x1)
		{
			ndx = x1;
			x1 = x2;
			x2 = ndx;

			ndx = y1;
			y1 = y2;
			y2 = ndx;
		}
		if (y1 < y2)
			inc_value = 1;
		else
			inc_value = -1;

		p_value = 2 * dy - dx;
		for (ndx = x1; ndx < x2; ndx++)
		{
			if (0 > p_value)    p_value += inc_2dy;
			else
			{
				p_value += inc_2dydx;
				y1 += inc_value;
			}
			if (pbIn[y1*nWidth + ndx] == nBk)
				return false;
		}
	}
	else
	{
		inc_2dy = 2 * dx;
		inc_2dydx = 2 * (dx - dy);

		if (y2 < y1)
		{
			ndx = y1;
			y1 = y2;
			y2 = ndx;

			ndx = x1;
			x1 = x2;
			x2 = ndx;
		}
		if (x1 < x2)
			inc_value = 1;
		else
			inc_value = -1;

		p_value = 2 * dx - dy;
		for (ndx = y1; ndx < y2; ndx++)
		{
			if (0 > p_value)
				p_value += inc_2dy;
			else
			{
				p_value += inc_2dydx;
				x1 += inc_value;
			}
			if (pbIn[ndx*nWidth + x1] == nBk)
				return false;
		}
	}
	return true;
}

void CalcMidStep(BYTE *pbIn, int nWidth, int nHeight, BYTE nBk, int nStep, IVector pt1, IVector pt2, IVector *pPointMid)
{
	int      dx, dy;
	int      p_value;
	int      inc_2dy;
	int      inc_2dydx;
	int      inc_value;
	int      ndx;
	int		 x1, x2, y1, y2;

	x1 = pt1.x;
	y1 = pt1.y;
	x2 = pt2.x;
	y2 = pt2.y;

	dx = abs(x2 - x1);
	dy = abs(y2 - y1);

	pPointMid->x = pt1.x + (pt2.x - pt1.x) * 20 / (dx + dy);
	pPointMid->y = pt1.y + (pt2.y - pt1.y) * 20 / (dx + dy);

	if (dy <= dx)
	{
		inc_2dy = 2 * dy;
		inc_2dydx = 2 * (dy - dx);

		if (x2 < x1)
		{
			ndx = x1;
			x1 = x2;
			x2 = ndx;

			ndx = y1;
			y1 = y2;
			y2 = ndx;
		}
		if (y1 < y2)
			inc_value = 1;
		else
			inc_value = -1;

		p_value = 2 * dy - dx;
		for (ndx = x1; ndx < x2; ndx++)
		{
			if (0 > p_value)    p_value += inc_2dy;
			else
			{
				p_value += inc_2dydx;
				y1 += inc_value;
			}
			if (pbIn[y1*nWidth + ndx] == nBk)
				return;

			if ((ndx - pt1.x)*(ndx - pt1.x) + (y1 - pt1.y)*(y1 - pt1.y) >= nStep * nStep)
			{
				pPointMid->x = ndx;
				pPointMid->y = y1;
				return;
			}
		}
	}
	else
	{
		inc_2dy = 2 * dx;
		inc_2dydx = 2 * (dx - dy);

		if (y2 < y1)
		{
			ndx = y1;
			y1 = y2;
			y2 = ndx;

			ndx = x1;
			x1 = x2;
			x2 = ndx;
		}
		if (x1 < x2)
			inc_value = 1;
		else
			inc_value = -1;

		p_value = 2 * dx - dy;
		for (ndx = y1; ndx < y2; ndx++)
		{
			if (0 > p_value)
				p_value += inc_2dy;
			else
			{
				p_value += inc_2dydx;
				x1 += inc_value;
			}
			if (pbIn[ndx*nWidth + x1] == nBk)
				return;

			if ((x1 - pt1.x)*(x1 - pt1.x) + (ndx - pt1.y)*(ndx - pt1.y) >= nStep * nStep)
			{
				pPointMid->x = x1;
				pPointMid->y = ndx;
				return;
			}
		}
	}
}


void CalcRefinePath(BYTE *pbIn, int nWidth, int nHeight, BYTE nBk, int nStep, int *pnPointsNum, IVector *pPoints)
{
	BYTE *pbRefine;
	int nPointNum = *pnPointsNum;

	pbRefine = new BYTE[nPointNum];
	memset(pbRefine, 0, nPointNum);

	int i = nPointNum - 1, j, k;
	while (i > 0)
	{
		k = i - 2;
		for (j = i - 2; j >= 0; j--)
		{
			if (IsConnected(pbIn, nWidth, nHeight, nBk, pPoints[i], pPoints[j]))
			{
				for (k = i - 1; k > j; k--)
					pbRefine[k] = 1;
			}
		}
		i = k;
	}

	j = 0;
	for (i = 0; i < nPointNum; i++)
	{
		if (pbRefine[i])
			continue;
		pPoints[j++] = pPoints[i];
	}
	nPointNum = j;

	int		nDist;
	IVector	pPointMid;

	nStep *= nStep;
	for (i = 0; i < nPointNum - 1; i++)
	{
		nDist = (pPoints[i].x - pPoints[i + 1].x) * (pPoints[i].x - pPoints[i + 1].x) + (pPoints[i].y - pPoints[i + 1].y) * (pPoints[i].y - pPoints[i + 1].y);
		if (nDist > nStep)
		{
			CalcMidStep(pbIn, nWidth, nHeight, nBk, nStep, pPoints[i], pPoints[i + 1], &pPointMid);
			for (j = nPointNum - 1; j > i; j--)
			{
				pPoints[j + 1] = pPoints[j];
			}
			nPointNum++;
			pPoints[i + 1] = pPointMid;
		}
	}


	*pnPointsNum = nPointNum;
	delete[] pbRefine;
}

void CalcScanPath(BYTE *pbIn, int nStartX, int nStartY, int nWidth, int nHeight, BYTE nBk, int nStep, int nPassedRange, int *pnPointsNum, IVector *pPoints)
{
	int nX, nY, i, j;
	int nPointNum = 0;

	*pnPointsNum = 0;
	for (nY = 0; nY < nHeight; nY += nStep)
	{
		for (nX = 0; nX < nWidth; nX += nStep)
		{
			if (pbIn[nY*nWidth + nX] != nBk)
			{
				pPoints[nPointNum].x = nX;
				pPoints[nPointNum].y = nY;
				nPointNum++;
			}
		}
	}

	BYTE *pbPassed = new BYTE[nPointNum];
	IVector *pPointsTemp = new IVector[nPointNum];
	int nPointNumTemp = 0;

	memset(pbPassed, 0, nPointNum);

	while (1)
	{
		// 먼점 1개 가까운점 3개
		int nMax, nDist;

		for (i = 0; i < nPointNum; i++)
		{
			for (j = 0; j < nPointNumTemp; j++)
			{
				nDist = (pPoints[i].x - pPointsTemp[j].x)*(pPoints[i].x - pPointsTemp[j].x) + (pPoints[i].y - pPointsTemp[j].y)*(pPoints[i].y - pPointsTemp[j].y);
				if (nDist < nPassedRange*nPassedRange)
				{
					pbPassed[i] = 1;
				}
			}
			if (!pbPassed[i])
				break;
		}
		if (i >= nPointNum)
			break;

		if (0)//nPointNumTemp % 4 == 0)
		{
			j = 0;
			nMax = 0;
			for (i = 0; i < nPointNum; i++)
			{
				if (pbPassed[i])
					continue;
				nDist = (pPoints[i].x - nStartX)*(pPoints[i].x - nStartX) + (pPoints[i].y - nStartY)*(pPoints[i].y - nStartY);
				if (nDist > nMax)
				{
					nMax = nDist;
					j = i;
				}
			}
			pbPassed[j] = 1;
			pPointsTemp[nPointNumTemp++] = pPoints[j];
			nStartX = pPoints[j].x;
			nStartY = pPoints[j].y;
		}
		else
		{
			j = 0;
			nMax = 0x7FFFFFFF;
			for (i = 0; i < nPointNum; i++)
			{
				if (pbPassed[i])
					continue;
				nDist = (pPoints[i].x - nStartX)*(pPoints[i].x - nStartX) + (pPoints[i].y - nStartY)*(pPoints[i].y - nStartY);
				if (nDist < nMax)
				{
					nMax = nDist;
					j = i;
				}
			}
			pbPassed[j] = 1;
			pPointsTemp[nPointNumTemp++] = pPoints[j];
			nStartX = pPoints[j].x;
			nStartY = pPoints[j].y;
		}
	}

	memcpy(pPoints, pPointsTemp, sizeof(IVector)*nPointNumTemp);
	*pnPointsNum = nPointNumTemp;

	delete[] pbPassed;
	delete[] pPointsTemp;
}

void CalcScanPath2(BYTE *pbIn, int nStartX, int nStartY, int nWidth, int nHeight, BYTE nBk, int nStep, int nPassedRange, int *pnPointsNum, IVector *pPoints)
{
	int *pnSum = new int[nWidth*nHeight];

	int x, y;

	for (y = 0; y < nHeight; y++)
	{
		for (x = 0; x < nWidth; x++)
		{
			pnSum[y*nWidth + x] = (pbIn[y*nWidth + x] == nBk) ? 0 : 1;
		}
	}
	for (y = 1; y < nHeight; y++)
		pnSum[y*nWidth] += pnSum[(y - 1)*nWidth];
	for (x = 1; x < nWidth; x++)
		pnSum[x] += pnSum[x - 1];
	for (y = 1; y < nHeight; y++)
	{
		for (x = 1; x < nWidth; x++)
		{
			pnSum[y*nWidth + x] = pnSum[y*nWidth + x] + pnSum[y*nWidth + x - 1] + pnSum[(y - 1)*nWidth + x] - pnSum[(y - 1)*nWidth + x - 1];
		}
	}

	int i, j, nPointNum, nR, nS;
	int nDist;

	nPointNum = 0;

	nR = (nStep + 1) / 2;
	for (y = nR; y < nHeight - nR; y++)
	{
		for (x = nR; x < nWidth - nR; x++)
		{
			if (pbIn[y * nWidth + x] == nBk)
				continue;
			nS = pnSum[(y + nR)*nWidth + x + nR] + pnSum[(y - nR) * nWidth + x - nR] - pnSum[(y + nR)*nWidth + x - nR] - pnSum[(y - nR)*nWidth + x + nR];
			if (nS > nR*nR * 320 / 100)
			{
				for (i = 0; i < nPointNum; i++)
				{
					nDist = (pPoints[i].x - x)*(pPoints[i].x - x) + (pPoints[i].y - y)*(pPoints[i].y - y);
					if (nDist < nPassedRange*nPassedRange)
						break;
				}
				if (i >= nPointNum)
				{
					pPoints[nPointNum].x = x;
					pPoints[nPointNum].y = y;
					nPointNum++;
				}
			}
		}
	}

	nR = (nR + 1) / 2;
	for (y = nR; y < nHeight - nR; y++)
	{
		for (x = nR; x < nWidth - nR; x++)
		{
			if (pbIn[y * nWidth + x] == nBk)
				continue;
			nS = pnSum[(y + nR)*nWidth + x + nR] + pnSum[(y - nR) * nWidth + x - nR] - pnSum[(y + nR)*nWidth + x - nR] - pnSum[(y - nR)*nWidth + x + nR];
			if (nS > nR*nR * 300 / 100)
			{
				for (i = 0; i < nPointNum; i++)
				{
					nDist = (pPoints[i].x - x)*(pPoints[i].x - x) + (pPoints[i].y - y)*(pPoints[i].y - y);
					if (nDist < nPassedRange*nPassedRange)
						break;
				}
				if (i >= nPointNum)
				{
					pPoints[nPointNum].x = x;
					pPoints[nPointNum].y = y;
					nPointNum++;
				}
			}
		}
	}

	BYTE *pbPassed = new BYTE[nPointNum];
	IVector *pPointsTemp = new IVector[nPointNum];
	int nPointNumTemp = 0, nMax;

	memset(pbPassed, 0, nPointNum);

	while (1)
	{
		for (i = 0; i < nPointNum; i++)
		{
			for (j = 0; j < nPointNumTemp; j++)
			{
				nDist = (pPoints[i].x - pPointsTemp[j].x)*(pPoints[i].x - pPointsTemp[j].x) + (pPoints[i].y - pPointsTemp[j].y)*(pPoints[i].y - pPointsTemp[j].y);
				if (nDist < nPassedRange*nPassedRange)
				{
					pbPassed[i] = 1;
				}
			}
			if (!pbPassed[i])
				break;
		}
		if (i >= nPointNum)
			break;

		j = 0;
		nMax = 0x7FFFFFFF;
		for (i = 0; i < nPointNum; i++)
		{
			if (pbPassed[i])
				continue;
			nDist = (pPoints[i].x - nStartX)*(pPoints[i].x - nStartX) + (pPoints[i].y - nStartY)*(pPoints[i].y - nStartY);
			if (nDist < nMax)
			{
				nMax = nDist;
				j = i;
			}
		}
		pbPassed[j] = 1;
		pPointsTemp[nPointNumTemp++] = pPoints[j];
		nStartX = pPoints[j].x;
		nStartY = pPoints[j].y;
	}

	memcpy(pPoints, pPointsTemp, sizeof(POINT)*nPointNumTemp);
	*pnPointsNum = nPointNumTemp;

	delete[] pbPassed;
	delete[] pPointsTemp;
	delete[] pnSum;
}
