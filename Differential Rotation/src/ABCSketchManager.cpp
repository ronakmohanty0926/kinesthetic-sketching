#include "ABCSketchManager.h"
#include "Renderer.h"

using namespace midl;

void ABCSketchManager::DrawCurve(Curve3 &C)
{	
	//glColor3fv(Blue3f);	
	float p1[3], p2[3];
	for (int i = 1; i < C.GetNumVertices(); i++)
	{	
		C.GetVertex(i - 1, p1);
		C.GetVertex(i, p2);		
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_LINE_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glLineWidth(6);		
		glBegin(GL_LINES);
		float r = maxZ - minZ;
		float c1, c2;
		c1 = 1 - (0.3*((p1[2] - minZ) / r) + 0.5);
		c2 = 1 - (0.3*((p2[2] - minZ) / r) + 0.5);
		glColor3f(c1, c1, c1);
		glVertex3f(p1[0], p1[1], p1[2]);
		glColor3f(c2, c2, c2);
		glVertex3f(p2[0], p2[1], p2[2]);
		glEnd();	
	}
}

void ABCSketchManager::DrawShadow(Curve3 &C)
{
	glColor3f(0.5, 0.5, 0.5);

	float p1[3], p2[3];
	for (int i = 1; i < C.GetNumVertices(); i++)
	{
		C.GetVertex(i - 1, p1);
		C.GetVertex(i, p2);
		float planePt[] = { 0.0, -0.5, 0.0 };
		float dist1 = PointToPlaneDist(p1, planePt, Yaxis);
		float dist2 = PointToPlaneDist(p2, planePt, Yaxis);

		/*p1[0] -= dist1;*/ p1[1] -= dist1; //p1[2] -= dist1;
		/*p2[0] -= dist2;*/ p2[1] -= dist1; //p2[2] -= dist2;		  

		//glTranslatef(0.0, -0.6, 0.0);
		glLineWidth(4);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_LINE_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glDisable(GL_DEPTH_TEST);
		glBegin(GL_LINES);
		glVertex3f(p1[0], p1[1], p1[2]);
		glVertex3f(p1[0], p2[1], p2[2]);
		glEnd();
		glEnable(GL_DEPTH_TEST);
		//glTranslatef(0.0, 0.6, 0.0);
	}
}

void ABCSketchManager::DrawStylusShadow()
{
	glColor3f(0.5, 0.5, 0.5);

	float p1[3];
	float planePt[] = { 0.0, -0.5, 0.0 };
	float dist1 = PointToPlaneDist(curr_stylus, planePt, Yaxis);
	p1[0] = curr_stylus[0]; p1[1] = curr_stylus[1]; p1[2] = curr_stylus[2];
	p1[1] -= dist1;

	DrawSphere(p1, 0.03f, Gray3f, 0.5F);
}

void ABCSketchManager::DrawPlaneShadow(float width, float height, float *center)
{	
	float v1[] = { -0.5*width, -0.5*height, 0.0 };
	float v2[] = { 0.5*width, -0.5*height, 0.0 };
	float planePt[] = { 0.0, -0.5, 0.0 };
	float dist1 = PointToPlaneDist(v1, planePt, Yaxis);
	float dist2 = PointToPlaneDist(v2, planePt, Yaxis);

	v1[1] -= dist1;
	v2[1] -= dist2;

	glPushMatrix();
	glTranslatef(center[0], 0.0, center[2]);
	glLineWidth(4);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glBegin(GL_LINES);	
	glVertex3f(v1[0], v1[1],v1[2]);
	glVertex3f(v2[0], v2[1], v2[2]);
	glEnd();
	glTranslatef(center[0], 0.0, center[2]);
	glPopMatrix();
	glDisable(GL_BLEND);
}

void ABCSketchManager::RenderGrid()
{	
	glTranslatef(0.0, -0.3, 0.0);	
	DrawGrid(GRID_PLANE::ZX, 2.0f, 1, Gray3f);
	glTranslatef(0.0, 0.3, 0.0);
}

