#version 330

// ping pong inputs
uniform sampler2DRect particles0;
uniform sampler2DRect particles1;

uniform vec3 target_pos;
uniform float radiusSquared;
uniform float elapsed;
uniform float resolution_x;
uniform float resolution_y;

in vec2 texCoordVarying;

layout(location = 0) out vec4 posOut;
layout(location = 1) out vec4 velOut;

void main() {
    vec3 pos = texture(particles0, texCoordVarying.st).xyz;
    vec3 vel = texture(particles1, texCoordVarying.st).xyz;

    // pos
    vec3 direction    = target_pos - pos.xyz;
    float distSquared = dot(direction, direction);
    float magnitude   = 500.0 * (1.0 - distSquared / radiusSquared);
    vec3 force        = step(distSquared, radiusSquared) * magnitude * normalize(direction);

    // gravity
    force += vec3(0.0, -0.5, 0.0);

    // accelerate
    vel += elapsed * force;

    // bounce off the sides
    vel.x *= step(abs(pos.x), resolution_x * 0.5) * 2.0 - 1.0;
    vel.y *= step(abs(pos.y), resolution_y * 0.5) * 2.0 - 1.0;

    // damping
    vel *= 0.995;

    // move
    pos += elapsed * vel;

    posOut = vec4(pos, 1.0);
    velOut = vec4(vel, 0.0);
}
