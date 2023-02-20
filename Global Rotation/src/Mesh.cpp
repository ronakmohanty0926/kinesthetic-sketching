#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <math.h>
#include <time.h>
#include <io.h>
#include <fcntl.h>
#include <cstdio>
#include <fstream>
#include <crtdbg.h>

#include "GL/glew.h"
#include "GL/glut.h"

#include <Mesh.h>


namespace midl
{
	Vert *_subdiv_NewVertex(int id,float *p,float *n,float *c)
	{
        Vert *v;
        v = new Vert;

		if(p == NULL || n == NULL || c == NULL)
			v->index = -1;
		else v->index = id;

		if(p != NULL)
		{
			v->position[0] = p[0];
			v->position[1] = p[1];
			v->position[2] = p[2];
		}
		else
		{
			v->position[0] = 0.0;
			v->position[1] = 0.0;
			v->position[2] = 0.0;
		}

		if(n != NULL)
		{
			v->normal[0] = n[0];
			v->normal[1] = n[1];
			v->normal[2] = n[2];
		}
		else
		{
			v->normal[0] = 0.0;
			v->normal[1] = 0.0;
			v->normal[2] = 0.0;
		}

		if(c != NULL)
		{
			v->color[0] = c[0];
			v->color[1] = c[1];
			v->color[2] = c[2];
		}
		else
		{
			v->color[0] = 0.0;
			v->color[1] = 0.0;
			v->color[2] = 0.0;
		}

		//---- Initialize Scale Field and Moving Frame
		v->value = 0.0;
		v->flag = false;
		v->frame.u[0] = 1.0;
		v->frame.u[1] = 0.0;
		v->frame.u[2] = 0.0;

		v->frame.v[0] = 0.0;
		v->frame.v[1] = 1.0;
		v->frame.v[2] = 0.0;

		v->frame.w[0] = 0.0;
		v->frame.w[1] = 0.0;
		v->frame.w[2] = 1.0;

		/*cdCompPair pr;
		pr.first = 0.0;
		pr.second = 0.0;
		components.push_back(pr);*/

		return v;
	}

	Edge *_subdiv_NewEdge()
	{
		Edge *e = new Edge;
		e->face = NULL;
		e->head = NULL;
		e->index = -1;
		e->len = 0.0;
		e->next = NULL;
		e->tail = NULL;
		e->twin = NULL;

		return e;
	}

    //---- Smoothing Weight Computations
    float EdgeWeights(Edge *e)
    {
        return e->len;
    }

    float CotangentWeights(Edge *e)
    {
        if(e == NULL || e->twin == NULL)
            return 0.0;

        float alpha,beta;

        Vert *v1 = e->next->tail;
        Vert *v2 = e->twin->next->tail;

        float l1[3],l2[3],_dot;

        SubVectors3(e->head->position,v1->position,l1);
        SubVectors3(e->tail->position,v1->position,l2);
        _dot = Dot3(l1,l2)/(Norm3(l1)*Norm3(l2));
        if(_isnan(_dot))_dot = 0.0;
        alpha = std::acosf(_dot);

        SubVectors3(e->head->position,v2->position,l1);
        SubVectors3(e->tail->position,v2->position,l2);
        _dot = Dot3(l1,l2)/(Norm3(l1)*Norm3(l2));
        if(_isnan(_dot))_dot = 0.0;
        beta = std::acosf(_dot);

        return -0.5*((1.0/std::tanf(alpha))+(1.0/std::tanf(beta)));
    }

    //---- PLY reading functions
    int ReadPlyHeader(const char file[],int size[2])
    {
        FILE *stream;
        char head[4],key[300];
        char separater[]   = " ,\t\n";
        char *token;
        int check1,check2,check3 = -1;
        int j=0;

        if((stream = fopen(file,"r")) != NULL)
        {
            fgets(head,4,stream);
            check1 = strcmp(head,"ply");

            if(check1 == 0)
            {
                check1 = -1;
                while(check1 != 0)
                {
                    fgets(key,300,stream);
                    check1 = strncmp(key,"end_header",10);
                    check2 = strncmp(key,"element vertex",13);
                    if(check2 == 0)
                    {
                        token = strtok(key, separater );
                        while( token != NULL )
                        {
                            size[0] = atoi(token);
                            token = strtok( NULL, separater );
                        }
                    }

                    check3 = strncmp(key,"element face",12);
                    if(check3 == 0)
                    {
                        token = strtok(key, separater );
                        while( token != NULL )
                        {
                            size[1] = atoi(token);
                            token = strtok( NULL, separater );
                        }
                    }
                    j++;
                }
            }
            else
            {
                size[0] = 0;
                size[1] = 0;
                cout<<"The file "<<file<<" is not a '.ply' file."<<endl;
                return 0;
            }
            fclose(stream);
        }
        else
        {
            size[0] = 0;
            size[1] = 0;
            cout<<file<<" could not be read."<<endl;
            return 0;
        }
        return j;
    }

    void TokenizePlyVertLine(char key[],float v[3],float n[3],float c[3])
    {
        char separater[]   = " ,\t\n";
        char *token,*copy;
        int i=0;
        int ci;

        copy = _strdup(key);
        token = strtok(copy, separater );
        while(token != NULL)
        {
            if(i < 3)
                v[i] = atof(token);

            if(i > 2 && i < 6)
                n[i-3] = atof(token);

            if(i > 5 && i < 9)
            {
                ci = atoi(token);
                c[i-6] = (float)ci/255.0;
            }
            token = strtok( NULL, separater );
            i++;
        }
        free(copy);
        return;
    }

    void TokenizePlyFaceLine(char key[],int f[3])
    {
        char separater[]   = " ,\t\n";
        char *token,*copy;
        int i=0;

        copy = _strdup(key);
        token = strtok(copy, separater );
        while(token != NULL)
        {
            if(i > 0) f[i-1] = atoi(token);
            token = strtok( NULL, separater );
            i++;
        }
        free(copy);
        return;
    }

	//---- Mesh Methods
	int Mesh::AddVertToMesh(int id,float *p,float *n,float *c)
	{
		Vert *v;
		v = new Vert;

		if(p == NULL || n == NULL || c == NULL)
			v->index = -1;
		else v->index = id;

		if(p != NULL)
		{
			v->position[0] = p[0];
			v->position[1] = p[1];
			v->position[2] = p[2];
		}
		else
		{
			v->position[0] = 0.0;
			v->position[1] = 0.0;
			v->position[2] = 0.0;
		}

		if(n != NULL)
		{
			v->normal[0] = n[0];
			v->normal[1] = n[1];
			v->normal[2] = n[2];
		}
		else
		{
			v->normal[0] = 0.0;
			v->normal[1] = 0.0;
			v->normal[2] = 0.0;
		}

		if(c != NULL)
		{
			v->color[0] = c[0];
			v->color[1] = c[1];
			v->color[2] = c[2];
		}
		else
		{
			v->color[0] = 0.0;
			v->color[1] = 0.0;
			v->color[2] = 0.0;
		}

		//---- Initialize Scale Field and Moving Frame
		v->value = 0.0;
		v->flag = false;
		v->frame.u[0] = 1.0;
		v->frame.u[1] = 0.0;
		v->frame.u[2] = 0.0;

		v->frame.v[0] = 0.0;
		v->frame.v[1] = 1.0;
		v->frame.v[2] = 0.0;

		v->frame.w[0] = 0.0;
		v->frame.w[1] = 0.0;
		v->frame.w[2] = 1.0;

		vertices.push_back(v);

		CompPair pr;
		pr.first = 0.0;
		pr.second = 0.0;
		components.push_back(pr);

		return 0;
	}

	int Mesh::BuildComplexFromFace(int *t)
	{
		if(t != NULL)
		{
			Edge *t_e1,*t_e2,*t_e3;
			t_e1 = new Edge;
			t_e2 = new Edge;
			t_e3 = new Edge;

			Face *t_f;
			t_f = new Face;

			t_e1->index = edges.size();
			t_e1->head = vertices[t[0]];
			t_e1->tail = vertices[t[1]];
			t_e1->next = t_e2;
			t_e1->face = t_f;
			t_e1->twin = NULL;
		
			t_e2->index = edges.size()+1;
			t_e2->head = vertices[t[1]];
			t_e2->tail = vertices[t[2]];
			t_e2->next = t_e3;
			t_e2->face = t_f;
			t_e2->twin = NULL;

			t_e3->index = edges.size()+2;
			t_e3->head = vertices[t[2]];
			t_e3->tail = vertices[t[0]];
			t_e3->next = t_e1;
			t_e3->face = t_f;
			t_e3->twin = NULL;
		
			t_f->index = faces.size();
			t_f->e1 = t_e1;
			t_f->e2 = t_e2;
			t_f->e3 = t_e3;

			vertices[t[0]]->edges.push_back(t_e1);
			vertices[t[1]]->edges.push_back(t_e2);
			vertices[t[2]]->edges.push_back(t_e3);

			vertices[t[0]]->faces.push_back(t_f);
			vertices[t[1]]->faces.push_back(t_f);
			vertices[t[2]]->faces.push_back(t_f);

			edges.push_back(t_e1);
			edges.push_back(t_e2);
			edges.push_back(t_e3);

			faces.push_back(t_f);
			return 0;
		}
		else return -1;
	}

	int Mesh::BuildHalfEdgeConnectivity()
	{
		// Iterate over edges
		Vert *v;
		pair<Edge*,Edge*> t_pair;
		for(size_t i = 0;i < edges.size();i++)
		{
			if(edges[i]->twin == NULL)
			{
				v = edges[i]->tail;
				for(size_t j = 0;j < v->edges.size();j++)
				{
					if(v->edges[j]->tail->index == edges[i]->head->index)
					{
						edges[i]->twin = v->edges[j];
						v->edges[j]->twin = edges[i];
						t_pair.first = edges[i];
						t_pair.second = edges[i]->twin;
						edge_pairs.push_back(t_pair);
						break;
					}
				}
			}
		}

		for(size_t i = 0;i < edges.size();i++)
		{
			if(edges[i]->twin == NULL)
			{
				t_pair.first = edges[i];
				t_pair.second = NULL;
				edge_pairs.push_back(t_pair);
			}
		}

		for(size_t i = 0;i < edge_pairs.size();i++)
		{
			if(edge_pairs[i].second != NULL)
				edge_pairs[i].second->index = i;

			edge_pairs[i].first->index = i;
		}

		return 0;
	}

	int Mesh::ComputeEdgeLength(Edge *e)
	{
		float line[3];

		if(e != NULL)
		{
            SubVectors3(e->head->position,e->tail->position,line);
			e->len = Norm3(line);
			if(e->twin != NULL)
				e->twin->len = e->len;
		}
		else if(e->twin != NULL)
		{
            SubVectors3(e->twin->head->position,e->twin->tail->position,line);
			e->twin->len = Norm3(line);
		}

		return 0;
	}

	int Mesh::UpdateEdgeLengths()
	{
		int i;
		int thID,nThrds;
	#pragma omp parallel private(thID, i)
		{
			float line[3];
			thID = omp_get_thread_num();
			if (thID == 0)
				nThrds = omp_get_num_threads();
	#pragma omp for
			for(i = 0;i < edge_pairs.size();i++)
			{
				if(edge_pairs[i].first != NULL)
				{
                    SubVectors3(edge_pairs[i].first->head->position,edge_pairs[i].first->tail->position,line);
					edge_pairs[i].first->len = Norm3(line);
					if(edge_pairs[i].second != NULL)
						edge_pairs[i].second->len = edge_pairs[i].first->len;
				}
				else if(edge_pairs[i].second != NULL)
				{
                    SubVectors3(edge_pairs[i].second->head->position,edge_pairs[i].second->tail->position,line);
					edge_pairs[i].second->len = Norm3(line);
				}
			}
		}
		return 0;
	}

	int Mesh::ComputeFaceArea(Face *f)
	{
		float v1[3],v2[3];
		if(f != NULL)
		{
            SubVectors3(f->e1->head->position,f->e1->tail->position,v1);
            SubVectors3(f->e1->next->tail->position,f->e1->tail->position,v2);
            Cross(v2,v1,f->normal);
			f->area = 0.5*Norm3(f->normal);
			Normalize3(f->normal);
		}

		return 0;
	}