void ABCSketchManager::RenderSketch()
{
	bool isFilled = true;

	float center[] = { 0.0,0.0, -2.5 };
	DrawQuad(6.0, 4.0, center, isFilled, Gray3f, 0.3f);
	DrawPlaneShadow(1.0 ,0.6, planeCenter);
	DrawStylusShadow();

	for (int i = 0; i < Sketch.size(); i++)DrawCurve(Sketch[i]);
	if(!Sketch.empty())	for (int i = 0; i < Sketch.size(); i++)DrawShadow(Sketch[i]);
	
	if (isPrevStytlusSketching && isCurrStytlusSketching)
	{
		isFilled = true;
		bool isFilledFrame = false;
		glDisable(GL_DEPTH_TEST);
		DrawSphere(cursor, 0.03f, Green3f, 1.0F);
		glEnable(GL_DEPTH_TEST);

		DrawQuad(1.0, 0.6, planeCenter, isFilled, Blue3f, 0.1f);
		DrawQuad(1.0, 0.6, planeCenter, isFilledFrame, Blue3f, 0.3f);		
		//cerr << "Pz" << planeCenter[2] << endl;
	}

	else if (mode == MODE_ROTATION || mode == MODE_TRANSLATION)
	{
		isFilled = true;
		bool isFilledFrame = false;
			
		DrawQuad(1.0, 0.6, planeCenter, isFilled, Blue3f, 0.1f);
		DrawQuad(1.0, 0.6, planeCenter, isFilledFrame, Blue3f, 0.3f);
		
	

		glDisable(GL_DEPTH_TEST);
		DrawSphere(curr_stylus, 0.03f, Blue3f, 0.5F);
		glEnable(GL_DEPTH_TEST);
		/*glBegin(GL_LINES);	
		glVertex3fv(curr_stylus);
		glVertex3fv(_pivot);
		glEnd();*/
	}
	else
	{
		isFilled = true;
		bool isFilledFrame = false;
		
		DrawQuad(1.0, 0.6, planeCenter, isFilled, Blue3f, 0.1f);
		DrawQuad(1.0, 0.6, planeCenter, isFilledFrame, Blue3f, 0.3f);
		
		glDisable(GL_DEPTH_TEST);
		DrawSphere(curr_stylus, 0.03f, Red3f, 0.5F);
		glEnable(GL_DEPTH_TEST);
	}
}

void ABCSketchManager::UpdateStylus(float *stylus)
{
	clock_t x;
	x = clock();
	stylusmov.push_back(x);

	Tuple3f _stylus;
	
	prev_stylus[0] = curr_stylus[0];
	prev_stylus[1] = curr_stylus[1];
	prev_stylus[2] = curr_stylus[2];	

	curr_stylus[0] = stylus[0];
	curr_stylus[1] = stylus[1];
	curr_stylus[2] = stylus[2];	

	_stylus.data[0] = curr_stylus[0];
	_stylus.data[1] = curr_stylus[2];
	_stylus.data[2] = curr_stylus[1];

	sty.push_back(_stylus);
	
	displacement[0] = curr_stylus[0] - prev_stylus[0];
	displacement[1] = curr_stylus[1] - prev_stylus[1];
	displacement[2] = curr_stylus[2] - prev_stylus[2];	
}

void ABCSketchManager::UpdateHapticStylus(float *stylus)
{
	clock_t x;
	x = clock();
	hapticstylusmov.push_back(x);

	Tuple3f _hstylus;

	_hstylus.data[0] = stylus[0];
	_hstylus.data[1] = stylus[2];
	_hstylus.data[2] = stylus[1];

	hapsty.push_back(_hstylus);
}

