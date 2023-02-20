#include "Curve.h"
#include "Core.h"


namespace midl
{

	bool _crv3_ComputeCircumCircle(float *p1, float *p2, float *p3, float &radius, float *center)
	{
		float A[3], B[3], C[3];
		float denVector[3];
		float denMag;

		SubVectors3(p1, p3, C);
		SubVectors3(p2, p1, A);
		SubVectors3(p3, p2, B);
		float a = Norm3(A);
		float b = Norm3(B);
		float c = Norm3(C);
		Cross(A, B, denVector);
		denMag = Norm3(denVector);
		radius = (a*b*c)/(2.0*denMag);

		float alpha, beta, gamma;
		float denominator = 2.0*denMag*denMag;

		alpha = -Dot3(B, B)*Dot3(A, C) / denominator;
		beta = -Dot3(C, C)*Dot3(A, B) / denominator;
		gamma = -Dot3(A, A)*Dot3(C, B) / denominator;

			center[0] = alpha*p1[0] + beta*p2[0] + gamma*p3[0];
			center[1] = alpha*p1[1] + beta*p2[1] + gamma*p3[1];
			center[2] = alpha*p1[2] + beta*p2[2] + gamma*p3[2];
			return true;
	
		if (denMag < 1.0e-6)return false;
		else return false;
	}

	//Class Curve2 : 2D Curves

	void Curve2::ComputeTangents()
	{
		float v[2];
		size_t previous, next;
		tangents.clear();

		for (int i = 0; i < vertices.size(); i++)
		{
			previous = i - 1;
			next = i + 1;
			SubVectors2(vertices[previous].data, vertices[next].data, v);
			Tuple2f t;
			t.data[0] = v[0];
			t.data[1] = v[1];
			Normalize2(t.data);
			tangents.push_back(t);
			//v[1] = vertices[next].data[1] - vertices[previous].data[1];
			//v[0] = vertices[next].data[0] - vertices[previous].data[0];
			//m = v[1] / v[0];
		}
	}

	void Curve2::ComputeNormalsOpenCurve()
	{
		size_t previous, next;
		size_t end = vertices.size() - 1;
		ComputeTangents();
		normals.clear();
		for (int i = 0; i < vertices.size(); i++)
		{
			previous = i - 1;
			next = i + 1;
			if (i == 0) previous = 0;
			else if (i == end) next = end;
			Tuple2f t;
			t.data[0] = -tangents[i].data[1];
			t.data[1] = tangents[i].data[0];
			Normalize2(t.data);
			normals.push_back(t);
			/*float v[2];
			size_t a, b;
			size_t end = vertices.size() - 1;
			float theta = PI / 2;
			normals.clear();
			for (int i = 0; i < vertices.size(); i++)
			{
			a = i - 1; b = i + 1;
			if (i == 0) a = 0;
			else if (i == end) b = end;

			SubVectors2(vertices[a].data, vertices[b].data, v);
			Tuple2f t;
			t.data[0] = v[0] * cos(theta) - v[1] * sin(theta);
			t.data[1] = v[1] * cos(theta) + v[0] * cos(theta);
			Normalize2(t.data);
			normals.push_back(t);
			*/
		}
	};

	void Curve2::ComputeNormalsClosedCurve()
	{
		size_t previous, next;
		size_t end = vertices.size() - 1;
		ComputeTangents();
		normals.clear();
		for (int i = 0; i < vertices.size(); i++)
		{
			previous = i - 1;
			next = i + 1;
			if (i == 0) previous = vertices.size() - 1;
			else if (i == end) next = 0;
			Tuple2f t;
			t.data[0] = -tangents[i].data[1];
			t.data[1] = tangents[i].data[0];
			Normalize2(t.data);
			normals.push_back(t);
		}
	};

	Curve2::Curve2()
	{
		isClosed = false;
		centroid[0] = 0.0;
		centroid[1] = 0.0;
		closureThreshold = 0.4;
	};