	int Mesh::UpdateFaceAreas()
	{
		int i;
		int thID,nThrds;
	#pragma omp parallel private(thID, i)
		{
			float v1[3],v2[3];
			thID = omp_get_thread_num();
			if (thID == 0)
				nThrds = omp_get_num_threads();
	#pragma omp for
			for(i = 0;i < faces.size();i++)
			{
				if(faces[i] != NULL)
				{
                    SubVectors3(faces[i]->e1->head->position,faces[i]->e1->tail->position,v1);
                    SubVectors3(faces[i]->e1->next->tail->position,faces[i]->e1->tail->position,v2);
                    Cross(v2,v1,faces[i]->normal);
					faces[i]->area = 0.5*Norm3(faces[i]->normal);
					Normalize3(faces[i]->normal);
                    TriCenter3(faces[i]->e1->head->position,faces[i]->e2->head->position,faces[i]->e3->head->position,faces[i]->center);
				}
			}
		}
		return 0;
	}

	int Mesh::ComputeNormalAtVert(int id)
	{
		if(vertices[id] != NULL)
		{
			vertices[id]->normal[0] = 0.0;
			vertices[id]->normal[1] = 0.0;
			vertices[id]->normal[2] = 0.0;
			for(size_t i = 0;i < vertices[id]->faces.size();i++)
			{
				/*
				ComputeFaceNormal(vertices[id]->faces[i],n);
				vertices[id]->normal[0] -= n[0];
				vertices[id]->normal[1] -= n[1];
				vertices[id]->normal[2] -= n[2];
				*/
				vertices[id]->normal[0] += vertices[id]->faces[i]->normal[0];
				vertices[id]->normal[1] += vertices[id]->faces[i]->normal[1];
				vertices[id]->normal[2] += vertices[id]->faces[i]->normal[2];
			}
			vertices[id]->normal[0] /= (float)vertices[id]->faces.size();
			vertices[id]->normal[1] /= (float)vertices[id]->faces.size();
			vertices[id]->normal[2] /= (float)vertices[id]->faces.size();
		}

		return 0;
	}

	int Mesh::ComputeNormalAtVert(Vert *v)
	{
		if(v != NULL)
		{
			v->normal[0] = 0.0;
			v->normal[1] = 0.0;
			v->normal[2] = 0.0;
			for(size_t i = 0;i < v->faces.size();i++)
			{
				/*
				ComputeFaceNormal(v->faces[i],n);
				v->normal[0] -= n[0];
				v->normal[1] -= n[1];
				v->normal[2] -= n[2];*/

				v->normal[0] += v->faces[i]->normal[0];
				v->normal[1] += v->faces[i]->normal[1];
				v->normal[2] += v->faces[i]->normal[2];
			}
			v->normal[0] /= (float)v->faces.size();
			v->normal[1] /= (float)v->faces.size();
			v->normal[2] /= (float)v->faces.size();
		}

		return 0;
	}

	MeshVertices Mesh::GetVertNhood(Vert *v)
	{
		MeshVertices nhood;
		for(size_t i = 0;i < v->edges.size();i++)
			nhood.push_back(v->edges[i]->tail);

		return nhood;
	}

	MeshFaces Mesh::GetFaceNhood(Face *f)
	{
		MeshFaces nhood;
		nhood.push_back(f->e1->twin->face);
		nhood.push_back(f->e2->twin->face);
		nhood.push_back(f->e3->twin->face);

		return nhood;
	}

	MeshFaces Mesh::GetFacesIncidentOnVert(Vert *v)
	{
		MeshFaces inc_faces;
		for(size_t i = 0;i < v->faces.size();i++)
			inc_faces.push_back(v->faces[i]);

		return inc_faces;
	}

	MeshFaces Mesh::GetFacesIncidentOnEdge(Edge *e)
	{
		MeshFaces inc_faces;
		inc_faces.push_back(e->face);
		inc_faces.push_back(e->twin->face);
		return inc_faces;
	}

	MeshEdges Mesh::GetEdgesIncidentOnFace(Face *f)
	{
		MeshEdges inc_edges;
		inc_edges.push_back(f->e1);
		inc_edges.push_back(f->e2);
		inc_edges.push_back(f->e3);
		return inc_edges;
	}

    /*Get vertices on incident faces for e
             v1
           / | \
          /  |  \
        v2   e   v4
          \  |  /
           \ | /
             v3
    */
	
	void Mesh::SplitEdge(EdgePair &pair)
	{
		// Original half-edges
		Edge *e1 = pair.first;
		Edge *e2 = pair.second;

		Edge *e23 = e1->next;
		Edge *e31 = e1->next->next;
		Edge *e14 = e2->next;
		Edge *e42 = e2->next->next;

		// Original vertices
		Vert *v1 = e1->head;
		Vert *v2 = e2->head;
		Vert *v3 = e1->next->tail;
		Vert *v4 = e2->next->tail;

		// 5-6-7 Condition
		//if(v1->edges.size() < 6 || v2->edges.size() < 6 || v3->edges.size() > 6 || v4->edges.size() > 6)return;
		
		// Original faces
		Face *f123 = e1->face;
		Face *f214 = e2->face;

		// After split, we will get:
		// 4 edges, i.e. 8 half-edges
		Edge *E1 = _subdiv_NewEdge(),*E2 = _subdiv_NewEdge();
		Edge *E3 = _subdiv_NewEdge(),*E4 = _subdiv_NewEdge();
		Edge *E5 = _subdiv_NewEdge(),*E6 = _subdiv_NewEdge();
		Edge *E7 = _subdiv_NewEdge(),*E8 = _subdiv_NewEdge();

		// Split Edge at center and add new vertex
		float p[3];
        MidPoint3(e1->head->position,e1->tail->position,p);
		Vert *v = _subdiv_NewVertex((int)vertices.size(),p,NULL,NULL);
		v->index = (int)vertices.size();

		v->edges.push_back(E2);
		v->edges.push_back(E5);
		v->edges.push_back(E3);
		v->edges.push_back(E8);
		vertices.push_back(v);

		// E1 and E2
		E1->head = v1;
		E1->tail = v;
		E2->head = v;
		E2->tail = v1;
		ComputeEdgeLength(E1);
		ComputeEdgeLength(E2);

		// E3 and E4
		E3->head = v;
		E3->tail = v2;
		E4->head = v2;
		E4->tail = v;
		ComputeEdgeLength(E3);
		ComputeEdgeLength(E4);

		// E5 and E6
		E5->head = v;
		E5->tail = v3;
		E6->head = v3;
		E6->tail = v;
		ComputeEdgeLength(E5);
		ComputeEdgeLength(E6);

		// E7 and E8
		E7->head = v4;
		E7->tail = v;
		E8->head = v;
		E8->tail = v4;
		ComputeEdgeLength(E7);
		ComputeEdgeLength(E8);

		// Nexts
		E1->next = E5;
		E2->next = e14;
		E3->next = e23;
		E4->next = E8;
		E5->next = e31;
		E6->next = E3;
		E7->next = E2;
		E8->next = e42;

		// Twins
		E1->twin = E2;
		E2->twin = E1;
		E3->twin = E4;
		E4->twin = E3;
		E5->twin = E6;
		E6->twin = E5;
		E7->twin = E8;
		E8->twin = E7;

		// Update Edge and Edge pair arrays
		// We will replace e1 and e2 with E1 and E2
		E1->index = e1->index;
		for(size_t i = 0;i < edges.size();i++)
		{
			if(edges[i] == e1)
			{
				edges[i] = E1;
				//E1->index = (int)i;
				break;
			}
		}
		E2->index = e2->index;
		for(size_t i = 0;i < edges.size();i++)
		{
			if(edges[i] == e2)
			{
				edges[i] = E2;
				//E2->index = (int)i;
				break;
			}
		}
		pair.first = E1;
		pair.second = E2;

		E3->index = (int)edges.size();
		E4->index = (int)edges.size()+1;
		E5->index = (int)edges.size()+2;
		E6->index = (int)edges.size()+3;
		E7->index = (int)edges.size()+4;
		E8->index = (int)edges.size()+5;

		edges.push_back(E3);
		edges.push_back(E4);
		edges.push_back(E5);
		edges.push_back(E6);
		edges.push_back(E7);
		edges.push_back(E8);

		EdgePair ep1,ep2,ep3;
		ep1.first = E3;
		ep1.second = E4;
		ep2.first = E5;
		ep2.second = E6;
		ep3.first = E7;
		ep3.second = E8;
		edge_pairs.push_back(ep1);
		edge_pairs.push_back(ep2);
		edge_pairs.push_back(ep3);

		// Create faces
		Face *f15,*f63,*f48,*f72;

		// Replace face f123 with f15
		f15 = new Face();f15->index = f123->index;
		f15->e1 = E1;f15->e2 = E5;f15->e3 = e31;
		faces[f123->index] = f15;
		ComputeFaceArea(f15);
		TriCenter3(f15->e1->head->position,f15->e2->head->position,f15->e3->head->position,f15->center);
		E1->face = f15;
		E5->face = f15;
		e31->face = f15;
		e31->next = E1;

		// Replace face f214 with f72
		f72 = new Face();f72->index = f214->index;
		f72->e1 = E7;f72->e2 = E2;f72->e3 = e14;
		faces[f214->index] = f72;
		ComputeFaceArea(f72);
		TriCenter3(f72->e1->head->position,f72->e2->head->position,f72->e3->head->position,f72->center);
		E7->face = f72;
		E2->face = f72;
		e14->face = f72;
		e14->next = E7;

		f63 = new Face();f63->index = (int)faces.size();
		f63->e1 = E6;f63->e2 = E3;f63->e3 = e23;
		ComputeFaceArea(f63);
		TriCenter3(f63->e1->head->position,f63->e2->head->position,f63->e3->head->position,f63->center);
		E6->face = f63;
		E3->face = f63;
		e23->face = f63;
		e23->next = E6;

		f48 = new Face();f48->index = (int)faces.size()+1;
		f48->e1 = E4;f48->e2 = E8;f48->e3 = e42;
		ComputeFaceArea(f48);
		TriCenter3(f48->e1->head->position,f48->e2->head->position,f48->e3->head->position,f48->center);
		E4->face = f48;
		E8->face = f48;
		e42->face = f48;
		e42->next = E4;

		faces.push_back(f63);
		faces.push_back(f48);

		v->faces.push_back(f15);
		v->faces.push_back(f72);
		v->faces.push_back(f63);
		v->faces.push_back(f48);
		for(size_t i = 0;i < v->faces.size();i++)
		{
			v->normal[0] += v->faces[i]->normal[0];
			v->normal[1] += v->faces[i]->normal[1];
			v->normal[2] += v->faces[i]->normal[2];
		}
		v->normal[0] /= 4.0;
		v->normal[1] /= 4.0;
		v->normal[2] /= 4.0;
		Normalize3(v->normal);

		// Update edge list at v1-v4
		for(size_t i = 0;i < v1->edges.size();i++)
		{
			if(v1->edges[i] == e1)
			{
				v1->edges[i] = E1;
				break;
			}
		}

		for(size_t i = 0;i < v2->edges.size();i++)
		{
			if(v2->edges[i] == e2)
			{
				v2->edges[i] = E4;
				break;
			}
		}

		v3->edges.push_back(E6);
		v4->edges.push_back(E7);

		// Update face list at v1-v4
		for(size_t i = 0;i < v1->faces.size();i++)
		{
			if(v1->faces[i] == f123)
				v1->faces[i] = f15;
			else if(v1->faces[i] == f214)
				v1->faces[i] = f72;
		}

		for(size_t i = 0;i < v2->faces.size();i++)
		{
			if(v2->faces[i] == f123)
				v2->faces[i] = f63;
			else if(v2->faces[i] == f214)
				v2->faces[i] = f48;
		}

		for(size_t i = 0;i < v3->faces.size();i++)
		{
			if(v3->faces[i] == f123)
			{
				v3->faces[i] = f15;
				break;
			}
		}
		v3->faces.push_back(f63);

		for(size_t i = 0;i < v4->faces.size();i++)
		{
			if(v4->faces[i] == f214)
			{
				v4->faces[i] = f48;
				break;
			}
		}
		v4->faces.push_back(f72);

		// Delete old edges and faces
		delete f123;
		delete f214;
		delete e1;
		delete e2;

		// HOPEFULLY WE ARE DONE!!
	}

	void Mesh::CollapseEdge(EdgePair &pair)
	{
	}

