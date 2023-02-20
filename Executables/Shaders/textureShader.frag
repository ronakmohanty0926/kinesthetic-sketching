varying vec4 position;  // position of the vertex (and fragment) in world space
varying vec3 varyingNormalDirection;  // surface normal vector in world space
varying vec4 texCoords; // the texture coordinates
uniform sampler2D textureUnit;
 
void main()
{
  vec4 textureColor = texture2D(textureUnit, texCoords.st);	
 	gl_FragColor.rgb = textureColor.rgb;
	gl_FragColor.a = 1.0;
  }
}