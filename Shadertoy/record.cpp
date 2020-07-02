float Circle(vec2 uv, vec2 p, float r, float blur)
{
    float d = length(uv - p);
    
    float c = smoothstep(r, r - blur, d);
    
    return c;
}

float Band(float t, float start, float end, float blur)
{
    float step1 = smoothstep(start - blur, start + blur, t);
    
    float step2 = smoothstep(end + blur, end - blur, t);
    
   	return step1 * step2;
}

float Rect(vec2 uv, float left, float right, float bottom, float top, float blur)
{
    float vertical = Band(uv.x, left, right, blur);
    
    float horizontal = Band(uv.y, bottom, top, blur);
    
    return vertical * horizontal;
}

float Smiley(vec2 uv, vec2 p, float size)
{
    uv -= p;
    
    uv /= size;
    
    float mask = Circle(uv, vec2(.0, .0), .4, .05);
    
    mask -= Circle(uv, vec2(-.13, .1), .07, .01);
    
    mask -= Circle(uv, vec2(.13, .1), .07, .01);
    
    float mouth = Circle(uv, vec2(.0, .01), .2, .02);
    
    mouth -= Circle(uv, vec2(.0, .1), .24, .02);
    
	mask -= mouth;
    
    return mask;
}


void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord / iResolution.xy;
    
    uv -= 0.5; // 0~1 -> -.5~.5
	
    uv.x *= iResolution.x / iResolution.y;
    
    // float mask = Smiley(uv, vec2(0.), .1);
    
    float mask = Rect(uv, -.2, .2, -.3, .3, .01);
    
    vec3 color = vec3(1., 1., 1.) * mask;

    fragColor = vec4(color, .0);
}