	void Mesh::SwapEdge(EdgePair &pair)
	{
		// Original half-edges
		Edge *e1 = pair.first;
		Edge *e2 = pair.second;

		Edge *e23 = e1->next;
		Edge *e31 = e1->next->next;
		Edge *e14 = e2->next;
		Edge *e42 = e2->next->next;

		// Original vertices
		Vert *v1 = e1->head;
		Vert *v2 = e2->head;
		Vert *v3 = e1->next->next->head;
		Vert *v4 = e2->next->next->head;

		// Original faces
		Face *f123 = e1->face;
		Face *f214 = e2->face;

		// Indices for e1 and e2 on v1 and v2 lists
		int i1 = -1,i2 = -1;
		for(size_t i = 0;i < v1->edges.size();i++)
		{
			if(v1->edges[i] == e1)
			{
				i1 = (int)i;
				break;
			}
		}
		for(size_t i = 0;i < v2->edges.size();i++)
		{
			if(v2->edges[i] == e2)
			{
				i2 = (int)i;
				break;
			}
		}

		// Indices for f123 and f214 on v1 and v2 face lists
		int if1 = -1,if2 = -1;
		for(size_t i = 0;i < v1->faces.size();i++)
		{
			if(v1->faces[i] == f214)
			{
				if1 = (int)i;
				break;
			}
		}
		for(size_t i = 0;i < v2->faces.size();i++)
		{
			if(v2->faces[i] == f123)
			{
				if2 = (int)i;
				break;
			}
		}

		//cerr<<i1<<"  "<<i2<<"  "<<if1<<"  "<<if2<<endl;

		// Check for condition
		float vec1[3],vec2[3];
		SubVectors3(v1->position,v2->position,vec1);
		SubVectors3(v3->position,v4->position,vec2);

		bool isInEdgeSet = false;
		for(size_t i = 0;i < v3->edges.size();i++)
			if(v3->edges[i]->tail == v4)isInEdgeSet = true;
		
		//if(Norm3(vec1) < Norm3(vec2) || isInEdgeSet)return;
		if(isInEdgeSet)return;

		if(v1 == v3 || v1 == v4 || v2 == v3 || v2 == v4)
		{
			cerr<<v1<<"  "<<v2<<"  "<<v3<<"  "<<v4<<endl;
			/*cerr<<v1->edges.size()<<"  ";
			cerr<<v2->edges.size()<<"  ";
			cerr<<v3->edges.size()<<"  ";
			cerr<<v4->edges.size()<<endl;*/
			cerr<<v1->faces.size()<<"  ";
			cerr<<v2->faces.size()<<"  ";
			cerr<<v3->faces.size()<<"  ";
			cerr<<v4->faces.size()<<endl;
		}

		if(i1 == -1 || i2 == -1 || if1 == -1 || if2 == -1)return;

		// Swap edges
		e1->head = v4;
		e1->tail = v3;
		e1->next = e31;
		e1->len = Norm3(vec2);

		e2->head = v3;
		e2->tail = v4;
		e2->next = e42;
		e2->len = Norm3(vec2);

		// Correct next edges for other edges
		e31->next = e14;
		e14->next = e1;
		e23->next = e2;
		e42->next = e23;

		// Update faces
		f123->e1 = e1;
		f123->e2 = e31;
		f123->e3 = e14;
		ComputeFaceArea(f123);
		TriCenter3(f123->e1->head->position,f123->e2->head->position,f123->e3->head->position,f123->center);

		f214->e1 = e2;
		f214->e2 = e42;
		f214->e3 = e23;
		ComputeFaceArea(f214);
		TriCenter3(f214->e1->head->position,f214->e2->head->position,f214->e3->head->position,f214->center);

		// Update edges on vertices
		v1->edges[i1] = v1->edges[v1->edges.size()-1];
		v1->edges.pop_back();

		v2->edges[i2] = v2->edges[v2->edges.size()-1];
		v2->edges.pop_back();

		v3->edges.push_back(e2);
		v4->edges.push_back(e1);

		// Update faces on vertices
		v1->faces[if1] = v1->faces[v1->faces.size()-1];
		v1->faces.pop_back();

		v2->faces[if2] = v2->faces[v2->faces.size()-1];
		v2->faces.pop_back();

		v3->faces.push_back(f214);
		v4->faces.push_back(f123);

		// Update vertex normals
		v1->normal[0] = 0.0;
		v1->normal[1] = 0.0;
		v1->normal[2] = 0.0;
		for(size_t i = 0;i < v1->faces.size();i++)
		{
			v1->normal[0] += v1->faces[i]->normal[0];
			v1->normal[1] += v1->faces[i]->normal[1];
			v1->normal[2] += v1->faces[i]->normal[2];
		}
		v1->normal[0] /= (float)v1->faces.size();
		v1->normal[1] /= (float)v1->faces.size();
		v1->normal[2] /= (float)v1->faces.size();
		Normalize3(v1->normal);

		v2->normal[0] = 0.0;
		v2->normal[1] = 0.0;
		v2->normal[2] = 0.0;
		for(size_t i = 0;i < v2->faces.size();i++)
		{
			v2->normal[0] += v2->faces[i]->normal[0];
			v2->normal[1] += v2->faces[i]->normal[1];
			v2->normal[2] += v2->faces[i]->normal[2];
		}
		v2->normal[0] /= (float)v2->faces.size();
		v2->normal[1] /= (float)v2->faces.size();
		v2->normal[2] /= (float)v2->faces.size();
		Normalize3(v2->normal);

		v3->normal[0] = 0.0;
		v3->normal[1] = 0.0;
		v3->normal[2] = 0.0;
		for(size_t i = 0;i < v3->faces.size();i++)
		{
			v3->normal[0] += v3->faces[i]->normal[0];
			v3->normal[1] += v3->faces[i]->normal[1];
			v3->normal[2] += v3->faces[i]->normal[2];
		}
		v3->normal[0] /= (float)v3->faces.size();
		v3->normal[1] /= (float)v3->faces.size();
		v3->normal[2] /= (float)v3->faces.size();
		Normalize3(v3->normal);

		v4->normal[0] = 0.0;
		v4->normal[1] = 0.0;
		v4->normal[2] = 0.0;
		for(size_t i = 0;i < v4->faces.size();i++)
		{
			v4->normal[0] += v4->faces[i]->normal[0];
			v4->normal[1] += v4->faces[i]->normal[1];
			v4->normal[2] += v4->faces[i]->normal[2];
		}
		v4->normal[0] /= (float)v4->faces.size();
		v4->normal[1] /= (float)v4->faces.size();
		v4->normal[2] /= (float)v4->faces.size();
		Normalize3(v4->normal);

		if(v1 == v3 || v1 == v4 || v2 == v3 || v2 == v4)
		{
			/*cerr<<v1->edges.size()<<"  ";
			cerr<<v2->edges.size()<<"  ";
			cerr<<v3->edges.size()<<"  ";
			cerr<<v4->edges.size()<<endl<<endl;*/
			cerr<<v1->faces.size()<<"  ";
			cerr<<v2->faces.size()<<"  ";
			cerr<<v3->faces.size()<<"  ";
			cerr<<v4->faces.size()<<endl<<endl;
		}
	}

	void Mesh::SplitFace(Face *f)
	{
		float diff[3];
		int idx = -1;
		for(int i = 0;i < (int)faces.size();i++)
		{
			if(faces[i] == f)
			{
				idx = i;
				break;
			}
		}

		if(idx == -1)return;

		Edge *e12 = f->e1;
		Edge *e23 = f->e2;
		Edge *e31 = f->e3;

		Vert *v1 = e12->head;
		Vert *v2 = e23->head;
		Vert *v3 = e31->head;

		Vert *v4 = _subdiv_NewVertex((int)vertices.size(),f->center,f->normal,NULL);
		v4->index = (int)vertices.size();
		vertices.push_back(v4);

		// New Edges
		Edge *e14 = new Edge(),*e41 = new Edge();
		Edge *e24 = new Edge(),*e42 = new Edge();
		Edge *e34 = new Edge(),*e43 = new Edge();

		// New Faces
		Face *f143 = new Face();
		Face *f342 = new Face();
		Face *f241 = new Face();

		// Fill edge data
		e14->face = f143;
		e14->head = v1;
		e14->tail = v4;
		e14->next = e43;
		e14->twin = e41;
		SubVectors3(v1->position,v4->position,diff);
		e14->len = Norm3(diff);
		e14->index = (int)edges.size();

		e41->face = f241;
		e41->head = v4;
		e41->tail = v1;
		e41->next = e12;
		e41->twin = e14;
		SubVectors3(v1->position,v4->position,diff);
		e41->len = Norm3(diff);
		e41->index = (int)edges.size()+1;

		e24->face = f241;
		e24->head = v2;
		e24->tail = v4;
		e24->next = e41;
		e24->twin = e42;
		SubVectors3(v2->position,v4->position,diff);
		e24->len = Norm3(diff);
		e24->index = (int)edges.size()+2;

		e42->face = f342;
		e42->head = v4;
		e42->tail = v2;
		e42->next = e23;
		e42->twin = e24;
		SubVectors3(v2->position,v4->position,diff);
		e42->len = Norm3(diff);
		e42->index = (int)edges.size()+3;

		e34->face = f342;
		e34->head = v3;
		e34->tail = v4;
		e34->next = e42;
		e34->twin = e43;
		SubVectors3(v3->position,v4->position,diff);
		e34->len = Norm3(diff);
		e34->index = (int)edges.size()+4;

		e43->face = f143;
		e43->head = v4;
		e43->tail = v3;
		e43->next = e31;
		e43->twin = e34;
		SubVectors3(v3->position,v4->position,diff);
		e43->len = Norm3(diff);
		e43->index = (int)edges.size()+5;

		edges.push_back(e14);
		edges.push_back(e41);
		edges.push_back(e24);
		edges.push_back(e42);
		edges.push_back(e34);
		edges.push_back(e43);

		EdgePair ep1,ep2,ep3;
		ep1.first = e14;
		ep1.second = e41;
		ep2.first = e24;
		ep2.second = e42;
		ep3.first = e34;
		ep3.second = e43;
		edge_pairs.push_back(ep1);
		edge_pairs.push_back(ep2);
		edge_pairs.push_back(ep3);

		// Fill face data
		f143->e1 = e14;
		f143->e2 = e43;
		f143->e3 = e31;
		f143->index = f->index;
		f143->normal[0] = f->normal[0];
		f143->normal[1] = f->normal[1];
		f143->normal[2] = f->normal[2];
		TriCenter3(v1->position,v4->position,v3->position,f143->center);

		f342->e1 = e34;
		f342->e2 = e42;
		f342->e3 = e23;
		f342->index = (int)faces.size();
		f342->normal[0] = f->normal[0];
		f342->normal[1] = f->normal[1];
		f342->normal[2] = f->normal[2];
		TriCenter3(v3->position,v4->position,v2->position,f342->center);

		f241->e1 = e24;
		f241->e2 = e41;
		f241->e3 = e12;
		f241->index = (int)faces.size()+1;
		f241->normal[0] = f->normal[0];
		f241->normal[1] = f->normal[1];
		f241->normal[2] = f->normal[2];
		TriCenter3(v2->position,v4->position,v1->position,f241->center);

		// Update face data
		faces[idx] = f143;// Replace original face 
		faces.push_back(f342);
		faces.push_back(f241);

		// Add new edges to vertices
		v1->edges.push_back(e14);
		v2->edges.push_back(e24);
		v3->edges.push_back(e34);
		v4->edges.push_back(e41);
		v4->edges.push_back(e42);
		v4->edges.push_back(e43);

		// Add new faces to vertices
		size_t fid;
		for(fid = 0;fid < v1->faces.size();fid++)
			if(v1->faces[fid] == f)break;
		v1->faces[fid] = v1->faces[(int)v1->faces.size()-1];
		v1->faces.pop_back();
		v1->faces.push_back(f143);
		v1->faces.push_back(f241);

		for(fid = 0;fid < v2->faces.size();fid++)
			if(v2->faces[fid] == f)break;
		v2->faces[fid] = v2->faces[(int)v2->faces.size()-1];
		v2->faces.pop_back();
		v2->faces.push_back(f241);
		v2->faces.push_back(f342);

		for(fid = 0;fid < v3->faces.size();fid++)
			if(v3->faces[fid] == f)break;
		v3->faces[fid] = v3->faces[(int)v3->faces.size()-1];
		v3->faces.pop_back();
		v3->faces.push_back(f342);
		v3->faces.push_back(f143);

		v4->faces.push_back(f143);
		v4->faces.push_back(f241);
		v4->faces.push_back(f342);

		delete f;
	}