	Curve2::~Curve2()
	{
		vertices.clear();
		tangents.clear();
		normals.clear();
	};

	int Curve2::GetNumVertices()
	{
		return (int)vertices.size();
	}

	bool Curve2::GetNormal(int idx, float *n)
	{
		if (isClosed)
		{
			ComputeNormalsClosedCurve();
			n[0] = normals[idx].data[0];
			n[1] = normals[idx].data[1];
			Normalize2(n);
		}
		else
		{
			ComputeNormalsOpenCurve();
			n[0] = normals[idx].data[0];
			n[1] = normals[idx].data[1];
			Normalize2(n);
		}

		return true;

	};

	bool Curve2::GetTangent(int idx, float *t)
	{
		ComputeTangents();
		t[0] = tangents[idx].data[0];
		t[1] = tangents[idx].data[1];
		Normalize2(t);
		return true;
	};

	bool Curve2::GetVertex(int idx, float *p)
	{
		p[0] = vertices[idx].data[0];
		p[1] = vertices[idx].data[1];
		return true;
	}

	bool Curve2::GetVertices(vector<float> &verts)
	{
		verts.clear();
		for (int i = 0; i < vertices.size(); i++)
		{
			verts.push_back(vertices[i].data[0]);
			verts.push_back(vertices[i].data[1]);
		}
		return true;

	}

	bool Curve2::GetTangents(vector<float> &tangs)
	{
		tangs.clear();
		ComputeTangents();
		for (int i = 0; i < tangents.size(); i++)
		{
			tangs.push_back(tangents[i].data[0]);
			tangs.push_back(tangents[i].data[1]);
		}
	}

	bool Curve2::GetNormals(vector<float> &norms)
	{
		norms.clear();
		if (isClosed)
		{
			ComputeNormalsClosedCurve();
			for (int i = 0; i < normals.size(); i++)
			{
				norms.push_back(normals[i].data[0]);
				norms.push_back(normals[i].data[1]);
			}
		}

		else
		{
			ComputeNormalsOpenCurve();
			for (int i = 0; i < normals.size(); i++)
			{
				norms.push_back(normals[i].data[0]);
				norms.push_back(normals[i].data[1]);
			}
		}

	}

	bool Curve2::SetThresh(float p)
	{
		closureThreshold = p;
		return true;
	}

	bool Curve2::SetClosure(bool closure)
	{
		isClosed = closure;
		return true;
	}

	bool Curve2::GetClosure()
	{
		return isClosed;
	}

	bool Curve2::CheckIfClosed()
	{
		isClosed = false;
		float v[2];
		SubVectors2(vertices[0].data, vertices[vertices.size() - 1].data, v);
		if (vertices.size() > 1 && Norm2(v) < closureThreshold)
		{
			isClosed = true;
			return isClosed;
		}
		else return false;

	}

	bool Curve2::AddPoint(float *p)
	{
		Tuple2f t;
		t.data[0] = p[0];
		t.data[1] = p[1];
		vertices.push_back(t);
		return true;

	}

	bool Curve2::FromData(vector<float> &data) //SetCurve from vertices
	{
		if (data.size() % 2 != 0) return false;

		data.clear();
		for (int i = 0; i < data.size() / 2; i++)
		{
			Tuple2f t;
			t.data[0] = data[2 * i];
			t.data[1] = data[(2 * i) + 1];
			vertices.push_back(t);
		}
		return true;
	}

	bool Curve2::Smooth() //Learnt and Solved in Lab
	{
		if (vertices.size() < 3)
		{
			return false;
		}

		for (int i = 1; i < vertices.size() - 1; i++)
		{
			vertices[i].data[0] = 0.5*(vertices[i - 1].data[0] + vertices[i + 1].data[0]);
			vertices[i].data[1] = 0.5*(vertices[i - 1].data[1] + vertices[i + 1].data[1]);

		}
		return true;
	}

