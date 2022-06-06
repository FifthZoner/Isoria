#extension GL_OES_standard_derivatives : enable

precision highp float;

// window resolution	 AS A F****** VECTOR2F! or you'll lose 2 days on debug
uniform vec2 resolution;

// additional / 2 * additional + resolution
uniform vec2 lowerLimit;

// additional + resolution / 2 * additional + resolution
uniform vec2 upperLimit;

// 25 * max vertical ratio
uniform float steps;

// current vertical ratios
uniform vec2 angle;

// shade texture
uniform sampler2D shade;

// shadow color
uniform vec4 shadeColor;

// light color
uniform vec4 sunColor;

// 1 / res
uniform vec2 mainStep;

// 1 / shadow res
uniform vec2 shadeStep;

// 250 / 255 / 25 / maxAngle
uniform float heightStep;

// res / res + 2 * additional
uniform vec2 range;

// angle * shadeStep / maxAngle;
uniform vec2 moveValue;

//		MAIN PART

// local things not uniforms
vec2 position;
vec2 shadePosition, initialShadePosition;

// returns position on shade texture, might delete later for performance
vec2 texturePosition( ){
	return lowerLimit + (position.xy * range);
}

vec4 getColor(){

float currentR = texture2D(shade, shadePosition).r;
	
	while (currentR <= 2.5 / 2.55){
		if(texture2D(shade, shadePosition).r > currentR){
			return shadeColor;
		}

		// increases curent height and moves by specified amount
		currentR += heightStep;
		shadePosition += moveValue;
	}

	return sunColor;
}



void main( void ) {

	// gets position relative to screen
	position = gl_FragCoord.xy / resolution.xy;

	// gets position relative to extended shadow screen
	shadePosition = texturePosition();
	initialShadePosition = shadePosition;

	gl_FragColor = getColor();

}