	void Mesh::Root3(Face *f)
	{
		float diff[3];
		int idx = -1;
		for(int i = 0;i < (int)faces.size();i++)
		{
			if(faces[i] == f)
			{
				idx = i;
				break;
			}
		}

		if(idx == -1)return;

		Edge *e12 = f->e1;
		Edge *e23 = f->e2;
		Edge *e31 = f->e3;

		Vert *v1 = e12->head;
		Vert *v2 = e23->head;
		Vert *v3 = e31->head;

		Vert *v4 = _subdiv_NewVertex((int)vertices.size(),f->center,f->normal,NULL);
		v4->index = (int)vertices.size();
		vertices.push_back(v4);

		// New Edges
		Edge *e14 = new Edge(),*e41 = new Edge();
		Edge *e24 = new Edge(),*e42 = new Edge();
		Edge *e34 = new Edge(),*e43 = new Edge();

		// New Faces
		Face *f143 = new Face();
		Face *f342 = new Face();
		Face *f241 = new Face();

		// Fill edge data
		e14->face = f143;
		e14->head = v1;
		e14->tail = v4;
		e14->next = e43;
		e14->twin = e41;
		SubVectors3(v1->position,v4->position,diff);
		e14->len = Norm3(diff);
		e14->index = (int)edges.size();

		e41->face = f241;
		e41->head = v4;
		e41->tail = v1;
		e41->next = e12;
		e41->twin = e14;
		SubVectors3(v1->position,v4->position,diff);
		e41->len = Norm3(diff);
		e41->index = (int)edges.size()+1;

		e24->face = f241;
		e24->head = v2;
		e24->tail = v4;
		e24->next = e41;
		e24->twin = e42;
		SubVectors3(v2->position,v4->position,diff);
		e24->len = Norm3(diff);
		e24->index = (int)edges.size()+2;

		e42->face = f342;
		e42->head = v4;
		e42->tail = v2;
		e42->next = e23;
		e42->twin = e24;
		SubVectors3(v2->position,v4->position,diff);
		e42->len = Norm3(diff);
		e42->index = (int)edges.size()+3;

		e34->face = f342;
		e34->head = v3;
		e34->tail = v4;
		e34->next = e42;
		e34->twin = e43;
		SubVectors3(v3->position,v4->position,diff);
		e34->len = Norm3(diff);
		e34->index = (int)edges.size()+4;

		e43->face = f143;
		e43->head = v4;
		e43->tail = v3;
		e43->next = e31;
		e43->twin = e34;
		SubVectors3(v3->position,v4->position,diff);
		e43->len = Norm3(diff);
		e43->index = (int)edges.size()+5;

		edges.push_back(e14);
		edges.push_back(e41);
		edges.push_back(e24);
		edges.push_back(e42);
		edges.push_back(e34);
		edges.push_back(e43);

		EdgePair ep1,ep2,ep3;
		ep1.first = e14;
		ep1.second = e41;
		ep2.first = e24;
		ep2.second = e42;
		ep3.first = e34;
		ep3.second = e43;
		edge_pairs.push_back(ep1);
		edge_pairs.push_back(ep2);
		edge_pairs.push_back(ep3);

		// Fill face data
		f143->e1 = e14;
		f143->e2 = e43;
		f143->e3 = e31;
		f143->index = f->index;
		f143->normal[0] = f->normal[0];
		f143->normal[1] = f->normal[1];
		f143->normal[2] = f->normal[2];
		TriCenter3(v1->position,v4->position,v3->position,f143->center);

		f342->e1 = e34;
		f342->e2 = e42;
		f342->e3 = e23;
		f342->index = (int)faces.size();
		f342->normal[0] = f->normal[0];
		f342->normal[1] = f->normal[1];
		f342->normal[2] = f->normal[2];
		TriCenter3(v3->position,v4->position,v2->position,f342->center);

		f241->e1 = e24;
		f241->e2 = e41;
		f241->e3 = e12;
		f241->index = (int)faces.size()+1;
		f241->normal[0] = f->normal[0];
		f241->normal[1] = f->normal[1];
		f241->normal[2] = f->normal[2];
		TriCenter3(v2->position,v4->position,v1->position,f241->center);

		// Update face data
		faces[idx] = f143;// Replace original face 
		faces.push_back(f342);
		faces.push_back(f241);

		// Add new edges to vertices
		v1->edges.push_back(e14);
		v2->edges.push_back(e24);
		v3->edges.push_back(e34);
		v4->edges.push_back(e41);
		v4->edges.push_back(e42);
		v4->edges.push_back(e43);

		// Add new faces to vertices
		size_t fid;
		for(fid = 0;fid < v1->faces.size();fid++)
			if(v1->faces[fid] == f)break;
		v1->faces[fid] = v1->faces[(int)v1->faces.size()-1];
		v1->faces.pop_back();
		v1->faces.push_back(f143);
		v1->faces.push_back(f241);

		for(fid = 0;fid < v2->faces.size();fid++)
			if(v2->faces[fid] == f)break;
		v2->faces[fid] = v2->faces[(int)v2->faces.size()-1];
		v2->faces.pop_back();
		v2->faces.push_back(f241);
		v2->faces.push_back(f342);

		for(fid = 0;fid < v3->faces.size();fid++)
			if(v3->faces[fid] == f)break;
		v3->faces[fid] = v3->faces[(int)v3->faces.size()-1];
		v3->faces.pop_back();
		v3->faces.push_back(f342);
		v3->faces.push_back(f143);

		v4->faces.push_back(f143);
		v4->faces.push_back(f241);
		v4->faces.push_back(f342);

		// Update original edges
		e12->face = f241;
		e12->next = e24;
		e23->face = f342;
		e23->next = e34;
		e31->face = f143;
		e31->next = e14;

		delete f;
	}

	void Mesh::CollapseFace(Face *f)
	{
	}

	int Mesh::DrawMesh()
	{
		glBegin(GL_TRIANGLES);

		for (int i = 0; i < faces.size(); i++)
		{
			ComputeFaceNormals();

			GLfloat col[3];
			col[0] = vertices[i]->color[0];
			col[1] = vertices[i]->color[1];
			col[2] = vertices[i]->color[2];

			GLfloat norm[3];
			norm[0] = vertices[i]->normal[0];
			norm[1] = vertices[i]->normal[1];
			norm[2] = vertices[i]->normal[2];

			GLfloat ver[3];
			ver[0] = vertices[i]->position[0];
			ver[1] = vertices[i]->position[1];
			ver[2] = vertices[i]->position[2];

			glColor3f(col[0], 0.0, 0.0);
			glNormal3f(norm[0], 0.0, 0.0);
			glVertex3f(ver[0], 0.0, 0.0);

			glColor3f(0.0, col[1], 0.0);
			glNormal3f(0.0, norm[1], 0.0);
			glVertex3f(0.0, ver[2], 0.0);

			glColor3f( 0.0, 0.0, col[2]);
			glNormal3f(0.0, 0.0, norm[2]);
			glVertex3f(0.0, 0.0, ver[2]);
		}

		glEnd();

		return 0;
	}

	int Mesh::InitMesh()
	{
		numVert = 0;
		numEdges = 0;
		numFaces = 0;
		isDeformable = false;

		for(int i = 0;i < 3;i++)
		{
			CompPair p;
			p.first = 0.0;
			p.second = 0.0;
			bounding_box.push_back(p);
		}

		return 0;
	}

	int Mesh::FromPly(char *file_name)
	{
		FILE *stream;
		int j = 0,i;
		char key[1000];
		int num_lines,size[2];
		float pnt[3];
		float norm[3];
		float col[3];
		int tri[3];

		num_lines = ReadPlyHeader(file_name,size);
		cout<<endl<<"Number of vertices read: "<<size[0]<<endl;
		cout<<"Number of triangles read: "<<size[1]<<endl<<endl;
		numVert = size[0];
		numFaces = size[1];

		if((stream = fopen(file_name,"r")) != NULL)
		{
			while(!feof(stream))
			{
				fgets(key,1000,stream);
				j++;
				if(j == num_lines)
				{
					for(i = 0;i < size[0];i++)
					{
						fgets(key,1000,stream);
						TokenizePlyVertLine(key,pnt,norm,col);
						AddVertToMesh(i,pnt,norm,col);
					}
					for(i = 0;i < size[1];i++)
					{
						fgets(key,1000,stream);
						TokenizePlyFaceLine(key,tri);
						BuildComplexFromFace(tri);
					}
					cerr<<"Building mesh connectivity."<<endl;
					BuildHalfEdgeConnectivity();
					numEdges = edge_pairs.size();
				}
			}
			UpdateEdgeLengths();
			UpdateFaceAreas();
			fclose(stream);
			stream  = NULL;
			cout<<"File reading successful."<<endl;
			return 0;
		}
		else
		{
			cout<<file_name<<" could not be read."<<endl;
			return -1;
		}
	}

	int Mesh::SetField(vector<float> _vals)
	{
		for(size_t i = 0;i < vertices.size();i++)
			vertices[i]->value = _vals[i];
		return 0;
	}

	int Mesh::SetDecimationMethod(int method)
	{
		decimation_method = method;
		return 0;
	}

	int Mesh::SetSmoothingWeight(int smoothing)
	{
		smoothing_weight = smoothing;
		return 0;
	}

	int Mesh::SetFloydWarshallMethod(int method)
	{
		fw_method = method;
		return 0;
	}

    bool Mesh::GetHoles(vector<vector<Tuple3f>> holes)
    {
        // Find boundary edges
        MeshEdges boundary_edges;
        for(int i = 0;i < (int)edges.size();i++)
            if(edges[i]->twin == NULL)
                boundary_edges.push_back(edges[i]);

        // Now compute loops
        // UMEMA
    }

	int Mesh::ComputeVertexNormals()
	{
	//#pragma omp parallel for
		UpdateFaceAreas();
		for(size_t i = 0;i < vertices.size();i++)
			ComputeNormalAtVert((int)i);

		return 0;
	}

	int Mesh::ComputeFaceNormals()
	{
		UpdateFaceAreas();
		return 0;
	}

	int Mesh::LaplacianSmoothing()
	{
        //Points t_pos;
        vector<Tuple3f> t_pos;

		//---- Initialize t_pos
		for(size_t j = 0;j < vertices.size();j++)
		{
            Tuple3f t_p;
            t_p.data[0] = 0.0;
            t_p.data[1] = 0.0;
            t_p.data[2] = 0.0;
			t_pos.push_back(t_p);
		}

		int i;
		int thID,nThrds;
	#pragma omp parallel private(thID, i) shared(t_pos)
		{
			bool isSm;
			float weight,sum;
			thID = omp_get_thread_num();
			if (thID == 0)
				nThrds = omp_get_num_threads();
	#pragma omp for
			for(i = 0;i < vertices.size();i++)
			{
				sum = 0.0;
                t_pos[i].data[0] = 0.0;
                t_pos[i].data[1] = 0.0;
                t_pos[i].data[2] = 0.0;
				for(size_t j = 0;j < vertices[i]->edges.size();j++)
				{
					if(vertices[i]->edges[j] != NULL && vertices[i]->edges[j]->twin != NULL)
					{
						if(smoothing_weight == SMOOTHING_WEIGHTS::UNIFORM)
							weight = 1.0;
						else if(smoothing_weight == SMOOTHING_WEIGHTS::EDGE_LENGTH)
							weight = 1.0/sq(vertices[i]->edges[j]->len);
						else if(smoothing_weight == SMOOTHING_WEIGHTS::COTANGENT)
							weight = CotangentWeights(vertices[i]->edges[j]);
						else if(smoothing_weight == SMOOTHING_WEIGHTS::FIELD)
							weight = vertices[i]->edges[j]->tail->value;

                        t_pos[i].data[0] += weight*vertices[i]->edges[j]->tail->position[0];
                        t_pos[i].data[1] += weight*vertices[i]->edges[j]->tail->position[1];
                        t_pos[i].data[2] += weight*vertices[i]->edges[j]->tail->position[2];
						sum += weight;
						isSm = true;
					}
					else
					{
						isSm = false;
						break;
					}
				}

				if(isSm)
				{
                    t_pos[i].data[0] /= sum;
                    t_pos[i].data[1] /= sum;
                    t_pos[i].data[2] /= sum;
				}
				else
				{
                    t_pos[i].data[0] = vertices[i]->position[0];
                    t_pos[i].data[1] = vertices[i]->position[1];
                    t_pos[i].data[2] = vertices[i]->position[2];
				}
			}
		}
		
		for(size_t j = 0;j < vertices.size();j++)
		{
            vertices[j]->position[0] = t_pos[j].data[0];
            vertices[j]->position[1] = t_pos[j].data[1];
            vertices[j]->position[2] = t_pos[j].data[2];
		}
		t_pos.clear();
		return 0;
	}