	bool Curve2::Simplify()
	{

	}

	bool Curve2::Subdivide()
	{

	}


	void Curve2::FindWindow2D(float t, vector<float> &param, size_t &id0, size_t &id1) //Borrowed from Tube
	{
		for (size_t i = 0; i < param.size(); i++)
		{
			if (param[i] == t)
			{
				id0 = i;
				id1 = i;
				break;
			}
			else if (param[i] > t && param[i - 1] < t)
			{
				id0 = i - 1;
				id1 = i;
				break;
			}
		}
	}

	bool Curve2::Resample(size_t npts) //borrowed from tube
	{
		if (isClosed)
		{
			vertices.push_back(vertices[0]);
			vertices.push_back(vertices[1]);
		}

		vector<float> param;
		vector< Tuple2f > rcoords;
		Tuple2f t;


		size_t id0, id1;
		Tuple2f p1, p2, p, w1, w2, x;
		for (size_t i = 0; i < npts; i++)
		{
			t.data[i] = (float)i / (float)(npts - 1);
			FindWindow2D(t.data[i], param, id0, id1);

			if (id0 == id1)
			{
				rcoords[i].data[0] = vertices[2 * id0].data[0];
				rcoords[i].data[0] = vertices[2 * id0 + 1].data[0];
				continue;
			}

			p1.data[0] = vertices[2 * id0].data[0]; p1.data[1] = vertices[2 * id0 + 1].data[1];
			p2.data[0] = vertices[2 * id1].data[0]; p2.data[1] = vertices[2 * id1 + 1].data[1];

			w1.data[0] = t.data[0] - param[id0];
			w2.data[1] = param[id1] - t.data[1];

			rcoords[i].data[0] = (w1.data[0] * p2.data[0] + w2.data[0] * p1.data[0]) / (w1.data[0] + w2.data[0]);
			rcoords[i].data[0] = (w1.data[1] * p2.data[1] + w2.data[1] * p1.data[1]) / (w1.data[1] + w2.data[1]);


		}

		vertices.clear();
		int lastid;
		if (isClosed)lastid = (int)rcoords.size() - 2;
		else lastid = (int)rcoords.size();
		for (int i = 0; i < lastid; i++)vertices.push_back(rcoords[i]);

		rcoords.clear();
		param.clear();
		return 0;
	}

	bool Curve2::Rotate(float angle, float *axis, float *pivot)
	{

	}

	bool Curve2::TranslateTo(float *target)
	{

	}

	bool Curve2::TranslateBy(float *diff)
	{

	}

	bool Curve2::Scale(float *scl, float *pivot)
	{

	}

	bool Curve2::OverSketch(vector<float> &overCurve)
	{

	}

	//Class Curve3 : 3D Curve

	void Curve3::ComputeTangents()
	{
		float v[3];
		tangents.clear();
		Tuple3f t1,t2;
		// First point of the curve
		if (isClosed)
		{
			SubVectors3(vertices[1].data, vertices[(int)vertices.size() - 1].data, t1.data);
			Normalize3(t1.data);
		}
		else
		{
			SubVectors3(vertices[1].data, vertices[0].data, t1.data);
			Normalize3(t1.data);
		}
		tangents.push_back(t1);

		// All points between first and last points of the curve
		for (int i = 1; i < (int)vertices.size()-1; i++)
		{
			Tuple3f t;
			SubVectors3(vertices[i+1].data, vertices[i-1].data, t.data);
			Normalize3(t.data);
			tangents.push_back(t);
		}

		// Last point of the curve
		if (isClosed)
		{
			SubVectors3(vertices[0].data, vertices[(int)vertices.size() - 2].data, t2.data);
			Normalize3(t2.data);
		}
		else
		{
			SubVectors3(vertices[(int)vertices.size() - 1].data, vertices[(int)vertices.size() - 2].data, t2.data);
			Normalize3(t2.data);
		}
		tangents.push_back(t2);
	}

