#include "ParaSet.h"

CvSeq* ContourExtr(IplImage* edge, IplImage* imcontour)
{
	/*对边缘图像进行形态学膨胀*/
	IplImage* morph = cvCreateImage(cvGetSize(edge), 8, 1);
	cvDilate(edge, morph, 0, 1);

	/*轮廓提取*/
	CvMemStorage* contours = cvCreateMemStorage();
	CvSeq* first_contour = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq), sizeof(CvPoint), contours);
	cvFindContours(morph, contours, &first_contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	//contours是轮廓集存储器的指针，默认大小为64K（应该是每块64K吧）
	//first_contour是第一个轮廓的指针
	//参数似乎还可以加CV_CHAIN_APPROX_SIMPLE和cvPoint(0,0)
	//CV_RETR_LIST

	/*轮廓筛选*/
	CvSeq* contour = first_contour;
	CvSeq* pct = first_contour;
	int MaxArea = 0, CurrentArea = 0;
	while (pct)
	{
		/*筛选标准是外接直矩形面积最大*/
		CvRect rect = cvBoundingRect(pct, 0);
		CurrentArea = rect.width * rect.height;
		if (MaxArea < CurrentArea)
		{
			contour = pct;
			MaxArea = CurrentArea;
		}
		pct = pct->h_next;
	}

	/*绘制目标轮廓图像*/
	int N = contour->total;
	CvPoint* p;
	for (int i = 0; i < N; i++)
	{
		p = (CvPoint*)cvGetSeqElem(contour, i);
		drpoint(imcontour, p);
	}

	return contour;
}
