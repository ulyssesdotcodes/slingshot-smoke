#version 330 core

uniform vec2 i_resolution;
uniform float i_dt;
uniform float i_time;
uniform float i_volume;
uniform vec2 i_smokePosition;
uniform sampler2D tex_prev;
uniform float i_fullness;

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

	vec4 current = texture2D(tex_prev, pos);
	current.x *= (1.0 - 0.8 * i_dt);
	current.y *= (1.0 - 0.05 * i_dt);

	vec2 dropDistance = pos - i_smokePosition;

	float density = max(0, 0.005 * i_fullness - dot(dropDistance, dropDistance)) * i_dt * i_volume * rand(vec2(i_time * pos.y, pos.x));

	float temperature = current.y + density * 2048;

	float hue = current.z;
	if(density > 0.0001) {
		hue = -abs(mod(i_time, 0.5) - 0.25);
	}

	fragColor = vec4(current.x + density, temperature, hue, 1.0);
}
