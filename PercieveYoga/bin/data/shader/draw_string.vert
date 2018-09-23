#version 330

uniform mat4 modelViewProjectionMatrix;
uniform sampler2DRect particles0;
uniform sampler2DRect particles1;

in vec4  position;
in vec2  texcoord;

out float v_progress;

void main()
{
    v_progress  = texture(particles1, texcoord).z;
    gl_Position = modelViewProjectionMatrix * vec4(texture(particles1, texcoord).xy, 0.0, 1.0);
}
