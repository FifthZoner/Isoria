#extension GL_OES_standard_derivatives : enable

precision highp float;

// window resolution
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



// returns position on shade texture, might delete later for performance
//vec2 texturePosition( ){
//	return lowerLimit + (position * range);
//}




void main( void ) {

	vec2 position = ( gl_FragCoord.xy / resolution.xy );

	

	gl_FragColor = vec4(position.x,position.y,0.,1.);

	
	
}