	void Curve3::ComputeBinormals()
	{
		float v[3];
		int lastID = (int)vertices.size() - 1;
		binormals.clear();

		Tuple3f b1, b2;

		// First point of the curve
		if (isClosed)
			TriNormal(vertices[lastID].data, vertices[0].data, vertices[1].data, b1.data);
		else
			TriNormal(vertices[0].data, vertices[1].data, vertices[2].data, b1.data);
		binormals.push_back(b1);

		// All points between first and last points of the curve
		for (int i = 1; i < (int)vertices.size()-1; i++)
		{
			Tuple3f b;
			TriNormal(vertices[i - 1].data, vertices[i].data, vertices[i + 1].data, b.data);
			binormals.push_back(b);
		}

		// Last point of the curve
		if (isClosed)
			TriNormal(vertices[lastID-1].data, vertices[lastID].data, vertices[0].data, b2.data);
		else
			TriNormal(vertices[lastID - 2].data, vertices[lastID - 1].data, vertices[lastID].data, b2.data);
		binormals.push_back(b2);
	}

	void Curve3::ComputeNormals()
	{
		if (tangents.empty())ComputeTangents();
		if (binormals.empty())ComputeBinormals();
		normals.clear();
		
		for (int i = 0; i < vertices.size(); i++)
		{
			Tuple3f n;
			Cross(binormals[i].data, tangents[i].data, n.data);
			normals.push_back(n);
		}
	}

	void Curve3::ComputeEdgeLengths()
	{
		float v[3];
		edgeLengths.clear();
		curveLength = 0.0;
		
		if (isClosed)
		{
			SubVectors3(vertices[0].data, vertices[(int)vertices.size() - 1].data, v);
			float l = Norm3(v);
			curveLength += l;
			edgeLengths.push_back(l);
		}
		else edgeLengths.push_back(0.0);

		for (int i = 1; i < vertices.size(); i++)
		{
			SubVectors3(vertices[i].data, vertices[i - 1].data, v);
			float l = Norm3(v);
			curveLength += l;
			edgeLengths.push_back(l);
		}		
	}

	/*void Curve3::ComputeCurvatures()
	{
		int lastID = (int)vertices.size()-1;
		float a, b, c;
		float alpha, beta, gamma;
		float v[3],v1[3],v2[3];
		float A[3], B[3], C[3];
		curvatures.clear();
		osculatingCenters.clear();

		if (isClosed)
		{
			SubVectors3(vertices[lastID].data, vertices[1].data, v);
			SubVectors3(vertices[0].data, vertices[lastID].data, v1);
			SubVectors3(vertices[0].data, vertices[1].data, v2);
			a = edgeLengths[0];
			b = edgeLengths[1];
			c = Norm3(v);
			Cross(v1, v2, v);
			curvatures.push_back(2.0*Norm3(v) / (a*b*c));

		}
		else curvatures.push_back(0.0);

		for (int i = 1; i < vertices.size()-1; i++)
		{
			SubVectors3(vertices[i-1].data, vertices[i+1].data, v);
			SubVectors3(vertices[i].data, vertices[i - 1].data, v1);
			SubVectors3(vertices[i].data, vertices[i + 1].data, v2);
			a = edgeLengths[i];
			b = edgeLengths[i + 1];
			c = Norm3(v);
			Cross(v1, v2, v);
			curvatures.push_back(2.0*Norm3(v)/(a*b*c));
		}

		if (isClosed)
		{
			SubVectors3(vertices[lastID-1].data, vertices[0].data, v);
			SubVectors3(vertices[lastID].data, vertices[lastID-1].data, v1);
			SubVectors3(vertices[lastID].data, vertices[0].data, v2);
			a = edgeLengths[lastID];
			b = edgeLengths[0];
			c = Norm3(v);
			Cross(v1, v2, v);
			curvatures.push_back(2.0*Norm3(v) / (a*b*c));
		}
		else curvatures.push_back(0.0);
	}*/

