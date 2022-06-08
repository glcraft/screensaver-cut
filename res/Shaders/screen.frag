#version 330 core

struct Line
{
    vec3 color;
    vec2 pos, dir;
    float overture;
};
// =>ro+rd*x
in vec2 currentPosition;

uniform float time;
uniform float ratio;
uniform float invratio;
uniform Line line;
uniform float sizeLine;
uniform int isTexture;
uniform sampler2D Texture0;


out vec4 outColor;

float rand(float n){return fract(sin(n) * 43758.5453123);}

float rand(vec2 n) { 
	return fract(sin(dot(n, vec2(12.9898, 4.1414))) * 43758.5453);
}

float noise(float p)
{
    float fl = floor(p);
    float fc = fract(p);
    return mix(rand(fl), rand(fl + 1.0), fc);
}
    
float noise(vec2 n) {
    const vec2 d = vec2(0.0, 1.0);
    vec2 b = floor(n), f = smoothstep(vec2(0.0), vec2(1.0), fract(n));
    return mix(mix(rand(b), rand(b + d.yx), f.x), mix(rand(b + d.xy), rand(b + d.yy), f.x), f.y);
}

float noise_sub(float p, int numb)
{
    float coef = 1., totalcoef=0., total = 0., size=1.;
    for(int i=0;i<numb;i++)
    {
        total+=noise(p*size)*coef;
        totalcoef+=coef;
        coef*=0.7;
        size*=2.;
    }
    return total/=totalcoef;
}
void main()
{
    float col = 0.;
    
    vec2 invDir = vec2(-line.dir.y, line.dir.x);
    float distOnLine = dot(line.dir,(currentPosition-line.pos));
    vec2 displacement=((invDir)*noise_sub(distOnLine*10., 4)*0.04);
    
    vec2 cpos = currentPosition*vec2(ratio, 1.)+displacement;
    float distOfLine = dot(invDir,(cpos-line.pos));
    vec2 uv=currentPosition*0.5+0.5;
    uv+=(distOfLine>0 ? -invDir : invDir)*(time/2)*line.overture*vec2(invratio, 1.);

    col = clamp(1.-abs(distOfLine*sizeLine)+(time*sizeLine)*line.overture, 0., 2.);
    vec3 coltex = texture(Texture0, uv).xyz;
    vec3 finalCol;
    if (col>=1.)
        finalCol = vec3(clamp((1-(col-1)*5), 0, 1))*(line.color+coltex);
    else
        finalCol = vec3(col)*line.color+coltex;

    outColor = vec4(finalCol,1.);
}