#version 410 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 8) out;

in VS_OUT
{
    vec2 texCoord;
}
gs_in[];

out vec2 texCoord;

// p1.x < p2.x인 두 점을 이은 직선을 ratio.x:ratio.y의 비율로 내분하는 점을 반환.
vec2 interiorDivide(vec2 ratio, vec2 p1, vec2 p2)
{
    vec2 res;
    res.x = (p1.x * ratio.y + p2.x * ratio.x) / (ratio.x + ratio.y);
    res.y = (p1.y * ratio.y + p2.y * ratio.x) / (ratio.x + ratio.y);
    return res;
}

// Assume that p1 < pivot < p2
vec2 getRatio(float pivot, float p1, float p2)
{
    return vec2(pivot - p1, p2 - pivot);
}

void main()
{

    gl_Position = gl_in[0].gl_Position;
    texCoord = gs_in[0].texCoord;
    EmitVertex();

    bool out1 = gl_in[1].gl_Position.x > 1.0;
    bool out2 = gl_in[2].gl_Position.x > 1.0;
    vec2 intersectPos1, intersectPos2;
    vec2 intersectTex1, intersectTex2;
    vec2 ratio;
    if (out1 && out2)
    {
        // inside
        ratio = getRatio(1.0, gl_in[0].gl_Position.x, gl_in[1].gl_Position.x);
        intersectPos1 = interiorDivide(ratio, gl_in[0].gl_Position.xy, gl_in[1].gl_Position.xy);
        intersectTex1 = interiorDivide(ratio, gs_in[0].texCoord, gs_in[1].texCoord);
        gl_Position = vec4(intersectPos1, 0, 1);
        texCoord = intersectTex1;
        EmitVertex();

        ratio = getRatio(1.0, gl_in[0].gl_Position.x, gl_in[2].gl_Position.x);
        intersectPos2 = interiorDivide(ratio, gl_in[0].gl_Position.xy, gl_in[2].gl_Position.xy);
        intersectTex2 = interiorDivide(ratio, gs_in[0].texCoord, gs_in[2].texCoord);
        gl_Position = vec4(intersectPos2, 0, 1);
        texCoord = intersectTex2;
        EmitVertex();
        EndPrimitive();

        // outside
        gl_Position = vec4(-1, intersectPos1.y, 0, 1);
        texCoord = intersectTex1;
        EmitVertex();
        gl_Position = gl_in[1].gl_Position - vec4(2, 0, 0, 0);
        texCoord = gs_in[1].texCoord;
        EmitVertex();
        gl_Position = vec4(-1, intersectPos2.y, 0, 1);
        texCoord = intersectTex2;
        EmitVertex();
        gl_Position = gl_in[2].gl_Position - vec4(2, 0, 0, 0);
        texCoord = gs_in[2].texCoord;
        EmitVertex();
        EndPrimitive();
    }
    else if (out1)
    {
        // inside
        ratio = getRatio(1.0, gl_in[0].gl_Position.x, gl_in[1].gl_Position.x);
        intersectPos1 = interiorDivide(ratio, gl_in[0].gl_Position.xy, gl_in[1].gl_Position.xy);
        intersectTex1 = interiorDivide(ratio, gs_in[0].texCoord, gs_in[1].texCoord);
        gl_Position = vec4(intersectPos1, 0, 1);
        texCoord = intersectTex1;
        EmitVertex();

        gl_Position = gl_in[2].gl_Position;
        texCoord = gs_in[2].texCoord;
        EmitVertex();

        ratio = getRatio(1.0, gl_in[2].gl_Position.x, gl_in[1].gl_Position.x);
        intersectPos2 = interiorDivide(ratio, gl_in[2].gl_Position.xy, gl_in[1].gl_Position.xy);
        intersectTex2 = interiorDivide(ratio, gs_in[2].texCoord, gs_in[1].texCoord);
        gl_Position = vec4(intersectPos2, 0, 1);
        texCoord = intersectTex2;
        EmitVertex();
        EndPrimitive();

        // outside
        gl_Position = vec4(-1, intersectPos1.y, 0, 1);
        texCoord = intersectTex1;
        EmitVertex();
        gl_Position = gl_in[1].gl_Position - vec4(2, 0, 0, 0);
        texCoord = gs_in[1].texCoord;
        EmitVertex();
        gl_Position = vec4(-1, intersectPos2.y, 0, 1);
        texCoord = intersectTex2;
        EmitVertex();
        EndPrimitive();
    }
    else
    {

        gl_Position = gl_in[1].gl_Position;
        texCoord = gs_in[1].texCoord;
        EmitVertex();
        gl_Position = gl_in[2].gl_Position;
        texCoord = gs_in[2].texCoord;
        EmitVertex();
        EndPrimitive();
    }
}