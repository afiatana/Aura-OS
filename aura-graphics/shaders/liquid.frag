#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 0) out vec4 outColor;

layout(push_constant) uniform PushConstants {
    float time;
} push;

void main() {
    // Coordinate normalization
    vec2 uv = gl_FragCoord.xy / vec2(800.0, 600.0);
    uv = uv * 2.0 - 1.0;
    uv.x *= 800.0 / 600.0; // Aspect ratio correction
    
    // Warping logic to make it look "liquid/liat"
    for(float i = 1.0; i < 4.0; i++) {
        uv.x += 0.3 / i * sin(i * 3.0 * uv.y + push.time);
        uv.y += 0.3 / i * cos(i * 3.0 * uv.x + push.time);
    }
    
    float d = length(uv);
    
    // Smooth threshold for the organic blob (gooey effect)
    float mask = smoothstep(0.7, 0.68, d);
    
    // Premium Color Palette: Aura Blue to Cosmic Purple
    vec3 auraBlue = vec3(0.1, 0.5, 1.0);
    vec3 cosmicPurple = vec3(0.6, 0.2, 1.0);
    vec3 mixedColor = mix(auraBlue, cosmicPurple, 0.5 + 0.5 * sin(push.time * 0.5 + d));
    
    // Add Electric Teal highlight
    mixedColor = mix(mixedColor, vec3(0.0, 1.0, 0.8), pow(max(0.0, 0.5 - d), 3.0));
    
    if (mask > 0.0) {
        // Dynamic glow and pulse
        float glow = 0.1 / d;
        outColor = vec4(mixedColor * mask + mixedColor * glow * 0.5, mask);
    } else {
        discard;
    }
}
