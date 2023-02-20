#pragma once

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <time.h>
#include <float.h>
#include <stdint.h>
#include <fstream>
#include <iomanip>

#include<vector>
#include<algorithm>

using namespace std;

namespace midl
{
	#define FLOAT_EPSILON 1.0e-6
	#define DOUBLE_EPSILON 1.0e-8
	#define PI 3.14159
	
	#define sq(a) ((a)*(a))
    #define round(x) ((x)>=0?static_cast<int>((x)+0.5):static_cast<int>((x)-0.5))

	// Static global variables
	static float Origin[] = { 0.0,0.0,0.0 };
	static float Xaxis[] = { 1.0,0.0,0.0 };
	static float Yaxis[] = { 0.0,1.0,0.0 };
	static float Zaxis[] = { 0.0,0.0,1.0 };
	class Transformation;

	// Data structures for 2 and 3 dimensional vectors
    typedef struct
	{
		int data[2];
    }Tuple2i;

	typedef struct Tuple2f
	{
		float data[2];
    }Tuple2f;

    typedef struct
	{
		double data[2];
    }Tuple2d;

    typedef struct
	{
		int data[3];
    }Tuple3i;

    typedef struct
	{
		float data[3];
    }Tuple3f;

    typedef struct
	{
		double data[3];
    }Tuple3d;

    typedef struct
    {
        int data[4];
    }Tuple4i;

	// Vector algebra
	bool AddVectors2(float *v1, float *v2, float *v);
	bool AddVectors3(float *v1, float *v2, float *v);

	// v = v1-v2
	bool SubVectors2(float *v1, float *v2, float *v);
	bool SubVectors3(float *v1, float *v2, float *v);

	bool PointstoVec2(float *v1, float *v2, float *v);
	bool PointstoVec3(float *v1, float *v2, float *v);

	float Dot2(float *v1, float *v2);
	float Dot3(float *v1, float *v2);

    void ScaleVector2(float *v,float s);
    void ScaleVector2(float *v,float s, float *sv);
    void ScaleVector3(float *v,float s);
    void ScaleVector3(float *v,float s,float *sv);

	// v = v1 x v2
	bool Cross(float *v1, float *v2, float *v);

	float Norm2(float *v);
	float Norm3(float *v);

	float NormSq2(float *v);
	float NormSq3(float *v);

	bool Normalize2(float *v);
	bool Normalize2(float *v, float *nv);

	bool Normalize3(float *v);
	bool Normalize3(float *v, float *nv);

	bool MidPoint2(float *p1, float *p2, float *midPoint);
	bool MidPoint3(float *p1, float *p2, float *midPoint);

	bool TriCenter2(float *a, float *b, float *c, float *d);
	bool TriCenter3(float *a, float *b, float *c, float *d);

	bool TriNormal(float *p1, float *p2, float *p3, float *n);
	
	float TriArea2(float *p1, float *p2, float *p3);
	float TriArea3(float *p1, float *p2, float *p3, float *n);

	float AxisAngle(float *axis, float angle, float *matrix);

    bool RotateVec3(float *v, float *matrix, float *t_v);
    bool RotateVec3(float *v, float *matrix);
    bool Cart2Sph3(float *cart,float *sph);
    bool MapVecToRange2(float *v,float inRange[2][2],float outRange[2][2],bool isConstrain);
    bool MapVecToRange3(float *v,float inRange[3][2],float outRange[3][2],bool isConstrain);

	bool ProjectPointOnTri(float *pnt, float *v0, float *v1, float *v2, float *intersect, float *param);

	//Geometry
    bool Cart2Sph3(float *cart,float *sph);

    float PointToLineDist2(float pt[2], float start[2], float end[2]);
    float PointToLineDist3(float pt[3], float start[3], float end[3]);

    float PointToPlaneDist(float pt[3], float normal[3], float d);
    float PointToPlaneDist(float pt[3], float planePt[3], float planeN[3]);

	bool ProjectPointOnLine2(float pt[2], float start[2], float end[2], float v[2]);
    bool ProjectPointOnLine3(float pt[2], float start[2], float end[2], float v[2]);
	
    float AcuteAngleBetweenVectors2(float v1[2], float v2[2]);
    float AngleBetweenVectors2(float v1[2], float v2[2]);
    float AngleBetweenVectors3(float v1[3], float v2[3]);

    bool PointInPolygon2(vector<float> &poly, float pt[2]);
    bool PointInPolygon2(vector<Tuple2f> &poly, float pt[2]);
    bool PointInPolygon3(vector<float> &poly, float normal[3], float centroid[3], float pt[3]);

    void GetPlaneFromTriangle(float p1[3],float p2[3],float p3[3],float normal[3], float center[3]);
    void GetAngleAxisBetweenDirections(float v1[3], float v2[3], float axis[3], float &angle);
	void GetRotationQuaternions(float v1[3], float v2[3], float q[4], float matrix[9]);
	void GetAngleAxisFromQuaternion(float q[4], float axis[3], float &angle);
	