	int Mesh::LaplacianFieldSmoothing()
	{
		vector<float> t_val;

		//---- Initialize t_pos
		for(size_t j = 0;j < vertices.size();j++)
			t_val.push_back(0.0);

		int i;
		int thID,nThrds;
	#pragma omp parallel private(thID, i) shared(t_val)
		{
			bool isSm;
			float weight,sum;
			thID = omp_get_thread_num();
			if (thID == 0)
				nThrds = omp_get_num_threads();
	#pragma omp for
			for(i = 0;i < vertices.size();i++)
			{
				sum = 0.0;
				for(size_t j = 0;j < vertices[i]->edges.size();j++)
				{
					if(vertices[i]->edges[j] != NULL && vertices[i]->edges[j]->twin != NULL)
					{
						weight = 1.0;
						t_val[i] += weight*vertices[i]->edges[j]->tail->value;
						sum += weight;
						isSm = true;
					}
					else
					{
						isSm = false;
						break;
					}
				}

				if(isSm)
					t_val[i] /= sum;
				else
					t_val[i] = vertices[i]->value;
			}
		}
		
		for(size_t j = 0;j < vertices.size();j++)
			vertices[j]->value = t_val[j];
		
		t_val.clear();
		return 0;
	}

	int Mesh::SelectiveLaplacianSmoothing()
	{
        vector<Tuple3f> t_pos;
		vector<float> e_len;
		for(size_t i = 0;i < vertices.size();i++)
		{
            Tuple3f t_p;
            t_p.data[0] = 0.0;
            t_p.data[1] = 0.0;
            t_p.data[2] = 0.0;
			float len = 0.0;
			for(size_t j = 0;j < vertices[i]->edges.size();j++)
			{
                t_p.data[0] += vertices[i]->edges[j]->tail->position[0];
                t_p.data[1] += vertices[i]->edges[j]->tail->position[1];
                t_p.data[2] += vertices[i]->edges[j]->tail->position[2];
				len += vertices[i]->edges[j]->len;
			}
            t_p.data[0] /= (float)vertices[i]->edges.size();
            t_p.data[1] /= (float)vertices[i]->edges.size();
            t_p.data[2] /= (float)vertices[i]->edges.size();
			len /= (float)vertices[i]->edges.size();
			t_pos.push_back(t_p);
			e_len.push_back(len);
		}
		float diff[3];
		for(size_t i = 0;i < vertices.size();i++)
		{
            diff[0] = vertices[i]->position[0] - t_pos[i].data[0];
            diff[1] = vertices[i]->position[1] - t_pos[i].data[1];
            diff[2] = vertices[i]->position[2] - t_pos[i].data[2];
			if(Norm3(diff) > 0.5*e_len[i])
			{
                vertices[i]->position[0] = t_pos[i].data[0];
                vertices[i]->position[1] = t_pos[i].data[1];
                vertices[i]->position[2] = t_pos[i].data[2];
			}
		}
		t_pos.clear();
		e_len.clear();
		return 0;
	}

    int Mesh::ComputeNearestVertex(float *p,size_t &idx,float &minDIST)
{
    float line[3],d;
    idx = 0;

            line[0] = p[0]-vertices[0]->position[0];
            line[1] = p[1]-vertices[1]->position[1];
            line[2] = p[2]-vertices[2]->position[2];
    minDIST = Norm3(line);
    for(size_t i = 1;i < vertices.size();i++)
    {
                    line[0] = p[0]-vertices[i]->position[0];
                    line[1] = p[1]-vertices[i]->position[1];
                    line[2] = p[2]-vertices[i]->position[2];

        d = Norm3(line);
        if(d < minDIST){minDIST = d;idx = i;}
    }

    return 0;
}

    int Mesh::ComputeFarthestVertex(float *p,size_t &idx,float &maxDIST)
{
    float line[3],d;
    idx = 0;

            line[0] = p[0]-vertices[0]->position[0];
            line[1] = p[1]-vertices[1]->position[1];
            line[2] = p[2]-vertices[2]->position[2];
    maxDIST = Norm3(line);
    for(size_t i = 1;i < vertices.size();i++)
    {
                    line[0] = p[0]-vertices[i]->position[0];
                    line[1] = p[1]-vertices[i]->position[1];
                    line[2] = p[2]-vertices[i]->position[2];

        d = Norm3(line);
        if(d > maxDIST){maxDIST = d;idx = i;}
    }

    return 0;
}

    int Mesh::ComputeDistanceFromPoint(float *p,size_t *min_max_idx,float *min_maxDIST,int norm_flag)
{
    float line[3];
    vector<float> temp;
    dist_vec.clear();
    if(norm_flag == DIST_NORMALIZE)
    {
        for(size_t i = 0;i < vertices.size();i++)
        {
                            line[0] = p[0]-vertices[i]->position[0];
                            line[1] = p[1]-vertices[i]->position[1];
                            line[2] = p[2]-vertices[i]->position[2];

            float d = Norm3(line);
            temp.push_back(d);
        }
        NormalizeRange(temp,dist_vec,min_max_idx,min_maxDIST);
    }
    else
    {
        for(size_t i = 0;i < vertices.size();i++)
        {
                            line[0] = p[0]-vertices[i]->position[0];
                            line[1] = p[1]-vertices[i]->position[1];
                            line[2] = p[2]-vertices[i]->position[2];

            float d = Norm3(line);
            dist_vec.push_back(d);
        }
    }

    temp.clear();
    return 0;
}

    int Mesh::ComputeDistanceFromPoint(float *p,vector<float> &d_list,size_t *min_max_idx,float *min_maxDIST,int norm_flag)
    {
        float line[3];
        d_list.clear();
        if(norm_flag == DIST_NORMALIZE)
        {
            vector<float> temp;

            min_max_idx[0] = 0;
            min_max_idx[1] = 0;

                        line[0] = p[0]-vertices[0]->position[0];
                        line[1] = p[1]-vertices[0]->position[1];
                        line[2] = p[2]-vertices[0]->position[2];

            float d0 = Norm3(line);
            min_maxDIST[0] = d0;
            min_maxDIST[1] = d0;

            temp.push_back(d0);

            for(size_t i = 0;i < vertices.size();i++)
            {
                                line[0] = p[0]-vertices[i]->position[0];
                                line[1] = p[1]-vertices[i]->position[1];
                                line[2] = p[2]-vertices[i]->position[2];

                float d = Norm3(line);

                if(d < min_maxDIST[0]){min_maxDIST[0] = d;min_max_idx[0] = i;}
                if(d > min_maxDIST[1]){min_maxDIST[1] = d;min_max_idx[1] = i;}

                temp.push_back(d);
            }
            NormalizeRange(temp,d_list,NULL,NULL);
            temp.clear();
        }
        else if(norm_flag == DIST_NORMINV)
        {
            vector<float> temp;
            for(size_t i = 0;i < vertices.size();i++)
            {
                                line[0] = p[0]-vertices[i]->position[0];
                                line[1] = p[1]-vertices[i]->position[1];
                                line[2] = p[2]-vertices[i]->position[2];

                float d = Norm3(line);

                if(d < min_maxDIST[0]){min_maxDIST[0] = d;min_max_idx[0] = i;}
                if(d > min_maxDIST[1]){min_maxDIST[1] = d;min_max_idx[1] = i;}

                temp.push_back(d);
            }
            NormalizeRange(temp,d_list,NULL,NULL);
            for(size_t i = 0;i < d_list.size();i++)
                d_list[i] = 1.0-d_list[i];
            temp.clear();
        }
        else if(norm_flag == DIST_STANDARD)
        {
            vector<float> temp;

            min_max_idx[0] = 0;
            min_max_idx[1] = 0;

                        line[0] = p[0]-vertices[0]->position[0];
                        line[1] = p[1]-vertices[0]->position[1];
                        line[2] = p[2]-vertices[0]->position[2];

            float d0 = Norm3(line);
            min_maxDIST[0] = d0;
            min_maxDIST[1] = d0;

            temp.push_back(d0);

            for(size_t i = 1;i < vertices.size();i++)
            {
                                line[0] = p[0]-vertices[i]->position[0];
                                line[1] = p[1]-vertices[i]->position[1];
                                line[2] = p[2]-vertices[i]->position[2];

                float d = Norm3(line);

                if(d < min_maxDIST[0]){min_maxDIST[0] = d;min_max_idx[0] = i;}
                if(d > min_maxDIST[1]){min_maxDIST[1] = d;min_max_idx[1] = i;}

                temp.push_back(d);
            }
            float ms[2];
            StandardScore(temp,d_list,ms);
            temp.clear();
        }
        else if(norm_flag == DIST_EXPMEANDIFF)
        {
            vector<float> temp;

            min_max_idx[0] = 0;
            min_max_idx[1] = 0;

                        line[0] = p[0]-vertices[0]->position[0];
                        line[1] = p[1]-vertices[0]->position[1];
                        line[2] = p[2]-vertices[0]->position[2];

            float d0 = Norm3(line);
            min_maxDIST[0] = d0;
            min_maxDIST[1] = d0;

            temp.push_back(d0);

            for(size_t i = 1;i < vertices.size();i++)
            {
                                line[0] = p[0]-vertices[i]->position[0];
                                line[1] = p[1]-vertices[i]->position[1];
                                line[2] = p[2]-vertices[i]->position[2];

                float d = Norm3(line);

                if(d < min_maxDIST[0]){min_maxDIST[0] = d;min_max_idx[0] = i;}
                if(d > min_maxDIST[1]){min_maxDIST[1] = d;min_max_idx[1] = i;}

                temp.push_back(d);
            }
            MeanDifference(temp,d_list);
            temp.clear();
        }
        else if(norm_flag == DIST_EXPNEG)
        {
            vector<float> temp;
            float rng[] = {-1.0,1.0};
            min_max_idx[0] = 0;
            min_max_idx[1] = 0;

                        line[0] = p[0]-vertices[0]->position[0];
                        line[1] = p[1]-vertices[0]->position[1];
                        line[2] = p[2]-vertices[0]->position[2];

            float d0 = Norm3(line);
            min_maxDIST[0] = d0;
            min_maxDIST[1] = d0;

            d0 = std::expf(-1.0*d0*d0);
            temp.push_back(d0);

            for(size_t i = 1;i < vertices.size();i++)
            {
                                line[0] = p[0]-vertices[i]->position[0];
                                line[1] = p[1]-vertices[i]->position[1];
                                line[2] = p[2]-vertices[i]->position[2];

                float d = Norm3(line);

                if(d < min_maxDIST[0]){min_maxDIST[0] = d;min_max_idx[0] = i;}
                if(d > min_maxDIST[1]){min_maxDIST[1] = d;min_max_idx[1] = i;}

                d = std::expf(-1.0*d*d);
                temp.push_back(d);
            }
            NormalizeRange(rng,temp,d_list,NULL,NULL);
            temp.clear();
        }
        else if(norm_flag == DIST_EXP)
        {
            min_max_idx[0] = 0;
            min_max_idx[1] = 0;

                        line[0] = p[0]-vertices[0]->position[0];
                        line[1] = p[1]-vertices[0]->position[1];
                        line[2] = p[2]-vertices[0]->position[2];

            float d0 = Norm3(line);
            min_maxDIST[0] = d0;
            min_maxDIST[1] = d0;

            d0 = std::expf(-1.0*d0*d0);
            d_list.push_back(d0);

            for(size_t i = 1;i < vertices.size();i++)
            {
                                line[0] = p[0]-vertices[i]->position[0];
                                line[1] = p[1]-vertices[i]->position[1];
                                line[2] = p[2]-vertices[i]->position[2];

                float d = Norm3(line);

                if(d < min_maxDIST[0]){min_maxDIST[0] = d;min_max_idx[0] = i;}
                if(d > min_maxDIST[1]){min_maxDIST[1] = d;min_max_idx[1] = i;}

                d = std::expf(-1.0*d*d);
                d_list.push_back(d);
            }
        }
        else if(norm_flag == DIST_EXPNORM)
        {
            vector<float> temp;

            min_max_idx[0] = 0;
            min_max_idx[1] = 0;

                        line[0] = p[0]-vertices[0]->position[0];
                        line[1] = p[1]-vertices[0]->position[1];
                        line[2] = p[2]-vertices[0]->position[2];

            float d0 = Norm3(line);
            min_maxDIST[0] = d0;
            min_maxDIST[1] = d0;

            d0 = std::expf(-1.0*d0*d0);
            temp.push_back(d0);

            for(size_t i = 1;i < vertices.size();i++)
            {
                                line[0] = p[0]-vertices[i]->position[0];
                                line[1] = p[1]-vertices[i]->position[1];
                                line[2] = p[2]-vertices[i]->position[2];

                float d = Norm3(line);

                if(d < min_maxDIST[0]){min_maxDIST[0] = d;min_max_idx[0] = i;}
                if(d > min_maxDIST[1]){min_maxDIST[1] = d;min_max_idx[1] = i;}

                d = std::expf(-1.0*d*d);
                temp.push_back(d);
            }
            NormalizeRange(temp,d_list,NULL,NULL);
            temp.clear();
        }
        else if(norm_flag == DIST_CAUCHY || norm_flag == DIST_CAUCHYLAP)
        {
            min_max_idx[0] = 0;
            min_max_idx[1] = 0;

                        line[0] = p[0]-vertices[0]->position[0];
                        line[1] = p[1]-vertices[0]->position[1];
                        line[2] = p[2]-vertices[0]->position[2];

            float d0 = NormSq3(line);
            min_maxDIST[0] = d0;
            min_maxDIST[1] = d0;

            if(norm_flag == DIST_CAUCHYLAP)
                d0 = (1.00-d0)/sq((sq(1.00+sq(d0))));
            else d0 = 1.00/(sq(1.00+sq(d0)));
            d_list.push_back(d0);

            for(size_t i = 1;i < vertices.size();i++)
            {
                                line[0] = p[0]-vertices[i]->position[0];
                                line[1] = p[1]-vertices[i]->position[1];
                                line[2] = p[2]-vertices[i]->position[2];

                float d = NormSq3(line);

                if(d < min_maxDIST[0]){min_maxDIST[0] = d;min_max_idx[0] = i;}
                if(d > min_maxDIST[1]){min_maxDIST[1] = d;min_max_idx[1] = i;}

                if(norm_flag == DIST_CAUCHYLAP)
                    d = (1.00-d)/sq((sq(1.00+sq(d))));
                else d = 1.0/(sq(1.00+sq(d)));
                d_list.push_back(d);
            }
        }
        else
        {
            for(size_t i = 0;i < vertices.size();i++)
            {
                                line[0] = p[0]-vertices[i]->position[0];
                                line[1] = p[1]-vertices[i]->position[1];
                                line[2] = p[2]-vertices[i]->position[2];

                float d = Norm3(line);
                d_list.push_back(d);
            }
        }
        return 0;
    }