void ABCSketchManager::UpdateSketch()
{
	if (mode == MODE_TRANSLATION)
	{
		isCurrStytlusSketching = false;
		isPrevStytlusSketching = false;

		mode = MODE_TRANSLATION;
		currentMode.push_back(mode);

		planeCenter[0] = -(trailElasticity*curr_stylus[0] + (1 - trailElasticity)*prev_stylus[0]) * 0.5;
		planeCenter[1] = -(trailElasticity*curr_stylus[1] + (1 - trailElasticity)*prev_stylus[1]) * 0.5;
		planeCenter[2] = -(trailElasticity*curr_stylus[2] + (1 - trailElasticity)*prev_stylus[2]) * 0.5;

		//temp[0] = translation[0];
		//temp[1] = translation[1];
		//temp[2] = translation[2];

		////cerr << "Sty=>" << translation[2] << endl;
		//if (temp[2] < 0.3 && temp[2] > -0.3)
		//{
		//	planeCenter[0] = temp[0];
		//	planeCenter[1] = temp[1];
		//	planeCenter[2] = temp[2];
		//}

		cursor[0] = curr_stylus[0];
		cursor[1] = curr_stylus[1];
		cursor[2] = curr_stylus[2];

	}
	else if (mode == MODE_ROTATION)
	{
		isCurrStytlusSketching = false;
		isPrevStytlusSketching = false;

		mode == MODE_ROTATION;
		currentMode.push_back(mode);

		Traj[Traj.size() - 1].AddPoint(curr_stylus);
		int lastID = (int)(Traj[Traj.size() - 1].GetNumVertices()) - 4;
		int seclastID = (int)(Traj[Traj.size() - 1].GetNumVertices()) - 7;

		float angle, area, axis[3];
		float v1[3], v2[3], v3[3], last[3], secLast[3];

		if (seclastID > 0)
		{
			Traj[Traj.size() - 1].GetVertex(lastID, last);
			Traj[Traj.size() - 1].GetVertex(seclastID, secLast);

			SubVectors3(curr_stylus, last, v1);//First and i -1
			Normalize3(v1);

			SubVectors3(secLast, last, v2);// i -2 and i - 1
			Normalize3(v2);

			Cross(v1, v2, axis);
			Normalize3(axis);

			bool _axisLength = Norm3(axis);

			SubVectors3(curr_stylus, secLast, v3);//First and i - 2

			bool distance = 1000 * Norm3(v3);
			//cerr << "Dist" << distance << endl;

			if (distance && _axisLength)
			{
				bool area = 0.5*Norm3(axis);
				angle = 6.5 * Norm3(v3);

				float q[4];

				q[0] = cos(angle*0.5);
				q[1] = (sin(angle*0.5))*axis[0];
				q[2] = (sin(angle*0.5))*axis[1];
				q[3] = (sin(angle*0.5))*axis[2];

				matrix[0] = (q[0] * q[0]) + (q[1] * q[1]) - (q[2] * q[2]) - (q[3] * q[3]);
				matrix[1] = (2 * q[1] * q[2]) - (2 * q[0] * q[3]);
				matrix[2] = (2 * q[1] * q[3]) + (2 * q[0] * q[2]);
				matrix[3] = (2 * q[1] * q[2]) + (2 * q[0] * q[3]);
				matrix[4] = (q[0] * q[0]) - (q[1] * q[1]) + (q[2] * q[2]) - (q[3] * q[3]);
				matrix[5] = (2 * q[2] * q[3]) - (2 * q[0] * q[1]);
				matrix[6] = (2 * q[1] * q[3]) - (2 * q[0] * q[2]);
				matrix[7] = (2 * q[2] * q[3]) + (2 * q[0] * q[1]);
				matrix[8] = (q[0] * q[0]) - (q[1] * q[1]) - (q[2] * q[2]) + (q[3] * q[3]);

				if (area)
				{
					meshEvent->Rotate(angle, axis);
					meshvert.clear();
					for (int v = 0; v < meshEvent->GetNumVert(); v++)
					{
						float temp[3];
						meshEvent->GetVertPosition(v, temp);

						vertex.data[0] = temp[0];
						vertex.data[1] = temp[1];
						vertex.data[2] = temp[2];

						meshvert.push_back(vertex);
					}
					for (int i = 0; i < Sketch.size(); i++)
					{
						//Sketch[i].Rotate(angle, axis, Origin);						
						
						for (int k = 0; k < Sketch[i].GetNumVertices(); k++)
						{
							float temp[3];
							Sketch[i].GetVertex(k, temp);
							//AxisAngle(axis, angle, matrix);
							RotateVec3(temp, matrix);
							Sketch[i].SetVertex(temp, k);
						}
					}
				}
				else
				{
					for (int i = 0; i < 9; i++)
					{
						matrix[i] = 0.0;
						matrix[0] = matrix[4] = matrix[4] = 1.0;
						for (int k = 0; k < Sketch.size(); i++)
						{
							Sketch[i].Rotate(angle, axis, Origin);
						}
					}
				}
			}

		}
	}

	else
	{
		isPrevStytlusSketching = isCurrStytlusSketching;
		if (Zdistance > -0.08 && Zdistance < 0.15 && curr_stylus[1] > -0.32 && curr_stylus[1] < 0.29 && curr_stylus[0] > -0.5 && curr_stylus[0] < 0.5)
		{isCurrStytlusSketching = true;}
		else isCurrStytlusSketching = false;

		if (isCurrStytlusSketching && isPrevStytlusSketching && !Sketch.empty())
		{
			float p[] = { curr_stylus[0],curr_stylus[1],curr_stylus[2] };

			int lastID = (int)(Sketch[Sketch.size() - 1].GetNumVertices()) - 1;

			if (lastID > -1)
			{			
				float temp[3];
				Sketch[Sketch.size() - 1].GetVertex(lastID, temp);
				p[0] = trailElasticity*curr_stylus[0] + (1 - trailElasticity)*temp[0];
				p[1] = trailElasticity*curr_stylus[1] + (1 - trailElasticity)*temp[1];
			}
			else
			{
				p[0] = curr_stylus[0]; p[1] = curr_stylus[1];
			}
			p[2] = planeCenter[2];
			Sketch[Sketch.size() - 1].AddPoint(p);
			
			cursor[0] = p[0];
			cursor[1] = p[1];
			cursor[2] = p[2];
			mode = MODE_SKETCH;
			currentMode.push_back(mode);
/*
			myfile2 << mode << " " << curr_stylus[0] << " " << curr_stylus[1] << " " << curr_stylus[2] << " " << stylusmov[stylusmov.size() - 1];
			myfile2 << endl;*/
		}
		else if (isCurrStytlusSketching && !isPrevStytlusSketching)
		{
				Curve3 c;
				Sketch.push_back(c);
				mode = MODE_IDLE;
				currentMode.push_back(mode);

				/*myfile2 << mode << " " << curr_stylus[0] << " " << curr_stylus[1] << " " << curr_stylus[2] << " " << stylusmov[stylusmov.size() - 1];
				myfile2 << endl;*/

			//cerr << "Sketch" << Sketch.size() - 1 << "Pushed back" << endl;	
		}	
			
	}
}

