#version 430

out vec4 vFragColor;
uniform vec3 Color;
uniform sampler2D sprite;

in vec2 TexCoord;

void main(void)
{ 
	if(Color.b == 0.5){
		vFragColor = texture(sprite, TexCoord);
	}
	else if(Color.r != -1.0){
		vFragColor = vec4(Color, 1.0);
	}
	else if(Color.r == -1.0){ 
		vFragColor = vec4(float(gl_PrimitiveID+1), float(gl_PrimitiveID+1), float(gl_PrimitiveID+1), 1.0);
	}
}
	
    