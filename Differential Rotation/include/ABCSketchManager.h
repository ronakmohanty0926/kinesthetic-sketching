#pragma once

#include <queue>
#include <deque>
#include "Core.h"
#include "Renderer.h"
#include "Curve.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "HapticsEventManager.h"

#define MODE_IDLE -1
#define MODE_SKETCH 0
#define MODE_TRANSLATION 1
#define MODE_ROTATION 2
#define UNDO 3
#define REDO 4

namespace midl {
	class Mesh;

	class ABCSketchManager
	{
	private:
		Shader colorShader, textureShader;
		int mode;
		float trailElasticity, Zdistance, Xdistance, Ydistance, planeCenter[3];
		float curr_stylus[3], prev_stylus[3], displacement[3], translation[3], cursor[3], _pivot[3], angle , axis[3];
			
		bool isCurrStytlusSketching;
		bool isPrevStytlusSketching;

		float minZ, maxZ;
		clock_t start, end;
		double time_elapsed;
		float mindeviation;

		vector<Tuple3f>meshvert;
		vector<Tuple3f>meshedge;
		Tuple3f vertex;

		vector<Curve3> Sketch;
		vector<Curve3> Traj;
		float matrix[9];

		deque<Curve3>Undo;

		vector<clock_t>stylusmov;
		vector<clock_t>hapticstylusmov;

		vector<int> currentMode;

		vector<Tuple3f>sty;
		vector<Tuple3f>hapsty;

		void DrawCurve(Curve3 &C);
		void DrawShadow(Curve3 &C);
		void DrawStylusShadow();
		void DrawPlaneShadow(float width, float height, float *center);
		void RenderGrid();
		void RenderSketch();

		void UpdateStylus(float *stylus);
		void UpdateHapticStylus(float *stylus);
		void UpdateSketch();
		void SortSketchZ();
	
		GLuint materialID;

		//Deviation vector
		vector<float> min_deviation;
		vector<Tuple2i> meshEdges;
		float RMS;
		
	public:
		//char *userID;

		ABCSketchManager();
		~ABCSketchManager();

		Curve3 c;
		Mesh *meshEvent;
		int undoPress;
		int redoPress;

		void InitShaders();
		void Render();

		void UpdatePointToPlaneDist();
		void ReadMesh(const char*fName);
		void DrawRefMesh();
				
		bool RotationOn();
		bool TranslationOn();
		void ManipulationOff();	
		bool GetUserID (int ID);
		bool GetTaskID;
		
		float SetPlaneCenter(float *newCenter);
		float SetPivot(float *pivot);
	
		int Update();
		int Listen(float *stylus);
		int HapListen(float *stylus);
		int userID;		

		bool needForce = false;
		bool CheckHapticsForce();
		bool SetHaptics();
		bool UndoSketch();
		bool RedoSketch();

		void ComputeError();
		bool SaveLog();
		bool SaveSketchData();
		bool SaveMeshData();
		bool SaveMetric();

	};
}