void ABCSketchManager::SortSketchZ()
{
	minZ = FLT_MAX;
	maxZ = -FLT_MAX;

	float p[3];
	for (int i = 0; i < Sketch.size(); i++)
	{
		for (int j = 0; j < Sketch[i].GetNumVertices(); j++)
		{
			Sketch[i].GetVertex(j, p);
			if (p[2] < minZ) minZ = p[2];
			if (p[2] > maxZ) maxZ = p[2];
		}
	}
}

ABCSketchManager::ABCSketchManager()
{
	trailElasticity = 0.5;
	angle = 0.0;

	isCurrStytlusSketching = false;
	isPrevStytlusSketching = false;	

	planeCenter[0] = planeCenter[1] = planeCenter[2] = 0.0;
	
	undoPress = 0;
	redoPress = 0;	

	Tuple2i t;
	t.data[0] = 0; t.data[1] = 1; //Edge 1
	meshEdges.push_back(t);
	t.data[0] = 1; t.data[1] = 3;//Edge 2
	meshEdges.push_back(t);
	t.data[0] = 2; t.data[1] = 3;//Edge 3
	meshEdges.push_back(t);
	t.data[0] = 0; t.data[1] = 2;//Edge 4
	meshEdges.push_back(t);
	t.data[0] = 4; t.data[1] = 6;//Edge 5
	meshEdges.push_back(t);
	t.data[0] = 6; t.data[1] = 7;//Edge 6
	meshEdges.push_back(t);
	t.data[0] = 7; t.data[1] = 5;//Edge 7
	meshEdges.push_back(t);
	t.data[0] = 5; t.data[1] = 4;//Edge 8
	meshEdges.push_back(t);
	t.data[0] = 4; t.data[1] = 0;//Edge 9
	meshEdges.push_back(t);
	t.data[0] = 6; t.data[1] = 2;//Edge 10
	meshEdges.push_back(t);
	t.data[0] = 7; t.data[1] = 3;//Edge 11
	meshEdges.push_back(t);
	t.data[0] = 5; t.data[1] = 1;//Edge 12
	meshEdges.push_back(t);
}

ABCSketchManager::~ABCSketchManager()
{	
		Sketch.clear();		
		Traj.clear();

		isCurrStytlusSketching = false;
		isPrevStytlusSketching = false;

		planeCenter[0] = planeCenter[1] = planeCenter[2] = 0.0;		
}

void ABCSketchManager::InitShaders()
{
	colorShader.Initialize(".//Shaders//diffuseShader.vert", ".//Shaders//diffuseShader.frag");
	textureShader.Initialize(".//Shaders//textureShader.vert", ".//Shaders//textureShader.frag");
}

bool ABCSketchManager::RotationOn()
{	
	if (!Sketch.empty())
	{
		Curve3 t;
		Traj.push_back(t);
		mode = MODE_ROTATION;		
		SortSketchZ();
		return true;
	}
}

