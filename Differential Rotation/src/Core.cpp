#include <Core.h>

namespace midl
{

	// Vector algebra
	bool AddVectors2(float *v1, float *v2, float *v)
	{
		v[0] = v1[0] + v2[0];
		v[1] = v1[1] + v2[1];
		return true;
	}

	bool AddVectors3(float *v1, float *v2, float *v)
	{
		v[0] = v1[0] + v2[0];
		v[1] = v1[1] + v2[1];
		v[2] = v1[2] + v2[2];
		return true;
	}

	bool SubVectors2(float *v1, float *v2, float *v)
	{
		v[0] = v1[0] - v2[0];
		v[1] = v1[1] - v2[1];
		return true;
	}

	bool SubVectors3(float *v1, float *v2, float *v)
	{
		v[0] = v1[0] - v2[0];
		v[1] = v1[1] - v2[1];
		v[2] = v1[2] - v2[2];
		return true;
	}

	bool PointstoVec2(float *v1, float *v2, float *v)
	{
		v[0] = v1[0] - v2[0];
		v[1] = v1[1] - v2[1];

		return true;
	}

	bool PointstoVec3(float *v1, float *v2, float *v)
	{
		v[0] = v1[0] - v2[0];
		v[1] = v1[1] - v2[1];
		v[2] = v1[2] - v2[2];

		return true;
	}

    void ScaleVector2(float *v,float s) {v[0]*=s;v[1]*=s;}
    void ScaleVector2(float *v,float s,float *sv) {sv[0]=s*v[0];sv[1]=s*v[1];}
    void ScaleVector3(float *v,float s) {v[0]=s*v[0];v[1]=s*v[1];v[2]=s*v[2];}
    void ScaleVector3(float *v,float s,float *sv) {sv[0]=s*v[0];sv[1]=s*v[1];sv[2]=s*v[2];}

	float Dot2(float *v1, float *v2)
	{
		return (v1[0] * v2[0]) + (v1[1] * v2[1]);
	}

	float Dot3(float *v1, float *v2)
	{
		return (v1[0] * v2[0]) + (v1[1] * v2[1]) + (v1[2] * v2[2]);
	}

	bool Cross(float *v1, float *v2, float *v)
	{

		v[0] = (v1[1] * v2[2] - v1[2] * v2[1]);
		v[1] = -(v1[0] * v2[2] - v1[2] * v2[0]);
		v[2] = (v1[0] * v2[1] - v1[1] * v2[0]);
		return true;
	}

	float Norm2(float *v)
	{
        return sqrtf(v[0] * v[0] + v[1] * v[1]);
	}

	float Norm3(float *v)
	{
        return sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	}

	float NormSq2(float *v)
	{
		return (v[0] * v[0] + v[1] * v[1]);
	}

	float NormSq3(float *v)
	{
		return (v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	}

	bool Normalize2(float *v)
	{
		float mag = sqrt(v[0] * v[0] + v[1] * v[1]);
		if (mag > FLOAT_EPSILON)
		{
			v[0] /= mag;
			v[1] /= mag;
			return true;
		}
		else return false;
	}

	bool Normalize2(float *v, float *nv)
	{
		float mag = sqrt(v[0] * v[0] + v[1] * v[1]);
		if (mag > FLOAT_EPSILON)
		{
			nv[0] = v[0] / mag;
			nv[1] = v[1] / mag;
			return true;
		}
		else return false;
	}

	bool Normalize3(float *v)
	{
		float mag = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
        if (mag > FLOAT_EPSILON) {
			v[0] /= mag;
			v[1] /= mag;
			v[2] /= mag;
			return true;
		}
		else return false;
	}

	bool Normalize3(float *v, float *nv)
	{
		float mag = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);

		if (mag > FLOAT_EPSILON)
		{
			nv[0] = v[0] / mag;
			nv[1] = v[1] / mag;
			nv[2] = v[2] / mag;
			return true;
		}
		else return false;
	}

	bool MidPoint2(float *p1, float *p2, float *midPoint)
	{
		midPoint[0] = 0.5*(p1[0] + p2[0]);
		midPoint[1] = 0.5*(p1[1] + p2[1]);
		return true;
	}

	bool MidPoint3(float *p1, float *p2, float *midPoint)
	{
		midPoint[0] = 0.5*(p1[0] + p2[0]);
		midPoint[1] = 0.5*(p1[1] + p2[1]);
		midPoint[2] = 0.5*(p1[2] + p2[2]);
		return true;
	}

	bool TriCenter2(float *a, float *b, float *c, float *d)
	{
		d[0] = (a[0] + b[0] + c[0]) / 3.0; 
		d[1] = (a[1] + b[1] + c[1]) / 3.0;

		return true;
	}

	bool TriCenter3(float *a, float *b, float *c, float *d)
	{
		d[0] = (a[0] + b[0] + c[0]) / 3.0; 
		d[1] = (a[1] + b[1] + c[1]) / 3.0; 
		d[2] = (a[2] + b[2] + c[2]) / 3.0;

		return true;
	}

	bool TriNormal(float *p1, float *p2, float *p3, float *n)
	{
		float e1[3], e2[3];
		PointstoVec3(p1, p2, e1);
		PointstoVec3(p3, p2, e2);
		Cross(e1, e2, n);
		bool b = Normalize3(n);

		return b;
	}

	float TriArea2(float *p1, float *p2, float *p3)
	{
		float v1[3],v2[3],v[3];
		SubVectors3(p2,p1,v1);
		SubVectors3(p3,p1,v2);
		v1[2] = 0.0;
		v2[2] = 0.0;
		Cross(v1,v2,v);
		return 0.5*v[2];
		//return 0.5*(p1[0] * (p2[1] - p3[1]) + p2[0] * (p3[1] - p1[1]) + p3[0] * (p1[1] - p2[1]));
	}
	
	float TriArea3(float *p1, float *p2, float *p3, float *n)
	{
		float v1[3],v2[3],v[3];
		SubVectors3(p2,p1,v1);
		SubVectors3(p3,p1,v2);
		Cross(v1,v2,v);
		return 0.5*Dot3(v,n);
	}
	
	bool Cart2Sph(float *cart, float *sphr)
	{
		sphr[2] = sqrt(NormSq3(cart));

		if (!_finite(cart[2] / sphr[2]) || _isnan(cart[2] / sphr[2]))
		{
			sphr[0] = 0.0;
			sphr[1] = 0.0;
			sphr[2] = 0.0;
			return false;
		}

		sphr[0] = acosf(cart[2] / sphr[2]);
		sphr[1] = atan2f(cart[1], cart[0]);

		return true;
	}

	float AxisAngle(float *axis, float angle, float *matrix)
	{
		float c = cosf(angle);
		float s = sinf(angle);
		float t = 1 - c;
		float x = axis[0];
		float y = axis[1];
		float z = axis[2];
		matrix[0] = t*x*x + c;
		matrix[1] = t*x*y - z*s;
		matrix[2] = t*x*z + y*s;
		matrix[3] = t*x*y + z*s;
		matrix[4] = t*y*y + c;
		matrix[5] = t*y*z - x*s;
		matrix[6] = t*x*z - y*s;
		matrix[7] = t*y*z + x*s;
		matrix[8] = t*z*z + c;

		return 1;
	}

	bool ProjectPointOnTri(float *pnt, float *v0, float *v1, float *v2, float *intersect, float *param)
	{
		float e1[3], e2[3];
		float n[3];

		PointstoVec3(v1, v0, e1);
		PointstoVec3(v2, v0, e2);
		Cross(e1, e2, n);
		if (!Normalize3(n))
		{
			intersect[0] = FLT_MAX;
			intersect[1] = FLT_MAX;
			intersect[2] = FLT_MAX;

			param[0] = 0.0;
			param[1] = 0.0;

			return false;
		}

		float sb, sn, sd;
		float r[3];
		PointstoVec3(pnt, v0, r);
		sn = -Dot3(n, r);
		sd = Dot3(n, n);

		sb = sn / sd;

		intersect[0] = pnt[0] + sb*n[0];
		intersect[1] = pnt[1] + sb*n[1];
		intersect[2] = pnt[2] + sb*n[2];

		float e[3], perp[3];
		PointstoVec3(intersect, v0, e);
		Cross(n, e2, perp);
		param[0] = Dot3(e, perp) / Dot3(e1, perp);
		param[1] = Dot3(e, perp) / Dot3(e2, perp);

		return true;
	}