	int Mesh::ComputeDistanceMatrix()
	{
		if(distance_matrix.empty())
		{
			for(size_t i = 0;i < vertices.size();i++)
			{
				vector<float> row;
				vertices[i]->flag = false;
				for(size_t j = 0;j < vertices.size();j++)
					row.push_back(0.0);
				distance_matrix.push_back(row);
			}
		}

		int i;
		int thID,nThrds;
	#pragma omp parallel private(thID, i)
		{
			float per = 0.0;
			float line[3],d;
			float sz = (float)vertices.size();
			thID = omp_get_thread_num();
			if (thID == 0)
				nThrds = omp_get_num_threads();
	#pragma omp for
			for(i = 0;i < vertices.size();i++)
			{
				for(size_t j = 0;j < vertices.size();j++)
				{
					if(i != j && std::fabsf(distance_matrix[i][j]) < 1.0e-6)
					{
						line[0] = vertices[i]->position[0]-vertices[j]->position[0];
						line[1] = vertices[i]->position[1]-vertices[j]->position[1];
						line[2] = vertices[i]->position[2]-vertices[j]->position[2];
						
						if(fw_method == FW_EXPONENTIAL || fw_method == FW_EXPNORM || fw_method == FW_EXPMEANDIFF)
						{
							d = Norm3(line);
							distance_matrix[i][j] = std::expf(-1.0*d*d);
							distance_matrix[j][i] = distance_matrix[i][j];
						}
						else
						{
							distance_matrix[i][j] = Norm3(line);
							distance_matrix[j][i] = distance_matrix[i][j];
						}					
					}
				}
			}
		}

		for(size_t r = 0;r < vertices.size();r++)
		{
			if(fw_method == FW_NORMALIZED || fw_method == FW_EXPNORM  || fw_method == FW_EXPNORMMEANDIFF)
				NormalizeRange(distance_matrix[r],NULL,NULL);
			else if(fw_method == FW_EXPMEANDIFF || fw_method == FW_EXPNORM)
				MeanDifference(distance_matrix[r]);
		}
		return 0;
	}

	int Mesh::RunFloydWarshall()
	{
		UpdateEdgeLengths();
		if(distance_matrix.empty())
		{
			for(size_t i = 0;i < vertices.size();i++)
			{
				vector<float> row;
				for(size_t j = 0;j < vertices.size();j++)
				{
					float _d = FLT_MAX;
					row.push_back(_d);
				}
				distance_matrix.push_back(row);
			}
		}

		for(size_t i = 0;i < vertices.size();i++)
			distance_matrix[i][i] = 0.0;

		for(size_t i = 0;i < edge_pairs.size();i++)
		{
			distance_matrix[edge_pairs[i].first->head->index][edge_pairs[i].second->head->index] = edge_pairs[i].first->len;
			distance_matrix[edge_pairs[i].second->head->index][edge_pairs[i].first->head->index] = edge_pairs[i].second->len;
		}

		int k;
		int thID,nThrds;
	#pragma omp parallel private(thID, k)
		{
			float per = 0.0;
			thID = omp_get_thread_num();
			if (thID == 0)
				nThrds = omp_get_num_threads();
	#pragma omp for
			for(k = 0;k < vertices.size();k++)
			{
				for(size_t i = 0;i < vertices.size();i++)
				{
					for(size_t j = 0;j < vertices.size();j++)
					{
						if(distance_matrix[i][k] + distance_matrix[k][j] < distance_matrix[i][j])
							distance_matrix[i][j] = distance_matrix[i][k] + distance_matrix[k][j];

						per += 1.0;
						printf("Progress: %f\r",per/(float)vertices.size());
					}
				}
			}
		}

		int r;
	#pragma omp parallel private(thID, r)
		{
			thID = omp_get_thread_num();
			if (thID == 0)
				nThrds = omp_get_num_threads();
	#pragma omp for
			for(r = 0;r < vertices.size();r++)
			{
				if(fw_method == FW_EXPONENTIAL || fw_method == FW_EXPNORM || fw_method == FW_EXPMEANDIFF)
					for(size_t c = 0;c < vertices.size();c++)
						distance_matrix[r][c] = std::expf(-1.0*distance_matrix[r][c]*distance_matrix[r][c]);

				if(fw_method == FW_NORMALIZED || fw_method == FW_EXPNORM)
					NormalizeRange(distance_matrix[r],NULL,NULL);
				else if(fw_method == FW_EXPMEANDIFF)
					MeanDifference(distance_matrix[r]);
			}
		}
				
		return 0;
	}

	int Mesh::Subdivide()
	{
		vector<EdgePair> rootThreePairs;
		for(size_t i = 0;i < edge_pairs.size();i++)
		{
			EdgePair _p;
			_p.first = edge_pairs[i].first;
			_p.second = edge_pairs[i].second;
			rootThreePairs.push_back(_p);
		}
		vector<Face*> inROI;
		for(int i = 0;i < (int)faces.size();i++)
		{
			inROI.push_back(faces[i]);
		}
		
		for(size_t i = 0;i < inROI.size();i++)
		{
			Face *f = inROI[i];
			inROI[i] = NULL;
			Root3(f);
		}
		for(size_t i = 0;i < rootThreePairs.size();i++)
			SwapEdge(rootThreePairs[i]);

		UpdateFaceAreas();
		ComputeVertexNormals();
		rootThreePairs.clear();
		inROI.clear();
		return 0;
	}
	
	int Mesh::Decimate()
	{
		// TODO
		return 0;
	}

	// Haker et al. "Conformal Surface Parameterization for Texture Mapping"
	bool Mesh::ComputeConformalParametrization()
	{
		// Choose random triangle on mesh
		int tid = RandomInt(0,faces.size());
		float A[3],B[3],C[3],E[3];
		float CA[3],BA[3],CE[3];
		float theta;
		int idxA,idxB,idxC;

		A[0] = faces[tid]->e1->head->position[0];
		A[1] = faces[tid]->e1->head->position[1];
		A[2] = faces[tid]->e1->head->position[2];
		idxA = faces[tid]->e1->head->index;

		B[0] = faces[tid]->e2->head->position[0];
		B[1] = faces[tid]->e2->head->position[1];
		B[2] = faces[tid]->e2->head->position[2];
		idxB = faces[tid]->e2->head->index;

		C[0] = faces[tid]->e3->head->position[0];
		C[1] = faces[tid]->e3->head->position[1];
		C[2] = faces[tid]->e3->head->position[2];
		idxC = faces[tid]->e3->head->index;

        SubVectors3(C,A,CA);
        SubVectors3(B,A,BA);

        theta = Dot3(CA,BA)/NormSq3(BA);

		E[0] = A[0] + theta*BA[0];
		E[1] = A[1] + theta*BA[1];
		E[2] = A[2] + theta*BA[2];

        SubVectors3(C,E,CE);

		// Inirialize the RHS
		vector<float> a,b;
		float ce = Norm3(CE);
		float ba = Norm3(BA);
		ba = 1.0/ba;
		ce = 1.0/ce;
		for(size_t i = 0;i < vertices.size();i++)
		{
			if(i == idxA)
			{
				a.push_back(-ba);
				b.push_back(-(1.0-theta)*ce);
				//cerr<<-ba<<"  "<<-(1.0-theta)*ce<<endl;
			}
			else if(i == idxB)
			{
				a.push_back(ba);
				b.push_back(-theta*ce);
				//cerr<<ba<<"  "<<-theta*ce<<endl;
			}
			else if(i == idxC)
			{
				a.push_back(0.0);
				b.push_back(ce);
				//cerr<<0.0<<"  "<<ce<<endl;
			}
			else
			{
				a.push_back(0.0);
				b.push_back(0.0);
			}
		}

		// Initialize Seed solution
		vector<float> x,y,tx,ty;
		for(size_t i = 0;i < vertices.size();i++)
		{
			x.push_back(1.0);
			y.push_back(1.0);
			tx.push_back(0.0);
			ty.push_back(0.0);
		}

		// Compute cotan weights for all edges
		vector<float> w;
		for(size_t i = 0;i < edge_pairs.size();i++)
			w.push_back(0.0);

		for(size_t i = 0;i < edge_pairs.size();i++)
			w[edge_pairs[i].first->index] = CotangentWeights(edge_pairs[i].first);

		// Solve Dx = a and Dy = b using Jacobi Iteration
		bool converged = false;
		int count_iteration = 0;
		float tw,sumx,sumy,errx,erry;

		while(!converged && count_iteration < 2000)
		{
			//Iteration
			int ii;
			int thID,nThrds;
	#pragma omp parallel private(thID, ii) shared(x,y,tx,ty,w,tw,sumx,sumy,errx,erry)
			{
						thID = omp_get_thread_num();
						if (thID == 0)
							nThrds = omp_get_num_threads();
	#pragma omp for
					for(ii = 0;ii < vertices.size();ii++)
					{
						tw = 0.0;
						sumx = 0.0;
						sumy = 0.0;
						for(size_t j = 0;j < vertices[ii]->edges.size();j++)
						{
							sumx += w[vertices[ii]->edges[j]->index]*x[vertices[ii]->edges[j]->tail->index];
							sumy += w[vertices[ii]->edges[j]->index]*y[vertices[ii]->edges[j]->tail->index];
							tw -= w[vertices[ii]->edges[j]->index];
						}
						tx[ii] = (a[ii]-sumx)/tw;
						ty[ii] = (b[ii]-sumy)/tw;
					}
			}
			//Error
			errx = 0.0;
			erry = 0.0;
			for(size_t i = 0;i < vertices.size();i++)
			{
				errx += sq(x[i]-tx[i]);
				erry += sq(y[i]-ty[i]);
				x[i] = tx[i];
				y[i] = ty[i];
			}
			errx = std::sqrtf(errx);
			erry = std::sqrtf(erry);

			/*
			for(size_t i = 0;i < vertices.size();i++)
			{
				sumx = 0.0;
				sumy = 0.0;
				tw = 0.0;
				for(size_t j = 0;j < vertices[i]->edges.size();j++)
				{
					sumx += w[vertices[i]->edges[j]->index]*x[vertices[i]->edges[j]->tail->index];
					sumy += w[vertices[i]->edges[j]->index]*y[vertices[i]->edges[j]->tail->index];
					tw -= w[vertices[i]->edges[j]->index];
				}
				errx += sq(a[i]-sumx-tw*x[i]);
				erry += sq(b[i]-sumy-tw*y[i]);
			}*/

			if(errx < 1.0e-4 || erry < 1.0e-4)
				converged = true;

			count_iteration++;
		}

		cerr<<"JACOBI: Number of iterations: "<<count_iteration<<endl;
		cerr<<"Error (X-vector): "<<errx<<endl;
		cerr<<"Error (Y-vector): "<<erry<<endl;

		// Compute conformal mapping
		float cart[3],sph[3];
		float rr;
		for(size_t i = 0;i < vertices.size();i++)
		{
			rr = sq(x[i]) + sq(y[i]);
			cart[0] = 2*x[i]/(1.0 + rr);
			cart[1] = 2*y[i]/(1.0 + rr);
			cart[2] = (2*rr/(1.0+rr)) - 1.0;

			//cerr<<x[i]<<"  "<<y[i]<<" -> "<<cart[0]<<"  "<<cart[1]<<"  "<<cart[2]<<endl;

			/*cerr<<std::fabsf(cart[0]-vertices[i]->position[0])<<"  "
				<<std::fabsf(cart[1]-vertices[i]->position[1])<<"  "
				<<std::fabsf(cart[2]-vertices[i]->position[2])<<endl;*/

			Cart2Sph3(cart,sph);

			CompPair _mp;
			_mp.first = sph[0]/PI;
			_mp.second = sph[1]/(2.0*PI);
			gMap.push_back(_mp);
		}

		x.clear();
		y.clear();
		tx.clear();
		ty.clear();
		w.clear();
		a.clear();
		b.clear();

		return true;
	}