void ABCSketchManager::ReadMesh(const char*fName)
{
	if (meshEvent != NULL)
	{
		meshEvent->DeleteMesh();
		delete meshEvent;
	}
	meshEvent = new Mesh();
	meshEvent->InitMesh();
	meshEvent->FromPly((char*)fName);
	meshEvent->TranslateTo(Origin);
	//mesh->ScaleToUnitBox();
	float xRng[] = { -0.5,0.5 }; float yRng[] = { -0.5, 0.5 }; float zRng[] = { -0.5, 0.5 };
	float xLen = xRng[1] - xRng[0]; float yLen = yRng[1] - yRng[0]; float zLen = zRng[1] - zRng[0];

	float newRng[] = { -0.12, 0.12 };
	float Rng = newRng[1] - newRng[0];

	for (int i = 0; i < meshEvent->GetNumVert(); i++)
	{
		float temp[3];
		meshEvent->GetVertPosition(i, temp);
		temp[0] = Rng*((temp[0] - xRng[0]) / xLen) + newRng[0];
		temp[1] = Rng*((temp[1] - yRng[0]) / yLen) + newRng[0];
		temp[2] = Rng*((temp[2] - zRng[0]) / zLen) + newRng[0];	
		meshEvent->SetVertPosition(temp, i);
	}

}

void ABCSketchManager::DrawRefMesh()
{
	DrawMesh((*meshEvent), Magenta3f, MESH_DISPLAY::SOLID_SMOOTH);
}

void ABCSketchManager::UpdatePointToPlaneDist()
{
	Zdistance = PointToPlaneDist(curr_stylus, planeCenter, Zaxis);
}

void ABCSketchManager::ManipulationOff()
{
	mode = MODE_SKETCH;
}

bool midl::ABCSketchManager::GetUserID(int ID)
{
	userID = ID;
	return true;
}

bool ABCSketchManager::TranslationOn()
{
	mode = MODE_TRANSLATION;	
	return true;
}

bool ABCSketchManager::SetHaptics()
{
	needForce = true;
	return true;
}

bool ABCSketchManager::CheckHapticsForce()
{
	return needForce;
}

float ABCSketchManager::SetPlaneCenter(float *newCenter)
{
	newCenter[0] = planeCenter[0];
	newCenter[1] = planeCenter[1];
	newCenter[2] = planeCenter[2];

	return newCenter[3];
}

float ABCSketchManager::SetPivot(float *pivot)
{
	pivot[0] = _pivot[0];
	pivot[1] = _pivot[1];
	pivot[2] = _pivot[2];

	return pivot[3];
}

bool ABCSketchManager::UndoSketch()
{
	if (!Sketch.empty())
	{
		mode = UNDO;
		currentMode.push_back(mode);
		undoPress++;

		Undo.push_back(Sketch[Sketch.size() - 1]);
		Sketch[Sketch.size() - 1].Clear();
		Sketch.erase(Sketch.begin()+Sketch.size()-1);
		return true;
	}
	else return false;
}

bool ABCSketchManager::RedoSketch()
{
	if (!Undo.empty())
	{
		mode = REDO;
		currentMode.push_back(mode);
		redoPress++;
		
		Curve3 newCrv;
		Undo.back().CopyTo(newCrv);
		Sketch.push_back(newCrv);
		DrawCurve(newCrv);
		Undo.back().Clear();
		Undo.erase(Undo.begin() + Undo.size()-1);
		return true;		
	}
	else return false;	
}

void ABCSketchManager::Render()
{
	//Render the 3D Curves
	RenderSketch();

	DrawCurve(c);

	//Render the 3D Depth Grid
	RenderGrid();	
}

int ABCSketchManager::Update()
{
	UpdateSketch();	
	UpdatePointToPlaneDist();
	return mode;
}

int ABCSketchManager::Listen(float *stylus)
{
	UpdateStylus(stylus);
	return Update();
}

int ABCSketchManager::HapListen(float *stylus)
{
	UpdateHapticStylus(stylus);

	return 0;
}

