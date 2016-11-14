#version 330
layout(lines) in;
layout(triangle_strip, max_vertices = 6) out;

in vec3 vertColour[2];
out vec3 gVertColour;

void main()
{
    vec4 offset = 0.5 * normalize(vec4(cross(gl_in[0].gl_Position.xyz, gl_in[1].gl_Position.xyz), 0.0));

    vec4 offsetTR = vec4(1.0, 1.0, 0.0, 0.0);
    vec4 offsetBR = vec4(1.0, -1.0, 0.0, 0.0);
    vec4 offsetTL = vec4(-1.0, 1.0, 0.0, 0.0);
    vec4 offsetBL = vec4(-1.0, -1.0, 0.0, 0.0);

    // Top joint
    /*
    gl_Position = gl_in[0].gl_Position + offsetBL;    // bottom left
    gVertColour = vertColour[0];
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + offsetTL;    // top left
    gVertColour = vertColour[0];
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + 2.0*offsetTR;    // top right
    gVertColour = vertColour[0];
    EmitVertex();

    gl_Position = gl_in[0].gl_Position +offsetBL;    // bottom left
    gVertColour = vertColour[0];
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + offsetTR;    // top right
    gVertColour = vertColour[0];
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + offsetBR;    // bottom right
    gVertColour = vertColour[0];
    EmitVertex();
    */


    // Bone
    gl_Position = gl_in[1].gl_Position - offset;    // bottom left
    gVertColour = vertColour[1];
    EmitVertex();

    gl_Position = gl_in[0].gl_Position - offset;    // top left
    gVertColour = vertColour[0];
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + offset;    // top right
    gVertColour = vertColour[0];
    EmitVertex();

    gl_Position = gl_in[1].gl_Position - offset;    // bottom left
    gVertColour = vertColour[1];
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + offset;    // top right
    gVertColour = vertColour[0];
    EmitVertex();

    gl_Position = gl_in[1].gl_Position + offset;    // bottom right
    gVertColour = vertColour[1];
    EmitVertex();


    // Bottom joint
    /*
    gl_Position = gl_in[1].gl_Position + offsetBL;    // bottom left
    gVertColour = vertColour[1];
    EmitVertex();

    gl_Position = gl_in[1].gl_Position + offsetTL;    // top left
    gVertColour = vertColour[1];
    EmitVertex();

    gl_Position = gl_in[1].gl_Position + 2.0*offsetTR;    // top right
    gVertColour = vertColour[1];
    EmitVertex();

    gl_Position = gl_in[1].gl_Position +offsetBL;    // bottom left
    gVertColour = vertColour[1];
    EmitVertex();

    gl_Position = gl_in[1].gl_Position + offsetTR;    // top right
    gVertColour = vertColour[1];
    EmitVertex();

    gl_Position = gl_in[1].gl_Position + offsetBR;    // bottom right
    gVertColour = vertColour[1];
    EmitVertex();
    */

    EndPrimitive();
}
