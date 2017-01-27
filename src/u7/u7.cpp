#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "mesh.h"

#include "gl_projection_vert.h"
#include "gl_projection_frag.h"

const unsigned int SIZE = 512;

#define PI 3.14159265358979323846f

float press=0.0f;
float press2=0.0f;

void OnKeyPress(GLFWwindow* /* window */, int key, int /* scancode */, int action, int /* mods */) {
    if (key == GLFW_KEY_S) {
        press+=0.05f;
    }

    if (key == GLFW_KEY_W) {
        press-=0.05f;
    }

    if (key == GLFW_KEY_A) {
        press2+=0.05f;
    }

    if (key == GLFW_KEY_D) {
        press2-=0.05f;
    }
}
struct Point{
    float x;
    float y;
    float z;
    Point(float x, float y, float z) : x(x), y(y), z(z){};
};

Point operator - (Point a, Point b) {
    return Point(a.x - b.x, a.y - b.y, a.z - b.z);
}

Point operator + (Point a, Point b) {
    return Point(a.x + b.x, a.y + b.y, a.z + b.z);
}

Point operator * (Point a, float s) {
    return Point(s * a.x, s * a.y, s * a.z);
}

Point interpolation(Point a, Point b, float i) {
    return a + (b - a) * i;
}

std::vector<Point> *Bezier4(Point a, Point b, Point c, Point d, int resolution, std::vector<Point> * result = nullptr) {
    if(!result)
        result = new std::vector<Point>();

    for(int n=0; n < resolution; ++n)
    {
        float i = (float) n / (float) (resolution - 1);
        Point point = interpolation(interpolation(a, b, i), interpolation(c, d, i), i);
        result->push_back(point);
    }
    return result;
}

void addToVertexTexture(std::vector<GLfloat> & vertex_buffer, std::vector<GLfloat> & texture_buffer, std::vector<Point> * lines, int x, int y, float sizeX, float sizeY){
    Point p=lines[x][y];
    vertex_buffer.push_back(p.x);
    vertex_buffer.push_back(p.y);
    vertex_buffer.push_back(p.z);

    texture_buffer.push_back((x * sizeX + sizeX));
    texture_buffer.push_back((y * sizeY + sizeY));
};

