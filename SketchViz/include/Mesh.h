#pragma once

#include <iostream>
#include <string>
#include <stdio.h>
#include <conio.h>
#include <math.h>
#include <vector>
#include <map>
#include <utility>
#include <algorithm>
#include <omp.h>

#include <Core.h>
#include <AABB.h>

using namespace std;

namespace midl{
    //---- Definitions
    class SMOOTHING_WEIGHTS
    {
    public:
        enum{
            UNIFORM,
            EDGE_LENGTH,
            COTANGENT,
            FIELD
        };
    };

    #define DIST_ORIGINAL 0
    #define DIST_NORMALIZE 1
    #define DIST_STANDARD 2
    #define DIST_EXPNEG 3
    #define DIST_EXP 4
    #define DIST_EXPNORM 5
    #define DIST_NORMINV 6
    #define DIST_EXPMEANDIFF 7
    #define DIST_CAUCHY 8
    #define DIST_CAUCHYLAP 9

    #define FW_EUCLIDEAN 0
    #define FW_EXPONENTIAL 1
    #define FW_NORMALIZED 2
    #define FW_EXPNORM 3
    #define FW_EXPMEANDIFF 4
    #define FW_EXPNORMMEANDIFF 5

	typedef struct Edge;
    typedef struct Face;

    typedef struct{
    float u[3],v[3],w[3];
    }Frame;

    typedef struct{
        int index;
        float position[3];
        float normal[3];
        float color[3];
        float value;
        bool flag;
        Frame frame;
        std::vector<Edge*> edges;
        std::vector<Face*> faces;
    }Vert;

    typedef struct Edge{
        int index;
        Vert *head,*tail;
        Edge *next;
        Edge *twin;
        Face *face;
        float len;
    };

    typedef struct Face{
        int index;
        float area;
        float normal[3];
        float center[3];
        Edge *e1,*e2,*e3;
    };

    typedef struct{
        int r,g,b,a;
    }VertColor;

    typedef vector<Vert*> MeshVertices;
    typedef vector<Edge*> MeshEdges;
    typedef vector<Face*> MeshFaces;
    typedef pair<Edge*,Edge*> EdgePair;
    typedef vector<EdgePair> EdgePairs;
    typedef pair<float,float> CompPair;
    typedef vector<CompPair> CompPairs;
    typedef vector<VertColor> ColorList;
    typedef vector< vector<bool> > MeshMatrixb;
    typedef vector< vector<unsigned int> > MeshMatrixu;
    typedef vector< vector<int> > MeshMatrixi;
    typedef vector< vector<float> > MeshMatrixf;

        //---- Mesh Data Strcuture
    class Mesh
    {
        protected:
            int numVert;
            int numEdges;
            int numFaces;
            bool isDeformable;

            int decimation_method;
            int subdivision_method;
            int smoothing_weight;
            int fw_method;

            float centroid[3];
            CompPairs bounding_box;

            MeshVertices vertices;
            MeshEdges edges;
            MeshFaces faces;
            EdgePairs edge_pairs;
            CompPairs components;
            CompPairs gMap;
            MeshMatrixf distance_matrix;

            vector<bool> face_partition;
            vector<float> dist_vec;

            int AddVertToMesh(int id,float *p,float *n,float *c);
            int BuildComplexFromFace(int *t);
            int BuildHalfEdgeConnectivity();
            int ComputeEdgeLength(Edge *e);
            int UpdateEdgeLengths();
            int ComputeFaceArea(Face *f);
            int UpdateFaceAreas();

            int ComputeNormalAtVert(int id);
            int ComputeNormalAtVert(Vert *v);

            MeshVertices GetVertNhood(Vert *v);
            MeshFaces GetFaceNhood(Face *f);

            MeshFaces GetFacesIncidentOnVert(Vert *v);
            MeshFaces GetFacesIncidentOnEdge(Edge *e);
            MeshEdges GetEdgesIncidentOnFace(Face *f);
			