	void ProjectPointTo3DPlane(float plane_pt[3], float n[3], float in_pt[3], float out_pt[3]);
	void ProjectVectorToPlane(float planePt[3], float planeN[3], float v[3], float projV[3]);

	bool LineLineIntersection2(float pt1[2], float pt2[2], float pt3[2], float pt4[2], float int_pt[2]);
	bool LineLineIntersection3(float pt1[3], float pt2[3], float pt3[3], float pt4[3], float int_pt[3]);

	//PCL functions
	bool TokenizeTuple2f(char key[], float v[2]);
	bool TokenizeTuple3f(char key[], float v[3]);

	bool ReadPclFromFile(char *fName, vector<Tuple2f> &pcl);
	bool ReadPclFromFile(char *fName, vector<Tuple3f> &pcl);

	bool PclCentroid(vector<Tuple2f> &pcl, float *centroid);
	bool PclCentroid(vector<Tuple3f> &pcl, float *centroid);

	bool TranslatePCLBy(vector<Tuple2f> &pcl, float *trans);
	bool TranslatePCLBy(vector<Tuple3f> &pcl, float *trans);
	bool TranslatePCLTo(vector<Tuple2f> &pcl, float *trans);
	bool TranslatePCLTo(vector<Tuple3f> &pcl, float *trans);

	bool RotatePcl(vector<Tuple2f> &pcl, float angle);
	bool RotatePcl(vector<Tuple2f> &pcl, float angle, float pivot[3]);
	bool RotatePcl(vector<Tuple3f> &pcl, float *axis, float angle);
	bool RotatePcl(vector<Tuple3f> &pcl, float *axis, float angle, float pivot[3]);

	bool ScalePcl(vector<Tuple2f> &pcl, float *scl);
	bool ScalePcl(vector<Tuple2f> &pcl, float *scl, float pivot[2]);
	bool ScalePcl(vector<Tuple3f> &pcl, float *scl);
	bool ScalePcl(vector<Tuple3f> &pcl, float *scl, float pivot[3]);
	bool ScaleGlobalPcl(vector<Tuple2f> &pcl, float *scl);
	bool ScaleGlobalPcl(vector<Tuple3f> &pcl, float *scl);

	bool TransformPcl(vector<Tuple3f> &pcl, vector<Tuple3f> &t_pcl, Transformation &t);
	bool PclAABB(vector<Tuple2f> &pcl, float *xRng, float *yRng);
	bool PclAABB(vector<Tuple3f> &pcl, float *xRng, float *yRng, float *zRng);
    int PclScaleToUnitBox(vector<Tuple3f> &pcl);
	bool PclPCA(vector<Tuple2f> &pcl);
    bool PclPCA(vector<Tuple3f> &pcl);

	//Basic Statistics
    #define RBF_GAUSSIAN 0
    #define RBF_TPS 1
    #define RBF_MULTIQUADRIC 2
    #define RBF_INVMULTIQUADRIC 3

	int RandomInt(int _min, int _max);
	float RandomFloat(float _min, float _max);
	int MeanStd(vector<float> &sample, float &mean, float &std_dev);
	float MeanDifference(vector<float> &sample, vector<float> &scores);
	float MeanDifference(vector<float> &sample);
    int StandardScore(vector<float> &sample,vector<float> &scores,float *mean_std);
    int RadialBasisVector(int rbf_choice,vector<float> &sample,vector<float> &rbf_vector,float *param);
    int NormalizeRange(vector<float> &sample,vector<float> &nrm_vector,size_t *min_max_idx,float *min_max_dist);
    int NormalizeRange(vector<float> &sample,size_t *min_max_idx,float *min_max_dist);
    int NormalizeRange(float *rng,vector<float> &sample,vector<float> &nrm_vector,size_t *min_max_idx,float *min_max_dist);
    int NormalizeRange(float *rng,vector<float> &sample,size_t *min_max_idx,float *min_max_dist);

	// Homogeneous transformations
	class Transformation
	{
	private:
		float elements[16];
	public:
		Transformation();
		~Transformation();

		void GetMatrix(float *matrix);
        void GetMatrixTranspose(float *matrix);
		void SetMatrix(float *matrix);

		void SetIdentity();
        void SetTranslationBy(float *translation);
        void SetXRotation(float angle);
        void SetYRotation(float angle);
        void SetZRotation(float angle);
        void SetRollPitchYawRotation(float *RPY);
        void SetAxisAngleRotation(float * axis, float angle);
        void SetScale(float *scale);

		Transformation Inverse();

		void ApplyTo(float *v);
		void ApplyTo(float *v, float *t_v);
		
        Transformation operator * (Transformation &b);
	};

}
