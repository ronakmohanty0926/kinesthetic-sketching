#include <MeshRenderer.h>

namespace midl {
    void DrawMesh(Mesh &m, int option)
    {
        float v1[3],v2[3],v3[3];
        float n1[3],n2[3],n3[3];
        float c1[3],c2[3],c3[3];

        for (int i = 0; i < (int)m.GetNumFaces(); i++)
        {
            m.GetVertsForFace(i,v1,v2,v3);
            m.GetVertColorsForFace(i,c1,c2,c3);

            if(option == MESH_DISPLAY::SOLID_FLAT)
            {
                m.GetFaceNormal(i,n1);
                m.GetFaceNormal(i,n2);
                m.GetFaceNormal(i,n3);
            }
            else m.GetVertNormalsForFace(i,n1,n2,n3);

            if (option == MESH_DISPLAY::WIREFRAME) glBegin(GL_LINE_LOOP);
            else glBegin(GL_TRIANGLES);
            glNormal3fv(n1);
            glColor3fv(c1);
            glVertex3fv(v1);

            glNormal3fv(n2);
            glColor3fv(c2);
            glVertex3fv(v2);

            glNormal3fv(n3);
            glColor3fv(c3);
            glVertex3fv(v3);
            glEnd();
        }
    }

    void DrawMesh(Mesh &m, float *color, int option)
    {
        float v1[3],v2[3],v3[3];
        float n1[3],n2[3],n3[3];
        for (int i = 0; i < (int)m.GetNumFaces(); i++)
        {
            m.GetVertsForFace(i,v1,v2,v3);

            if(option == MESH_DISPLAY::SOLID_FLAT)
            {
                m.GetFaceNormal(i,n1);
                m.GetFaceNormal(i,n2);
                m.GetFaceNormal(i,n3);
            }
            else m.GetVertNormalsForFace(i,n1,n2,n3);

            if (option == MESH_DISPLAY::WIREFRAME) glBegin(GL_LINE_LOOP);
            else glBegin(GL_TRIANGLES);
            glNormal3fv(n1);
            glColor3fv(color);
            glVertex3fv(v1);

            glNormal3fv(n2);
            glColor3fv(color);
            glVertex3fv(v2);

            glNormal3fv(n3);
            glColor3fv(color);
            glVertex3fv(v3);
            glEnd();
        }
    }

    void DrawMesh(Mesh &m, vector<Tuple3f> &colorList, int option)
    {
        if(colorList.size() != m.GetNumVert())return;

        float v1[3],v2[3],v3[3];
        float n1[3],n2[3],n3[3];
        int vIdx[3];
        for (int i = 0; i < (int)m.GetNumFaces(); i++)
        {
            m.GetVertIdxForFace(i,vIdx);
            m.GetVertsForFace(i,v1,v2,v3);

            if(option == MESH_DISPLAY::SOLID_FLAT)
            {
                m.GetFaceNormal(i,n1);
                m.GetFaceNormal(i,n2);
                m.GetFaceNormal(i,n3);
            }
            else m.GetVertNormalsForFace(i,n1,n2,n3);

            if (option == MESH_DISPLAY::WIREFRAME) glBegin(GL_LINE_LOOP);
            else glBegin(GL_TRIANGLES);
            glNormal3fv(n1);
            glColor3fv(colorList[vIdx[0]].data);
            glVertex3fv(v1);

            glNormal3fv(n2);
            glColor3fv(colorList[vIdx[1]].data);
            glVertex3fv(v2);

            glNormal3fv(n3);
            glColor3fv(colorList[vIdx[2]].data);
            glVertex3fv(v3);
            glEnd();
        }
    }
	
    void DrawMesh(Mesh &m, unsigned int shaderID, GLuint &texture)
	{
        //TODO
	}
}