void animate(std::vector<GLfloat> & vertex_buffer, std::vector<GLfloat> & texture_buffer){
    float time = (float)sin(glfwGetTime());
    int resolution = 10;
    std::vector<Point> *line1 = Bezier4(Point(-1.0f+time, -1.0f-time, -1.0f), Point(-1.0f, -1.0f+time, -1.0f+time), Point(-1.0f, 0.0f+time, -2.5f+time), Point(-1.0f, 0.0f, -2.5f), resolution);
    std::vector<Point> *line2 = Bezier4(Point(-1.0f+time, -1.0f-time, -1.0f), Point(-1.0f, -1.0f+time, -1.0f+time), Point(-1.0f, 1.0f+time, -2.5f+time), Point(-1.0f, 1.0f, -2.5f), resolution);
    std::vector<Point> *line3 = Bezier4(Point( 1.0f+time, -1.0f-time, -1.0f), Point( 1.0f, -1.0f+time, -1.0f+time), Point( 1.0f, 1.0f+time, -2.5f+time), Point( 1.0f, 1.0f, -2.5f), resolution);
    std::vector<Point> *line4 = Bezier4(Point( 1.0f+time, -1.0f-time, -2.5f), Point( 1.0f, -1.0f+time, -2.5f+time), Point( 1.0f, 1.0f+time, -2.5f+time), Point( 1.0f, 1.0f, -2.5f), resolution);

    std::vector<Point> *lines = new std::vector<Point>[resolution];

    for (int j = 0; j < resolution; j++){
        lines[j]=  *Bezier4(line1->at(j), line2->at(j), line3->at(j), line4->at(j), resolution);
    }


    float sizeX = 1.0f / resolution;
    float sizeY = 1.0f / resolution;


    for(int i=1 ; i<resolution; i++){
        for(int j=1 ; j<resolution; j++){
            addToVertexTexture(vertex_buffer, texture_buffer, lines, i, j - 1, sizeX, sizeY);
            addToVertexTexture(vertex_buffer, texture_buffer, lines, i, j, sizeX, sizeY);
            addToVertexTexture(vertex_buffer, texture_buffer, lines, i - 1, j, sizeX, sizeY);

            addToVertexTexture(vertex_buffer, texture_buffer, lines, i, j - 1, sizeX, sizeY);
            addToVertexTexture(vertex_buffer, texture_buffer, lines, i - 1, j - 1, sizeX, sizeY);
            addToVertexTexture(vertex_buffer, texture_buffer, lines, i - 1, j, sizeX, sizeY);
        }
    }
//    delete lines;
    delete line1;
    delete line2;
    delete line3;
    delete line4;
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return EXIT_FAILURE;

    }
    // Setup OpenGL context
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Try to create a window
    auto window = glfwCreateWindow(SIZE, SIZE, "PPGSO gl_projection", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to open GLFW window, your graphics card is probably only capable of OpenGL 2.1"
                  << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // Finalize window setup
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    glewInit();
    if (!glewIsSupported("GL_VERSION_3_3")) {
        std::cerr << "Failed to initialize GLEW with OpenGL 3.3!" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwSetKeyCallback(window, OnKeyPress);
    // Set up OpenGL options
    // Enable Z-buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    std::vector<GLfloat> texture_buffer;
    std::vector<GLfloat> vertex_buffer;

    // Main execution loop
    while (!glfwWindowShouldClose(window)) {

        texture_buffer.clear();
        vertex_buffer.clear();
        auto program = ShaderPtr(new Shader{gl_projection_vert, gl_projection_frag});
        program->Use();

        // Generate a vertex array object
        // This keeps track of what attributes are associated with buffers
        GLuint vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        animate(vertex_buffer, texture_buffer);

        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertex_buffer.size() * sizeof(GLfloat), vertex_buffer.data(), GL_STATIC_DRAW);

        auto position_attrib = program->GetAttribLocation("Position");
        glVertexAttribPointer(position_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(position_attrib);


        GLuint cbo;
        glGenBuffers(1, &cbo);
        glBindBuffer(GL_ARRAY_BUFFER, cbo);
        glBufferData(GL_ARRAY_BUFFER, texture_buffer.size() * sizeof(GLfloat), texture_buffer.data(), GL_STATIC_DRAW);

        // Same thing for colors
        auto color_attrib = program->GetAttribLocation("TexCoord");
        glVertexAttribPointer(color_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(color_attrib);


        auto texture = TexturePtr(new Texture{"lena.rgb", 512, 512});
        program->SetTexture(texture, "Texture");

        // Initialize projection
        // Create projection matrix (field of view (radians), aspect ratio, near plane distance, far plane distance)
        auto projection = glm::perspective( (PI/180.f) * 60.0f, 1.0f, 0.1f, 10.0f);
        program->SetMatrix(projection, "ProjectionMatrix");

        // Create view matrix (translate camera a bit backwards, so we can see the geometry)


        // Quad positions
        auto quad1Mat = glm::translate(glm::mat4(1.0f), glm::vec3(0,0,1));
        // Update time and create a rotation matrix

        auto rotateMat = glm::rotate(glm::mat4(1.0f), press2, glm::vec3(0,1,0));

        auto view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, (-3.0f-press)));
        program->SetMatrix(view, "ViewMatrix");
        // Set gray background
        glClearColor(.5f,.5f,.5f,0);
        // Clear depth and color buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        program->SetMatrix(rotateMat * quad1Mat, "ModelMatrix");
        glDrawArrays(GL_TRIANGLES, 0, vertex_buffer.size() / 3);

        // Display result
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up
    glfwTerminate();

    return EXIT_SUCCESS;
}