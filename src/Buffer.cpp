#include"Buffer.h"

Buffer::Buffer(int width, int height):
    m_width(width), m_height(height),
    m_pixels(new Pixel[width * height])
{
    glGenVertexArrays(1, &m_vao_id);
    glGenBuffers(1, &m_vbo_id);
    glGenTextures(1, &m_tex_id);
    m_program_id = glCreateProgram();
    m_v_shader_id = glCreateShader(GL_VERTEX_SHADER);
    m_f_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

    // Setup VAO and VBO
    glBindVertexArray(m_vao_id);

    const float VERTICES[] =
    {
        -1.0f, -1.0f,   0.0f, 1.0f,
         1.0f, -1.0f,   1.0f, 1.0f,
         1.0f,  1.0f,   1.0f, 0.0f,
        -1.0f, -1.0f,   0.0f, 1.0f,
         1.0f,  1.0f,   1.0f, 0.0f,
        -1.0f,  1.0f,   0.0f, 0.0f
    };

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, (2 + 2) * sizeof(float), reinterpret_cast<void*>(0 * sizeof(float)));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, (2 + 2) * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Setup texture
    glBindTexture(GL_TEXTURE_2D, m_tex_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Setup shaders
    const char *const VERTEX_SOURCE = R"(
        #version 130

        in vec2 position;
        in vec2 uv;

        out vec2 _uv;

        void main() {
            gl_Position = vec4(position, 0.0, 1.0);
            _uv = uv;
        }
    )";

    const char *const FRAGMENT_SOURCE = R"(
        #version 130

        in vec2 _uv;

        out vec4 color;

        uniform sampler2D tex;

        void main() {
            color = texture(tex, _uv);
        }
    )";

    glShaderSource(m_v_shader_id, 1, &VERTEX_SOURCE, nullptr);
    glShaderSource(m_f_shader_id, 1, &FRAGMENT_SOURCE, nullptr);

    glCompileShader(m_v_shader_id);
    glCompileShader(m_f_shader_id);

    glAttachShader(m_program_id, m_v_shader_id);
    glAttachShader(m_program_id, m_f_shader_id);

    glBindAttribLocation(m_program_id, 0, "position");
    glBindAttribLocation(m_program_id, 1, "uv");

    glLinkProgram(m_program_id);

    glDetachShader(m_program_id, m_v_shader_id);
    glDetachShader(m_program_id, m_f_shader_id);
}

Buffer::~Buffer()
{
    delete[] m_pixels;

    glDeleteVertexArrays(1, &m_vao_id);
    glDeleteBuffers(1, &m_vbo_id);
    glDeleteTextures(1, &m_tex_id);

    glDeleteProgram(m_program_id);
    glDeleteShader(m_v_shader_id);
    glDeleteShader(m_f_shader_id);
}

const Pixel *Buffer::GetData() const { return m_pixels; }
Pixel *Buffer::MapData() { return m_pixels; }
void Buffer::UnmapData()
{
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, m_width, m_height, 0, GL_RGB, GL_FLOAT, m_pixels);
}

void Buffer::Render()
{
    glUseProgram(m_program_id);

    glBindTexture(GL_TEXTURE_2D, m_tex_id);
    glBindVertexArray(m_vao_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}