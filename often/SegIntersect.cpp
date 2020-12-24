#include "SegIntersect.h"
#include <math.h>

#ifndef MAX
#define MAX(a,b) ( ((a)>(b))?(a):(b) )
#endif

#ifndef MIN
#define MIN(a,b) ( ((a)<(b))?(a):(b) )
#endif

#define LINEINTERSECT_CROSS(ps,pe,p) ((pe->x-ps->x)*(p->y-ps->y)-(p->x-ps->x)*(pe->y-ps->y))

//返回true 为相交，false为不相交
bool cxLineIntersect32F(POINT32F *p1, POINT32F *p2, POINT32F *p3, POINT32F *p4)
{
	return MAX(p1->x, p2->x) >= MIN(p3->x, p4->x) &&
		MAX(p3->x, p4->x) >= MIN(p1->x, p2->x) &&
		MAX(p1->y, p2->y) >= MIN(p3->y, p4->y) &&
		MAX(p3->y, p4->y) >= MIN(p1->y, p2->y) &&
		LINEINTERSECT_CROSS(p1, p2, p3)*LINEINTERSECT_CROSS(p1, p2, p4) < 0 &&
		LINEINTERSECT_CROSS(p3, p4, p1)*LINEINTERSECT_CROSS(p3, p4, p2) < 0;
}