			void SplitEdge(EdgePair &pair);
			void CollapseEdge(EdgePair &pair);
			void SwapEdge(EdgePair &pair);
			void SplitFace(Face *f);
			void Root3(Face *f);
			void CollapseFace(Face *f);

        public:
            int InitMesh();
            int FromPly(char *file_name);
            int SetField(vector<float> _vals);
            int SetDecimationMethod(int method);
            int SetSmoothingWeight(int smoothing);
            int SetFloydWarshallMethod(int method);
			int DrawMesh();

            bool GetHoles(vector<vector<Tuple3f>> holes);

            //---- Mesh Processing Functions
            int ComputeVertexNormals();
            int ComputeFaceNormals();
            int ComputeGaussMapParametrization();
            int LaplacianSmoothing();
            int LaplacianFieldSmoothing();
            int SelectiveLaplacianSmoothing();

            int ComputeNearestVertex(float *p,size_t &idx,float &minDIST);
            int ComputeFarthestVertex(float *p,size_t &idx,float &maxDIST);
            int ComputeDistanceFromPoint(float *p,size_t *min_max_idx,float *min_maxDIST,int norm_flag = DIST_NORMALIZE);
            int ComputeDistanceFromPoint(float *p,vector<float> &d_list,size_t *min_max_idx,float *min_max,int norm_flag = DIST_NORMALIZE);
            int ComputeDistanceMatrix();
            int RunFloydWarshall();
            int Subdivide();
            int Decimate();

            bool ComputeConformalParametrization();

            //---- Mesh Rigid Transformations
            int ComputeCentroid();
            int ComputeBoundingBox();
            int TranslateToOrigin();
            int ScaleToUnitBox();
            int TranslateBy(float *xyz);
            int TranslateTo(float *xyz);
            int Rotate(float angle,float *axis);
            int Scale(float sc_x,float sc_y,float sc_z);
            int Update();

            //---- Query Functions
            int GetNumVert();
            int GetNumEdges();
            int GetNumFaces();

            int GetCentroid(float *cen);
            int GetBBox(float *xrng,float *yrng,float *zrng);

            int GetVertPosition(int vert_id,float *p);
			int SetVertPosition(float *p, int vert_id);
			int GetVertNormal(int vert_id,float *n);
            bool GetVertFlag(int vert_id);
            int GetFaceNormal(int face_id,float *n);
            int GetFaceCenter(int face_id,float *c);
            bool GetFaceFlag(int face_id);
            int GetVertFrame(int vert_id,Frame *fr);
            int GetVertColor(int vert_id,float *c);
            float GetVertValue(int vert_id);
            float GetVertGraphDistance(int src_id,int vert_id);

            int GetVertsForFace(int face_id,float *p1,float *p2,float *p3);
            int GetVertNormalsForFace(int face_id,float *n1,float *n2,float *n3);
            int GetVertColorsForFace(int face_id,float *c1,float *c2,float *c3);
            int GetVertValuesForFace(int face_id,float *vals);
            int GetVertGraphDistanceForFace(int src_id,int face_id,float *vals);
            int GetVertIdxForFace(int face_id,int *idx);

            int GetNumNeighborsForVert(int vert_id);
            int GetNumFacesOnVert(int vert_id);
            int GetVertNeighbors(int vert_id,vector<Tuple3f> &neighbors);

            int GetFaceNhood(int face_id,int *f1,int *f2,int *f3);
            int GetFacesIncidentOnVert(int vert_id,int *faces);
            int GetFacesIncidentOnEdge(int edge_id,int *faces,int *f1,int *f2);
            int GetQuadOnEdge(int edge_id,int *faces,int *idx);
            int GetEdgesIncidentOnFace(int face_id,int *e);

            //---- Console Display Functions
            int PrintVerts();
            int PrintEdges();
            int PrintFaces();
            int PrintVertAdjacency();
            int PrintFaceAdjacency();
            int PrintMesh();

            //---- Save Mesh to file
            int SaveToPly(char fName[]);
            int SaveToPly(char fName[],ColorList &c_list);

            int DeleteMesh();
        };
}

