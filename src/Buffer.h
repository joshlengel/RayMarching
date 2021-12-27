#pragma once

#include<glad/glad.h>

struct Pixel
{
    float r, g, b;
};

class Buffer
{
public:
    Buffer(int width, int height);
    ~Buffer();

    const Pixel *GetData() const;
    Pixel *MapData();
    void UnmapData();

    void Render();

private:
    int m_width, m_height;
    Pixel *m_pixels;

    GLuint m_vao_id, m_vbo_id, m_tex_id;
    GLuint m_program_id, m_v_shader_id, m_f_shader_id;
};