#version 410 core
in vec2 UV; //190
out vec4 fragColor;
out vec3 color; //190
uniform sampler2D myTextureSampler; //190
void main(void) {
	color = texture( myTextureSampler, UV).rgb;
    fragColor = vec4(color, 1.0);
}