	void Curve3::ComputeCurvatures()
	{
		int lastID = (int)vertices.size() - 1;
		float a, b, c;
		float alpha, beta, gamma;
		float v[3], v1[3], v2[3];
		float A[3], B[3], C[3];
		curvatures.clear();
		osculatingCenters.clear();

		if (isClosed)
		{
			float r;
			Tuple3f c;
			bool isOk = _crv3_ComputeCircumCircle(vertices[lastID].data, vertices[0].data, vertices[1].data, r, c.data);
			if (isOk)
			{
				curvatures.push_back(1.0 / r);
				osculatingCenters.push_back(c);
			}
			else
			{
				curvatures.push_back(0.0);
				osculatingCenters.push_back(c);
			}

		}
		else curvatures.push_back(0.0);

		for (int i = 1; i < vertices.size() - 1; i++)
		{
			float r;
			Tuple3f c;
			bool isOk = _crv3_ComputeCircumCircle(vertices[i - 1].data, vertices[i].data, vertices[i + 1].data, r, c.data);
			if (isOk)
			{
				curvatures.push_back(1.0 / r);
				osculatingCenters.push_back(c);
				//cout << "The radius" << r << endl;
			}
			else
			{
				curvatures.push_back(0.0);
				osculatingCenters.push_back(c);
				//cout << c.data[0] << "," << c.data[1] << "," << c.data[2] << endl;

			}
		}

		if (isClosed)
		{
			float r;
			Tuple3f c;
			bool isOk = _crv3_ComputeCircumCircle(vertices[lastID - 1].data, vertices[lastID].data, vertices[0].data, r, c.data);
			if (isOk)
			{
				curvatures.push_back(1.0 / r);
				osculatingCenters.push_back(c);
			}
			else
			{
				curvatures.push_back(0.0);
				osculatingCenters.push_back(c);
			}


		}
		else curvatures.push_back(0.0);
	}

	void Curve3::ComputeCentroid()
	{
		centroid[0] = 0.0;
		centroid[1] = 0.0;
		centroid[2] = 0.0;

		for (size_t i = 0; i < vertices.size(); i++)
		{
			centroid[0] += vertices[i].data[0];
			centroid[1] += vertices[i].data[1];
			centroid[2] += vertices[i].data[2];
		}
		centroid[0] /= (float)vertices.size();
		centroid[1] /= (float)vertices.size();
		centroid[2] /= (float)vertices.size();
	}

	Curve3::Curve3()
	{
		isClosed = false;
		centroid[0] = 0.0;
		centroid[1] = 0.0;
		centroid[2] = 0.0;
		curveLength = 0.0;
		closureThreshold = 0.0;
	}

	Curve3::~Curve3()
	{
		vertices.clear();
		normals.clear();
		tangents.clear();
		binormals.clear();
		edgeLengths.clear();
		curvatures.clear();
		osculatingCenters.clear();
	};

	int Curve3::GetNumVertices()
	{
		return vertices.size();
	}

	bool Curve3::GetNormal(int idx, float *n)
	{
		n[0] = normals[idx].data[0];
		n[1] = normals[idx].data[1];
		n[2] = normals[idx].data[2];
		return true;
	}

	bool Curve3::GetBinormal(int idx, float *b)
	{
		b[0] = binormals[idx].data[0];
		b[1] = binormals[idx].data[1];
		b[2] = binormals[idx].data[2];
		return true;
	}

	bool Curve3::GetTangent(int idx, float *t)
	{
		t[0] = tangents[idx].data[0];
		t[1] = tangents[idx].data[1];
		t[2] = tangents[idx].data[2];
		return true;
	};

	bool Curve3::GetCentroid(float *c)
	{
		c[0] = centroid[0];
		c[1] = centroid[1];
		c[2] = centroid[2];
		return true;
	}

