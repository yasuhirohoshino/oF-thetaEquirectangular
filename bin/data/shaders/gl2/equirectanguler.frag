// based on http://qiita.com/zinziroge/items/3676b4e0f7715fa60336

#version 120
#define PI 3.14159265359

struct CameraParams{
    float radius[2];
    vec2 centerPos[2];
    float rotate[2];
};

uniform sampler2DRect source;
uniform vec2 resolution;
uniform float blendingRatio; // 1.0=no alpha blending
uniform CameraParams cam[2];
uniform float translate;

mat3 createRotMatrix(float ang_x, float ang_y, float ang_z)
{
    mat3 rot_mat_x = mat3(
                          1,           0,          0,
                          0,  cos(ang_x), sin(ang_x),
                          0, -sin(ang_x), cos(ang_x));
    mat3 rot_mat_y = mat3(
                          cos(ang_y), 0, -sin(ang_y),
                          0, 1,           0,
                          sin(ang_y), 0,  cos(ang_y));
    mat3 rot_mat_z = mat3(
                          cos(ang_z), sin(ang_z), 0,
                          -sin(ang_z), cos(ang_z), 0,
                          0,          0, 1);
    mat3 rot;
    rot = rot_mat_y * rot_mat_x;
    rot = rot_mat_z * rot;
    return rot;
}

vec4 getBlendedPixel(float u, float v, int witchCam, float thresholdRadius, vec3 sphere, float mixValue, float blending)
{
    vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
    float dist = 0.0;
    float radius = 0.0;
    float alpha = 0.0;
    float blendU = 0.0;
    float blendV = 0.0;
    float phi = 0.0;
    
    float radius1 = mix(cam[0].radius[1], cam[0].radius[0], mixValue);
    vec2 center1 = mix(cam[0].centerPos[1], cam[0].centerPos[0], vec2(mixValue));
    float rot1 = mix(cam[0].rotate[1], cam[0].rotate[0], mixValue);
    
    float radius2 = mix(cam[1].radius[0], cam[1].radius[1], mixValue);
    vec2 center2 = mix(cam[1].centerPos[0], cam[1].centerPos[1], vec2(mixValue));
    float rot2 = mix(cam[1].rotate[0], cam[1].rotate[1], mixValue);
    
    if( witchCam == 0 ) {
        dist = thresholdRadius - radius2 * blending;
        if ( dist < 0 ) {
            color = texture2DRect(source, vec2(u, v));
        }
        else {
            alpha = clamp(1.0 - 0.5 * dist / (radius1 * (1.0 - blending)), 0.0, 1.0);
            radius = radius1 + (radius2 * (1.0 - blending) - dist);
            phi = atan(-sphere.x , sphere.z) - rot2;
            blendU = center2.x + radius * cos(phi);
            blendV = center2.y + radius * sin(phi);
            vec4 baseColor = texture2DRect(source, clamp(vec2(u, v), vec2(0.0), resolution));
            vec4 blendColor = texture2DRect(source, vec2(blendU, blendV));
            color = alpha * baseColor + (1.0 - alpha) * blendColor;
        }
    } else {
        dist = thresholdRadius - radius1 * blending;
        if ( dist < 0 ) {
            color = texture2DRect(source, vec2(u, v));
        }
        else {
            alpha = clamp(1.0 - 0.5 * dist / (radius2 * (1.0 - blending)), 0.0, 1.0);
            radius = radius2 + (radius1 * (1.0 - blending) - dist);
            phi = atan(-sphere.x , -sphere.z) - rot1;
            blendU = center1.x + radius * cos(phi);
            blendV = center1.y + radius * sin(phi);
            vec4 baseColor = texture2DRect(source, vec2(blendU, blendV));
            vec4 blendColor = texture2DRect(source, clamp(vec2(u, v), vec2(0.0), resolution));
            color = (1.0 - alpha) * baseColor + alpha * blendColor;
        }
    }
    
    return color;
}

void main (void) {
    float x = mod(translate + resolution.x - gl_FragCoord.x, resolution.x);
    float y = resolution.y - gl_FragCoord.y;
    float xTransition = 0.5 + 0.5 * sin(-PI + 2 * PI * ((gl_FragCoord.x / resolution.x)));
    
    //equi_xy_to_lnglat
    float lat = -PI / 2.0 + y / resolution.y * PI;
    float lng = -PI + 2.0 * PI * x / resolution.x;
    
    //lnglat_to_sph
    vec3 sph_1 = vec3(cos(lat) * cos(lng), sin(lat), -cos(lat) * sin(lng));
    
    // sph_1 -> sph_2
    mat3 rot_base = createRotMatrix(PI/2, 0.0, -PI/2);
    vec3 sph_2 = rot_base * sph_1;
    
    float theta;
    if( abs(sph_2.y) >= 1.0 ) {
        theta = 0;
    } else {
        theta = acos(abs(sph_2.y));
    }
    
    if( sph_2.y >= 0.0 ) {
        float radius = mix(cam[0].radius[1], cam[0].radius[0], xTransition);
        vec2 center = mix(cam[0].centerPos[1], cam[0].centerPos[0], vec2(xTransition));
        float rot = mix(cam[0].rotate[1], cam[0].rotate[0], xTransition);
        
        float phi = atan(-sph_2.x, -sph_2.z) - rot;
        radius = radius * 1.41421356 * abs(sin(theta / 2.0));
        float u = center.x + radius * cos(phi);
        float v = center.y + radius * sin(phi);
        float blending = clamp(blendingRatio, 0.0001, 0.99999);
        gl_FragColor = getBlendedPixel(u, v, 0, radius, sph_2, xTransition, blending);
    } else {
        float radius = mix(cam[1].radius[0], cam[1].radius[1], xTransition);
        vec2 center = mix(cam[1].centerPos[0], cam[1].centerPos[1], vec2(xTransition));
        float rot = mix(cam[1].rotate[0], cam[1].rotate[1], xTransition);
        
        float phi = atan(-sph_2.x,  sph_2.z) - rot;
        radius = radius * 1.41421356 * abs(sin(theta / 2.0));
        float u = center.x + radius * cos(phi);
        float v = center.y + radius * sin(phi);
        float blending = clamp(blendingRatio, 0.0001, 0.99999);
        gl_FragColor = getBlendedPixel(u, v, 1, radius, sph_2, xTransition, blending);
    }
}