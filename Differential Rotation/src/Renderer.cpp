#include <Renderer.h>
#include <vector>

namespace midl {

    PerspectiveView::PerspectiveView(){}

    PerspectiveView::~PerspectiveView(){}

    void PerspectiveView::SetParameters(float prj_fov, float prj_near, float prj_far)
    {
        this->prj_fov = prj_fov;
        this->prj_near = prj_near;
        this->prj_far = prj_far;
    }

    void PerspectiveView::SetCameraEye(float x, float y, float z)
    {
        this->eye[0] = x;
        this->eye[1] = y;
        this->eye[2] = z;
    }

    void PerspectiveView::SetCameraEye(float *eye)
    {
        this->eye[0] = eye[0];
        this->eye[1] = eye[1];
        this->eye[2] = eye[2];	
    }

    void PerspectiveView::SetCameraCenter(float x, float y, float z)
    {
        this->center[0] = x;
        this->center[1] = y;
        this->center[2] = z;
    }

    void PerspectiveView::SetCameraCenter(float *center)
    {
        this->center[0] = center[0];
        this->center[1] = center[1];
        this->center[2] = center[2];
    }

    void PerspectiveView::SetCameraHead(float x, float y, float z)
    {
        this->head[0] = x;
        this->head[1] = y;
        this->head[2] = z;
    }

    void PerspectiveView::SetCameraHead(float *head)
    {
        this->head[0] = head[0];
        this->head[1] = head[1];
        this->head[2] = head[2];
    }

    void PerspectiveView::PixelToWindow(int px, int py, float *windowCoordinates)
    {
        windowCoordinates[0] = (float)px;
        windowCoordinates[1] = (float)(viewport[3] - py);
    }

    void PerspectiveView::PixelToNormalized(int px, int py, float *normalizedCoordinates)
    {
        normalizedCoordinates[0] = -0.5 + (float)px/(float)viewport[2];
        normalizedCoordinates[1] = -0.5 + (float)(viewport[3] - py);
    }

    void PerspectiveView::PixelToWorld(int px, int py, float depth, float *worldCoordinates)
    {
        GLfloat winX, winY;
        winX = (float)px;
        winY = (float)(viewport[3] - py);

        double objX, objY, objZ;

        gluUnProject(winX, winY, depth, modelMatrix, projectionMatrix, viewport, &objX, &objY, &objZ);

        worldCoordinates[0] = objX;
        worldCoordinates[1] = objY;
        worldCoordinates[2] = objZ;
    }

    void PerspectiveView::PixelToRay(int px, int py, float *rayStart, float *rayEnd)
    {
        GLfloat winX, winY;
        winX = (float)px;
        winY = (float)(viewport[3] - py);

        double objX1, objY1, objZ1;
        double objX2, objY2, objZ2;

        gluUnProject(winX, winY, 0.0, modelMatrix, projectionMatrix, viewport, &objX1, &objY1, &objZ1);
        gluUnProject(winX, winY, 1.0, modelMatrix, projectionMatrix, viewport, &objX2, &objY2, &objZ2);

        rayStart[0] = objX1;
        rayStart[1] = objY1;
        rayStart[2] = objZ1;

        rayEnd[0] = objX2;
        rayEnd[1] = objY2;
        rayEnd[2] = objZ2;
    }