	bool Curve3::GetVertex(int idx, float *p)
	{
		p[0] = vertices[idx].data[0];
		p[1] = vertices[idx].data[1];
		p[2] = vertices[idx].data[2];
		return true;
	}
	
	bool Curve3::SetVertex(float *p, int idx)
	{
		vertices[idx].data[0] = p[0];
		vertices[idx].data[1] = p[1];
		vertices[idx].data[2] = p[2];
		return true;
	}

	float Curve3::GetCurvature(int idx)
	{
		return curvatures[idx];
	}

	bool Curve3::GetOsculatingCentre(int idx, float *centre)
	{
		centre[0] = osculatingCenters[idx].data[0];
		centre[1] = osculatingCenters[idx].data[1];
		centre[2] = osculatingCenters[idx].data[2];
		return true;
	}

	bool Curve3::GetVertices(vector<float> &verts)
	{
		verts.clear();

		for (int i = 0; i < vertices.size(); i++)
		{
			verts.push_back(vertices[i].data[0]);
			verts.push_back(vertices[i].data[1]);
			verts.push_back(vertices[i].data[2]);
		}

		return true;

	}

	bool Curve3::GetTangents(vector<float> &tangs)
	{
		tangs.clear();
		for (int i = 0; i < tangents.size(); i++)
		{
			tangs.push_back(tangents[i].data[0]);
			tangs.push_back(tangents[i].data[1]);
			tangs.push_back(tangents[i].data[2]);
		}
		return true;
	}

	bool Curve3::GetNormals(vector<float> &norms)
	{
		norms.clear();
		for (int i = 0; i < normals.size(); i++)
		{
			norms.push_back(normals[i].data[0]);
			norms.push_back(normals[i].data[1]);
			norms.push_back(normals[i].data[2]);
		}
		return true;
	}

	bool Curve3::GetBinormals(vector<float> &bnorms)
	{
		bnorms.clear();
		for (int i = 0; i < binormals.size(); i++)
		{
			bnorms.push_back(binormals[i].data[0]);
			bnorms.push_back(binormals[i].data[1]);
			bnorms.push_back(binormals[i].data[2]);
		}
		return true;
	}


	bool Curve3::GetCurvatures(vector<float> &curvs) 
	{
		curvs.clear();
		for (int i = 0; i < curvatures.size(); i++)
		{
			curvs.push_back(curvatures[i]);
		}
		return true;
	}

	bool Curve3::GetOsculatingCentres(vector<float> &centres)
	{
		centres.clear();
		for (int i = 0; i < centres.size(); i++)
		{
			centres.push_back(osculatingCenters[i].data[0]);
			centres.push_back(osculatingCenters[i].data[1]);
			centres.push_back(osculatingCenters[i].data[2]);
		}
		return true;
	}

	bool Curve3::SetThresh(float p)
	{
		closureThreshold = p;
		return true;
	}

	bool Curve3::SetClosure(bool closure)
	{
		isClosed = closure;
		return true;
	}

	bool Curve3::AddPoint(float *p)
	{
		Tuple3f t;
		t.data[0] = p[0];
		t.data[1] = p[1];
		t.data[2] = p[2];
		vertices.push_back(t);
		return true;

	}

	bool Curve3::FromData(vector<float> &data)
	{
		if (data.size() % 3 != 0) return false;

		data.clear();
		for (int i = 0; i < data.size() / 3; i++)
		{
			Tuple3f t;
			t.data[0] = data[2 * i];
			t.data[1] = data[(2 * i) + 1];
			t.data[2] = data[(2 * i) + 2];
			vertices.push_back(t);
		}
		Update();
		return true;
	}

	bool Curve3::Update()
	{
		if (vertices.empty()) return false;

		ComputeCentroid();
		ComputeTangents();
		ComputeBinormals();
		ComputeNormals();
		ComputeEdgeLengths();
		ComputeCurvatures();

		return true;
	}

