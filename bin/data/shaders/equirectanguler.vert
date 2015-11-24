#version 400

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 textureMatrix;
uniform mat4 modelViewProjectionMatrix;

in vec4  position;
in vec4  color;
in vec3  normal;
in vec2  texcoord;

out vec2 texCoordVarying;

void main() {
    texCoordVarying = texcoord;
    gl_Position = modelViewProjectionMatrix * position;
}