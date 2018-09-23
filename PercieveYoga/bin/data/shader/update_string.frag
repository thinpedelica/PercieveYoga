#version 330

// ping pong inputs
uniform sampler2DRect particles0;
uniform sampler2DRect particles1;

uniform vec2 start_pos;
uniform float delta;

in vec2 texCoordVarying;

layout(location = 0) out vec4 outVals1;
layout(location = 1) out vec4 outVals2;

float interpolate(float progress) {
    return 1.0 - pow(1.0 - progress, 2.0);
}

float updateProgress(float progress, float delta, float duration) {
    float update_val = progress + delta / duration;
    update_val = update_val >= 1.0 ? 1.0 : update_val;

    return update_val;
}

void main() {
    vec2 end_pos     = texture(particles0, texCoordVarying.st).xy;
    vec2 anchor      = texture(particles0, texCoordVarying.st).zw;
    vec2 current_pos = texture(particles1, texCoordVarying.st).xy;
    float progress   = texture(particles1, texCoordVarying.st).z;
    float duration   = texture(particles1, texCoordVarying.st).w;

    if (progress < 0.0) discard;
    progress = updateProgress(progress, delta, duration);
    float fi = interpolate(progress);

    current_pos.x = mix(mix(start_pos.x, anchor.x, fi), mix(anchor.x, end_pos.x, fi), fi);
    current_pos.y = mix(mix(start_pos.y, anchor.y, fi), mix(anchor.y, end_pos.y, fi), fi);

    outVals1 = vec4(end_pos, anchor);
    outVals2 = vec4(current_pos, progress, duration);
}
