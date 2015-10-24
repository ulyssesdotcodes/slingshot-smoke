
#version 330 core

uniform vec2 i_resolution;
uniform float i_dt;
uniform float i_time;

uniform sampler2D tex_prev;

out vec4 fragColor;

void main() {
	vec2 pos = gl_FragCoord.xy / i_resolution.xy;

	vec4 current = texture2D(tex_prev, pos);
	current.x *= (1.0 - 0.8 * i_dt);
	current.y *= (1.0 - 0.05 * i_dt);

	fragColor = current;
}
