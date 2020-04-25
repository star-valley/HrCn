#include "ParaSet.h"

Gesture drc(CvPoint* Dweb, CvSeq* contour);

Gesture GesDtm(vector<float> cwdpths, CvPoint* Dweb, CvSeq* contour)
{
	Gesture gesture;
	int Nweb = 1;
	for (int i = 1; i < 4; i++)
	{
		if (cwdpths[i] > cwdpths[0] / 2)
		{
			Nweb++;
		}
	}

	switch (Nweb)
	{
	case 4:gesture = pause; break;
	case 3:gesture = on; break;
	case 2:gesture = confirm; break;
	case 1:gesture = drc(Dweb, contour); break;
	default:gesture = illegal;
	}

	return gesture;
}

Gesture drc(CvPoint* Dweb, CvSeq* contour)
{
	CvPoint* center = new CvPoint;
	int N = contour->total;
	int sum_x = 0, sum_y = 0;
	CvPoint* p;
	for (int idx = 0; idx < N; idx++)
	{
		p = (CvPoint*)cvGetSeqElem(contour, idx);
		sum_x += p->x;
		sum_y += p->y;
	}
	center->x = sum_x / N;
	center->y = sum_y / N;

	Gesture drcgs;
	//注意图像的像素是从上往下排的
	if (Dweb->y < center->y)
		drcgs = up;
	else
		drcgs = down;

	return drcgs;
}
