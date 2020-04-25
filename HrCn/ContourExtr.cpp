#include "ParaSet.h"

CvSeq* ContourExtr(IplImage* edge, IplImage* imcontour)
{
	/*�Ա�Եͼ�������̬ѧ����*/
	IplImage* morph = cvCreateImage(cvGetSize(edge), 8, 1);
	cvDilate(edge, morph, 0, 1);

	/*������ȡ*/
	CvMemStorage* contours = cvCreateMemStorage();
	CvSeq* first_contour = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq), sizeof(CvPoint), contours);
	cvFindContours(morph, contours, &first_contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	//contours���������洢����ָ�룬Ĭ�ϴ�СΪ64K��Ӧ����ÿ��64K�ɣ�
	//first_contour�ǵ�һ��������ָ��
	//�����ƺ������Լ�CV_CHAIN_APPROX_SIMPLE��cvPoint(0,0)
	//CV_RETR_LIST

	/*����ɸѡ*/
	CvSeq* contour = first_contour;
	CvSeq* pct = first_contour;
	int MaxArea = 0, CurrentArea = 0;
	while (pct)
	{
		/*ɸѡ��׼�����ֱ����������*/
		CvRect rect = cvBoundingRect(pct, 0);
		CurrentArea = rect.width * rect.height;
		if (MaxArea < CurrentArea)
		{
			contour = pct;
			MaxArea = CurrentArea;
		}
		pct = pct->h_next;
	}

	/*����Ŀ������ͼ��*/
	int N = contour->total;
	CvPoint* p;
	for (int i = 0; i < N; i++)
	{
		p = (CvPoint*)cvGetSeqElem(contour, i);
		drpoint(imcontour, p);
	}

	return contour;
}