	int Mesh::ComputeCentroid()
	{
		centroid[0] = 0.0;
		centroid[1] = 0.0;
		centroid[2] = 0.0;

		for(size_t i = 0;i < vertices.size();i++)
		{
			centroid[0] += vertices[i]->position[0];
			centroid[1] += vertices[i]->position[1];
			centroid[2] += vertices[i]->position[2];
		}
		centroid[0] /= (float)numVert;
		centroid[1] /= (float)numVert;
		centroid[2] /= (float)numVert;

		return 0;
	}

	int Mesh::ComputeBoundingBox()
	{
		bounding_box[0].first = vertices[0]->position[0];
		bounding_box[0].second = vertices[0]->position[0];
		bounding_box[1].first = vertices[0]->position[1];
		bounding_box[1].second = vertices[0]->position[1];
		bounding_box[2].first = vertices[0]->position[2];
		bounding_box[2].second = vertices[0]->position[2];

		for(size_t i = 1;i < vertices.size();i++)
		{
			if(vertices[i]->position[0] < bounding_box[0].first)
				bounding_box[0].first = vertices[i]->position[0];
			if(vertices[i]->position[0] > bounding_box[0].second)
				bounding_box[0].second = vertices[i]->position[0];

			if(vertices[i]->position[1] < bounding_box[1].first)
				bounding_box[1].first = vertices[i]->position[1];
			if(vertices[i]->position[1] > bounding_box[1].second)
				bounding_box[1].second = vertices[i]->position[1];

			if(vertices[i]->position[2] < bounding_box[2].first)
				bounding_box[2].first = vertices[i]->position[2];
			if(vertices[i]->position[2] > bounding_box[2].second)
				bounding_box[2].second = vertices[i]->position[2];
		}

		return 0;
	}

	int Mesh::TranslateToOrigin()
	{
		ComputeCentroid();
		for(size_t i = 0;i < vertices.size();i++)
		{
			cerr << "CentroidZ" << centroid[2];
			vertices[i]->position[0] -= centroid[0];
			vertices[i]->position[1] -= centroid[1];
			vertices[i]->position[2] -= centroid[2];
		}
		return 0;
	}

	int Mesh::ScaleToUnitBox()
	{
		ComputeBoundingBox();
		float len[]={bounding_box[0].second-bounding_box[0].first
			,bounding_box[1].second-bounding_box[1].first
			,bounding_box[2].second-bounding_box[2].first};

		cerr<<"Box Dimensions: "<<len[0]<<"  "<<len[1]<<"  "<<len[2]<<endl;

		float scl;

		if(len[0] > len[1] && len[0] > len[1])
			scl = 1.0/len[0];
		else if(len[1] > len[0] && len[1] > len[2])
			scl = 1.0/len[1];
		else scl = 1.0/len[2];

		if(_isnan(scl) || !_finite(scl))scl = 1.0;

		cerr<<"Mesh scaled by a factor of "<<scl<<endl;
		for(size_t i = 0;i < vertices.size();i++)
            ScaleVector3(vertices[i]->position,scl);

		return 0;
	}

	int Mesh::TranslateBy(float *xyz)
	{
		for(size_t i = 0;i < vertices.size();i++)
		{
			vertices[i]->position[0] += xyz[0];
			vertices[i]->position[1] += xyz[1];
			vertices[i]->position[2] += xyz[2];
		}
		return 0;
	}

	int Mesh::TranslateTo(float *xyz)
	{
		for(size_t i = 0;i < vertices.size();i++)
		{
			vertices[i]->position[0] += xyz[0]-centroid[0];
			vertices[i]->position[1] += xyz[1]-centroid[1];
			vertices[i]->position[2] += xyz[2]-centroid[2];
		}
		return 0;
	}

	int Mesh::Rotate(float angle,float *axis)
	{
		float matrix[9];
		float t_v[3];
		AxisAngle(axis,angle,matrix);
		for(size_t i = 0;i < vertices.size();i++)
		{
			RotateVec3(vertices[i]->position,matrix,t_v);
			vertices[i]->position[0] = t_v[0];
			vertices[i]->position[1] = t_v[1];
			vertices[i]->position[2] = t_v[2];
			RotateVec3(vertices[i]->normal,matrix,t_v);
            vertices[i]->normal[0] = t_v[0];
            vertices[i]->normal[1] = t_v[1];
            vertices[i]->normal[2] = t_v[2];
            Normalize3(vertices[i]->normal);
		}
		UpdateFaceAreas();
		return 0;
	}

	int Mesh::Scale(float sc_x,float sc_y,float sc_z)
	{
		for(size_t i = 0;i < vertices.size();i++)
		{
			vertices[i]->position[0] *= sc_x;
			vertices[i]->position[1] *= sc_y;
			vertices[i]->position[2] *= sc_z;
		}
		UpdateFaceAreas();
		return 0;
	}

	int Mesh::GetNumVert()
	{
		return (int)vertices.size();
	}

	int Mesh::GetNumEdges()
	{
		return (int)edge_pairs.size();
	}

	int Mesh::GetNumFaces()
	{
		return (int)faces.size();
	}

	int Mesh::GetCentroid(float *cen)
	{
		cen[0] = centroid[0];
		cen[1] = centroid[1];
		cen[2] = centroid[2];
		return 0;
	}

	int Mesh::GetBBox(float *xrng,float *yrng,float *zrng)
	{
		xrng[0] = bounding_box[0].first;
		xrng[1] = bounding_box[0].second;

		yrng[0] = bounding_box[1].first;
		yrng[1] = bounding_box[1].second;

		zrng[0] = bounding_box[1].first;
		zrng[1] = bounding_box[1].second;
		return 0;
	}

	int Mesh::GetVertPosition(int vert_id,float *p)
	{
		p[0] = vertices[vert_id]->position[0];
		p[1] = vertices[vert_id]->position[1];
		p[2] = vertices[vert_id]->position[2];
		return 0;
	}
	int Mesh::SetVertPosition(float *p, int vert_id)
	{
		vertices[vert_id]->position[0] = p[0];
		vertices[vert_id]->position[1] = p[1];
		vertices[vert_id]->position[2] = p[2];
		return 0;
	}

	int Mesh::GetVertNormal(int vert_id,float *n)
	{
		n[0] = vertices[vert_id]->normal[0];
		n[1] = vertices[vert_id]->normal[1];
		n[2] = vertices[vert_id]->normal[2];
		return 0;
	}

	bool Mesh::GetVertFlag(int vert_id)
	{
		return vertices[vert_id]->flag;
	}

	int Mesh::GetFaceNormal(int face_id,float *n)
	{
		n[0] = faces[face_id]->normal[0];
		n[1] = faces[face_id]->normal[1];
		n[2] = faces[face_id]->normal[2];
		return 0;
	}

	int Mesh::GetFaceCenter(int face_id,float *c)
	{
		c[0] = faces[face_id]->center[0];
		c[1] = faces[face_id]->center[1];
		c[2] = faces[face_id]->center[2];
		return 0;
	}

	bool Mesh::GetFaceFlag(int face_id)
	{
		if(vertices[faces[face_id]->e1->head->index]->flag || 
			vertices[faces[face_id]->e2->head->index]->flag ||
			vertices[faces[face_id]->e3->head->index]->flag)
			return true;
		else return false;
	}

	int Mesh::GetVertFrame(int vert_id,Frame *fr)
	{
		fr->u[0] = vertices[vert_id]->frame.u[0];
		fr->u[1] = vertices[vert_id]->frame.u[1];
		fr->u[2] = vertices[vert_id]->frame.u[2];

		fr->v[0] = vertices[vert_id]->frame.v[0];
		fr->v[1] = vertices[vert_id]->frame.v[1];
		fr->v[2] = vertices[vert_id]->frame.v[2];

		fr->w[0] = vertices[vert_id]->frame.w[0];
		fr->w[1] = vertices[vert_id]->frame.w[1];
		fr->w[2] = vertices[vert_id]->frame.w[2];

		return 0;
	}

	int Mesh::GetVertColor(int vert_id,float *c)
	{
		c[0] = vertices[vert_id]->color[0];
		c[1] = vertices[vert_id]->color[1];
		c[2] = vertices[vert_id]->color[2];
		return 0;
	}

	float Mesh::GetVertValue(int vert_id)
	{
		return vertices[vert_id]->value;
	}

	float Mesh::GetVertGraphDistance(int src_id,int vert_id)
	{
		return distance_matrix[src_id][vert_id];
	}

	int Mesh::GetVertsForFace(int face_id,float *p1,float *p2,float *p3)
	{
		if(faces[face_id] != NULL && faces[face_id]->e1 != NULL && faces[face_id]->e2 != NULL && faces[face_id]->e3 != NULL)
		{
			GetVertPosition(faces[face_id]->e1->head->index,p1);
			GetVertPosition(faces[face_id]->e2->head->index,p2);
			GetVertPosition(faces[face_id]->e3->head->index,p3);
		}
		return 0;
	}

	int Mesh::GetVertNormalsForFace(int face_id,float *n1,float *n2,float *n3)
	{
		GetVertNormal(faces[face_id]->e1->head->index,n1);
		GetVertNormal(faces[face_id]->e2->head->index,n2);
		GetVertNormal(faces[face_id]->e3->head->index,n3);
		return 0;
	}

    int Mesh::GetVertColorsForFace(int face_id,float *c1,float *c2,float *c3)
    {
        GetVertColor(faces[face_id]->e1->head->index,c1);
        GetVertColor(faces[face_id]->e2->head->index,c2);
        GetVertColor(faces[face_id]->e3->head->index,c3);
        return 0;
    }

	int Mesh::GetVertValuesForFace(int face_id,float *vals)
	{
		vals[0] = GetVertValue(faces[face_id]->e1->head->index);
		vals[1] = GetVertValue(faces[face_id]->e2->head->index);
		vals[2] = GetVertValue(faces[face_id]->e3->head->index);
		return 0;
	}

	int Mesh::GetVertGraphDistanceForFace(int src_id,int face_id,float *vals)
	{
		vals[0] = GetVertGraphDistance(src_id,faces[face_id]->e1->head->index);
		vals[1] = GetVertGraphDistance(src_id,faces[face_id]->e2->head->index);
		vals[2] = GetVertGraphDistance(src_id,faces[face_id]->e3->head->index);
		return 0;
	}

	int Mesh::GetVertIdxForFace(int face_id,int *idx)
	{
		idx[0] = faces[face_id]->e1->head->index;
		idx[1] = faces[face_id]->e2->head->index;
		idx[2] = faces[face_id]->e3->head->index;
		return 0;
	}

	int Mesh::GetNumNeighborsForVert(int vert_id)
	{
		return vertices[vert_id]->edges.size();
	}

	int Mesh::GetNumFacesOnVert(int vert_id)
	{
		return vertices[vert_id]->faces.size();
	}

