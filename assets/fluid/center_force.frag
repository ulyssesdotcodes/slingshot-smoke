#version 330 core

uniform vec2 i_resolution;
uniform sampler2D tex_velocity;
uniform sampler2D tex_pressure;

uniform float i_time;
uniform float i_dt;

out vec4 fragColor;

vec4 boundary(vec2 pos) {
	vec2 offset = vec2(0, 0);

	if(pos.x <= 1. / i_resolution.x) {
		offset.x = 1/i_resolution.x;
	}
	else if(pos.x * i_resolution.x >= i_resolution.x - 1) {
		offset.x = -1/i_resolution.x;
	}

	if(pos.y <= 1. / i_resolution.y) {
		offset.y = 1/i_resolution.y;
	}
	else if(pos.y * i_resolution.y >= i_resolution.y - 1) {
		offset.y = -1/i_resolution.y;
	}

	return vec4(texture2D(tex_velocity, pos + offset).xy, 1.0, 1);
}

vec4 inner(vec2 pos) {
	vec4 velocity = texture2D(tex_velocity, pos);
	vec2 v = velocity.xy * (1.0 - 1.0 * i_dt);

	// Apply force away from the center
	vec2 vel = pos - vec2(0.5);
	if(vel.x > 0.04 || vel.y > 0.04) {
		return vec4(v, velocity.z, 1);
	}
	vel.y *= -1;
	vel.x *= 2.0 * (0.5 - cos(i_time));
	v += vec2(1 * vel.x, 0 * vel.y) * i_dt;

	return vec4(v, velocity.z, 1);
}

void main() {
	vec4 outVelocity;
	vec2 pos = gl_FragCoord.xy / i_resolution.xy;
	if(pos.x <= 1. / i_resolution.x || pos.y <= 1. / i_resolution.y || pos.x >= 1.0 - 1.1 / i_resolution.x || pos.y >= 1.0 - 1.1 / i_resolution.y) {
		outVelocity = boundary(pos);
	}
	else {
		outVelocity = inner(pos);
	}

	fragColor = outVelocity;
}
