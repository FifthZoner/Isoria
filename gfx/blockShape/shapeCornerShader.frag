#extension GL_OES_standard_derivatives : enable

precision highp float;

// texture that is to be changed
uniform sampler2D targetTexture;
uniform sampler2D targetShadeTexture;
// textures from the blockShapeVariant that is to be the base of processing the block
uniform sampler2D upLeftTexture;
uniform sampler2D upRightTexture;
uniform sampler2D downLeftTexture;
uniform sampler2D downRightTexture;


// this is block resolution, not screen
uniform vec2 resolution;

uniform bool addUpRight;
uniform bool addDownRight;
uniform bool addDownLeft;
uniform bool addUpLeft;

uniform bool isShadow;

// it takes corner textures and checks if any active one is in the spot and then displays these parts of main texture
void main( void ){

	vec2 position = gl_FragCoord.xy / resolution.xy;

	bool showTarget = false;

	if (addUpRight == true && texture2D(upRightTexture, position).r > 0.0){
		showTarget = true;
	}
	else if (addUpLeft == true && texture2D(upLeftTexture, position).r > 0.0){
		showTarget = true;
	}
	else if (addDownRight == true && texture2D(downRightTexture, position).r > 0.0){
		showTarget = true;
	}
	else if (addDownLeft == true && texture2D(downLeftTexture, position).r > 0.0){
		showTarget = true;
	}

	if (showTarget == true){
		if (isShadow == true){
			gl_FragColor = texture2D(targetShadeTexture, position);
		}
		else{
			gl_FragColor = texture2D(targetTexture, position);
		}
	}
	else{
		gl_FragColor = vec4(0,0,0,0);
	}
}