	int Mesh::GetVertNeighbors(int vert_id,vector<Tuple3f> &neighbors)
	{
		if(vert_id > numVert)
			return -1;
		
		neighbors.clear();

		for(size_t i = 0;i < vertices[vert_id]->edges.size();i++)
		{
			Tuple3f nb;
			nb.data[0] = vertices[vert_id]->edges[i]->tail->position[0];
			nb.data[1] = vertices[vert_id]->edges[i]->tail->position[1];
			nb.data[2] = vertices[vert_id]->edges[i]->tail->position[2];
			neighbors.push_back(nb);
		}
		return (int)vertices[vert_id]->edges.size();
	}

	int Mesh::GetFaceNhood(int face_id,int *f1,int *f2,int *f3)
	{
		if(face_id > numFaces)
		{
			f1[0] = f1[1] = f1[2] = -1;
			f2[0] = f2[1] = f2[2] = -1;
			f3[0] = f3[1] = f3[2] = -1;
			return -1;
		}

		int count = 0;
		Face *t_f1,*t_f2,*t_f3;
		t_f1 = NULL;
		t_f2 = NULL;
		t_f3 = NULL;

		if(faces[face_id]->e1->twin != NULL)
			t_f1 = faces[face_id]->e1->twin->face;

		if(faces[face_id]->e2->twin != NULL)
			t_f2 = faces[face_id]->e2->twin->face;

		if(faces[face_id]->e3->twin != NULL)
			t_f3 = faces[face_id]->e3->twin->face;

		if(t_f1 != NULL)
		{
			f1[0] = t_f1->e1->head->index;
			f1[1] = t_f1->e2->head->index;
			f1[2] = t_f1->e3->head->index;
			count++;
		}
		else {f1[0] = f1[1] = f1[2] = -1;}

		if(t_f2 != NULL)
		{
			f2[0] = t_f2->e1->head->index;
			f2[1] = t_f2->e2->head->index;
			f2[2] = t_f2->e3->head->index;
			count++;
		}
		else {f2[0] = f2[1] = f2[2] = -1;}

		if(t_f3 != NULL)
		{
			f3[0] = t_f3->e1->head->index;
			f3[1] = t_f3->e2->head->index;
			f3[2] = t_f3->e3->head->index;
			count++;
		}
		else {f3[0] = f3[1] = f3[2] = -1;}

		return count;
	}

	int Mesh::GetFacesIncidentOnVert(int vert_id,int *faces)
	{
		if(vert_id > numVert)
			return -1;

		for(size_t i = 0;i < vertices[vert_id]->faces.size();i++)
		{
			faces[3*i] = vertices[vert_id]->faces[i]->e1->head->index;
			faces[(3*i)+1] = vertices[vert_id]->faces[i]->e2->head->index;
			faces[(3*i)+2] = vertices[vert_id]->faces[i]->e3->head->index;
		}

		return vertices[vert_id]->faces.size();
	}

	int Mesh::GetFacesIncidentOnEdge(int edge_id,int *faces,int *f1_vert,int *f2_vert)
	{
		int count=0;
		if(edge_id > numEdges)
		{
			faces[0] = faces[1] = -1;
			f1_vert[0] = f1_vert[1] = f1_vert[2] = -1;
			f2_vert[0] = f2_vert[1] = f2_vert[2] = -1;
			return -1;
		}

		if(edge_pairs[edge_id].first != NULL)
		{
			Face *t_f = edge_pairs[edge_id].first->face;
			faces[0] = t_f->index;
			f1_vert[0] = t_f->e1->head->index;
			f1_vert[1] = t_f->e2->head->index;
			f1_vert[2] = t_f->e3->head->index;
			count++;
		}
		else{faces[0] = -1;f1_vert[0] = f1_vert[1] = f1_vert[2] = -1;}

		if(edge_pairs[edge_id].second != NULL)
		{
			Face *t_f = edge_pairs[edge_id].second->face;
			faces[1] = t_f->index;
			f2_vert[0] = t_f->e1->head->index;
			f2_vert[1] = t_f->e2->head->index;
			f2_vert[2] = t_f->e3->head->index;
			count++;
		}
		else{faces[1] = -1;f2_vert[0] = f2_vert[1] = f2_vert[2] = -1;}
		return count;
	}

	int Mesh::GetQuadOnEdge(int edge_id,int *faces,int *idx)
	{
		Edge *e;
		e = edge_pairs[edge_id].first;

		if(e == NULL || e->twin == NULL)
		{
			idx[0] = 0;
			idx[1] = 0;
			idx[2] = 0;
			idx[3] = 0;
			return -1;
		}

		faces[0] = e->face->index;
		faces[1] = e->twin->face->index;

		idx[1] = e->head->index;
		idx[3] = e->tail->index;
		idx[0] = e->next->tail->index;
		idx[2] = e->twin->next->tail->index;
		return 0;
	}

	int Mesh::GetEdgesIncidentOnFace(int face_id,int *e)
	{
		e[0] = faces[face_id]->e1->index;
		e[1] = faces[face_id]->e2->index;
		e[2] = faces[face_id]->e3->index;
		return 0;
	}

	int Mesh::PrintVerts()
	{
		cerr<<"========"<<endl;
		cerr<<"VERTICES"<<endl;
		cerr<<"========"<<endl;
		cerr<<"Number of vertices: "<<vertices.size()<<endl;
		cerr<<"==================="<<endl;
		for(size_t i = 0;i < vertices.size();i++)
		{
			printf("%d -> %4.2f %4.2f %4.2f|%4.2f %4.2f %4.2f|%4.2f %4.2f %4.2f|\n",i,
				vertices[i]->position[0],vertices[i]->position[1],vertices[i]->position[2],
				vertices[i]->normal[0],vertices[i]->normal[1],vertices[i]->normal[2],
				vertices[i]->color[0],vertices[i]->color[1],vertices[i]->color[2]);
		}
		return 0;
	}

	int Mesh::PrintEdges()
	{
		int i1[]={-1,-1},i2[]={-1,-1};
		cerr<<"=========="<<endl;
		cerr<<"EDGE PAIRS"<<endl;
		cerr<<"=========="<<endl;
		cerr<<"Number of edges: "<<edge_pairs.size()<<endl;
		cerr<<"================"<<endl;
		for(size_t i = 0;i < edge_pairs.size();i++)
		{
			if(edge_pairs[i].first != NULL)
			{
				i1[0] = edge_pairs[i].first->head->index;
				i1[1] = edge_pairs[i].first->tail->index;
			}

			if(edge_pairs[i].second != NULL)
			{
				i2[0] = edge_pairs[i].second->head->index;
				i2[1] = edge_pairs[i].second->tail->index;
			}

			printf("%d -> (%d %d) (%d %d)\n",i,i1[0],i1[1],i2[0],i2[1]);
		}
		return 0;
	}

	int Mesh::PrintFaces()
	{
		cerr<<"====="<<endl;
		cerr<<"FACES"<<endl;
		cerr<<"====="<<endl;
		cerr<<"Number of faces: "<<faces.size()<<endl;
		cerr<<"================"<<endl;
		for(size_t i = 0;i < faces.size();i++)
			printf("%d -> %d %d %d\n",i,faces[i]->e1->head->index,faces[i]->e2->head->index,faces[i]->e3->head->index);
		return 0;
	}

	int Mesh::PrintVertAdjacency()
	{
		cerr<<"=================="<<endl;
		cerr<<"VERTEX ADJACENCIES"<<endl;
		cerr<<"=================="<<endl;
		for(size_t i = 0;i < vertices.size();i++)
		{
			printf("%d -> ",i);
			for(size_t j = 0;j < vertices[i]->edges.size();j++)
				printf("%d ",vertices[i]->edges[j]->tail->index);
			printf("\n");

		}
		return 0;
	}

	int Mesh::PrintFaceAdjacency()
	{
		cerr<<"================"<<endl;
		cerr<<"FACE ADJACENCIES"<<endl;
		cerr<<"================"<<endl;
		for(size_t i = 0;i < faces.size();i++)
		{
			printf("%d -> %d %d %d\n",i,
				faces[i]->e1->twin->face->index,
				faces[i]->e2->twin->face->index,
				faces[i]->e3->twin->face->index);
		}

		return 0;
	}

	int Mesh::PrintMesh()
	{
		PrintVerts();
		PrintEdges();
		PrintFaces();
		return 0;
	}

	int Mesh::SaveToPly(char fName[])
	{
		FILE *stream;
		stream = fopen(fName,"w");
		if(stream == NULL) return -1;

		//---- Write PLY header
		fprintf(stream,"ply\n");
		fprintf(stream,"format ascii 1.0\n");
		fprintf(stream,"comment I generated\n");
		fprintf(stream,"element vertex %d\n",(int)vertices.size());
		fprintf(stream,"property float x\n");
		fprintf(stream,"property float y\n");
		fprintf(stream,"property float z\n");
		fprintf(stream,"property float nx\n");
		fprintf(stream,"property float ny\n");
		fprintf(stream,"property float nz\n");
		fprintf(stream,"property uchar red\n");
		fprintf(stream,"property uchar green\n");
		fprintf(stream,"property uchar blue\n");
		fprintf(stream,"property uchar alpha\n");
		fprintf(stream,"element face %d\n",(int)faces.size());
		fprintf(stream,"property list uchar int vertex_indices\n");
		fprintf(stream,"end_header\n");

		//---- Write Vertex Data -> x y z nx ny nz r g b a
		for(size_t i = 0;i < vertices.size();i++)
		{
			fprintf(stream,"%f %f %f ",vertices[i]->position[0],vertices[i]->position[1],vertices[i]->position[2]);
			fprintf(stream,"%f %f %f ",vertices[i]->normal[0],vertices[i]->normal[1],vertices[i]->normal[2]);
			fprintf(stream,"127 127 127 255\n");
		}
		//---- Write Face Data -> #vert i1 i2 i3
		for(size_t i = 0;i < faces.size();i++)
			fprintf(stream,"3 %d %d %d\n",faces[i]->e1->head->index,faces[i]->e2->head->index,faces[i]->e3->head->index);
		fclose(stream);
		return 0;
	}

	int Mesh::SaveToPly(char fName[],ColorList &c_list)
	{
		FILE *stream;
		stream = fopen(fName,"w");
		if(stream == NULL) return -1;

		//---- Write PLY header
		fprintf(stream,"ply\n");
		fprintf(stream,"format ascii 1.0\n");
		fprintf(stream,"comment I generated\n");
		fprintf(stream,"element vertex %d\n",(int)vertices.size());
		fprintf(stream,"property float x\n");
		fprintf(stream,"property float y\n");
		fprintf(stream,"property float z\n");
		fprintf(stream,"property float nx\n");
		fprintf(stream,"property float ny\n");
		fprintf(stream,"property float nz\n");
		fprintf(stream,"property uchar red\n");
		fprintf(stream,"property uchar green\n");
		fprintf(stream,"property uchar blue\n");
		fprintf(stream,"property uchar alpha\n");
		fprintf(stream,"element face %d\n",(int)faces.size());
		fprintf(stream,"property list uchar int vertex_indices\n");
		fprintf(stream,"end_header\n");

		//---- Write Vertex Data -> x y z nx ny nz r g b a
		for(size_t i = 0;i < vertices.size();i++)
		{
			fprintf(stream,"%f %f %f ",vertices[i]->position[0],vertices[i]->position[1],vertices[i]->position[2]);
			fprintf(stream,"%f %f %f ",vertices[i]->normal[0],vertices[i]->normal[1],vertices[i]->normal[2]);
			fprintf(stream,"%d %d %d %d\n",c_list[i].r,c_list[i].g,c_list[i].b,c_list[i].a);
		}
		//---- Write Face Data -> #vert i1 i2 i3
		for(size_t i = 0;i < faces.size();i++)
			fprintf(stream,"3 %d %d %d\n",faces[i]->e1->head->index,faces[i]->e2->head->index,faces[i]->e3->head->index);
		fclose(stream);
		return 0;
	}

	int Mesh::DeleteMesh()
	{
		if(!faces.empty())
		{
			Face *f;
			for(size_t i = 0;i < faces.size();i++)
			{
				f = faces[i];
				delete f;
				faces[i] = NULL;
			}
			faces.clear();
		}

		if(!edges.empty())
		{
			Edge *e;
			for(size_t i = 0;i < edges.size();i++)
			{
				e = edges[i];
				delete e;
				edges[i] = NULL;
			}
			edges.clear();
		}

		if(!vertices.empty())
		{
			Vert *v;
			for(size_t i = 0;i < vertices.size();i++)
			{
				v = vertices[i];
				delete v;
				vertices[i] = NULL;
			}
			vertices.clear();
		}

		if(!edge_pairs.empty())edge_pairs.clear();

		if(!face_partition.empty())face_partition.clear();

		cerr<<"Mesh deleted successfully."<<endl;
		return 0;
	}
}

