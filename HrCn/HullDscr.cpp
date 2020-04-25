#include "ParaSet.h"

struct Defect {
	CvPoint front;
	CvPoint rear;
	CvPoint valley;
	double depth;
};

class Hull {
public:
	Hull(CvSeq* contour);
	void gethull(IplImage* imhull);
	void getdefects(IplImage* imdefects);
	void getw();
	vector<float> cwdpths;
	CvPoint Dwebc;

private:
	int length;
	vector<CvPoint> src;
	vector<int> ids;
	vector<Defect> defects;

	int N;
	CvSeq* contour;

	vector<double> depths;
	vector<CvPoint> valleys;
	int counter;

	void part(int idS, int idE);
	void draw(IplImage* imhull);
};

Hull::Hull(CvSeq* contour)
{
	length = 0;
	counter = 0;
	this->contour = contour;
	//ǳ�������������ı�contour��û��ϵ
	N = contour->total;
}

void Hull::draw(IplImage* imhull)
{
	uchar R, G, B;
	uchar light = (uchar)255, dark = (uchar)0;
	for (int i = 0; i < length - 1; i++)
		drline(imhull, &src[i], &src[i + 1]);
}

void Hull::getdefects(IplImage* imdefects)
{
	Defect defect;
	CvPoint front, rear, valley;
	int idf, idr;
	int width = imdefects->width, height = imdefects->height;
	for (int i = 0; i < length - 1; i++)
	{
		front = src[i];
		rear = src[i + 1];
		idf = ids[i];
		idr = ids[i + 1];
		defect.front = front;
		defect.rear = rear;
		int xfr = rear.x - front.x;
		int yfr = rear.y - front.y;
		double d = 0.0, depth = 0.0;
		valley = front;
		CvPoint prs;
		//contour����Ŵ�����ˣ����ﲻ�õ���
		for (int j = idf; j < idr; j++)
		{
			prs = *(CvPoint*)cvGetSeqElem(contour, j);
			int xfp = prs.x - front.x;
			int yfp = prs.y - front.y;
			d = fabs(xfp * yfr - yfp * xfr) / sqrt(xfr * xfr + yfr * yfr);
			if (d > depth)
			{
				depth = d;
				valley = prs;
			}
		}
		defect.valley = valley;
		defect.depth = depth;

		defects.push_back(defect);

		if (depth > 0 && front.x % (width - 2) && front.y % (height - 2))
		{
			depths.push_back(depth);
			valleys.push_back(valley);
			counter++;

			drclrcircle(imdefects, &valley, 4, 255, 0, 0);
			drclrcircle(imdefects, &front, 4, 0, 0, 255);
			drclrcircle(imdefects, &rear, 4, 0, 0, 255);
			drclrline(imdefects, &front, &valley, 0, 255, 0);
			drclrline(imdefects, &valley, &rear, 0, 255, 0);
		}
	}
}

void Hull::getw()
{
	/*������������*/
	int N_depths = counter;
	int idxm = 0;
	int mdepth = 0;
	for (int i = 0; i < N_depths; i++)
	{
		if (mdepth < depths[i])
		{
			idxm = i;
			mdepth = depths[i];
		}
	}
	Dwebc = valleys[idxm];

	/*����Ԥ���������*/
	float t;
	for (int i = 0; i < 4; i++)
	{
		for (int j = N_depths - 1; j; j--)
		{
			if (depths[j] > depths[j - 1])
			{
				t = depths[j];
				depths[j] = depths[j - 1];
				depths[j - 1] = t;
			}
		}
		cwdpths.push_back(depths[i]);
	}
}

void Hull::part(int idS, int idE)
{
	CvPoint Vs = *(CvPoint*)cvGetSeqElem(contour, idS);
	CvPoint Ve = *(CvPoint*)cvGetSeqElem(contour, idE);
	src.push_back(Vs);
	ids.push_back(idS % N);
	length++;
	int l0 = length;
	idE = idE ? idE : N;
	if (idE - idS <= 1)
		return;

	int xse = Ve.x - Vs.x;
	int yse = -1 * (Ve.y - Vs.y);
	CvPoint prs = *(CvPoint*)cvGetSeqElem(contour, 0);
	int idx = idS;
	while (idx <= idE)
	{
		if (idx == N)
			prs = Ve;
		else
			prs = *(CvPoint*)cvGetSeqElem(contour, idx);
		int xsp = prs.x - Vs.x;
		int ysp = -1 * (prs.y - Vs.y);
		int crs_hypo = xsp * yse - xse * ysp;
		int cls_hypo = crs_hypo >= 0 ? 1 : 0;//��ͬ�ͱ���8
		if (!cls_hypo)
		{
			idx++;
			continue;
		}

		if (length == l0)
		{
			src.push_back(prs);
			ids.push_back(idx % N);
			length++;
			idx++;
			continue;
		}

		int x23 = prs.x - src[length - 1].x;
		int y23 = -1 * (prs.y - src[length - 1].y);
		int x13 = prs.x - src[length - 2].x;
		int y13 = -1 * (prs.y - src[length - 2].y);
		int x12 = src[length - 1].x - src[length - 2].x;
		int y12 = -1 * (src[length - 1].y - src[length - 2].y);
		int crs = x13 * y12 - x12 * y13;
		int cl = crs >= 0 ? 1 : 0;//��˳�͵�
		//������ѭ���ⶨ��
		//src�ĳ����������Լ�С�ڴ�ռ��
		if (cl)
		{
			//ΪӦ�Ը߽װ���ֻ����ѹ���Ҳ�����
			src.pop_back();
			ids.pop_back();
			length--;
		}
		else if (x23 * xse >= 0 && y23 * yse >= 0)//���������ѹ���ǵ�������͹�
		{
			src.push_back(prs);
			ids.push_back(idx % N);
			length++;
			idx++;
		}
		else
			idx++;
	}

	//�ɴ�ȫ��¼,����ҲûʲôӰ��
	//�Ժ�Ҫ����src��ids�ͱ�����length-2������
}

void Hull::gethull(IplImage* imhull)
{
	CvPoint Vl = *(CvPoint*)cvGetSeqElem(contour, 0), Vb = Vl, Vr = Vl, Vt = Vl;
	int idL = 0, idB = 0, idR = 0, idT = 0;
	CvPoint prs = Vl;
	for (int idx = 0; idx < N; idx++)
	{
		prs = *(CvPoint*)cvGetSeqElem(contour, idx);
		//���ظ��Ļ�ѡ��һ��
		if (prs.x < Vl.x)
		{
			Vl = prs;
			idL = idx;
		}
		if (prs.y > Vb.y)
		{
			Vb = prs;
			idB = idx;
		}
		if (prs.x > Vr.x)
		{
			Vr = prs;
			idR = idx;
		}
		if (prs.y < Vt.y)
		{
			Vt = prs;
			idT = idx;
		}
	}

	part(idL, idB);
	part(idB, idR);
	part(idR, idT);
	part(idT, idL);

	draw(imhull);
}

vector<float> HullDscr(CvSeq* contour, CvPoint* Dweb, IplImage* imhull, IplImage* imdefects)
{
	/*������������͹����*/
	Hull hull(contour);

	/*��͹������ͼ*/
	hull.gethull(imhull);

	/*���ȱ�ݲ���ͼ*/
	hull.getdefects(imdefects);

	/*��Ԥ����������������*/
	hull.getw();
	vector<float> cwdpths = hull.cwdpths;
	*Dweb = hull.Dwebc;

	return cwdpths;
}