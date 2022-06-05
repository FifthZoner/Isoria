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

// 1 / step amount
uniform float heightStep;

// res / res + 2 * additional
uniform vec2 range;

//		MAIN PART

// local things not uniforms
vec2 position;
vec2 shadePosition;

// returns position on shade texture, might delete later for performance
vec2 texturePosition( ){
	return lowerLimit + (position.xy * range);
}

vec4 getColor(){
	
	if (texture2D(shade, shadePosition).r > 0.0){
		return shadeColor;
	}

	return sunColor;
}



void main( void ) {

	// gets position relative to screen
	position = gl_FragCoord.xy / resolution.xy;

	// gets position relative to extended shadow screen
	shadePosition = texturePosition();

	gl_FragColor = getColor();

}