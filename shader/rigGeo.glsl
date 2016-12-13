#version 330
layout(lines) in;
layout(triangle_strip, max_vertices = 18) out;

in vec3 vertColour[2];
out vec3 gVertColour;

void main()
{
    vec3 norm = cross(gl_in[0].gl_Position.xyz, gl_in[1].gl_Position.xyz);
    float projZ = dot(norm, vec3(0.0, 0.0, 1.0));
    vec3 projVec = norm - (projZ*vec3(0.0, 0.0, 1.0));
    vec4 boneStartOffset = 8.5 * normalize(vec4(projVec, 0.0));
    vec4 boneEndOffset = 0.1 * boneStartOffset;

    vec4 jointOffsetTR = 9.9f*vec4(1.0, 1.0, 0.0, 0.0);
    vec4 jointOffsetBR = 9.9f*vec4(1.0, -1.0, 0.0, 0.0);
    vec4 jointOffsetTL = 9.9f*vec4(-1.0, 1.0, 0.0, 0.0);
    vec4 jointOffsetBL = 9.9f*vec4(-1.0, -1.0, 0.0, 0.0);

    // Top joint
    gl_Position = gl_in[0].gl_Position + jointOffsetBL;    // bottom left
    gVertColour = vertColour[0];
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + jointOffsetTL;    // top left
    gVertColour = vertColour[0];
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + jointOffsetTR;    // top right
    gVertColour = vertColour[0];
    EmitVertex();

    gl_Position = gl_in[0].gl_Position +jointOffsetBL;    // bottom left
    gVertColour = vertColour[0];
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + jointOffsetTR;    // top right
    gVertColour = vertColour[0];
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + jointOffsetBR;    // bottom right
    gVertColour = vertColour[0];
    EmitVertex();


    EndPrimitive();


    // Bone
    gl_Position = gl_in[1].gl_Position - boneEndOffset;    // bottom left
    gVertColour = vertColour[1];
    EmitVertex();

    gl_Position = gl_in[0].gl_Position - boneStartOffset;    // top left
    gVertColour = vertColour[0];
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + boneStartOffset;    // top right
    gVertColour = vertColour[0];
    EmitVertex();

    gl_Position = gl_in[1].gl_Position - boneEndOffset;    // bottom left
    gVertColour = vertColour[1];
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + boneStartOffset;    // top right
    gVertColour = vertColour[0];
    EmitVertex();

    gl_Position = gl_in[1].gl_Position + boneEndOffset;    // bottom right
    gVertColour = vertColour[1];
    EmitVertex();


    EndPrimitive();



    // Bottom joint
    gl_Position = gl_in[1].gl_Position + jointOffsetBL;    // bottom left
    gVertColour = vertColour[1];
    EmitVertex();

    gl_Position = gl_in[1].gl_Position + jointOffsetTL;    // top left
    gVertColour = vertColour[1];
    EmitVertex();

    gl_Position = gl_in[1].gl_Position + jointOffsetTR;    // top right
    gVertColour = vertColour[1];
    EmitVertex();

    gl_Position = gl_in[1].gl_Position +jointOffsetBL;    // bottom left
    gVertColour = vertColour[1];
    EmitVertex();

    gl_Position = gl_in[1].gl_Position + jointOffsetTR;    // top right
    gVertColour = vertColour[1];
    EmitVertex();

    gl_Position = gl_in[1].gl_Position + jointOffsetBR;    // bottom right
    gVertColour = vertColour[1];
    EmitVertex();


    EndPrimitive();
}
