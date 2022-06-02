#extension GL_OES_standard_derivatives : enable

precision highp float;

// window resolution
uniform vec2 resolution;
// additional / 2 * additional + resolution
uniform vec2 lowerLimit;
// additional + resolution / 2 * additional + resolution
uniform vec2 upperLimit;
// resolution / resolution + 2 * additional
uniform float range;
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


vec2 position;

// returns position on shade texture, might delete later for performance
vec2 texturePosition( ){
	return lowerLimit + (position * range);
}


void main( void ) {

	position = ( gl_FragCoord.xy / resolution.xy );
	
	float currentHeight = 0;
	for (int n = 0; n < steps; n = n + 1){
		if ()
	}



	gl_FragColor = sunColor;

}