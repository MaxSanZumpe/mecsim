#version 330 core

in vec3 fragPos;
in vec3 ourColor;
in vec3 center;
in float radius;

out vec4 FragColor;

void main()
{
    float dist = length(fragPos.xy - center.xy);

    // Soft edge range
    float edgeWidth = 0.01;
    float edge0 = radius - edgeWidth;
    float edge1 = radius;

    if (dist > edge1) {
         discard;
    }

    float minAlpha = 0.1;
    float t = smoothstep(edge0, edge1, dist);
    float alpha = mix(1.0, minAlpha, t); // 1.0 at center, minAlpha at edge
    FragColor = vec4(ourColor, alpha);
}