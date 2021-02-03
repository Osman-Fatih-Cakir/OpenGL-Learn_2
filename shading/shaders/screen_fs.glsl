#version 330 core

in vec2 fTexCoord;

out vec4 out_color;

uniform sampler2D screen_texture;
uniform int is_blur;

float offset = 1.0 / 300.0;

void main()
{
	vec4 last_color = vec4(0.0);
	if (is_blur == 0) // Normal screen
	{
		last_color = texture(screen_texture, fTexCoord);
	}
	// Blurred screen
	else 
	{
		// Kernel matrix
		float kernel[9] = float[](
			1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0,
            2.0 / 16.0, 4.0 / 16.0, 2.0 / 16.0,
            1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0  
		);

		// Screen pixel locations
		vec2 offsets[9] = vec2[](
			vec2(-offset,  offset), // top-left
            vec2( 0.0f,    offset), // top-center
            vec2( offset,  offset), // top-right
            vec2(-offset,  0.0f),   // center-left
            vec2( 0.0f,    0.0f),   // center-center
            vec2( offset,  0.0f),   // center-right
            vec2(-offset, -offset), // bottom-left
            vec2( 0.0f,   -offset), // bottom-center
            vec2( offset, -offset)  // bottom-right  
		);

		vec3 sampleTex[9];
        for(int i = 0; i < 9; i++) // Take all pixel colors around a pixel
            sampleTex[i] = vec3(texture(screen_texture, fTexCoord.st + offsets[i]));

		vec3 color = vec3(0.0);
        for(int i = 0; i < 9; i++) // Multiply with kernels and add all colors
            color += sampleTex[i] * kernel[i];

		last_color = vec4(color, 1.0);
	}
	
	out_color = last_color;
}