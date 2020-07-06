#ifdef GL_ES
precision highp float;
#endif


// Because for some reason this isn't a part of glsl
float clamp01(float v) { return clamp(v, 0.,1.); }
vec2 clamp01(vec2 v) { return clamp(v, 0.,1.); }
vec3 clamp01(vec3 v) { return clamp(v, 0.,1.); }
vec4 clamp01(vec4 v) { return clamp(v, 0.,1.); }
vec2 uv;

uniform float time;
uniform vec2 resolution;

const vec2 ch_size  = vec2(1.0, 2.0);              // character size (X,Y)
const vec2 ch_space = ch_size + vec2(1.0, 1.0);    // character distance Vector(X,Y)
const vec2 ch_start = vec2 (0);
vec2 ch_pos   = vec2 (0.0, 0.0);             // character position(X,Y)

#define REPEAT_SIGN false // True/False; True=Multiple, False=Single

/* 16 segment display...Akin to LED Display.

Segment bit positions:
  __2__ __1__
 |\    |    /|
 | \   |   / |
 3  11 10 9  0
 |   \ | /   |
 |    \|/    |
  _12__ __8__
 |           |
 |    /|\    |
 4   / | \   7
 | 13 14  15 |
 | /   |   \ |
  __5__|__6__

15 12 11 8 7  4 3  0
 |  | |  | |  | |  |
 0000 0000 0000 0000

example: letter A

   12    8 7  4 3210
    |    | |  | ||||
 0001 0001 1001 1111

 binary to hex -> 0x119F
*/

#define n0 ddigit(0x22FF);
#define n1 ddigit(0x0281);
#define n2 ddigit(0x1177);
#define n3 ddigit(0x11E7);
#define n4 ddigit(0x5508);
#define n5 ddigit(0x11EE);
#define n6 ddigit(0x11FE);
#define n7 ddigit(0x2206);
#define n8 ddigit(0x11FF);
#define n9 ddigit(0x11EF);

#define A ddigit(0x119F);
#define B ddigit(0x927E);
#define C ddigit(0x007E);
#define D ddigit(0x44E7);
#define E ddigit(0x107E);
#define F ddigit(0x101E);
#define G ddigit(0x807E);
#define H ddigit(0x1199);
#define I ddigit(0x4466);
#define J ddigit(0x4436);
#define K ddigit(0x9218);
#define L ddigit(0x0078);
#define M ddigit(0x0A99);
#define N ddigit(0x8899);
#define O ddigit(0x00FF);
#define P ddigit(0x111F);
#define Q ddigit(0x80FF);
#define R ddigit(0x911F);
#define S ddigit(0x8866);
#define T ddigit(0x4406);
#define U ddigit(0x00F9);
#define V ddigit(0x2218);
#define W ddigit(0xA099);
#define X ddigit(0xAA00);
#define Y ddigit(0x4A00);
#define Z ddigit(0x2266);
#define _ ch_pos.x += ch_space.x;
#define _h ch_pos.x += ch_space.x * .5;

#define s_dot     ddigit(0);
#define s_minus   ddigit(0x1100);
#define s_plus    ddigit(0x5500);
#define s_greater ddigit(0x2800);
#define s_less    ddigit(0x8200);
#define s_sqrt    ddigit(0x0C02);


#define cr ch_pos.x = ch_start.x;
#define lf ch_pos.y -= 3.0;
#define lf_h ch_pos.y -= 1.0;

#define nl ch_pos.x = ch_start.x; ch_pos.y -= 3.0;
#define nl_h ch_pos.x = ch_start.x; ch_pos.y -= 1.0;

#define nl0 ch_pos = ch_start;
#define nl1 ch_pos = ch_start;  ch_pos.y -= 3.0;
#define nl2 ch_pos = ch_start;  ch_pos.y -= 6.0;
#define nl3 ch_pos = ch_start;	ch_pos.y -= 9.0;

float dseg(vec2 p0, vec2 p1)
{
        vec2 dir = normalize(p1 - p0);
        vec2 cp = (uv - ch_pos - p0) * mat2(dir.x, dir.y,-dir.y, dir.x);
        return distance(cp, clamp(cp, vec2(0), vec2(distance(p0, p1), 0)));
}

bool bit(int n, int b)
{
        return mod(floor(float(n) / exp2(floor(float(b)))), 2.0) != 0.0;
}

float d = 1e6;