	int pnpoly(float testx, float testy, vector<Tuple2f> &curve, vector<float> &vertx, vector<float> &verty)
	{
		int   i, j = curve.size() - 1;
		bool  oddNodes = 0;

		/*for (i = 0; i<curve.size(); i++)
		{
		if ((verty[i]< testy && verty[j] >= testy
		|| verty[j]< testy && verty[i] >= testy)
		&& (vertx[i] <= testx || vertx[j] <= testx)) {
		oddNodes ^= (vertx[i] + (testy - verty[i]) / (verty[j] - verty[i])*(vertx[j] - vertx[i])<testx);
		}
		j = i;
		}*/
		int cn = 0;
		for (i = 0; i<curve.size(); i++)
		{
			if ((verty[i] <= testy && verty[j] > testy
				|| verty[j] <= testy && verty[i] > testy)
				&& (vertx[i] <= testx || vertx[j] <= testx))
			{
				float vt = (float)(testy - verty[i]) / (verty[j] - verty[i]);
				if (testx <  vertx[i] + vt * (vertx[j] - vertx[i])) // P.x < intersect
					++cn;
				//oddNodes ^= (vertx[i] + (testy - verty[i]) / (verty[j] - verty[i])*(vertx[j] - vertx[i])<testx);
			}
			j = i;
		}

		return (cn & 1);
	}

	inline int isLeft(float P0x, float P0y, float P1x, float P1y, float P2x, float P2y)
	{
		return ((P1x - P0x) * (P2y - P0y)
			- (P2x - P0x) * (P1y - P0y));
	}

	int wn_PnPoly(float testx, float testy, vector<Tuple2f> &curve, vector<float> &vertx, vector<float> &verty)
	{
		int wn = 0;    // the  winding number counter
		int   i, j = curve.size() - 1;
		// loop through all edges of the polygon
		for (i = 0; i<curve.size(); i++)
		{   // edge from V[i] to  V[i+1]
			if (verty[i] <= testy) {          // start y <= P.y
				if (verty[j]  > testy)      // an upward crossing
					if (isLeft(vertx[i], verty[i], vertx[j], verty[j], testx, testy) > 0)  // P left of  edge
						++wn;            // have  a valid up intersect
			}
			else
			{                        // start y > P.y (no test needed)
				if (verty[j] <= testy)     // a downward crossing
					if (isLeft(vertx[i], verty[i], vertx[j], verty[j], testx, testy) < 0)  // P right of  edge
						--wn;            // have  a valid down intersect
			}
			j = i;
		}
		return wn;
	}

	//Geometry
    //-- cart[0,1,2] = x,y,z
    //-- sph[0,1,2] = elevation,azimuth,radius
    //--- elevation -> [0,pi]
    //--- azimuth -> [0,2pi]
    bool Cart2Sph3(float *cart,float *sph)
    {
        sph[2] = Norm3(cart);

        if(!_finite(cart[2]/sph[2]) || _isnan(cart[2]/sph[2]))
        {
            sph[0] = 0.0;
            sph[1] = 0.0;
            sph[2] = 0.0;
            return false;
        }
        else
        {
            sph[0] = std::acosf(cart[2]/sph[2]);
            sph[1] = std::atan2f(cart[1],cart[0]);
            return true;
        }
    }

    bool MapVecToRange2(float *v,float inRange[2][2],float outRange[2][2],bool isConstrain)
    {
        v[0] = (v[0]-inRange[0][0])/(inRange[0][1]-inRange[0][0]);
        v[1] = (v[1]-inRange[1][0])/(inRange[1][1]-inRange[1][0]);

        if(isConstrain)
        {
            if(v[0] < 0.0)v[0] = 0.0;
            if(v[0] > 1.0)v[0] = 1.0;

            if(v[1] < 0.0)v[1] = 0.0;
            if(v[1] > 1.0)v[1] = 1.0;
        }

        v[0] = v[0]*(outRange[0][1]-outRange[0][0]) + outRange[0][0];
        v[1] = v[1]*(outRange[1][1]-outRange[1][0]) + outRange[1][0];
        return true;
    }

    bool MapVecToRange3(float *v,float inRange[3][2],float outRange[3][2],bool isConstrain)
    {
        v[0] = (v[0]-inRange[0][0])/(inRange[0][1]-inRange[0][0]);
        v[1] = (v[1]-inRange[1][0])/(inRange[1][1]-inRange[1][0]);
        v[2] = (v[2]-inRange[2][0])/(inRange[2][1]-inRange[2][0]);

        if(isConstrain)
        {
            if(v[0] < 0.0)v[0] = 0.0;
            if(v[0] > 1.0)v[0] = 1.0;

            if(v[1] < 0.0)v[1] = 0.0;
            if(v[1] > 1.0)v[1] = 1.0;

            if(v[2] < 0.0)v[2] = 0.0;
            if(v[2] > 1.0)v[2] = 1.0;
        }

        v[0] = v[0]*(outRange[0][1]-outRange[0][0]) + outRange[0][0];
        v[1] = v[1]*(outRange[1][1]-outRange[1][0]) + outRange[1][0];
        v[2] = v[2]*(outRange[2][1]-outRange[2][0]) + outRange[2][0];
        return true;
    }

	float PointToLineDist2(float pt[2], float start[2], float end[2])
	{
		float v[2], u[2], a[2], x[2], d;

		u[0] = end[0] - start[0]; u[1] = end[1] - start[1];
		Normalize2(u);
		a[0] = pt[0] - start[0]; a[1] = pt[1] - start[1];
		d = Dot2(a, u);

		x[0] = start[0] + d*u[0]; x[1] = start[1] + d*u[1];
		v[0] = x[0] - pt[0]; v[1] = x[1] - pt[1];

		return Norm2(v);
	}

	float PointToLineDist3(float pt[3], float start[3], float end[3])
	{
		float v[2], u[2], a[2], x[2], d;

		PointstoVec3(start, end, u);
		Normalize3(u);
		PointstoVec3(start, pt, a);
		d = Dot3(a, u);

		x[0] = start[0] + d*u[0];
		x[1] = start[1] + d*u[1];
		x[2] = start[2] + d*u[2];

		v[0] = x[0] - pt[0];
		v[1] = x[1] - pt[1];
		v[2] = x[2] - pt[2];

		return Norm3(v);
	}

    float PointToPlaneDist(float pt[3], float normal[3], float d)
    {
        float dist;
        dist = normal[0] * pt[0] + normal[1] * pt[1] + normal[2] * pt[2] + d;
        dist /= sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);

