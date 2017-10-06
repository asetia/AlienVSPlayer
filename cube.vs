#version 410 core
uniform mat4 ProjectionMatrix = mat4(1);
uniform mat4 CameraMatrix = mat4(1);
uniform mat4 InstanceTransform = mat4(1);
layout(location = 0) in vec4 Position;
layout(location = 1) in vec2 vertexUV; //190
out vec3 vertNormal;
out vec2 UV; //190
void main(void) {
	UV = vertexUV; //190
   mat4 ViewXfm = CameraMatrix * InstanceTransform;
   //mat4 ViewXfm = CameraMatrix;
   gl_Position = ProjectionMatrix * ViewXfm * Position;
}