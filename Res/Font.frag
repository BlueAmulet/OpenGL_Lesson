#version 410
 layout(location=0) in vec2 inTexCoord;
 layout(location=1) in vec4 inColor;

 out vec4 fragColor;
 uniform sampler2D colorSampler;
 const float smoothing = 1.0f / 16.0f;
 void main(){
	float distance = texture(colorSampler, inTexCoord).r;
	float alpha = smoothstep(0.5 + smoothing, 0.5 - smoothing, distance);
	fragColor = vec4(inColor.rgb, inColor.a * alpha);
 }