#version 330 core

uniform vec2 i_resolution;
uniform float i_dt;
uniform sampler2D tex_prev;

out vec4 fragColor;

vec3 hsv2rgb(vec3 c) {
  vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
  vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
  return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main() {
	vec2 pos = gl_FragCoord.xy / i_resolution.xy;
	if(pos.x <= 10. / i_resolution.x || pos.y <= 10. / i_resolution.y || pos.x >= 1.0 - 1.1 / i_resolution.x || pos.y >= 1.0 - 1.1 / i_resolution.y) {
		fragColor = vec4(0);
		return;
	}

	vec3 current = texture2D(tex_prev, pos).xyz * 0.99;

	vec2 dropDistance = pos - vec2(0.5);

	float smoke = max(0, 0.04 - dot(dropDistance, dropDistance)) * i_dt * 16;

	fragColor = vec4(current + vec3(smoke), 1);

	//fragColor = vec4(pos.x, pos.y, 0, 1);
}
