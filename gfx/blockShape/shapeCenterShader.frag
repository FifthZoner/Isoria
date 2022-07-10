#extension GL_OES_standard_derivatives : enable

precision highp float;

// texture that is to be changed
uniform sampler2D targetTexture;
// textures from the blockShapeVariant that is to be the base of processing the block
uniform sampler2D upTexture;
uniform sampler2D downTexture;
uniform sampler2D rightTexture;
uniform sampler2D leftTexture;
uniform sampler2D centerTexture;

// this is block resolution, not screen
uniform vec2 resolution;

uniform bool addUp;
uniform bool addDown;
uniform bool addLeft;
uniform bool addRight;
uniform bool addCenter;

vec2 position;

// it takes center textures and checks if any active one is in the spot and then displays these parts of main texture
void main( void ){

	vec2 position = vec2( gl_FragCoord.x / resolution.x, gl_FragCoord.y / resolution.y );

	bool showTarget = false;

	if (addRight == true && texture2D(rightTexture, position).r > 0.0){
		showTarget = true;
	}
	else if (addLeft == true && texture2D(leftTexture, position).r > 0.0){
		showTarget = true;
	}
	else if (addDown == true && texture2D(downTexture, position).r > 0.0){
		showTarget = true;
	}
	else if (addUp == true && texture2D(upTexture, position).r > 0.0){
		showTarget = true;
	}
	else if (addCenter == true && texture2D(centerTexture, position).r > 0.0){
		showTarget = true;
	}

	if (showTarget == true){
		gl_FragColor = texture2D(targetTexture, position);
	}
	else{
		gl_FragColor = vec4(0,0,0,0);
	}
}