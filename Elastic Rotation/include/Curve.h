#pragma once

#include "Core.h"

namespace midl
{
#define EXPONENTIAL_SMOOTHING 0
#define LAPLACIAN_SMOOTHING 1
#define PI 3.14

	class CURVE_SMOOTHING
	{
	public:
		enum
		{
			UNIFORM_LAPLACIAN,
			EDGE_WEIGHTED_LAPLACIAN,
			EXPONENTIAL
		};
	};

	class Curve2
	{
	private:
		vector<Tuple2f> vertices;
		vector<Tuple2f> tangents;
		vector<Tuple2f> normals;

		float centroid[2];
		bool isClosed;
		float closureThreshold;

		void ComputeTangents();
		void ComputeNormalsClosedCurve();
		void ComputeNormalsOpenCurve();

	public:

		Curve2();
		~Curve2();


		int GetNumVertices();
		bool GetVertex(int idx, float * p);
		bool GetNormal(int idx, float *n);
		bool GetTangent(int idx, float *t);

		bool GetVertices(vector<float> &verts);
		bool GetTangents(vector<float> &tangs);
		bool GetNormals(vector<float> &norms);

		bool SetThresh(float p);
		bool SetClosure(bool closure);
		bool GetClosure();
		bool CheckIfClosed();
		bool AddPoint(float *p);// IMP
		bool FromData(vector<float> &data);//IMP

		bool Smooth();//IMP
		bool Simplify();
		bool Subdivide();

		void FindWindow2D(float t, vector<float> &param, size_t &id0, size_t &id1);
		bool Resample(size_t npts);// imp

		bool Rotate(float angle, float *axis, float *pivot);
		bool TranslateTo(float *target);
		bool TranslateBy(float *diff);
		bool Scale(float *scl, float *pivot);

		bool OverSketch(vector<float> &overCurve);


	};

	class Curve3
	{
	protected:
		vector<Tuple3f> vertices;
		vector<Tuple3f> tangents;
		vector<Tuple3f> normals;
		vector<Tuple3f> binormals;
		vector<Tuple3f> osculatingCenters;
		vector<float> curvatures;
		vector<float> edgeLengths;

		float centroid[3];
		float curveLength;
		bool isClosed;
		float closureThreshold;

		void ComputeTangents();
		void ComputeBinormals();
		void ComputeNormals();
		void ComputeEdgeLengths();
		void ComputeCurvatures();
		void ComputeCentroid();
		void FindWindow(float t, vector<float> &param, size_t &id0, size_t &id1, size_t &id2);
		
	public:
		Curve3();
		~Curve3();

		int GetNumVertices();
		bool GetCentroid(float *c);
		bool GetVertex(int idx, float * p);
		bool SetVertex(float *p, int idx);
		bool GetNormal(int idx, float *n);
		bool GetBinormal(int idx, float *n);
		bool GetTangent(int idx, float *t);
		float GetCurvature(int idx);
		bool GetOsculatingCentre(int idx, float *centre);

		bool GetVertices(vector<float> &verts);
		bool GetTangents(vector<float> &tangs);
		bool GetBinormals(vector<float> &bnorms);
		bool GetNormals(vector<float> &norms);
		bool GetCurvatures(vector<float> &curvs);
		bool GetOsculatingCentres(vector<float> &centres);
		
		
		bool SetThresh(float p);
		bool SetClosure(bool closure);
		bool AddPoint(float *p);// IMP
		bool FromData(vector<float> &data);//IMP

		bool Update();

		bool Smooth(int option, float smoothingConstant = 0.0f);//IMP
		bool Simplify();
		bool Subdivide();

		bool Rotate(float angle, float *axis, float *pivot);
		bool TranslateTo(float *target);
		bool TranslateBy(float *diff);
		bool Scale(float *scl, float *pivot);
		bool Resample(size_t nPts);// IMP

		bool CopyTo(Curve3 &C);
		bool Clear();
	};
}