void ddigit(int n)
{
        float v = 1e6;
        vec2 cp = uv - ch_pos;
        if (n == 0)     v = min(v, dseg(vec2(-0.405, -1.000), vec2(-0.500, -1.000)));
        if (bit(n,  0)) v = min(v, dseg(vec2( 0.500,  0.063), vec2( 0.500,  0.937)));
        if (bit(n,  1)) v = min(v, dseg(vec2( 0.438,  1.000), vec2( 0.063,  1.000)));
        if (bit(n,  2)) v = min(v, dseg(vec2(-0.063,  1.000), vec2(-0.438,  1.000)));
        if (bit(n,  3)) v = min(v, dseg(vec2(-0.500,  0.937), vec2(-0.500,  0.062)));
        if (bit(n,  4)) v = min(v, dseg(vec2(-0.500, -0.063), vec2(-0.500, -0.938)));
        if (bit(n,  5)) v = min(v, dseg(vec2(-0.438, -1.000), vec2(-0.063, -1.000)));
        if (bit(n,  6)) v = min(v, dseg(vec2( 0.063, -1.000), vec2( 0.438, -1.000)));
        if (bit(n,  7)) v = min(v, dseg(vec2( 0.500, -0.938), vec2( 0.500, -0.063)));
        if (bit(n,  8)) v = min(v, dseg(vec2( 0.063,  0.000), vec2( 0.438, -0.000)));
        if (bit(n,  9)) v = min(v, dseg(vec2( 0.063,  0.063), vec2( 0.438,  0.938)));
        if (bit(n, 10)) v = min(v, dseg(vec2( 0.000,  0.063), vec2( 0.000,  0.937)));
        if (bit(n, 11)) v = min(v, dseg(vec2(-0.063,  0.063), vec2(-0.438,  0.938)));
        if (bit(n, 12)) v = min(v, dseg(vec2(-0.438,  0.000), vec2(-0.063, -0.000)));
        if (bit(n, 13)) v = min(v, dseg(vec2(-0.063, -0.063), vec2(-0.438, -0.938)));
        if (bit(n, 14)) v = min(v, dseg(vec2( 0.000, -0.938), vec2( 0.000, -0.063)));
        if (bit(n, 15)) v = min(v, dseg(vec2( 0.063, -0.063), vec2( 0.438, -0.938)));
        ch_pos.x += ch_space.x;
        d = min(d, v);
}

const mat4 text = mat4(
        2.5, .5, 2.5, 2.5,
        2.5, .32, 1.5, 4.5,
        1.5, 11.2, 1.5, 2.5,
        1, 1, 6, .05
);


float cf(vec2 uv, vec4 p, float ts) {
        return (p.x + p.z + sin(ts*p.y*time+p.y*(uv.y+uv.x)) * p.z) * p.w;
}
vec3 cc(vec2 uv, mat4 m, float ts) {
        vec4 f = m[3];
        return abs(f.a *
                       vec3(f.r*cf(uv, m[0], ts),
                            f.g*cf(uv, m[1], ts),
                            f.b*cf(uv, m[2], ts)));
}
vec3 cc(vec2 uv, mat4 m) { return cc(uv, m, 1.0); }

vec3 dblend(vec3 c, float v, vec2 m) {
        return clamp01((1.0-m.x*v) * c)
                + clamp01(mix(c, vec3(0,0,0), 1.0 - (m.y / v)));
}



#define c_conj(a) vec2(a.x, -a.y)
#define c_exp(a) vec2(exp(a.x)*cos(a.y), exp(a.x)*sin(a.y))
#define c_sqr(a) vec2(a.x*a.x-a.y*a.y, 2.*a.x*a.y)
#define c_mul(a, b) vec2(a.x*b.x-a.y*b.y, a.x*b.y+a.y*b.x)
#define c_div(a, b) vec2((a.x*b.x+a.y*b.y)/(b.x*b.x+b.y*b.y), (a.y*b.x-a.x*b.y)/(b.x*b.x+b.y*b.y))
#define c_sin(a) vec2(sin(a.x)*cosh(a.y), cos(a.x)*sinh(a.y))
#define c_cos(a) vec2(cos(a.x)*cosh(a.y), -sin(a.x)*sinh(a.y))
#define c_cartToPolar(a) vec2(length(a), atan(a.y, a.x))
#define c_polarToCart(a) a.x * vec2(cos(a.y), sin(a.y))

void main() {
        //d = 1e6;
        vec2 aspect = resolution.xy / resolution.y;
        uv = ( gl_FragCoord.xy / resolution.y ) - aspect / 2.0;
        uv *= 30.0;


        // uv = c_conj(uv);	// cool - mirrored
        // uv = c_exp(uv);	// cool
        uv = c_sqr(uv);	// cool
        // uv = c_sqr(uv);	// cool
        uv *= .2;
        uv = c_mul(uv, vec2(sin(time),cos(time)));
        // uv = c_div(uv, vec2(sin(time),cos(time)));
        // - uv = c_sin(a);
        // - uv = c_cos(uv.x);
        // uv = c_cartToPolar(uv);
        uv = c_polarToCart(uv);	// cool


        ch_pos = vec2(0.0, 0.0);

        // Y O U _ A R E _ I N _ T H E _ M A T R I X nl N E O
        _ F O L L O W _ N E O


        vec3 textc = cc(uv, text);

        vec3 color = vec3(0, 0, 0);

        color += dblend(textc, d, vec2(10., .1));

        gl_FragColor = vec4(color, 1.0);
}