void  ABCSketchManager::ComputeError()
{
	vector<float>deviation;

	for (int i = 0; i < Sketch.size(); i++)
	{
		for (int j = 0; j < Sketch[i].GetNumVertices(); j++)
		{
			float temp[3];
			Sketch[i].GetVertex(j, temp);

			for (int k = 0; k < meshEdges.size(); k++)
			{
				Tuple2i t;
				t = meshEdges[k];
				float dev = PointToLineDist3(temp, meshvert[t.data[0]].data, meshvert[t.data[1]].data);
				deviation.push_back(dev);
			}

			//Find the closest edge
			int closest = -1; float tempDist = FLT_MAX;
			for (int j = 0; j < deviation.size(); j++)
			{
				if (deviation[j] < tempDist)
				{
					tempDist = deviation[j]; closest = j;
				}
			}
			//cerr << "closest edge->" << closest << endl;
			if (closest > -1) //Found closest edge
			{
				min_deviation.push_back(deviation[closest]);
				//skdeviation.push_back(deviation[closest]);
			}
			deviation.clear();
		}
	}

	//Calculate the RMS error
	float numerator, denominator;
	numerator = 0.0;
	denominator = min_deviation.size();
	for (int i = 0; i < min_deviation.size(); i++)
	{
		//cerr << "mindevsize" << min_deviation.size();
		numerator += (min_deviation[i] * min_deviation[i]);
	}

	RMS = sqrt(numerator / denominator);
	cerr << "Error->" << RMS << endl;	/*for (int d = 0; d < deviation.size(); d++)
										{
										if (!deviation.empty())
										{
										float numerator, denominator;
										numerator += NormSq3(deviation.data[d]);
										denominator = deviation.size();
										float RMS = sqrt(numerator / denominator);
										}
										}*/

}

bool ABCSketchManager::SaveLog()
{
	if (!Sketch.empty())
	{
		ofstream myfile;
		myfile.open("Differential_UserLOG.txt");
		time_elapsed = float(stylusmov[stylusmov.size() - 1] - stylusmov[0]) / CLOCKS_PER_SEC;
		myfile << "Time Elapsed->" << time_elapsed << " " << "secs" << endl;
		myfile << "Mode_TYPE" << " " << "StyX" << " " << "StyY" << " " << "StyZ" << " " << "HapStyX" << " " << "HapStyY" << " " << "HapStyZ" << " " << "Stylus_TimeStamp" << endl;
		for (int i = 0; i < currentMode.size(); i++)
		{
			/*string mode;
			if (currentMode[i] == -1) mode = 'Idle';
			else if (currentMode[i] == 0) mode = 'Skch';
			else if (currentMode[i] == 1) mode = 'Trns';
			else if (currentMode[i] == 2) mode = 'Rot';
			else if (currentMode[i] == 3) mode = 'Undo';
			else if (currentMode[i] == 4) mode = 'Redo';*/

			myfile << currentMode[i] << " " << sty[i].data[0] << " " << sty[i].data[1] << " " << sty[i].data[2] << " " << hapsty[i].data[0] << " " << hapsty[i].data[1] << " " << hapsty[i].data[2] << " " << stylusmov[i] << endl;

		}
	}
	return true;
}

bool ABCSketchManager::SaveSketchData()
{
	if (!Sketch.empty())
	{
		ofstream myfile;
		myfile.open("Differential_SketchData.txt");
		int t = 0;

		for (int i = 0; i < Sketch.size(); i++)
		{
			myfile << "Sketch->" << i << endl;
			for (int j = 0; j < Sketch[i].GetNumVertices(); j++)
			{
				float temp[3];
				Sketch[i].GetVertex(j, temp);
				myfile << temp[0] << " " << temp[1] << " " << temp[2] << endl;
			}
		}
	}
	return true;
}

bool ABCSketchManager::SaveMeshData()
{
	if (!Sketch.empty())
	{
		ofstream myfile;
		myfile.open("Differential_MeshData.txt");
		int t = 0;
		myfile << "X" << " " << "Y" << " " << "Z" << endl;
		for (int i = 0; i < meshvert.size(); i++)
		{
			myfile << meshvert[i].data[0] << " " << meshvert[i].data[1] << " " << meshvert[i].data[2] << endl;
		}
	}
	return true;
}

bool ABCSketchManager::SaveMetric()
{
	if (!Sketch.empty())
	{
		ComputeError();
		ofstream myfile;
		myfile.open("Differential_Metric.txt");
		myfile << "RMS Error->" << RMS << endl;
		time_elapsed = float(stylusmov[stylusmov.size() - 1] - stylusmov[0]) / CLOCKS_PER_SEC;
		myfile << "Time Elapsed->" << time_elapsed << " " << "secs" << endl;
		for (int i = 0; i < 9; i++)
		{
			myfile << "matrix" << "[" << i << "]" << "->" << matrix[i] << endl;
		}
		for (int j = 0; j < min_deviation.size(); j++)
		{
			myfile << "Minimum Deviations" << ":[" << j << "]->" << min_deviation[j] << endl;
		}


	}
	return true;
}