	bool Curve3::Smooth(int option, float smoothingConstant)
	{
		if (vertices.size() < 3)
			return false;

		if (option == CURVE_SMOOTHING::UNIFORM_LAPLACIAN)
		{
			if (isClosed)
			{
				vertices[0].data[0] = 0.5*(vertices[(int)vertices.size() - 1].data[0] + vertices[1].data[0]);
				vertices[0].data[1] = 0.5*(vertices[(int)vertices.size() - 1].data[1] + vertices[1].data[1]);
				vertices[0].data[2] = 0.5*(vertices[(int)vertices.size() - 1].data[2] + vertices[1].data[2]);
			}

			for (int i = 1; i < vertices.size() - 1; i++)
			{
				vertices[i].data[0] = 0.5*(vertices[i - 1].data[0] + vertices[i + 1].data[0]);
				vertices[i].data[1] = 0.5*(vertices[i - 1].data[1] + vertices[i + 1].data[1]);
				vertices[i].data[2] = 0.5*(vertices[i - 1].data[2] + vertices[i + 1].data[2]);
			}

			if (isClosed)
			{
				vertices[(int)vertices.size() - 1].data[0] = 0.5*(vertices[(int)vertices.size() - 2].data[0] + vertices[0].data[0]);
				vertices[(int)vertices.size() - 1].data[1] = 0.5*(vertices[(int)vertices.size() - 2].data[1] + vertices[0].data[1]);
				vertices[(int)vertices.size() - 1].data[2] = 0.5*(vertices[(int)vertices.size() - 2].data[2] + vertices[0].data[2]);
			}
		}
		else if (option == CURVE_SMOOTHING::EDGE_WEIGHTED_LAPLACIAN)
		{
			float l1, l2;
			float w1, w2;
			if (isClosed)
			{
				l1 = 1.0 / edgeLengths[0];
				l2 = 1.0 / edgeLengths[1];
				w1 = l1 / (l1 + l2);
				w2 = l2 / (l1 + l2);
				vertices[0].data[0] = w1*vertices[(int)vertices.size() - 1].data[0] + w2*vertices[1].data[0];
				vertices[0].data[1] = w1*vertices[(int)vertices.size() - 1].data[1] + w2*vertices[1].data[1];
				vertices[0].data[2] = w1*vertices[(int)vertices.size() - 1].data[2] + w2*vertices[1].data[2];
			}

			for (int i = 1; i < vertices.size() - 1; i++)
			{
				l1 = 1.0 / edgeLengths[i];
				l2 = 1.0 / edgeLengths[i+1];
				w1 = l1 / (l1 + l2);
				w2 = l2 / (l1 + l2);
				vertices[i].data[0] = w1*vertices[i - 1].data[0] + w2*vertices[i + 1].data[0];
				vertices[i].data[1] = w1*vertices[i - 1].data[1] + w2*vertices[i + 1].data[1];
				vertices[i].data[2] = w1*vertices[i - 1].data[2] + w2*vertices[i + 1].data[2];
			}

			if (isClosed)
			{
				l1 = 1.0 / edgeLengths[(int)vertices.size() - 1];
				l2 = 1.0 / edgeLengths[0];
				w1 = l1 / (l1 + l2);
				w2 = l2 / (l1 + l2);
				vertices[(int)vertices.size() - 1].data[0] = w1*vertices[(int)vertices.size() - 2].data[0] + w2*vertices[0].data[0];
				vertices[(int)vertices.size() - 1].data[1] = w1*vertices[(int)vertices.size() - 2].data[1] + w2*vertices[0].data[1];
				vertices[(int)vertices.size() - 1].data[2] = w1*vertices[(int)vertices.size() - 2].data[2] + w2*vertices[0].data[2];
			}
		}
		else if (option == CURVE_SMOOTHING::EXPONENTIAL)
		{
			for (int i = 1; i < vertices.size(); i++)
			{
				vertices[i].data[0] = (1.0 - smoothingConstant)*vertices[i - 1].data[0] + smoothingConstant*vertices[i].data[0];
				vertices[i].data[1] = (1.0 - smoothingConstant)*vertices[i - 1].data[1] + smoothingConstant*vertices[i].data[1];
				vertices[i].data[2] = (1.0 - smoothingConstant)*vertices[i - 1].data[2] + smoothingConstant*vertices[i].data[2];
			}
		}
		else return false;

		
		Update();
		return true;
	}