    void PerspectiveView::Reshape(int w, int h)
    {
        glViewport(0, 0, (GLsizei)w, (GLsizei)h);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(prj_fov, (GLfloat)w/(GLfloat)h, prj_near, prj_far);
        glMatrixMode(GL_MODELVIEW);

        glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);
        glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
        glGetIntegerv(GL_VIEWPORT, viewport);
    }

    void PerspectiveView::Bind()
    {
        glLoadIdentity();
        gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], head[0], head[1], head[2]);
    }

    void PerspectiveView::Unbind()
    {
        glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);
        glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
        glGetIntegerv(GL_VIEWPORT, viewport);
    }

    Light::Light()
    {
        lightpos[0] = 0.0;
        lightpos[1] = 0.0;
        lightpos[2] = 10.0;
        lightpos[3] = 1.0;

        diffuse[0] = 1.0;
        diffuse[1] = 1.0;
        diffuse[2] = 1.0;
        diffuse[3] = 1.0;

        ambient[0] = 0.7;
        ambient[1] = 0.7;
        ambient[2] = 0.7;
        ambient[3] = 1.0;

        specular[0] = 1.0;
        specular[1] = 1.0;
        specular[2] = 1.0;
        specular[3] = 1.0;

        rotation.SetIdentity();
    }

    Light::~Light(){}

    void Light::SetPosition(float *pos)
    {
        lightpos[0] = pos[0];
        lightpos[1] = pos[1];
        lightpos[2] = pos[2];
    }

    void Light::SetDiffuseColor(float *color)
    {
        diffuse[0] = color[0];
        diffuse[1] = color[1];
        diffuse[2] = color[2];
    }

    void Light::SetAmbientColor(float *color)
    {
        ambient[0] = color[0];
        ambient[1] = color[1];
        ambient[2] = color[2];
    }

    void Light::SetSpecularColor(float *color)
    {
        specular[0] = color[0];
        specular[1] = color[1];
        specular[2] = color[2];
    }

    void Light::Rotate(float *axis, float angle)
    {
        Transformation T;
        T.SetAxisAngleRotation(axis,angle);
        rotation = T*rotation;
    }

    void Light::Bind()
    {
        float lPos[] = {lightpos[0],lightpos[1],lightpos[2]};
        rotation.Inverse().ApplyTo(lPos);
        float finalPosition[] = {lPos[0],lPos[1],lPos[2],1.0};

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glLightfv(GL_LIGHT0, GL_POSITION, finalPosition);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
        glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
        glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    }

    void Light::Unbind()
    {
        glDisable(GL_LIGHTING);
    }

    GLuint LoadTexture(const char * filename, int width, int height)
    {
        GLuint tex = 0;
        unsigned char * data;
        FILE * file;
        //The following code will read in our RAW file
        file = fopen( filename, "rb" );
        if ( file == NULL )
            cout<<"Unable to read texture."<<endl;

        //cout<<"Texture Dimensions: "<<width<<"  "<<height<<endl;

        data = (unsigned char *)malloc( width * height * 3 );
        fread( data, width * height * 3, 1, file );
        fclose( file );

        glGenTextures( 1, &tex); //generate the texture with the loaded data
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glBindTexture( GL_TEXTURE_2D, tex); //bind the texture to it's array
        glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL ); //set texture environment parameters

        //And if you go and use extensions, you can use Anisotropic filtering textures which are of an
        //even better quality, but this will do for now.
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

        //Here we are setting the parameter to repeat the texture instead of clamping the texture
        //to the edge of our shape.
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

        //Generate the texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

        //cout<<filename<<" read successfully."<<endl;

        free( data ); //free the texture
        return tex;
    }

    GLuint LoadTexture(int width, int height,unsigned char * data,bool alphaFlag)
    {
        GLuint tex = 0;
        glGenTextures( 1, &tex); //generate the texture with the loaded data
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glBindTexture( GL_TEXTURE_2D, tex); //bind the texture to it's array
        glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL ); //set texture environment parameters

        //And if you go and use extensions, you can use Anisotropic filtering textures which are of an
        //even better quality, but this will do for now.
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

        //Here we are setting the parameter to repeat the texture instead of clamping the texture
        //to the edge of our shape.
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

        //Generate the texture
        if(alphaFlag)glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
        else glTexImage2D(GL_TEXTURE_2D, 0, GL_BGR, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        return tex;
    }

    void DrawGrid(int plane, float side, int resolution, float *color)
    {
		
        glColor3fv(color);
        float Lim[] = {-0.5*side,0.5*side};
        int gridSize = resolution;
        float t;
		glLineWidth(3);

        if(plane == GRID_PLANE::XY || plane == GRID_PLANE::ZX)
        {
            // x-parallel lines
            glBegin(GL_LINES);
            for(int i = 0;i < gridSize+1;i++)
            {
                t = Lim[0] + ((float)i/gridSize)*(Lim[1]-Lim[0]);
                if(plane == GRID_PLANE::ZX)
                {
                    glVertex3f(Lim[0], 0.0, t);
                    glVertex3f(Lim[1], 0.0, t);
                }
                else
                {
                    glVertex3f(Lim[0], t, 0.0);
                    glVertex3f(Lim[1], t, 0.0);
                }

            }
            glEnd();
        }

        if(plane == GRID_PLANE::XY || plane == GRID_PLANE::YZ)
        {
            // y-parallel lines
            glBegin(GL_LINES);
            for(int i = 0;i < gridSize+1;i++)
            {
                t = Lim[0] + ((float)i/gridSize)*(Lim[1]-Lim[0]);
                if(plane == GRID_PLANE::XY)
                {
                    glVertex3f(t, Lim[0], 0.0);
                    glVertex3f(t, Lim[1], 0.0);
                }
                else
                {
                    glVertex3f(0.0, Lim[0], t);
                    glVertex3f(0.0, Lim[1], t);
                }

            }
            glEnd();
        }

        if(plane == GRID_PLANE::YZ || plane == GRID_PLANE::ZX)
        {
            // z-parallel liness
            glBegin(GL_LINES);
            for(int i = 0;i < gridSize+1;i++)
            {
                t = Lim[0] + ((float)i/gridSize)*(Lim[1]-Lim[0]);
                if(plane == GRID_PLANE::ZX)
                {
                    glVertex3f(t, 0.0, Lim[0]);
                    glVertex3f(t, 0.0, Lim[1]);
                }
                else
                {
                    glVertex3f(0.0, t, Lim[0]);
                    glVertex3f(0.0, t, Lim[1]);
                }

            }
            glEnd();
			
        }
    }

    void DrawQuad(float width, float height, float *center, bool isFilled, float color[3], float transparency)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glPushMatrix();
        glTranslatef(center[0], center[1],center[2]);

        glColor4f(color[0], color[1], color[2], transparency);
        if (isFilled) glBegin(GL_QUADS);
        else glBegin(GL_LINE_LOOP);
            glNormal3f(0.0,0.0,1.0);
            glVertex3f(-0.5*width, 0.5*height, 0.0);
            glNormal3f(0.0,0.0,1.0);
            glVertex3f(0.5*width, 0.5*height, 0.0);
            glNormal3f(0.0,0.0,1.0);
            glVertex3f(0.5*width, -0.5*height, 0.0);
            glNormal3f(0.0,0.0,1.0);
            glVertex3f(-0.5*width, -0.5*height, 0.0);
        glEnd();
        glPopMatrix();
        glDisable(GL_BLEND);
    }

	void DrawQuadShadow(float width, float height, float *center, bool isFilled, float color[3], float transparency)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glPushMatrix();
		glTranslatef(center[0], -0.4, center[2]-0.1);

		glColor4f(color[0], color[1], color[2], transparency);
		if (isFilled) glBegin(GL_QUADS);
		else glBegin(GL_LINE_LOOP);
		glNormal3f(0.0, 1.0, 0.0);
		glVertex3f(-0.5*width, 0.0, 0.5*height);
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3f(0.5*width, 0.0, 0.5*height);
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3f(0.5*width, 0.0, -0.5*height);
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3f(-0.5*width, 0.0, -0.5*height);
		glEnd();
		glPopMatrix();
		glDisable(GL_BLEND);
	}
	
	void DrawQuad(float width, float height, float *center, float *normal, bool isFilled, float color[3], float transparency)
    {
		float axis[3], angle=0.0;
		
		//float leftBottomCorner[3] = { center[0], center[1], 1.0 };
		
		GetAngleAxisBetweenDirections(Zaxis, normal, axis, angle);
				
		
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glPushMatrix();
        glTranslatef(center[0],center[1],center[2]);
		glRotatef(180.0*angle/PI,axis[0],axis[1],axis[2]);

        glColor4f(color[0], color[1], color[2], transparency);
        if (isFilled) glBegin(GL_QUADS);
        else glBegin(GL_LINE_LOOP);
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3f(0.0, 0.0, 0.0);
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3f(width, 0.0, 0.0);
		
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3f(width, height, 0.0);
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3f(0.0, height, 0.0);
		
            /*glNormal3f(0.0,0.0,1.0);
            glVertex3f(-0.5*width, 0.5*height, 0.0);
            glNormal3f(0.0,0.0,1.0);
            glVertex3f(0.5*width, 0.5*height, 0.0);
            glNormal3f(0.0,0.0,1.0);
            glVertex3f(0.5*width, -0.5*height, 0.0);
            glNormal3f(0.0,0.0,1.0);
            glVertex3f(-0.5*width, -0.5*height, 0.0);*/
        glEnd();
        glPopMatrix();
        glDisable(GL_BLEND);
    }


	void DrawQuadShadow(float width, float height, float *center, float *normal, bool isFilled, float color[3], float transparency)
	{
		float axis[3], angle = 0.0, mat[9];
		

		GetAngleAxisBetweenDirections(Zaxis, normal, axis, angle);
		
		AxisAngle(axis, angle, mat);		
		
		float vert1[3] = { 0.0, 0.0, height };
		float newVert1[3];
		RotateVec3(vert1, mat, newVert1);

		float vert2[3] = { width, 0.0, height };
		float newVert2[3];
		RotateVec3(vert2, mat, newVert2);

		float vert3[3] = { width, 0.0, 0.0 };
		float newVert3[3];
		RotateVec3(vert3, mat, newVert3);

		float vert4[3] = { 0.0, 0.0, 0.0 };
		float newVert4[3];
		RotateVec3(vert4, mat, newVert4);	


		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glPushMatrix();
		//glTranslatef(center[0], center[1], center[2]);
		//glRotatef(180.0*angle / PI, axis[0], axis[1], axis[2]);

		glColor4f(color[0], color[1], color[2], transparency);
		if (isFilled) glBegin(GL_QUADS);
		else glBegin(GL_LINE_LOOP);
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3f(vert1[0] + center[0], vert1[1], vert1[2] + center[2]);
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3f(vert2[0] + center[0], vert2[1], vert2[2] + center[2]);
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3f(vert3[0] + center[0], vert3[1], vert3[2] + center[2]);
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3f(vert4[0] + center[0], vert4[1], vert4[2] + center[2]);

		/*glNormal3f(0.0, 0.0, 1.0);
		glVertex3f(newVert1[0] + center[0], newVert1[1] + center[1], newVert1[2] + center[2]);
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3f(newVert2[0] + center[0], newVert2[1] + center[1], newVert2[2] + center[2]);
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3f(newVert3[0] + center[0], newVert3[1] + center[1], newVert3[2] + center[2]);
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3f(newVert4[0] + center[0], newVert4[1] + center[1], newVert4[2] + center[2]);*/

		glEnd();
		glPopMatrix();
		glDisable(GL_BLEND);
	}



    void DrawQuad(float width, float height, float *center, unsigned int shaderID, GLuint &texture)
    {
        if(shaderID > 0)
        {
            glPushMatrix();
            glTranslatef(center[0],center[1],center[2]);
            glEnable(GL_TEXTURE_2D);
            glActiveTexture(GL_TEXTURE0);
            int texture_location = glGetUniformLocation(shaderID,"textureDiff");
            glBindTexture(GL_TEXTURE_2D, texture);
            glUniform1i(texture_location, 0);
            glBegin(GL_QUADS);
            glNormal3f(0.0,0.0,1.0);
            glTexCoord2f(0.0,0.0);glVertex3f(-0.5*width, 0.5*height, 0.0);
            glNormal3f(0.0,0.0,1.0);
            glTexCoord2f(1.0,0.0);glVertex3f(0.5*width, 0.5*height, 0.0);
            glNormal3f(0.0,0.0,1.0);
            glTexCoord2f(1.0,1.0);glVertex3f(0.5*width, -0.5*height, 0.0);
            glNormal3f(0.0,0.0,1.0);
            glTexCoord2f(0.0,1.0);glVertex3f(-0.5*width, -0.5*height, 0.0);
            glEnd();
            glBindTexture(GL_TEXTURE_2D, 0);
            glDisable(GL_TEXTURE_2D);
            glPopMatrix();
        }
    }

    void DrawCircle(float *center, float radius, bool isFilled, float *color, float transparency, int resolution)
    {
        float angle = 2 * PI / resolution;
        float theta;

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glColor4f(color[0], color[1], color[2], transparency);
        if (isFilled) glBegin(GL_POLYGON);
        else glBegin(GL_LINE_LOOP);
        for (size_t i = 0; i < resolution; i++)
        {
            theta = i*angle;
            glVertex3f(center[0] + radius*cosf(theta), center[1] + radius*sinf(theta),center[2]);
        }
        glEnd();

        glDisable(GL_BLEND);
    }

	
    void DrawSphere(float *center, float radius, float *color, float transparency)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glColor4f(color[0], color[1], color[2], transparency);

        glPushMatrix();
        glTranslatef(center[0],center[1],center[2]);
        glScalef(radius,radius,radius);
        glutSolidSphere(0.5,20,20);
        glPopMatrix();

        glDisable(GL_BLEND);
    }

    void DrawCylinder(float *p1, float *p2, float radius, float *color, float transparency, bool areEndsSolid, int resolution)
    {
        float angleIncrement = 2.0 * PI/(float)resolution;
        float theta1, theta2;

        float diff[3];
        SubVectors3(p1,p2,diff);
        float l = Norm3(diff);
        Normalize3(diff);
        float axis[3],angle;

        GetAngleAxisBetweenDirections(Zaxis,diff,axis,angle);

        if(!Normalize3(axis))return;

        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glColor4f(color[0], color[1], color[2], transparency);

        glPushMatrix();
        glTranslatef(p2[0],p2[1],p2[2]);
        glRotatef(180.0*angle/PI,axis[0],axis[1],axis[2]);
        glScalef(0.5*radius,0.5*radius,l);

        // STANDARD CYLINDER ALONG Z-AXIS WITH ONE FACE ON XY PLANE
        glBegin(GL_QUADS);
        for (size_t i = 0; i < resolution; i++)
        {
            theta1 = i*angleIncrement;
            theta2 = (i+1)*angleIncrement;
            //glColor3fv(color);
            glNormal3f(cosf(theta1), sinf(theta1),0.0);
            glVertex3f(radius*cosf(theta1), radius*sinf(theta1),1.0);

            //glColor3fv(color);
            glNormal3f(cosf(theta2), sinf(theta2),0.0);
            glVertex3f(radius*cosf(theta2), radius*sinf(theta2),1.0);

            //glColor3fv(color);
            glNormal3f(cosf(theta2), sinf(theta2),0.0);
            glVertex3f(radius*cosf(theta2), radius*sinf(theta2),0.0);

            //glColor3fv(color);
            glNormal3f(cosf(theta1), sinf(theta1),0.0);
            glVertex3f(radius*cosf(theta1), radius*sinf(theta1),0.0);
        }
        glEnd();

        if(areEndsSolid)
        {
            glBegin(GL_POLYGON);
            for (size_t i = 0; i < resolution; i++)
            {
                glNormal3f(0.0,0.0,1.0);
                glVertex3f(radius*cosf(i*angleIncrement), radius*sinf(i*angleIncrement),1.0);
            }
            glEnd();

            glBegin(GL_POLYGON);
            for (size_t i = resolution-1; i > -1; i++)
            {
                glNormal3f(0.0,0.0,-1.0);
                glVertex3f(radius*cosf(i*angleIncrement), radius*sinf(i*angleIncrement),0.0);
            }
            glEnd();
        }
        // STANDARD CYLINDER END

        glPopMatrix();
        //glDisable(GL_BLEND);
    }

    void DrawPoint(float *position, float *color, float transparency, float pointSize, int option)
    {
        if(option == POINT_DISPLAY::PIXEL)
        {
            glPointSize(4.0f);
            glBegin(GL_POINTS);
            glColor3fv(color);
            glVertex3f(position[0],position[1],position[2]);
            glEnd();
            glPointSize(1.0f);
        }
        else if(option == POINT_DISPLAY::QUAD_LINE)
        {
            DrawQuad(pointSize,pointSize,position,false,color,transparency);
        }
        else if(option == POINT_DISPLAY::QUAD_FILL)
        {
            DrawQuad(pointSize,pointSize,position,true,color,transparency);
        }
        else if(option == POINT_DISPLAY::CIRCLE_LINE)
        {
            DrawCircle(position,pointSize,false,color,transparency, 20);
        }
        else if(option == POINT_DISPLAY::CIRCLE_FILL)
        {
            DrawCircle(position,pointSize,true,color,transparency, 20);
        }
        else if(option == POINT_DISPLAY::SPHERE)
        {
            DrawSphere(position,pointSize,color,transparency);
        }
    }

    void DrawLine(float *p1, float *p2, float *color, float transparency, float thickness, int option)
    {
        if(option == LINE_DISPLAY::PIXEL)
        {
            glLineWidth(4.0f);
            glBegin(GL_LINES);
            glColor3fv(color);
            glVertex3fv(p1);
            glVertex3fv(p2);
            glEnd();
            glLineWidth(1.0f);
        }
        else if(option == LINE_DISPLAY::CYLINDER)
        {
            DrawCylinder(p1,p2,thickness,color,transparency,true,40);
        }
        else if(option == LINE_DISPLAY::BOX)
        {
            float diff[3];
            SubVectors3(p1,p2,diff);
            float l = Norm3(diff);
            Normalize3(diff);
            float axis[3],angle;

            Cross(Zaxis,diff,axis);
            angle = std::acosf(Dot3(Zaxis,diff));

            //GetAngleAxisBetweenDirections(Zaxis,diff,axis,angle);

            if(!Normalize3(axis))return;

            //glEnable(GL_BLEND);
            //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glColor4f(color[0], color[1], color[2], transparency);

            glPushMatrix();
            glTranslatef(p2[0],p2[1],p2[2]);
            glRotatef(180.0*angle/PI,axis[0],axis[1],axis[2]);
            glScalef(0.5*thickness,0.5*thickness,l);
            glTranslatef(0.0,0.0,0.5);
            glutSolidCube(1.0);
            glPopMatrix();

            //glDisable(GL_BLEND);
        }

    }

    void DrawPointCloud(vector<float> &pcl, float *color, float transparency, float pointSize, int option)
    {
        float p[3];
        for(int i = 0;i < pcl.size();i+=3)
        {
            p[0] = pcl[i];
            p[1] = pcl[i+1];
            p[2] = pcl[i+2];
            DrawPoint(p,color,transparency,pointSize,option);
        }
    }

    void DrawPointCloud(vector<Tuple3f> &pcl, float *color, float transparency, float pointSize, int option)
    {
        for(int i = 0;i < pcl.size();i++)
            DrawPoint(pcl[i].data,color,transparency,pointSize,option);
    }

    void DrawEdgeList(vector<float> &vertices, vector<int> &edges, float *color, float transparency, float thickness, int option)
    {
        int i1,i2;
        float p1[3],p2[3];
        for(int i = 0;i < edges.size();i+=2)
        {
            i1 = edges[i];
            i2 = edges[i+1];

            p1[0] = vertices[3*i1];
            p1[1] = vertices[3*i1 + 1];
            p1[2] = vertices[3*i1 + 2];

            p2[0] = vertices[3*i2];
            p2[1] = vertices[3*i2 + 1];
            p2[2] = vertices[3*i2 + 2];

            DrawLine(p1,p2,color,transparency,thickness,option);
        }
    }

    void DrawEdgeList(vector<Tuple3f> &vertices, vector<int> &edges, float *color, float transparency, float thickness, int option)
    {
        for(int i = 0;i < edges.size();i+=2)
            DrawLine(vertices[edges[i]].data,vertices[edges[i+1]].data,color,transparency,thickness,option);
    }

    void DrawEdgeList(vector<float> &vertices, vector<Tuple2i> &edges, float *color, float transparency, float thickness, int option)
    {
        int i1,i2;
        float p1[3],p2[3];
        for(int i = 0;i < edges.size();i++)
        {
            i1 = edges[i].data[0];
            i2 = edges[i].data[1];

            p1[0] = vertices[3*i1];
            p1[1] = vertices[3*i1 + 1];
            p1[2] = vertices[3*i1 + 2];

            p2[0] = vertices[3*i2];
            p2[1] = vertices[3*i2 + 1];
            p2[2] = vertices[3*i2 + 2];

            DrawLine(p1,p2,color,transparency,thickness,option);
        }
    }

    void DrawEdgeList(vector<Tuple3f> &vertices, vector<Tuple2i> &edges, float *color, float transparency, float thickness, int option)
    {
       // cerr<<"Edge list size"<<edges.size()<<endl;
        for(int i = 0;i < edges.size();i++)
            DrawLine(vertices[edges[i].data[0]].data,vertices[edges[i].data[1]].data,color,transparency,thickness,option);
    }

    void DrawEdges(vector<Tuple3f> &vertices, vector<Tuple2i> &edges)
    {
        glLineWidth(2.0);
        glColor3f(0.5, 0.0, 0.5);
        glBegin(GL_LINES);
        for(int i = 0;i < edges.size();i+=2)
         {
             glVertex3fv(vertices[edges[i].data[0]].data);
             glVertex3fv(vertices[edges[i].data[1]].data);
         }
        glEnd();
    }

    GLuint RenderScreenToTexture(int w,int h, bool isDepth)
    {
        unsigned int textureId;
        glGenTextures(1,&textureId);
        glBindTexture(GL_TEXTURE_2D,textureId);
        glTexImage2D(GL_TEXTURE_2D,0,(!isDepth ? GL_RGBA8 : GL_DEPTH_COMPONENT),w,h,0,isDepth ? GL_DEPTH_COMPONENT : GL_RGBA,GL_FLOAT,NULL);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

        int i;
        i=glGetError();
        if(i!=0)
        {
            cout << "Error loading the screen-capture texture: " << i << endl;
            return 0;
        }
        glBindTexture(GL_TEXTURE_2D,0);
        return textureId;
    }
}
