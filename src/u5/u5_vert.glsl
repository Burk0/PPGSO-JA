#version 150
// Get vertex position from the vertex buffer
in vec2 Position;
in vec3 Color;

// This will be passed on to the fragment shader
out vec3 VertexColor;
uniform mat3 Transform;

void main() {
  VertexColor = Color;
  // The vertex shader needs to output a vec4 gl_Position so we fill in z and w to comply
  gl_Position =  vec4(Transform * vec3(Position, 1.0), 1.0);
//  gl_Position = vec4(Position, 0.0, 1.0);
}