        return dist;
    }

    float PointToPlaneDist(float pt[3], float planePt[3], float planeN[3])
    {
        float d;

        d = -Dot3(planeN, planePt);
        return PointToPlaneDist(pt, planeN, d);
    }

    float AngleBetweenVectors2(float v1[2], float v2[2])
    {
        float angle, t_v1[3], t_v2[3], cross[3], axis[3];
        float cross_norm, dot;

        t_v1[0] = v1[0]; t_v1[1] = v1[1]; t_v1[2] = 0;
        t_v2[0] = v2[0]; t_v2[1] = v2[1]; t_v2[2] = 0;
        Normalize2(t_v1); Normalize2(t_v2);

        Cross(t_v1, t_v2, cross);
        cross_norm = Norm3(cross);
        dot = Dot3(t_v1, t_v2);
        angle = atan2(cross_norm, dot);
        if (cross[2] < 0)
            angle = -angle;

        return angle;
    }

    float AcuteAngleBetweenVectors2(float v1[2], float v2[2])
    {
        float angle, t_v1[3], t_v2[3], cross[3], axis[3];
        float cross_norm, dot;

        t_v1[0] = v1[0]; t_v1[1] = v1[1]; t_v1[2] = 0;
        t_v2[0] = v2[0]; t_v2[1] = v2[1]; t_v2[2] = 0;
        Normalize2(t_v1); Normalize2(t_v2);

        Cross(t_v1, t_v2, cross);
        cross_norm = Norm3(cross);
        dot = Dot3(t_v1, t_v2);
        angle = atan2(cross_norm, dot);

        if (angle > PI / 2) angle = PI - angle;
        if (cross[2] < 0) angle = -angle;

        return angle;
    }

    float AngleBetweenVectors3(float v1[3], float v2[3])
	{
		float angle, cross[3];
		Normalize3(v1); Normalize3(v2);
		Cross(v1, v2, cross);
		float cross_mag = Norm3(cross);
		float dotprod = Dot3(v1, v2);
		angle = atan2(cross_mag, dotprod);

		return angle;
	}

    void DirectionBetweenPoints(float start_pt[3], float end_pt[3], float v[3])
	{
		for (int i = 0; i < 3; i++)
			v[i] = end_pt[i] - start_pt[i];
		Normalize3(v);
	}

    bool PointInPolygon3(vector<float> &poly, float normal[3], float centroid[3], float pt[3])
	{
        float *trans_pts, t_pt[3], new_pt[3], test_pt[2], out_pt[2], t_v[3];
        float angle, axis[3], matrix[9], z_axis[3];
        int npts;
        bool is_inside;

        is_inside = false;
        z_axis[0] = 0; z_axis[1] = 0; z_axis[2] = 1;
        out_pt[0] = 100; out_pt[1] = 100;
        GetAngleAxisBetweenDirections(normal,z_axis,axis,angle);
        if (abs(angle) > (PI/2)) {
            angle = PI - angle;
            axis[0] *= -1.0;
            axis[1] *= -1.0;
            axis[2] *= -1.0;
        }
        AxisAngle(axis,angle,matrix);
        npts = poly.size()/3;
        trans_pts = new float[2*npts];

        /*FILE *fp = fopen("../Test_Files/test.asc","w");
        fprintf(fp,"%d\n",npts);*/

        //Transform polygon
        for (int i = 0; i < npts; i++) {
            for (int j = 0; j < 3; j++)
                t_pt[j] = poly[3*i+j] - centroid[j];
            RotateVec3(t_pt,matrix,t_v);
            //fprintf(fp,"%f %f %f\n",t_v[0],t_v[1],t_v[2]);
            trans_pts[2*i] = t_v[0];
            trans_pts[2*i+1] = t_v[1];
        }

        //Transform point
        for (int i = 0; i < 3; i++)
            new_pt[i] = pt[i] - centroid[i];
        RotateVec3(new_pt,matrix,t_v);
        //fprintf(fp,"%f %f %f\n",t_v[0],t_v[1],t_v[2]);
        test_pt[0] = t_v[0];
        test_pt[1] = t_v[1];

        //fclose(fp);

        int start_id, end_id, count;
        float start_pt[2], end_pt[2], int_pt2[2];

        count = 0;
        for (int i = 0; i < npts; i++) {
            start_id = i;
            end_id = (i+1) % npts;
            for (int j = 0; j < 2; j++) {
                start_pt[j] = trans_pts[2*start_id+j];
                end_pt[j] = trans_pts[2*end_id+j];
            }
            if (LineLineIntersection2(start_pt,end_pt,test_pt,out_pt,int_pt2))
                count++;
        }
        if ((count%2) > 0)
            is_inside = true;

        delete trans_pts;

        return is_inside;
	}

    bool PointInPolygon2(vector<float> &poly, float pt[2])
	{
		int npts, start_id, end_id, int_count;
		float out_pt[2], int_pt[2], start_pt[2], end_pt[2];

		npts = poly.size() / 2;
		out_pt[0] = 500; out_pt[1] = 500;
		int_count = 0;
		for (int i = 0; i < npts; i++) {
			start_id = i;
			end_id = (i + 1) % npts;
			for (int j = 0; j < 2; j++) {
				start_pt[j] = poly[2 * start_id + j];
				end_pt[j] = poly[2 * end_id + j];
			}
			if (LineLineIntersection2(start_pt, end_pt, pt, out_pt, int_pt))
				int_count++;
		}

		if ((int_count % 2) > 0)
			return true;
		else
			return false;
	}

    bool PointInPolygon2(vector<Tuple2f> &poly, float pt[2])
	{
		int npts, start_id, end_id, int_count;
		float out_pt[2], int_pt[2], start_pt[2], end_pt[2];

		npts = poly.size();
		out_pt[0] = 500; out_pt[1] = 500;
		int_count = 0;
		for (int i = 0; i < npts; i++) {
			start_id = i;
			end_id = (i + 1) % npts;
			for (int j = 0; j < 2; j++) {
				start_pt[j] = poly[start_id].data[j];
				end_pt[j] = poly[end_id].data[j];
			}
			if (LineLineIntersection2(start_pt, end_pt, pt, out_pt, int_pt))
				int_count++;
		}

		if ((int_count % 2) == 1)
			return true;
		else
			return false;
	}

    bool IsPointInAABB(float pt[2], float box[6])
	{
		if (pt[0] >= box[0] && pt[0] <= box[1]) {
			if (pt[1] >= box[2] && pt[1] <= box[3]) {
				if (pt[2] >= box[4] && pt[2] <= box[5]) {
					return true;
				}
			}
		}

		return false;
	}

	void Compute2DCrvBoundingBox(vector<Tuple2f> &crv, float xrng[2], float yrng[2])
	{
		float xmin, xmax, ymin, ymax;

		xmin = FLT_MAX; xmax = -FLT_MAX;
		ymin = FLT_MAX; ymax = -FLT_MAX;
		for (size_t i = 0; i < crv.size(); i++) {
			if (crv[i].data[0] < xmin) xmin = crv[i].data[0];
			if (crv[i].data[0] > xmax) xmax = crv[i].data[0];

			if (crv[i].data[1] < ymin) ymin = crv[i].data[1];
			if (crv[i].data[1] > ymax) ymax = crv[i].data[1];
		}

		xrng[0] = xmin; xrng[1] = xmax;
		yrng[0] = ymin; yrng[1] = ymax;
	}

    void GetDirectionBetweenPoints(float start_pt[3], float end_pt[3], float v[3])
    {
        for (int i = 0; i < 3; i++)
            v[i] = end_pt[i] - start_pt[i];
        Normalize3(v);
    }

    float PointToPointDist3(float pt1[3], float pt2[3])
    {
        return sqrt(sq(pt1[0]-pt2[0])+sq(pt1[1]-pt2[1])+sq(pt1[2]-pt2[2]));
    }

    float PointToPointDist2(float pt1[2], float pt2[2])
    {
        return sqrt(sq(pt1[0]-pt2[0])+sq(pt1[1]-pt2[1]));
    }

	void GetPlaneFrom3Pts(float pts[9], float normal[3], float &d)
	{
		float v1[3], v2[3];
		int a = 0, b = 1, c = 2;
		v1[0] = pts[3 * a] - pts[3 * b]; v2[0] = pts[3 * c] - pts[3 * b];
		v1[1] = pts[3 * a + 1] - pts[3 * b + 1]; v2[1] = pts[3 * c + 1] - pts[3 * b + 1];
		v1[2] = pts[3 * a + 2] - pts[3 * b + 2]; v2[2] = pts[3 * c + 2] - pts[3 * b + 2];
		Cross(v1, v2, normal);
		Normalize3(normal);
		d = -(normal[0] * pts[3 * b] + normal[1] * pts[3 * b + 1] + normal[2] * pts[3 * b + 2]);
	}

    void GetPlaneFromTriangle(float p1[3],float p2[3],float p3[3],float normal[3], float center[3])
    {
        float v1[3], v2[3];
        int a = 0, b = 1, c = 2;
        SubVectors3(p2,p1,v1);
        SubVectors3(p3,p1,v2);
        Cross(v1, v2, normal);
        Normalize3(normal);
        TriCenter3(p1,p2,p3,center);
    }

    void GetAngleAxisBetweenDirections(float v1[3], float v2[3], float axis[3], float &angle)
	{
        Normalize3(v1);
        Normalize3(v2);
		Cross(v1, v2, axis);
        Normalize3(axis);
        //float dotprod = Dot3(v1, v2);
        //angle = atan2(cross_mag, dotprod);
        angle = acosf(Dot3(v1, v2));
	}

	void GetRotationQuaternions(float v1[3], float v2[3], float q[4], float matrix[9])
	{
		float angle, axis[3];
        GetAngleAxisBetweenDirections(v1, v2, axis, angle);
		q[0] = cos(angle / 2);
		q[1] = (sin(angle / 2))*axis[0];
		q[2] = (sin(angle / 2))*axis[1];
		q[3] = (sin(angle / 2))*axis[2];

		matrix[0] = (q[0] * q[0]) + (q[1] * q[1]) - (q[2] * q[2]) - (q[3] * q[3]);
		matrix[1] = (2 * q[1] * q[2]) - (2 * q[0] * q[3]);
		matrix[2] = (2 * q[1] * q[3]) + (2 * q[0] * q[2]);
		matrix[3] = (2 * q[1] * q[2]) + (2 * q[0] * q[3]);
		matrix[4] = (q[0] * q[0]) - (q[1] * q[1]) + (q[2] * q[2]) - (q[3] * q[3]);
		matrix[5] = (2 * q[2] * q[3]) - (2 * q[0] * q[1]);
		matrix[6] = (2 * q[1] * q[3]) - (2 * q[0] * q[2]);
		matrix[7] = (2 * q[2] * q[3]) + (2 * q[0] * q[1]);
		matrix[8] = (q[0] * q[0]) - (q[1] * q[1]) - (q[2] * q[2]) + (q[3] * q[3]);
	}

	void GetAngleAxisFromQuaternion(float q[4], float axis[3], float &angle)
	{
		angle = 2 * acos(q[0]);
		for (int i = 0; i < 3; i++)
			axis[i] = q[i + 1] / sin(angle / 2);
	}

	void ProjectPointTo3DPlane(float plane_pt[3], float n[3], float in_pt[3], float out_pt[3])
	{
		float t;

		t = (Dot3(n, plane_pt) - Dot3(n, in_pt)) / (pow(n[0], 2) + pow(n[1], 2) + pow(n[2], 2));
		for (int i = 0; i < 3; i++) {
			out_pt[i] = in_pt[i] + n[i] * t;
		}
	}

	void ProjectVectorToPlane(float planePt[3], float planeN[3], float v[3], float projV[3])
	{
		float pt[3], projPt[3];

		pt[0] = planePt[0] + v[0];
		pt[1] = planePt[1] + v[1];
		pt[2] = planePt[2] + v[2];

		ProjectPointTo3DPlane(planePt, planeN, pt, projPt);
        GetDirectionBetweenPoints(planePt, projPt, projV);
	}

	bool LineLineIntersection2(float pt1[2], float pt2[2], float pt3[2], float pt4[2], float int_pt[2])
	{
		float den, num_x, num_y;
		float dist1, dist2, dist;

		den = (pt1[0] - pt2[0])*(pt3[1] - pt4[1]) - (pt1[1] - pt2[1])*(pt3[0] - pt4[0]);

		if (abs(den) < 1e-5)
			return false;

		num_x = (pt1[0] * pt2[1] - pt1[1] * pt2[0])*(pt3[0] - pt4[0])
			- (pt1[0] - pt2[0])*(pt3[0] * pt4[1] - pt3[1] * pt4[0]);

		num_y = (pt1[0] * pt2[1] - pt1[1] * pt2[0])*(pt3[1] - pt4[1])
			- (pt1[1] - pt2[1])*(pt3[0] * pt4[1] - pt3[1] * pt4[0]);

		int_pt[0] = num_x / den;
		int_pt[1] = num_y / den;

        dist1 = sqrtf(sq(pt1[0]-int_pt[0]) + sq(pt1[1]-int_pt[1]));
        dist2 = sqrtf(sq(pt2[0]-int_pt[0]) + sq(pt2[1]-int_pt[1]));
        dist = sqrtf(sq(pt1[0]-pt2[0]) + sq(pt1[1]-pt2[1]));
		if (dist1 > dist || dist2 > dist)
			return false;

        dist1 = sqrtf(sq(pt3[0]-int_pt[0]) + sq(pt3[1]-int_pt[1]));
        dist2 = sqrtf(sq(pt4[0]-int_pt[0]) + sq(pt4[1]-int_pt[1]));
        dist = sqrtf(sq(pt3[0]-pt4[0]) + sq(pt3[1]-pt4[1]));
        if (dist1 > dist || dist2 > dist)
			return false;

		return true;
	}

	bool LineLineIntersection3(float pt1[3], float pt2[3], float pt3[3], float pt4[3], float int_pt[3])
	{
		float v1[3], v2[3], a, b, temp[3], l_v[3], r_v[3];
		float dist1, dist2, dist;
		int elem_id;

		GetDirectionBetweenPoints(pt1, pt2, v1);
		GetDirectionBetweenPoints(pt3, pt4, v2);
		Cross(v1, v2, l_v);
		for (int i = 0; i < 3; i++)
			temp[i] = pt3[i] - pt1[i];
		Cross(temp, v2, r_v);

		a = r_v[0] / l_v[0];
		for (int i = 0; i < 3; i++)
			int_pt[i] = pt1[i] + a*v1[i];

		dist1 = PointToPointDist3(pt1, int_pt);
		dist2 = PointToPointDist3(pt2, int_pt);
		dist = PointToPointDist3(pt1, pt2);
		if (dist1 > dist || dist2 > dist)
			return false;

		dist1 = PointToPointDist3(pt3, int_pt);
		dist2 = PointToPointDist3(pt4, int_pt);
		dist = PointToPointDist3(pt3, pt4);
		if (dist1 > dist || dist2 > dist)
			return false;

		cout << int_pt[0] << int_pt[1] << int_pt[2] << endl;
		return true;
	}

	//PCL Functions
	bool TokenizeTuple2f(char key[], float v[2])
	{
		char separater[] = " ,\t\n";
		char *token, *copy;
		int i = 0;

		copy = _strdup(key);
		token = strtok(copy, separater);
		while (token != NULL)
		{
			if (i < 2)
				v[i] = atof(token);
			token = strtok(NULL, separater);
			i++;
		}
		free(copy);
		return true;
	}

	bool TokenizeTuple3f(char key[], float v[3])
	{
		char separater[] = " ,\t\n";
		char *token, *copy;
		int i = 0;

		copy = _strdup(key);
		token = strtok(copy, separater);
		while (token != NULL)
		{
			if (i < 3)
				v[i] = atof(token);
			token = strtok(NULL, separater);
			i++;
		}
		free(copy);
		return true;
	}

	bool ReadPclFromFile(char *fName, vector<Tuple2f> &pcl)
	{
		pcl.clear();
		FILE *stream;
		stream = fopen(fName, "r");
		if (stream == NULL)return false;

		char key[1000];
		while (!feof(stream))
		{
			Tuple2f v;
			strcpy(key, "");
			fgets(key, 1000, stream);
			if (strcmp(key, "") != 0)
			{
				TokenizeTuple2f(key, v.data);
				pcl.push_back(v);
			}
		}

		return true;
	}

	bool ReadPclFromFile(char *fName, vector<Tuple3f> &pcl)
	{
		pcl.clear();
		FILE *stream;
		stream = fopen(fName, "r");
		if (stream == NULL)return false;

		char key[1000];
		while (!feof(stream))
		{
			Tuple3f v;
			strcpy(key, "");
			fgets(key, 1000, stream);
			if (strcmp(key, "") != 0)
			{
				TokenizeTuple3f(key, v.data);
				pcl.push_back(v);
			}
		}
		return true;
	}

	bool PclCentroid(vector<Tuple2f> &pcl, float *centroid)
	{
		centroid[0] = 0.0;
		centroid[1] = 0.0;

		if (pcl.empty())return false;

		for (int i = 0; i < pcl.size(); i ++)
		{
			centroid[0] += pcl[i].data[0];
			centroid[1] += pcl[i].data[1];
		}
		centroid[0] /= (float)pcl.size();
		centroid[1] /= (float)pcl.size();

		return true;
	}

	bool PclCentroid(vector<Tuple3f> &pcl, float *centroid)
	{
		centroid[0] = 0.0;
		centroid[1] = 0.0;
		centroid[2] = 0.0;

		if (pcl.empty())return false;

		for (int i = 0; i < pcl.size(); i++)
		{
			centroid[0] += pcl[i].data[0];
			centroid[1] += pcl[i].data[1];
			centroid[2] += pcl[i].data[2];
		}

		centroid[0] /= (float)pcl.size();
		centroid[1] /= (float)pcl.size();
		centroid[2] /= (float)pcl.size();

		return true;
	}

	bool TranslatePCLBy(vector<Tuple2f> &pcl, float *trans)
	{
		if (pcl.empty())return false;
		for (int i = 0; i < pcl.size(); i ++)
		{
			pcl[i].data[0] += trans[0];
			pcl[i].data[1] += trans[1];
		}
		return true;
	}

	bool TranslatePCLBy(vector<Tuple3f> &pcl, float *trans)
	{
		if (pcl.empty())return false;
		for (int i = 0; i < pcl.size(); i++)
		{
			pcl[i].data[0] += trans[0];
			pcl[i].data[1] += trans[1];
			pcl[i].data[2] += trans[2];
		}
		return true;
	}

	bool TranslatePCLTo(vector<Tuple2f> &pcl, float *trans)
	{
		if (pcl.empty())return false;
		float c[2];
		PclCentroid(pcl, c);

		for (int i = 0; i < pcl.size(); i ++)
		{
			pcl[i].data[0] += trans[0] - c[0];
			pcl[i].data[1] += trans[1] - c[1];
		}
		return true;
	}

	bool TranslatePCLTo(vector<Tuple3f> &pcl, float *trans)
	{
		if (pcl.empty())return false;
		float c[3];
		PclCentroid(pcl, c);

		for (int i = 0; i < pcl.size(); i++)
		{
			pcl[i].data[0] += trans[0] - c[0];
			pcl[i].data[1] += trans[1] - c[1];
			pcl[i].data[2] += trans[2] - c[2];
		}

		return true;
	}

	bool RotatePcl(vector<Tuple2f> &pcl, float angle)
	{
		if (pcl.empty())return false;
		float mat[9], v[3], t_v[3], c[3];
		PclCentroid(pcl, c);
		AxisAngle(Zaxis, angle, mat);
		for (int i = 0; i < pcl.size(); i++)
		{
			v[0] = pcl[i].data[0] - c[0];
			v[1] = pcl[i].data[1] - c[1];
			v[2] = 0.0;
			RotateVec3(v, mat, t_v);
			pcl[i].data[0] = t_v[0] + c[0];
			pcl[i].data[1] = t_v[1] + c[1];
		}
		return true;
	}

	bool RotatePcl(vector<Tuple2f> &pcl, float angle, float pivot[3])
	{
		if (pcl.empty())return false;
		float mat[9], v[3], t_v[3];
		AxisAngle(Zaxis, angle, mat);
		for (int i = 0; i < pcl.size(); i++)
		{
			v[0] = pcl[i].data[0] - pivot[0];
			v[1] = pcl[i].data[1] - pivot[1];
			v[2] = 0.0;
			RotateVec3(v, mat, t_v);
			pcl[i].data[0] = t_v[0] + pivot[0];
			pcl[i].data[1] = t_v[1] + pivot[1];
		}
		return true;
	}

	bool RotatePcl(vector<Tuple3f> &pcl, float *axis, float angle)
	{
		if (pcl.empty())return false;
		float mat[9], c[3], v[3], t_v[3];
		PclCentroid(pcl, c);
		AxisAngle(axis, angle, mat);
		for (int i = 0; i < pcl.size(); i++)
		{
			v[0] = pcl[i].data[0] - c[0];
			v[1] = pcl[i].data[1] - c[1];
			v[2] = pcl[i].data[2] - c[2];
			RotateVec3(v, mat, t_v);
			pcl[i].data[0] = t_v[0] + c[0];
			pcl[i].data[1] = t_v[1] + c[1];
			pcl[i].data[2] = t_v[2] + c[2];
		}
		return true;
	}

	bool RotatePcl(vector<Tuple3f> &pcl, float *axis, float angle, float pivot[3])
	{
		if (pcl.empty())return false;
		float mat[9], v[3], t_v[3];

		AxisAngle(axis, angle, mat);
		for (int i = 0; i < pcl.size(); i++)
		{
			v[0] = pcl[i].data[0] - pivot[0];
			v[1] = pcl[i].data[1] - pivot[1];
			v[2] = pcl[i].data[2] - pivot[2];

			RotateVec3(v, mat, t_v);

			pcl[i].data[0] = t_v[0] + pivot[0];
			pcl[i].data[1] = t_v[1] + pivot[1];
			pcl[i].data[2] = t_v[2] + pivot[2];
		}
		return true;
	}

	bool ScalePcl(vector<Tuple2f> &pcl, float *scl)
	{
		if (pcl.empty())return false;
		float c[2];
		PclCentroid(pcl, c);
		for (int i = 0; i < pcl.size(); i++)
		{
			pcl[i].data[0] = scl[0] * (pcl[i].data[0] - c[0]) + c[0];
			pcl[i].data[1] = scl[1] * (pcl[i].data[1] - c[1]) + c[1];
		}
		return true;
	}

	bool ScalePcl(vector<Tuple2f> &pcl, float *scl, float pivot[2])
	{
		if (pcl.empty())return false;

		for (int i = 0; i < pcl.size(); i++)
		{
			pcl[i].data[0] -= pivot[0];
			pcl[i].data[1] -= pivot[1];

			pcl[i].data[0] *= scl[0];
			pcl[i].data[1] *= scl[1];

			pcl[i].data[0] += pivot[0];
			pcl[i].data[1] += pivot[1];
		}
		return true;
	}

	bool ScalePcl(vector<Tuple3f> &pcl, float *scl)
	{

		if (pcl.empty())return false;
		float c[3];
		PclCentroid(pcl, c);
		for (int i = 0; i < pcl.size(); i++)
		{
			pcl[i].data[0] = scl[0] * (pcl[i].data[0] - c[0]) + c[0];
			pcl[i].data[1] = scl[1] * (pcl[i].data[1] - c[1]) + c[1];
			pcl[i].data[2] = scl[2] * (pcl[i].data[2] - c[2]) + c[2];
		}
		return true;
	}

	bool ScalePcl(vector<Tuple3f> &pcl, float *scl, float pivot[3])
	{
		if (pcl.empty())return false;

		for (int i = 0; i < pcl.size(); i++)
		{
			pcl[i].data[0] = scl[0] * (pcl[i].data[0] - pivot[0]) + pivot[0];
			pcl[i].data[1] = scl[1] * (pcl[i].data[1] - pivot[1]) + pivot[1];
			pcl[i].data[2] = scl[2] * (pcl[i].data[2] - pivot[2]) + pivot[2];
		}
		return true;
	}

	bool ScaleGlobalPcl(vector<Tuple2f> &pcl, float *scl)
	{
		if (pcl.empty())return false;
		for (int i = 0; i < pcl.size(); i++)
		{
			pcl[i].data[0] *= scl[0];
			pcl[i].data[1] *= scl[1];
		}
		return true;
	}

	bool ScaleGlobalPcl(vector<Tuple3f> &pcl, float *scl)
	{
		if (pcl.empty())return false;
		for (int i = 0; i < pcl.size(); i++)
		{
			pcl[i].data[0] *= scl[0];
			pcl[i].data[1] *= scl[1];
			pcl[i].data[2] *= scl[2];
		}
		return true;
	}

	bool TransformPcl(vector<Tuple3f> &pcl, vector<Tuple3f> &t_pcl, Transformation &t)
	{
		t_pcl.clear();
		if (pcl.empty())return false;
		for (int i = 0; i < pcl.size(); i++)
		{
			Tuple3f t_v;
			t.ApplyTo(pcl[i].data, t_v.data);
			t_pcl.push_back(t_v);
		}
		return true;
	}

	bool PclAABB(vector<Tuple2f> &pcl, float *xRng, float *yRng)
	{
		if (pcl.size() == 0 ) return false;

		xRng[0] = FLT_MAX; xRng[1] = FLT_MIN;
		yRng[0] = FLT_MAX; yRng[1] = FLT_MIN;

		for (size_t i = 0; i < pcl.size(); i++) {
			if (pcl[i].data[0] < xRng[0]) xRng[0] = pcl[i].data[0];
			if (pcl[i].data[0] > xRng[1]) xRng[1] = pcl[i].data[0];

			if (pcl[i].data[1] < yRng[0]) yRng[0] = pcl[i].data[1];
			if (pcl[i].data[1] > yRng[1]) yRng[1] = pcl[i].data[1];
		}

		return true;
	}

	bool PclAABB(vector<Tuple3f> &pcl, float *xRng, float *yRng, float *zRng)
	{
		if (pcl.size() == 0) return false;

		xRng[0] = FLT_MAX; xRng[1] = FLT_MIN;
		yRng[0] = FLT_MAX; yRng[1] = FLT_MIN;
		zRng[0] = FLT_MAX; zRng[1] = FLT_MIN;

		for (size_t i = 0; i < pcl.size(); i++) {
			if (pcl[i].data[0] < xRng[0]) xRng[0] = pcl[i].data[0];
			if (pcl[i].data[0] > xRng[1]) xRng[1] = pcl[i].data[0];

			if (pcl[i].data[1] < yRng[0]) yRng[0] = pcl[i].data[1];
			if (pcl[i].data[1] > yRng[1]) yRng[1] = pcl[i].data[1];

			if (pcl[i].data[2] < zRng[0]) zRng[0] = pcl[i].data[2];
			if (pcl[i].data[2] > zRng[1]) zRng[1] = pcl[i].data[2];
		}

		return true;
	}

    int PclScaleToUnitBox(vector<Tuple3f> &pcl)
    {
        if(pcl.size()==0)
            return -1;

        float centroid[3]; float origin[]={0.0,0.0,0.0};
        float xRng[2], yRng[2], zRng[2];
        PclCentroid(pcl, centroid);
        TranslatePCLTo(pcl, origin);
        PclAABB(pcl, xRng, yRng, zRng);
        float len[]={xRng[1]-xRng[0], yRng[1]-yRng[0], zRng[1]-zRng[0]};

       // cerr<<"Pcl Box Dimensions: "<<len[0]<<"  "<<len[1]<<"  "<<len[2]<<endl;

        float scl;

        if(len[0] > len[1] && len[0] > len[1])
            scl = 1.0/len[0];
        else if(len[1] > len[0] && len[1] > len[2])
            scl = 1.0/len[1];
        else scl = 1.0/len[2];

        if(_isnan(scl) || !_finite(scl))scl = 1.0;

      
        for(size_t i=0;i<pcl.size();i++)
        {
			// cerr<<"Pcl scaled by a factor of "<<scl<<endl;
           pcl[i].data[0]=scl*pcl[i].data[0];
           pcl[i].data[1]=scl*pcl[i].data[1];
           pcl[i].data[2]=scl*pcl[i].data[2];
        }

        return 0;
    }

	bool PclPCA(vector<Tuple2f> &pcl)
	{
		return true;
	}

	bool PclPCA(vector<Tuple3f> &pcl)
	{
		return true;
	}

	bool RotateVec3(float *v, float *matrix, float *t_v)
	{
		t_v[0] = matrix[0] * v[0] + matrix[1] * v[1] + matrix[2] * v[2];
		t_v[1] = matrix[3] * v[0] + matrix[4] * v[1] + matrix[5] * v[2];
		t_v[2] = matrix[6] * v[0] + matrix[7] * v[1] + matrix[8] * v[2];

		return true;
	}

	bool RotateVec3(float *v, float *matrix)
	{
		float t_v[3];
		t_v[0] = matrix[0] * v[0] + matrix[1] * v[1] + matrix[2] * v[2];
		t_v[1] = matrix[3] * v[0] + matrix[4] * v[1] + matrix[5] * v[2];
		t_v[2] = matrix[6] * v[0] + matrix[7] * v[1] + matrix[8] * v[2];

		v[0] = t_v[0];
		v[1] = t_v[1];
		v[2] = t_v[2];

		return true;
	}

	int RandomInt(int _min, int _max)
	{
		int diff = _max - _min;
		return _min + rand() % diff;
	}

	float RandomFloat(float _min, float _max)
	{
		float random = ((float)rand()) / (float)RAND_MAX;
		float diff = _max - _min;
		float r = random * diff;
		return _min + r;
	}

	int MeanStd(vector<float> &sample, float &mean, float &std_dev)
	{
		mean = 0.0;
		std_dev = 0.0;

		for (int i = 0; i < sample.size(); i++)
			mean += sample[i];
		mean /= (float)sample.size();

		for (int i = 0; i < sample.size(); i++)
			std_dev += (sample[i] - mean)*(sample[i] - mean);
		std_dev /= (float)sample.size();
		std_dev = std::sqrtf(std_dev);
		return 0;
	}

	float MeanDifference(vector<float> &sample, vector<float> &scores)
	{
        float mean = 0.0;

        for(size_t i = 0;i < sample.size();i++)
            mean += sample[i];
        mean /= (float)sample.size();

        if(scores.empty() || scores.size() != sample.size())
        {
            scores.clear();
            for(size_t i = 0;i < sample.size();i++)
                scores.push_back(sample[i]-mean);
        }
        else
        {
            for(size_t i = 0;i < sample.size();i++)
                scores[i] = sample[i]-mean;
        }

        return mean;
	}

	float MeanDifference(vector<float> &sample)
	{
        float mean = 0.0;

        for(size_t i = 0;i < sample.size();i++)
            mean += sample[i];
        mean /= (float)sample.size();

        for(size_t i = 0;i < sample.size();i++)
            sample[i] = sample[i]-mean;

        return mean;
	}


    int StandardScore(vector<float> &sample,vector<float> &scores,float *mean_std)
    {
        scores.clear();
        float mean = 0.0;
        float std_dev = 0.0;

        for(size_t i = 0;i < sample.size();i++)
            mean += sample[i];
        mean /= (float)sample.size();

        for(size_t i = 0;i < sample.size();i++)
            std_dev += (sample[i]-mean)*(sample[i]-mean);
        std_dev /= (float)sample.size();
        std_dev = std::sqrtf(std_dev);

        for(size_t i = 0;i < sample.size();i++)
            scores.push_back((sample[i]-mean)/std_dev);

        if(mean_std != NULL)
        {
            mean_std[0] = mean;
            mean_std[1] = std_dev;
        }
        return 0;
    }

    int RadialBasisVector(int rbf_choice,vector<float> &sample,vector<float> &rbf_vector,float *param)
    {
        rbf_vector.clear();
        param[0] = 0.0;
        param[1] = 0.0;

        for(size_t i = 0;i < sample.size();i++)
            param[0] += sample[i];
        param[0] /= (float)sample.size();

        for(size_t i = 0;i < sample.size();i++)
            param[1] += (sample[i]-param[0])*(sample[i]-param[0]);
        param[1] /= (float)sample.size();
        param[1] = std::sqrtf(param[1]);

        for(size_t i = 0;i < sample.size();i++)
        {
            float rbf_val;
            if(rbf_choice == RBF_GAUSSIAN)
                rbf_val = std::expf(-0.5*(sample[i]-param[0])*(sample[i]-param[0])/(param[1]*param[1]));
            else if(rbf_choice == RBF_TPS)
            {
                if(std::fabsf(sample[i]-param[0]) < 1.0e-6)
                    rbf_val = 0.0;
                else rbf_val = (sample[i]-param[0])*(sample[i]-param[0])*std::logf(sample[i]-param[0]);
            }
            else if(rbf_choice == RBF_MULTIQUADRIC)
                rbf_val = std::sqrtf(1.0+(param[1]*param[1]*(sample[i]-param[0])*(sample[i]-param[0])));
            else if(rbf_choice == RBF_INVMULTIQUADRIC)
                rbf_val = 1.0/std::sqrtf(1.0+(param[1]*param[1]*(sample[i]-param[0])*(sample[i]-param[0])));
            else rbf_val = sample[i];
            rbf_vector.push_back(rbf_val);
        }
        return 0;
    }

    int NormalizeRange(vector<float> &sample,vector<float> &nrm_vector,size_t *min_max_idx,float *min_max_dist)
    {
        nrm_vector.clear();
        float _min = sample[0];
        float _max = sample[0];

        size_t min_idx = 0;
        size_t max_idx = 0;

        for(size_t i = 1;i < sample.size();i++)
        {
            if(_min > sample[i])
            {
                _min = sample[i];
                min_idx = i;
            }

            if(_max < sample[i])
            {
                _max = sample[i];
                max_idx = i;
            }
        }

        if(min_max_dist != NULL)
        {
            min_max_dist[0] = _min;
            min_max_dist[1] = _max;
        }

        if(min_max_idx != NULL)
        {
            min_max_idx[0] = min_idx;
            min_max_idx[1] = max_idx;
        }

        float range = _max-_min;
        if(nrm_vector.empty() || nrm_vector.size() != sample.size())
        {
            nrm_vector.clear();
            if(_isnan(1.0/range) || !_finite(1.0/range))
            {
                for(size_t i = 0;i < sample.size();i++)
                    nrm_vector.push_back(1.0);
            }
            else
            {
                for(size_t i = 0;i < sample.size();i++)
                    nrm_vector.push_back((sample[i]-_min)/range);
            }
        }
        else
        {
            if(_isnan(1.0/range) || !_finite(1.0/range))
            {
                for(size_t i = 0;i < sample.size();i++)
                    nrm_vector[i] = 1.0;
            }
            else
            {
                for(size_t i = 0;i < sample.size();i++)
                    nrm_vector[i] = (sample[i]-_min)/range;
            }
        }

        return 0;
    }

    int NormalizeRange(vector<float> &sample,size_t *min_max_idx,float *min_max_dist)
    {
        float _min = sample[0];
        float _max = sample[0];

        size_t min_idx = 0;
        size_t max_idx = 0;

        for(size_t i = 1;i < sample.size();i++)
        {
            if(_min > sample[i])
            {
                _min = sample[i];
                min_idx = i;
            }

            if(_max < sample[i])
            {
                _max = sample[i];
                max_idx = i;
            }
        }

        if(min_max_dist != NULL)
        {
            min_max_dist[0] = _min;
            min_max_dist[1] = _max;
        }

        if(min_max_idx != NULL)
        {
            min_max_idx[0] = min_idx;
            min_max_idx[1] = max_idx;
        }

        float range = _max-_min;
        if(_isnan(1.0/range) || !_finite(1.0/range))
        {
            for(size_t i = 0;i < sample.size();i++)
                sample[i] = 1.0;
        }
        else
        {
            for(size_t i = 0;i < sample.size();i++)
                sample[i] = (sample[i]-_min)/range;
        }

        return 0;
    }

    int NormalizeRange(float *rng,vector<float> &sample,vector<float> &nrm_vector,size_t *min_max_idx,float *min_max_dist)
    {
        float _min = sample[0];
        float _max = sample[0];

        size_t min_idx = 0;
        size_t max_idx = 0;

        for(size_t i = 1;i < sample.size();i++)
        {
            if(_min > sample[i])
            {
                _min = sample[i];
                min_idx = i;
            }

            if(_max < sample[i])
            {
                _max = sample[i];
                max_idx = i;
            }
        }

        if(min_max_dist != NULL)
        {
            min_max_dist[0] = _min;
            min_max_dist[1] = _max;
        }

        if(min_max_idx != NULL)
        {
            min_max_idx[0] = min_idx;
            min_max_idx[1] = max_idx;
        }

        float range = _max-_min;
        float R = rng[1]-rng[0];

        if(nrm_vector.empty() || nrm_vector.size() != sample.size())
        {
            nrm_vector.clear();
            if(_isnan(1.0/range) || !_finite(1.0/range))
            {
                for(size_t i = 0;i < sample.size();i++)
                    nrm_vector.push_back(rng[1]);
            }
            else
            {
                for(size_t i = 0;i < sample.size();i++)
                    nrm_vector.push_back(rng[0]+(R*(sample[i]-_min)/range));
            }
        }
        else
        {
            if(_isnan(1.0/range) || !_finite(1.0/range))
            {
                for(size_t i = 0;i < sample.size();i++)
                    nrm_vector[i] = rng[1];
            }
            else
            {
                for(size_t i = 0;i < sample.size();i++)
                    nrm_vector[i] = rng[0]+(R*(sample[i]-_min)/range);
            }
        }

        return 0;
    }

    int NormalizeRange(float *rng,vector<float> &sample,size_t *min_max_idx,float *min_max_dist)
    {
        float _min = sample[0];
        float _max = sample[0];

        size_t min_idx = 0;
        size_t max_idx = 0;

        for(size_t i = 1;i < sample.size();i++)
        {
            if(_min > sample[i])
            {
                _min = sample[i];
                min_idx = i;
            }

            if(_max < sample[i])
            {
                _max = sample[i];
                max_idx = i;
            }
        }

        if(min_max_dist != NULL)
        {
            min_max_dist[0] = _min;
            min_max_dist[1] = _max;
        }

        if(min_max_idx != NULL)
        {
            min_max_idx[0] = min_idx;
            min_max_idx[1] = max_idx;
        }

        float range = _max-_min;
        float R = rng[1]-rng[0];

        if(_isnan(1.0/range) || !_finite(1.0/range))
        {
            for(size_t i = 0;i < sample.size();i++)
                sample[i] = rng[1];
        }
        else
        {
            for(size_t i = 0;i < sample.size();i++)
                sample[i] = rng[0]+(R*(sample[i]-_min)/range);
        }

        return 0;
    }


	// Transformation
	float _transfrm_Dot4(float *v1, float *v2)
	{
		return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2] + v1[3] * v2[3];
	}

	float Det33(float mat[9])
	{
		float det;

		det = mat[0] * ((mat[4] * mat[8]) - (mat[5] * mat[7]))
			- mat[1] * ((mat[3] * mat[8]) - (mat[5] * mat[6]))
			+ mat[2] * ((mat[3] * mat[7]) - (mat[4] * mat[6]));

		return det;
	}

	Transformation::Transformation()
	{
		for (int i = 0; i < 16; i++)
			elements[i] = 0;

		elements[0] = 1.0;
		elements[5] = 1.0;
		elements[10] = 1.0;
		elements[15] = 1.0;
	}

	Transformation::~Transformation(){}

	void Transformation::GetMatrix(float *matrix)
	{
		for (int i = 0; i < 16; i++)
			matrix[i] = elements[i];
	}

    void Transformation::GetMatrixTranspose(float *matrix)
    {
        matrix[0] = elements[0];
        matrix[1] = elements[4];
        matrix[2] = elements[8];
        matrix[3] = elements[12];

        matrix[4] = elements[1];
        matrix[5] = elements[5];
        matrix[6] = elements[9];
        matrix[7] = elements[13];

        matrix[8] = elements[2];
        matrix[9] = elements[6];
        matrix[10] = elements[10];
        matrix[11] = elements[14];

        matrix[12] = elements[3];
        matrix[13] = elements[7];
        matrix[14] = elements[11];
        matrix[15] = elements[15];
    }

    void Transformation::SetMatrix(float *matrix)
	{
		for (int i = 0; i < 16; i++)
			elements[i] = matrix[i];
	}

    void Transformation::SetTranslationBy(float *translation)
    {
        for (int i = 0; i < 16; i++)
            elements[i] = 0.0;

        elements[0] = 1.0;
        elements[5] = 1.0;
        elements[10] = 1.0;
        elements[15] = 1.0;

        elements[3] = translation[0];
        elements[7] = translation[1];
        elements[11] = translation[2];
    }

	void Transformation::SetIdentity()
	{
		for (int i = 0; i < 16; i++)
			elements[i] = 0.0;

		elements[0] = 1.0;
		elements[5] = 1.0;
		elements[10] = 1.0;
		elements[15] = 1.0;
    }

    void Transformation::SetXRotation(float angle)
	{
		elements[0] = 1;
		elements[1] = 0;
		elements[2] = 0;
		elements[3] = 0;
		elements[4] = 0;
		elements[5] = cos(angle);
		elements[6] = -sin(angle);
		elements[7] = 0;
		elements[8] = 0;
		elements[9] = sin(angle);
		elements[10] = cos(angle);
		elements[11] = 0;
		elements[12] = 0;
		elements[13] = 0;
		elements[14] = 0;
		elements[15] = 1;
	}

    void Transformation::SetYRotation(float angle)
	{
		elements[0] = cos(angle);
		elements[1] = 0;
		elements[2] = sin(angle);
		elements[3] = 0;
		elements[4] = 0;
		elements[5] = 1;
		elements[6] = 0;
		elements[7] = 0;
		elements[8] = -sin(angle);
		elements[9] = 0;
		elements[10] = cos(angle);
		elements[11] = 0;
		elements[12] = 0;
		elements[13] = 0;
		elements[14] = 0;
		elements[15] = 1;
	}

    void Transformation::SetZRotation(float angle)
	{
		elements[0] = cos(angle);
		elements[1] = -sin(angle);
		elements[2] = 0;
		elements[3] = 0;
		elements[4] = sin(angle);
		elements[5] = cos(angle);
		elements[6] = 0;
		elements[7] = 0;
		elements[8] = 0;
		elements[9] = 0;
		elements[10] = 1;
		elements[11] = 0;
		elements[12] = 0;
		elements[13] = 0;
		elements[14] = 0;
		elements[15] = 1;
	}

    void Transformation::SetRollPitchYawRotation(float *RPY)
	{
        float cu = cosf(RPY[0]);
        float su = sinf(RPY[0]);
        float cv = cosf(RPY[1]);
        float sv = sinf(RPY[1]);
        float cw = cosf(RPY[2]);
        float sw = sinf(RPY[2]);

        elements[0] = cv*cw;
        elements[1] = su*sv*cw - cu*sw;
        elements[2] = su*sw + cu*sv*cw;
        elements[3] = 0.0;

        elements[4] = cv*sw;
        elements[5] = cu*cw + su*sv*sw;
        elements[6] = cu*sv*sw - su*cw;
        elements[7] = 0.0;

        elements[8] = -sv;
        elements[9] = su*cv;
        elements[10] = cu*cv;
        elements[11] = 0.0;

        elements[12] = 0.0;
        elements[13] = 0.0;
        elements[14] = 0.0;
        elements[15] = 1.0;
	}

    void Transformation::SetAxisAngleRotation(float *axis, float angle)
	{
		float L = axis[0] * axis[0] + axis[1] * axis[1] + axis[2] * axis[2];
		float theta = (angle*3.14159) / 180;
		float u = axis[0] * axis[0];
		float k = axis[1] * axis[1];
		float w = axis[2] * axis[2];

        elements[0] = (u + (k + w) * cos(theta)) / L;
        elements[1] = (axis[0] * axis[1] * (1 - cos(theta)) - axis[2] * sqrt(L) * sin(theta)) / L;
        elements[2] = (axis[0] * axis[2] * (1 - cos(theta)) + axis[1] * sqrt(L) * sin(theta)) / L;
        elements[3] = 0.0;

        elements[4] = (axis[0] * axis[1] * (1 - cos(theta)) + axis[2] * sqrt(L) * sin(theta)) / L;
        elements[5] = (k + (u + w) * cos(theta)) / L;
        elements[6] = (axis[1] * axis[2] * (1 - cos(theta)) - axis[0] * sqrt(L) * sin(theta)) / L;
        elements[7] = 0.0;

        elements[8] = (axis[0] * axis[2] * (1 - cos(theta)) - axis[1] * sqrt(L) * sin(theta)) / L;
        elements[9] = (axis[1] * axis[2] * (1 - cos(theta)) + axis[0] * sqrt(L) * sin(theta)) / L;
        elements[10] = (w + (u + k) * cos(theta)) / L;
        elements[11] = 0.0;

        elements[12] = 0.0;
        elements[13] = 0.0;
        elements[14] = 0.0;
        elements[15] = 1.0;
	}

    void Transformation::SetScale(float *scale)
	{
        for (int i = 0; i < 16; i++)
            elements[i] = 0.0;

        elements[0] = scale[0];
        elements[5] = scale[1];
        elements[10] = scale[2];
        elements[15] = 1.0;
	}

	Transformation Transformation::Inverse()
	{
        Transformation T;
		int sign,i,j,count = 0;
		int row,col;
		float cof_mat[9],cof_det;

		for(row = 0;row < 4;row++)
		{
			for(col = 0;col < 4;col++)
			{
				count  = 0;
				for(i = 0;i < 4;i++)
				{
					for(j = 0;j < 4;j++)
					{
						if(i != row && j != col)
						{
							cof_mat[count] = elements[4*i+j];
							count++;
						}
					}
				}
				if ((row+col)%2 == 0)
					sign = 1;
				if ((row+col)%2 == 1)
					sign = -1;
				cof_det = sign*Det33(cof_mat);
				T.elements[4*col+row] = cof_det;
			}
		}
		return T;
	}

	void Transformation::ApplyTo(float *v)
	{
		float t_v[3];
		t_v[0] = elements[0] * v[0] + elements[1] * v[1] + elements[2] * v[2] + elements[3];
		t_v[1] = elements[4] * v[0] + elements[5] * v[1] + elements[6] * v[2] + elements[7];
		t_v[2] = elements[8] * v[0] + elements[9] * v[1] + elements[10] * v[2] + elements[11];

		v[0] = t_v[0];
		v[1] = t_v[1];
		v[2] = t_v[2];
	}

	void Transformation::ApplyTo(float *v, float *t_v)
	{
		t_v[0] = elements[0] * v[0] + elements[1] * v[1] + elements[2] * v[2] + elements[3];
		t_v[1] = elements[4] * v[0] + elements[5] * v[1] + elements[6] * v[2] + elements[7];
		t_v[2] = elements[8] * v[0] + elements[9] * v[1] + elements[10] * v[2] + elements[11];
	}

    Transformation Transformation::operator * (Transformation &b)
	{
        Transformation c;
		int i,j,k,mat_off1,mat_off2,mat_off3;
		for (i = 0;i < 4;i++)
		{
			for (j = 0;j < 4;j++)
			{
				mat_off3 = i*4+j;
                c.elements[mat_off3] = 0.0;
				for (k  = 0;k < 4;k++)
				{
					mat_off2 = k*4+j;
					mat_off1 = i*4+k;
                    c.elements[mat_off3] += this->elements[mat_off1]*b.elements[mat_off2];
				}
			}
		}
		return (c);
    }


}
