//
// Bui Tuong Phong shading model (per-pixel) 
// 
// by 
// Massimiliano Corsini
// Visual Computing Lab (2006)
// 

varying vec3 normal;
varying vec3 vpos;
varying mat3 nMatrix;

void main()
{	
	// vertex normal
	normal = normalize(gl_NormalMatrix * gl_Normal);
		
	// vertex position
	vpos = vec3(gl_ModelViewMatrix * gl_Vertex);
	
	nMatrix = gl_NormalMatrix;
	
	gl_FrontColor = gl_Color;
	
	// vertex position
	gl_Position = ftransform();
}