	bool Curve3::Simplify()
	{

	}

	bool Curve3::Subdivide()
	{

	}

	void Curve3::FindWindow(float t, vector<float> &param, size_t &id0, size_t &id1, size_t &id2)
	{
		for (size_t i = 0; i < param.size(); i++)
		{
			if (param[i] == t)
			{
				id0 = i;
				id1 = i;
				id2 = i;
				break;
			}
			else if (param[i] > t && param[i - 1] < t)
			{
				id0 = i - 1;
				id1 = i;
				break;
			}
		}
	}

	bool Curve3::Rotate(float angle, float *axis, float *pivot)
	{
		float matrix[9];
		AxisAngle(axis, angle, matrix);

		for (int i = 0; i < vertices.size(); i++)
		{
			if (pivot != NULL)
			{
				vertices[i].data[0] -= pivot[0];
				vertices[i].data[1] -= pivot[1];
				vertices[i].data[2] -= pivot[2];
			}
			
			RotateVec3(vertices[i].data, matrix);

			if (pivot != NULL)
			{
				vertices[i].data[0] += pivot[0];
				vertices[i].data[1] += pivot[1];
				vertices[i].data[2] += pivot[2];
			}	
		}

		Update();

		return true;
	}

	bool Curve3::TranslateTo(float *target)
	{
		for (int i = 0; i < vertices.size(); i++)
		{
			vertices[i].data[0] += target[0] - centroid[0];
			vertices[i].data[1] += target[1] - centroid[1];
			vertices[i].data[2] += target[2] - centroid[2];
		}
		centroid[0] = target[0];
		centroid[1] = target[1];
		centroid[2] = target[2];
		return true;
	}

	bool Curve3::TranslateBy(float *diff)
	{
		for (int i = 0; i < vertices.size(); i++)
		{
			vertices[i].data[0] += diff[0];
			vertices[i].data[1] += diff[1];
			vertices[i].data[2] += diff[2];
		}
		centroid[0] += diff[0];
		centroid[1] += diff[1];
		centroid[2] += diff[2];
		return true;
	}

	bool Curve3::Scale(float *scl, float *pivot)
	{
		for (int i = 0; i < vertices.size(); i++)
		{
			if (pivot != NULL)
			{
				vertices[i].data[0] -= pivot[0];
				vertices[i].data[1] -= pivot[1];
				vertices[i].data[2] -= pivot[2];
			}

			vertices[i].data[0] *= scl[0];
			vertices[i].data[1] *= scl[1];
			vertices[i].data[2] *= scl[2];

			if (pivot != NULL)
			{
				vertices[i].data[0] += pivot[0];
				vertices[i].data[1] += pivot[1];
				vertices[i].data[2] += pivot[2];
			}
		}

		Update();

		return true;
	}

	bool Curve3::Resample(size_t nPts)
	{

	}

	bool Curve3::CopyTo(Curve3 &C)
	{
		for (int i = 0; i < vertices.size(); i++)
		{			
			C.AddPoint(vertices[i].data);
		}
		return true;
	}

	bool Curve3::Clear()
	{
		vertices.clear();
		normals.clear();
		tangents.clear();
		binormals.clear();
		edgeLengths.clear();
		curvatures.clear();
		osculatingCenters.clear();

		return true;
	}
}

