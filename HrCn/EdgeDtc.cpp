#include "ParaSet.h"

IplImage* EdgeDtc(IplImage* bnr)
{
	IplImage* edge = cvCreateImage(cvGetSize(bnr), 8, 1);
	cvZero(edge);

	cvCanny(bnr, edge, 20, 100, 3);

	return edge;
}
