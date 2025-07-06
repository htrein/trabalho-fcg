//     Universidade Federal do Rio Grande do Sul
//             Instituto de Informática
//       Departamento de Informática Aplicada
//
//    INF01047 Fundamentos de Computação Gráfica
//               Prof. Eduardo Gastal
//
// Arquivos "headers" padrões de C podem ser incluídos em um
// programa C++, sendo necessário somente adicionar o caractere
// "c" antes de seu nome, e remover o sufixo ".h". Exemplo:
//    #include <stdio.h> // Em C
//  vira
//    #include <cstdio> // Em C++
//
#include <cmath>
#include <cstdio>
#include <cstdlib>

// Headers abaixo são específicos de C++
#include <map>
#include <stack>
#include <string>
#include <vector>
#include <limits>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

// Headers das bibliotecas OpenGL
#include <glad/glad.h>   // Criação de contexto OpenGL 3.3
#include <GLFW/glfw3.h>  // Criação de janelas do sistema operacional

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

// Headers da biblioteca para carregar modelos obj
#include <tiny_obj_loader.h>
#include <stb_image.h>

// Headers locais, definidos na pasta "include/"
#include "utils.h"
#include "collisions.hpp"
#include "object.hpp"
#include "colliders.hpp"
#include "boundingBox.hpp"


// NOVO
#define SPHERE 0
#define BUNNY  1
#define PLANE  2
#define CHAIR 3
#define SKY_SPHERE 4
#define BOX 5
#define SOCCER_BALL 6
#define CARROT 7

std::vector<ColliderBox> box_colliders; //vetor de objetos colidíveis
std::vector<ColliderSphere> sphere_colliders;
std::vector<ColliderBox> carrot_colliders;

// Declaração de funções utilizadas para pilha de matrizes de modelagem.
void PushMatrix(glm::mat4 M);
void PopMatrix(glm::mat4& M);

// Declaração de várias funções utilizadas em main().  Essas estão definidas
// logo após a definição de main() neste arquivo.
void LoadShadersFromFiles(); // Carrega os shaders de vértice e fragmento, criando um programa de GPU
void DrawVirtualObject(const char* object_name); // Desenha um objeto armazenado em g_VirtualScene
GLuint LoadShader_Vertex(const char* filename);   // Carrega um vertex shader
GLuint LoadShader_Fragment(const char* filename); // Carrega um fragment shader
void LoadShader(const char* filename, GLuint shader_id); // Função utilizada pelas duas acima
GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id); // Cria um programa de GPU

// Declaração de funções auxiliares para renderizar texto dentro da janela
// OpenGL. Estas funções estão definidas no arquivo "textrendering.cpp".
void TextRendering_Init();
float TextRendering_LineHeight(GLFWwindow* window);
float TextRendering_CharWidth(GLFWwindow* window);
void TextRendering_PrintString(GLFWwindow* window, const std::string &str, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrix(GLFWwindow* window, glm::mat4 M, float x, float y, float scale = 1.0f);
void TextRendering_PrintVector(GLFWwindow* window, glm::vec4 v, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProduct(GLFWwindow* window, glm::mat4 M, glm::vec4 v, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProductMoreDigits(GLFWwindow* window, glm::mat4 M, glm::vec4 v, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProductDivW(GLFWwindow* window, glm::mat4 M, glm::vec4 v, float x, float y, float scale = 1.0f);

// Funções abaixo renderizam como texto na janela OpenGL algumas matrizes e
// outras informações do programa. Definidas após main().
void TextRendering_ShowFramesPerSecond(GLFWwindow* window);

// Funções callback para comunicação com o sistema operacional e interação do
// usuário. Veja mais comentários nas definições das mesmas, abaixo.
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ErrorCallback(int error, const char* description);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

GLuint LoadTextureFromFile(const char* filename);
void LoadTextureImage(const char* filename);
glm::vec3 BezierCubic(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t);

// A cena virtual é uma lista de objetos nomeados, guardados em um dicionário
// (map).  Veja dentro da função BuildTrianglesAndAddToVirtualScene() como que são incluídos
// objetos dentro da variável g_VirtualScene, e veja na função main() como
// estes são acessados.
std::map<std::string, SceneObject> g_VirtualScene;

// Pilha que guardará as matrizes de modelagem.
std::stack<glm::mat4>  g_MatrixStack;

// Razão de proporção da janela (largura/altura). Veja função FramebufferSizeCallback().
float g_ScreenRatio = 1.0f;

// Ângulos de Euler que controlam a rotação de um dos cubos da cena virtual
float g_AngleX = 0.0f;
float g_AngleY = 0.0f;
float g_AngleZ = 0.0f;

// "g_LeftMouseButtonPressed = true" se o usuário está com o botão esquerdo do mouse
// pressionado no momento atual. Veja função MouseButtonCallback().
bool g_LeftMouseButtonPressed = false;
bool g_RightMouseButtonPressed = false; // Análogo para botão direito do mouse
bool g_MiddleMouseButtonPressed = false; // Análogo para botão do meio do mouse

// Variáveis que definem a câmera em coordenadas esféricas, controladas pelo
// usuário através do mouse (veja função CursorPosCallback()). A posição
// efetiva da câmera é calculada dentro da função main(), dentro do loop de
// renderização.
float g_CameraTheta = 0.0f; // Ângulo no plano ZX em relação ao eixo Z
float g_CameraPhi = 0.0f;   // Ângulo em relação ao eixo Y
float g_CameraDistance = 3.5f; // Distância da câmera para a origem

// Variáveis que controlam rotação do antebraço
float g_ForearmAngleZ = 0.0f;
float g_ForearmAngleX = 0.0f;

// Variáveis que controlam translação do torso
float g_TorsoPositionX = 0.0f;
float g_TorsoPositionY = 0.0f;

// Variável que controla o tipo de projeção utilizada: perspectiva ou ortográfica.
bool g_UsePerspectiveProjection = true;

// Variável que controla se o texto informativo será mostrado na tela.
bool g_ShowInfoText = true;

// Variáveis que definem um programa de GPU (shaders). Veja função LoadShadersFromFiles().
GLuint g_GpuProgramID = 0;
GLint g_model_uniform;
GLint g_view_uniform;
GLint g_projection_uniform;
GLint g_object_id_uniform;
GLuint g_NumLoadedTextures = 0;
GLint g_camera_position_uniform;

bool firstpCamera = false;
glm::vec3 g_BunnyPosition = glm::vec3(3.0f, -1.0f, 0.0f);
float g_BunnySpeed = 2.0f;

//Curva de Bezier Obstáculos
glm::vec3 bezier_p0 = glm::vec3(8.0f, 2.8f, 0.0f);   
glm::vec3 bezier_p1 = glm::vec3(9.5f, 4.5f,  2.0f);   
glm::vec3 bezier_p2 = glm::vec3(11.0f, 2.0f, -2.0f);  
glm::vec3 bezier_p3 = glm::vec3(12.5f, 3.5f, 0.0f);  
float bezier_time = 0.0f;
float bezier_speed = 0.08f; 
int bezier_direction = 1;
//------

//Sistema de Pontuação
int score = 0;
std::vector<bool> carrots_collected(10, false);
void TextRendering_ShowScore(GLFWwindow* window);
//------

int main(int argc, char* argv[])
{
    // Inicializamos a biblioteca GLFW, utilizada para criar uma janela do
    // sistema operacional, onde poderemos renderizar com OpenGL.
    int success = glfwInit();
    if (!success)
    {
        fprintf(stderr, "ERROR: glfwInit() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    // Definimos o callback para impressão de erros da GLFW no terminal
    glfwSetErrorCallback(ErrorCallback);

    // Pedimos para utilizar OpenGL versão 3.3 (ou superior)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // Pedimos para utilizar o perfil "core", isto é, utilizaremos somente as
    // funções modernas de OpenGL.
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Criamos uma janela do sistema operacional, com 800 colunas e 600 linhas
    // de pixels, e com título "INF01047 ...".
    GLFWwindow* window;
    window = glfwCreateWindow(800, 600, "Jogo sem nome", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        fprintf(stderr, "ERROR: glfwCreateWindow() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    // Definimos a função de callback que será chamada sempre que o usuário
    // pressionar alguma tecla do teclado ...
    glfwSetKeyCallback(window, KeyCallback);
    // ... ou clicar os botões do mouse ...
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    // ... ou movimentar o cursor do mouse em cima da janela ...
    glfwSetCursorPosCallback(window, CursorPosCallback);
    // ... ou rolar a "rodinha" do mouse.
    glfwSetScrollCallback(window, ScrollCallback);

    // Indicamos que as chamadas OpenGL deverão renderizar nesta janela
    glfwMakeContextCurrent(window);

    // Carregamento de todas funções definidas por OpenGL 3.3, utilizando a
    // biblioteca GLAD.
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    // Definimos a função de callback que será chamada sempre que a janela for
    // redimensionada, por consequência alterando o tamanho do "framebuffer"
    // (região de memória onde são armazenados os pixels da imagem).
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    FramebufferSizeCallback(window, 800, 600); // Forçamos a chamada do callback acima, para definir g_ScreenRatio.

    // Imprimimos no terminal informações sobre a GPU do sistema
    const GLubyte *vendor      = glGetString(GL_VENDOR);
    const GLubyte *renderer    = glGetString(GL_RENDERER);
    const GLubyte *glversion   = glGetString(GL_VERSION);
    const GLubyte *glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION);

    printf("GPU: %s, %s, OpenGL %s, GLSL %s\n", vendor, renderer, glversion, glslversion);

    // Carregamos os shaders de vértices e de fragmentos que serão utilizados
    // para renderização. Veja slides 180-200 do documento Aula_03_Rendering_Pipeline_Grafico.pdf.

    LoadShadersFromFiles();

    // Carregamento das texturas
    LoadTextureImage("../../data/Textures/hare_diffuse.png"); // TextureImage0
    LoadTextureImage("../../data/Textures/leather_chair_BaseColor.png"); // TextureImage1
    LoadTextureImage("../../data/Textures/fundo.jpg"); // TextureImage2
    LoadTextureImage("../../data/Textures/Wooden Crate_Crate_BaseColor.png"); // TextureImage3
    LoadTextureImage("../../data/Textures/Football_Diffuse.jpg"); // TextureImage4
    LoadTextureImage("../../data/Textures/plane.jpg"); // TextureImage5
    LoadTextureImage("../../data/Textures/Carrot_v01.jpg"); //TextureImage6

    // Construímos a representação de objetos geométricos através de malhas de triângulos
    ComputeObject("../../data/sphere.obj", &g_VirtualScene);
    ComputeObject("../../data/sky.obj", &g_VirtualScene);
    ComputeObject("../../data/plane.obj", &g_VirtualScene);
    
    ObjModel carrotmodel = ComputeObject("../../data/carrot.obj", &g_VirtualScene);

    ObjModel bunnymodel = ComputeObject("../../data/hare.obj", &g_VirtualScene);
    ColliderBox bunny_collider = createBoundingBox(bunnymodel.attrib);
    
    ObjModel soccer_ball = ComputeObject("../../data/soccer_ball.obj", &g_VirtualScene);
    ColliderSphere collider_soccer = createBoundingSphereRitter(soccer_ball.attrib);
    sphere_colliders.push_back(collider_soccer);

    ObjModel boxmodel = ComputeObject("../../data/woodenCrate.obj", &g_VirtualScene);
    ColliderBox box_limits = createBoundingBox(boxmodel.attrib);
   
    float base_scale = 0.3f;
    float base_spacing = 1.0f;
    float step_height = 0.9f;
    for (int i = 0; i < 5; ++i) {
        float scale = base_scale - i * 0.05f;
        float spacing = base_spacing + i * 0.5f;
        glm::vec3 pos(i * spacing, i * step_height, 0.0f);
        glm::vec3 bbox_min = box_limits.bbox_min * scale;
        glm::vec3 bbox_max = box_limits.bbox_max * scale;
        box_colliders.push_back({pos, bbox_min, bbox_max});
        glm::vec3 carrot_bbox_min = glm::vec3(-0.3f, -0.3f, -0.3f); 
        glm::vec3 carrot_bbox_max = glm::vec3(0.3f, 0.3f, 0.3f);    
        glm::vec3 carrot_pos(i * spacing, step_height + 0.5f, 0.0f);
        carrot_colliders.push_back({carrot_pos, carrot_bbox_min, carrot_bbox_max});
    }

    glm::vec3 last_pos = box_colliders.back().pos;
    float last_scale = base_scale - 4 * 0.05f; 
    float spiral_radius = 2.0f; 
    float spiral_angle_step = glm::radians(60.0f); 
    float spiral_height_step = 1.3f; 

    for (int i = 0; i < 5; ++i) {
        float scale = std::max(0.05f, last_scale - i * 0.05f);
        float angle = i * spiral_angle_step;
        glm::vec3 pos = last_pos
            + glm::vec3(
                spiral_radius * cos(angle),
                (i+1) * spiral_height_step * (1 + 0.2f * i),
                spiral_radius * sin(angle)
            );
        glm::vec3 bbox_min = box_limits.bbox_min * scale;
        glm::vec3 bbox_max = box_limits.bbox_max * scale;
        box_colliders.push_back({pos, bbox_min, bbox_max});
        glm::vec3 carrot_bbox_min = glm::vec3(-0.3f, -0.3f, -0.3f);
        glm::vec3 carrot_bbox_max = glm::vec3(0.3f, 0.3f, 0.3f);
        glm::vec3 carrot_pos = pos + glm::vec3(0.0f, 0.5f + scale * 7.0f, 0.0f);
        carrot_colliders.push_back({carrot_pos, carrot_bbox_min, carrot_bbox_max});
    }

    ObjModel chair = ComputeObject("../../data/leather_chair.obj", &g_VirtualScene);
    ColliderBox chair_limits = createBoundingBox(chair.attrib);
    chair_limits.pos = glm::vec3(-2.0f, -1.0f, 0.0f);
    chair_limits.bbox_min *= glm::vec3(2.0f, 1.0f, 2.0f);
    chair_limits.bbox_max *= glm::vec3(2.0f, 1.0f, 2.0f);
    box_colliders.push_back(chair_limits);
    
    if ( argc > 1 )
    {
        ObjModel model(argv[1]);
        BuildTrianglesAndAddToVirtualScene(&model, &g_VirtualScene);
    }

    // Inicializamos o código para renderização de texto.
    TextRendering_Init();

    // Habilitamos o Z-buffer. Veja slides 104-116 do documento Aula_09_Projecoes.pdf.
    glEnable(GL_DEPTH_TEST);

    // Habilitamos o Backface Culling. Veja slides 8-13 do documento Aula_02_Fundamentos_Matematicos.pdf, slides 23-34 do documento Aula_13_Clipping_and_Culling.pdf e slides 112-123 do documento Aula_14_Laboratorio_3_Revisao.pdf.
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    float last_time = (float)glfwGetTime();

    // Ficamos em um loop infinito, renderizando, até que o usuário feche a janela
    while (!glfwWindowShouldClose(window))
    {
        // Aqui executamos as operações de renderização

        // Definimos a cor do "fundo" do framebuffer como branco.
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

        // "Pintamos" todos os pixels do framebuffer com a cor definida acima,
        // e também resetamos todos os pixels do Z-buffer (depth buffer).
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Pedimos para a GPU utilizar o programa de GPU criado acima (contendo
        // os shaders de vértice e fragmentos).
        glUseProgram(g_GpuProgramID);

        // Computamos a posição da câmera utilizando coordenadas esféricas.  As
        // variáveis g_CameraDistance, g_CameraPhi, e g_CameraTheta são
        // controladas pelo mouse do usuário. Veja as funções CursorPosCallback()
        // e ScrollCallback().
        float r = g_CameraDistance;
        float y = r*sin(g_CameraPhi);
        float z = r*cos(g_CameraPhi)*cos(g_CameraTheta);
        float x = r*cos(g_CameraPhi)*sin(g_CameraTheta);

        // Abaixo definimos as varáveis que efetivamente definem a câmera virtual.
        // Veja slides 195-227 e 229-234 do documento Aula_08_Sistemas_de_Coordenadas.pdf.

        glm::vec4 camera_position_c, camera_lookat_l, camera_view_vector, camera_up_vector;
        camera_up_vector   = glm::vec4(0.0f,1.0f,0.0f,0.0f);
        if(firstpCamera){
            camera_position_c = glm::vec4(g_BunnyPosition.x, g_BunnyPosition.y + 0.5f, g_BunnyPosition.z, 1.0f);
            camera_view_vector = -(glm::vec4(x,y,z,0.0f));
            camera_lookat_l = camera_position_c + glm::vec4(x,y,z,0.0f);
        } else {
            camera_position_c = glm::vec4(x, y, z, 1.0f) + glm::vec4(g_BunnyPosition, 0.0f);
            camera_lookat_l = glm::vec4(g_BunnyPosition, 1.0f);
            camera_view_vector = camera_lookat_l - camera_position_c;
        }

        glUniform4f(g_camera_position_uniform, camera_position_c.x, camera_position_c.y, camera_position_c.z, camera_position_c.w);

        
        // Cálculos bezier
        float current_time = (float)glfwGetTime();
        float delta_time = current_time - last_time;
        last_time = current_time;

        bezier_time += bezier_speed * delta_time * bezier_direction;
        if (bezier_time > 1.0f) {
            bezier_time = 2.0f - bezier_time; 
            bezier_direction = -1;
        }
        if (bezier_time < 0.0f) {
            bezier_time = -bezier_time; 
            bezier_direction = 1;
        }
        glm::vec3 obstacle_pos = BezierCubic(bezier_p0, bezier_p1, bezier_p2, bezier_p3, bezier_time);
        
        // Posicao no mundo da bola de futebol
        glm::vec3 soccer_ball_pos = glm::vec3(obstacle_pos.x, obstacle_pos.y, obstacle_pos.z);

        // Cálculos ângulo coelho
        glm::vec3 front_vector = glm::normalize(glm::vec3(camera_view_vector.x, 0.0f, camera_view_vector.z));
        glm::vec3 right_vector = glm::normalize(glm::cross(front_vector, glm::vec3(0.0f, 1.0f, 0.0f)));
        float camera_speed = g_BunnySpeed * delta_time;
        
        static glm::vec3 previous_bunny_position = g_BunnyPosition;

        glm::vec3 move_direction(0.0f);
        
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
            g_BunnyPosition += front_vector * camera_speed; move_direction += front_vector;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
            g_BunnyPosition -= front_vector * camera_speed; move_direction -= front_vector;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
            g_BunnyPosition -= right_vector * camera_speed; move_direction -= right_vector;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
            g_BunnyPosition += right_vector * camera_speed; move_direction += right_vector;
        }
        if (glm::length(move_direction) > 0.0f)
        {
            move_direction = glm::normalize(move_direction);
            float target_angle = atan2(-move_direction.x, -move_direction.z);
            g_AngleY = target_angle;
        }

        // Cálculo do pulo do coelho
        static bool jumping = false;
        static float jump_velocity = 0.0f;
        const float gravity = 9.8f;
        const float jump_strength = 7.0f; //varíavel importante pra adaptar a dificuldade do jogo

        if (jumping)
        {
            g_BunnyPosition.y += jump_velocity * delta_time;
            jump_velocity -= gravity * delta_time; //adiciona gravidade
            if (g_BunnyPosition.y <= -1.0f) //voltar ao chão
            {
                g_BunnyPosition.y = -1.0f;
                jumping = false;
                jump_velocity = 0.0f;
            }
        }

        // Projeção
        glm::mat4 view = Matrix_Camera_View(camera_position_c, camera_view_vector, camera_up_vector);
        glm::mat4 projection;

        // Note que, no sistema de coordenadas da câmera, os planos near e far
        // estão no sentido negativo! Veja slides 176-204 do documento Aula_09_Projecoes.pdf.
        float nearplane = -0.1f;  // Posição do "near plane"
        float farplane  = -100.0f; // Posição do "far plane"

        if (g_UsePerspectiveProjection)
        {
            // Projeção Perspectiva.
            // Para definição do field of view (FOV), veja slides 205-215 do documento Aula_09_Projecoes.pdf.
            float field_of_view = 3.141592 / 3.0f;
            projection = Matrix_Perspective(field_of_view, g_ScreenRatio, nearplane, farplane);
        }
        else
        {
            // Projeção Ortográfica.
            // Para definição dos valores l, r, b, t ("left", "right", "bottom", "top"),
            // PARA PROJEÇÃO ORTOGRÁFICA veja slides 219-224 do documento Aula_09_Projecoes.pdf.
            // Para simular um "zoom" ortográfico, computamos o valor de "t"
            // utilizando a variável g_CameraDistance.
            float t = 1.5f*g_CameraDistance/2.5f;
            float b = -t;
            float r = t*g_ScreenRatio;
            float l = -r;
            projection = Matrix_Orthographic(l, r, b, t, nearplane, farplane);
        }

        glm::mat4 model = Matrix_Identity(); // Transformação identidade de modelagem

        // Enviamos as matrizes "view" e "projection" para a placa de vídeo
        // (GPU). Veja o arquivo "shader_vertex.glsl", onde estas são
        // efetivamente aplicadas em todos os pontos.
        glUniformMatrix4fv(g_view_uniform, 1 , GL_FALSE , glm::value_ptr(view));
        glUniformMatrix4fv(g_projection_uniform , 1 , GL_FALSE , glm::value_ptr(projection));
        
        // Chão
        model = Matrix_Translate(0.0f, -1.0f, 0.0f) * Matrix_Scale(50.0f, 50.0f, 50.0f);
        glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
        glUniform1i(g_object_id_uniform, PLANE);
        DrawVirtualObject("plane");
        
        // Esfera ceu
        glDepthMask(GL_FALSE); // Desabilita escrita de profundidade
        glCullFace(GL_FRONT);   // Oculta a frente dos vértices

        // Usa o modelo de esfera pra calcular o céu
        // Movimenta ele junto com a camera
        model = Matrix_Translate(camera_position_c.x, camera_position_c.y, camera_position_c.z)
                                   * Matrix_Scale(20.0f, 20.0f, 20.0f); // Ajustar conforme necessário
        glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
        glUniform1i(g_object_id_uniform, SKY_SPHERE);
        DrawVirtualObject("sky"); 

        glCullFace(GL_BACK);
        glDepthMask(GL_TRUE); // Faz voltar os parâmetros originais

        // Bola de futebol
        glm::mat4 model_obstacle = Matrix_Translate(soccer_ball_pos.x, soccer_ball_pos.y, soccer_ball_pos.z) * Matrix_Scale(2.0f, 2.0f, 2.0f);
        glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model_obstacle));
        glUniform1i(g_object_id_uniform, SOCCER_BALL); 
        DrawVirtualObject("soccer_ball"); 

        glm::mat4 transform_bunny = Matrix_Translate(g_BunnyPosition.x, g_BunnyPosition.y, g_BunnyPosition.z)
            * Matrix_Rotate_Z(g_AngleZ)
            * Matrix_Rotate_Y(g_AngleY)
            * Matrix_Rotate_X(g_AngleX);
        if (!firstpCamera)  
        {
            model = transform_bunny;
            glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
            glUniform1i(g_object_id_uniform, BUNNY);
            DrawVirtualObject("hare");
        }   
        //drawBoundingBox(bunny_collider, transform_bunny, g_model_uniform, g_object_id_uniform); 
        
        glActiveTexture(GL_TEXTURE3);
        glm::mat4 model2 = Matrix_Identity();
        PushMatrix(model2);
        for (int i = 0; i < 5; ++i)
        {
            PushMatrix(model2);

            float base_scale = 0.3f;
            float scale = base_scale - i * 0.05f;

            float base_spacing = 1.0f;
            float spacing = base_spacing + i * 0.5f;

            float step_height = 0.9f * i;

            model2 = model2 * Matrix_Translate(i * spacing, step_height, 0.0f);
            model2 = model2 * Matrix_Scale(scale, scale, scale);

            glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model2));
            glUniform1i(g_object_id_uniform, BOX);
            DrawVirtualObject("Crate_Plane.005");

            if (!carrots_collected[i]) {
                glm::vec4 carrot_world_pos = model2 * glm::vec4(0.0f, 7.0f, 0.0f, 1.0f);
                carrot_colliders[i].pos = glm::vec3(carrot_world_pos);
                glm::mat4 carrot_model = model2 
                    * Matrix_Translate(0.0f, 7.0f, 0.0f)     
                    * Matrix_Scale(0.02f, 0.02f, 0.02f)      
                    * Matrix_Rotate_X(-3.141592/2.0f);       
                
                glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(carrot_model));
                glUniform1i(g_object_id_uniform, CARROT);
                DrawVirtualObject("10170_Carrot_v01");
            }  

            PopMatrix(model2);
        }

        glm::vec3 last_pos = box_colliders[4].pos;
        float last_scale = 0.3f - 4 * 0.05f;
        float spiral_radius = 2.0f;
        float spiral_angle_step = glm::radians(60.0f);
        float spiral_height_step = 1.3f;

        for (int i = 0; i < 5; ++i)
        {
            float scale = std::max(0.05f, last_scale - i * 0.05f);
            float angle = i * spiral_angle_step;
            glm::vec3 pos = last_pos
                + glm::vec3(
                    spiral_radius * cos(angle),
                    (i+1) * spiral_height_step * (1 + 0.2f * i),
                    spiral_radius * sin(angle)
                );

            glm::mat4 box_model = Matrix_Translate(pos.x, pos.y, pos.z) * Matrix_Scale(scale, scale, scale);

            glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(box_model));
            glUniform1i(g_object_id_uniform, BOX);
            DrawVirtualObject("Crate_Plane.005");

            int carrot_idx = i + 5;
            if (!carrots_collected[carrot_idx]) {
                glm::mat4 carrot_model = box_model
                    * Matrix_Translate(0.0f, 7.0f, 0.0f)
                    * Matrix_Scale(0.02f, 0.02f, 0.02f)
                    * Matrix_Rotate_X(-3.141592f/2.0f);

                glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(carrot_model));
                glUniform1i(g_object_id_uniform, CARROT);
                DrawVirtualObject("10170_Carrot_v01");
            }
        }

        // Cadeira
        glm::mat4 transform_chair = Matrix_Translate(-2.0f, -1.0f, 0.0f) * Matrix_Scale(2.0f, 1.0f, 2.0f);
        model = transform_chair;
        glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
        glUniform1i(g_object_id_uniform, CHAIR);
        DrawVirtualObject("leather_chair");
        //drawBoundingBox(chair_limits, transform_chair, g_model_uniform, g_object_id_uniform); 
        
        // Cálculo colisões
        glm::vec3 bunny_min = g_BunnyPosition + bunny_collider.bbox_min;
        glm::vec3 bunny_max = g_BunnyPosition + bunny_collider.bbox_max;
        bool on_top = false; //verifica se o personagem está em cima de um obj
        const float folga = 0.01f; //folga para evitar jittering
        //para cada objeto colidivel testa se há colisao
        for (const auto& col : box_colliders) {
            glm::vec3 obj_min = col.pos + col.bbox_min;
            glm::vec3 obj_max = col.pos + col.bbox_max;
            if (AABBCollision(bunny_min, bunny_max, obj_min, obj_max)) {
                // Se o coelho estava acima do objeto, mas seu limite inferior já estava dentro
                collisionTreatmentAABB(&g_BunnyPosition, bunny_collider, obj_min, obj_max, previous_bunny_position, folga, &jumping, &on_top, &jump_velocity);
            }
        }

        // std::pair<glm::vec4, glm::vec4> test_line_plane_collision = std::pair<glm::vec4, glm::vec4>(glm::vec4(-10.0f, 1.0f, -10.0f, 1.0f), glm::vec4(10.0f, 1.0f, 10.0f, 1.0f));
        // if(BoxPlaneCollision(bunny_collider, test_line_plane_collision, transform_bunny, Matrix_Identity())){
        //     if(!bunny_collider.collidedWithPlane){
        //         printf("c\n");
        //         bunny_collider.collidedWithPlane = true;
        //     }
        // } else {
        //     bunny_collider.collidedWithPlane = false;
        // }
        
        //Carrot Collision  
        for (long unsigned int i = 0; i < carrot_colliders.size(); i++) {
            if (!carrots_collected[i]) {  
                glm::vec3 carrot_min = carrot_colliders[i].pos + carrot_colliders[i].bbox_min;
                glm::vec3 carrot_max = carrot_colliders[i].pos + carrot_colliders[i].bbox_max;
                glm::vec3 bunny_min = g_BunnyPosition + bunny_collider.bbox_min;
                glm::vec3 bunny_max = g_BunnyPosition + bunny_collider.bbox_max;

                if (AABBCollision(bunny_min, bunny_max, carrot_min, carrot_max)) {
                    carrots_collected[i] = true;  
                    score += 100;  
                }
            }
        }
        // Iterador dos colliders
        for (auto& col : sphere_colliders) {
            // Colisor no espaco do mundo
            glm::vec3 sphere_world_center = soccer_ball_pos + col.pos;

            glm::vec3 sphere_center_in_bunny_local = sphere_world_center - g_BunnyPosition;
            
            glm::vec3 intersection_point; 

            if (SphereBoxCollision(sphere_center_in_bunny_local, col.radius, bunny_collider, &intersection_point)) {
                // Se está abaixo da bola
                if(sphere_center_in_bunny_local.y > 1.0f){
                    jump_velocity = 0.0f;
                    g_BunnyPosition = Matrix_Translate(0.0f, -0.07f, 0.0f) * glm::vec4(g_BunnyPosition, 1.0f);
                } else {
                    g_BunnyPosition.y = (g_BunnyPosition.y + intersection_point.y) - bunny_collider.bbox_min.y;
                    if (jump_velocity <= 0.0f) {
                        jumping = false;
                        jump_velocity = 0.0f;
                        on_top = true;
                    }
                }
            }
        }

        //queda
        if (!on_top && g_BunnyPosition.y > 0.0f && !jumping) {
            jumping = true;
            jump_velocity = 0.0f;
        }
        //permite pulo se está no chao ou se esta em cima de outro objeto
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !jumping && (g_BunnyPosition.y <= 0.0f || on_top))
        {
            jumping = true;
            jump_velocity = jump_strength;
        }
        //atualiza posicao
        previous_bunny_position = g_BunnyPosition;

        TextRendering_ShowScore(window); 

        // Imprimimos na tela informação sobre o número de quadros renderizados
        // por segundo (frames per second).
        TextRendering_ShowFramesPerSecond(window);

        // O framebuffer onde OpenGL executa as operações de renderização não
        // é o mesmo que está sendo mostrado para o usuário, caso contrário
        // seria possível ver artefatos conhecidos como "screen tearing". A
        // chamada abaixo faz a troca dos buffers, mostrando para o usuário
        // tudo que foi renderizado pelas funções acima.
        // Veja o link: https://en.wikipedia.org/w/index.php?title=Multiple_buffering&oldid=793452829#Double_buffering_in_computer_graphics
        glfwSwapBuffers(window);

        // Verificamos com o sistema operacional se houve alguma interação do
        // usuário (teclado, mouse, ...). Caso positivo, as funções de callback
        // definidas anteriormente usando glfwSet*Callback() serão chamadas
        // pela biblioteca GLFW.
        glfwPollEvents();
    }

    // Finalizamos o uso dos recursos do sistema operacional
    glfwTerminate();

    // Fim do programa
    return 0;
}

// Função que carrega uma imagem para ser utilizada como textura
void LoadTextureImage(const char* filename)
{
    printf("Carregando imagem \"%s\"... ", filename);

    // Primeiro fazemos a leitura da imagem do disco
    stbi_set_flip_vertically_on_load(true);
    int width;
    int height;
    int channels;
    unsigned char *data = stbi_load(filename, &width, &height, &channels, 3);

    if ( data == NULL )
    {
        fprintf(stderr, "ERROR: Cannot open image file \"%s\".\n", filename);
        std::exit(EXIT_FAILURE);
    }

    printf("OK (%dx%d).\n", width, height);

    // Agora criamos objetos na GPU com OpenGL para armazenar a textura
    GLuint texture_id;
    GLuint sampler_id;
    glGenTextures(1, &texture_id);
    glGenSamplers(1, &sampler_id);

    // Veja slides 95-96 do documento Aula_20_Mapeamento_de_Texturas.pdf
    glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Parâmetros de amostragem da textura.
    glSamplerParameteri(sampler_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glSamplerParameteri(sampler_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Agora enviamos a imagem lida do disco para a GPU
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

    GLuint textureunit = g_NumLoadedTextures;
    glActiveTexture(GL_TEXTURE0 + textureunit);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindSampler(textureunit, sampler_id);

    stbi_image_free(data);

    g_NumLoadedTextures += 1;
}

// Função que desenha um objeto armazenado em g_VirtualScene. Veja definição
// dos objetos na função BuildTrianglesAndAddToVirtualScene().
void DrawVirtualObject(const char* object_name)
{
    // "Ligamos" o VAO. Informamos que queremos utilizar os atributos de
    // vértices apontados pelo VAO criado pela função BuildTrianglesAndAddToVirtualScene(). Veja
    // comentários detalhados dentro da definição de BuildTrianglesAndAddToVirtualScene().
    glBindVertexArray(g_VirtualScene[object_name].vertex_array_object_id);

    // Pedimos para a GPU rasterizar os vértices dos eixos XYZ
    // apontados pelo VAO como linhas. Veja a definição de
    // g_VirtualScene[""] dentro da função BuildTrianglesAndAddToVirtualScene(), e veja
    // a documentação da função glDrawElements() em
    // http://docs.gl/gl3/glDrawElements.
    glDrawElements(
        g_VirtualScene[object_name].rendering_mode,
        g_VirtualScene[object_name].num_indices,
        GL_UNSIGNED_INT,
        (void*)(g_VirtualScene[object_name].first_index * sizeof(GLuint))
    );

    // "Desligamos" o VAO, evitando assim que operações posteriores venham a
    // alterar o mesmo. Isso evita bugs.
    glBindVertexArray(0);
}

// Função que carrega os shaders de vértices e de fragmentos que serão
// utilizados para renderização. Veja slides 180-200 do documento Aula_03_Rendering_Pipeline_Grafico.pdf.
//
void LoadShadersFromFiles()
{
    // Note que o caminho para os arquivos "shader_vertex.glsl" e
    // "shader_fragment.glsl" estão fixados, sendo que assumimos a existência
    // da seguinte estrutura no sistema de arquivos:
    //
    //    + FCG_Lab_01/
    //    |
    //    +--+ bin/
    //    |  |
    //    |  +--+ Release/  (ou Debug/ ou Linux/)
    //    |     |
    //    |     o-- main.exe
    //    |
    //    +--+ src/
    //       |
    //       o-- shader_vertex.glsl
    //       |
    //       o-- shader_fragment.glsl
    //
    GLuint vertex_shader_id = LoadShader_Vertex("../../src/shader_vertex.glsl");
    GLuint fragment_shader_id = LoadShader_Fragment("../../src/shader_fragment.glsl");

    // Deletamos o programa de GPU anterior, caso ele exista.
    if ( g_GpuProgramID != 0 )
        glDeleteProgram(g_GpuProgramID);

    // Criamos um programa de GPU utilizando os shaders carregados acima.
    g_GpuProgramID = CreateGpuProgram(vertex_shader_id, fragment_shader_id);

    // Buscamos o endereço das variáveis definidas dentro do Vertex Shader.
    // Utilizaremos estas variáveis para enviar dados para a placa de vídeo
    // (GPU)! Veja arquivo "shader_vertex.glsl" e "shader_fragment.glsl".
    g_model_uniform      = glGetUniformLocation(g_GpuProgramID, "model"); // Variável da matriz "model"
    g_view_uniform       = glGetUniformLocation(g_GpuProgramID, "view"); // Variável da matriz "view" em shader_vertex.glsl
    g_projection_uniform = glGetUniformLocation(g_GpuProgramID, "projection"); // Variável da matriz "projection" em shader_vertex.glsl
    g_object_id_uniform  = glGetUniformLocation(g_GpuProgramID, "object_id"); // Variável "object_id" em shader_fragment.glsl
    g_camera_position_uniform = glGetUniformLocation(g_GpuProgramID, "position_camera"); // Variável "position_camera" em shader_fragment.glsl

    glUseProgram(g_GpuProgramID);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage0"), 0);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage1"), 1);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage2"), 2);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage3"), 3);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage4"), 4);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage5"), 5);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage6"), 6);
    glUseProgram(0);
}

// Função que pega a matriz M e guarda a mesma no topo da pilha
void PushMatrix(glm::mat4 M)
{
    g_MatrixStack.push(M);
}

// Função que remove a matriz atualmente no topo da pilha e armazena a mesma na variável M
void PopMatrix(glm::mat4& M)
{
    if ( g_MatrixStack.empty() )
    {
        M = Matrix_Identity();
    }
    else
    {
        M = g_MatrixStack.top();
        g_MatrixStack.pop();
    }
}

// Carrega um Vertex Shader de um arquivo GLSL. Veja definição de LoadShader() abaixo.
GLuint LoadShader_Vertex(const char* filename)
{
    // Criamos um identificador (ID) para este shader, informando que o mesmo
    // será aplicado nos vértices.
    GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);

    // Carregamos e compilamos o shader
    LoadShader(filename, vertex_shader_id);

    // Retorna o ID gerado acima
    return vertex_shader_id;
}

// Carrega um Fragment Shader de um arquivo GLSL . Veja definição de LoadShader() abaixo.
GLuint LoadShader_Fragment(const char* filename)
{
    // Criamos um identificador (ID) para este shader, informando que o mesmo
    // será aplicado nos fragmentos.
    GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

    // Carregamos e compilamos o shader
    LoadShader(filename, fragment_shader_id);

    // Retorna o ID gerado acima
    return fragment_shader_id;
}

// Função auxilar, utilizada pelas duas funções acima. Carrega código de GPU de
// um arquivo GLSL e faz sua compilação.
void LoadShader(const char* filename, GLuint shader_id)
{
    // Lemos o arquivo de texto indicado pela variável "filename"
    // e colocamos seu conteúdo em memória, apontado pela variável
    // "shader_string".
    std::ifstream file;
    try {
        file.exceptions(std::ifstream::failbit);
        file.open(filename);
    } catch ( std::exception& e ) {
        fprintf(stderr, "ERROR: Cannot open file \"%s\".\n", filename);
        std::exit(EXIT_FAILURE);
    }
    std::stringstream shader;
    shader << file.rdbuf();
    std::string str = shader.str();
    const GLchar* shader_string = str.c_str();
    const GLint   shader_string_length = static_cast<GLint>( str.length() );

    // Define o código do shader GLSL, contido na string "shader_string"
    glShaderSource(shader_id, 1, &shader_string, &shader_string_length);

    // Compila o código do shader GLSL (em tempo de execução)
    glCompileShader(shader_id);

    // Verificamos se ocorreu algum erro ou "warning" durante a compilação
    GLint compiled_ok;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled_ok);

    GLint log_length = 0;
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);

    // Alocamos memória para guardar o log de compilação.
    // A chamada "new" em C++ é equivalente ao "malloc()" do C.
    GLchar* log = new GLchar[log_length];
    glGetShaderInfoLog(shader_id, log_length, &log_length, log);

    // Imprime no terminal qualquer erro ou "warning" de compilação
    if ( log_length != 0 )
    {
        std::string  output;

        if ( !compiled_ok )
        {
            output += "ERROR: OpenGL compilation of \"";
            output += filename;
            output += "\" failed.\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }
        else
        {
            output += "WARNING: OpenGL compilation of \"";
            output += filename;
            output += "\".\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }

        fprintf(stderr, "%s", output.c_str());
    }

    // A chamada "delete" em C++ é equivalente ao "free()" do C
    delete [] log;
}

// Esta função cria um programa de GPU, o qual contém obrigatoriamente um
// Vertex Shader e um Fragment Shader.
GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id)
{
    // Criamos um identificador (ID) para este programa de GPU
    GLuint program_id = glCreateProgram();

    // Definição dos dois shaders GLSL que devem ser executados pelo programa
    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);

    // Linkagem dos shaders acima ao programa
    glLinkProgram(program_id);

    // Verificamos se ocorreu algum erro durante a linkagem
    GLint linked_ok = GL_FALSE;
    glGetProgramiv(program_id, GL_LINK_STATUS, &linked_ok);

    // Imprime no terminal qualquer erro de linkagem
    if ( linked_ok == GL_FALSE )
    {
        GLint log_length = 0;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);

        // Alocamos memória para guardar o log de compilação.
        // A chamada "new" em C++ é equivalente ao "malloc()" do C.
        GLchar* log = new GLchar[log_length];

        glGetProgramInfoLog(program_id, log_length, &log_length, log);

        std::string output;

        output += "ERROR: OpenGL linking of program failed.\n";
        output += "== Start of link log\n";
        output += log;
        output += "\n== End of link log\n";

        // A chamada "delete" em C++ é equivalente ao "free()" do C
        delete [] log;

        fprintf(stderr, "%s", output.c_str());
    }

    // Os "Shader Objects" podem ser marcados para deleção após serem linkados
    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    // Retornamos o ID gerado acima
    return program_id;
}

// Definição da função que será chamada sempre que a janela do sistema
// operacional for redimensionada, por consequência alterando o tamanho do
// "framebuffer" (região de memória onde são armazenados os pixels da imagem).
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // Indicamos que queremos renderizar em toda região do framebuffer. A
    // função "glViewport" define o mapeamento das "normalized device
    // coordinates" (NDC) para "pixel coordinates".  Essa é a operação de
    // "Screen Mapping" ou "Viewport Mapping" vista em aula ({+ViewportMapping2+}).
    glViewport(0, 0, width, height);

    // Atualizamos também a razão que define a proporção da janela (largura /
    // altura), a qual será utilizada na definição das matrizes de projeção,
    // tal que não ocorra distorções durante o processo de "Screen Mapping"
    // acima, quando NDC é mapeado para coordenadas de pixels. Veja slides 205-215 do documento Aula_09_Projecoes.pdf.
    //
    // O cast para float é necessário pois números inteiros são arredondados ao
    // serem divididos!
    g_ScreenRatio = (float)width / height;
}

// Variáveis globais que armazenam a última posição do cursor do mouse, para
// que possamos calcular quanto que o mouse se movimentou entre dois instantes
// de tempo. Utilizadas no callback CursorPosCallback() abaixo.
double g_LastCursorPosX, g_LastCursorPosY;

// Função callback chamada sempre que o usuário aperta algum dos botões do mouse
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        // Se o usuário pressionou o botão esquerdo do mouse, guardamos a
        // posição atual do cursor nas variáveis g_LastCursorPosX e
        // g_LastCursorPosY.  Também, setamos a variável
        // g_LeftMouseButtonPressed como true, para saber que o usuário está
        // com o botão esquerdo pressionado.
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_LeftMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        // Quando o usuário soltar o botão esquerdo do mouse, atualizamos a
        // variável abaixo para false.
        g_LeftMouseButtonPressed = false;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        // Se o usuário pressionou o botão esquerdo do mouse, guardamos a
        // posição atual do cursor nas variáveis g_LastCursorPosX e
        // g_LastCursorPosY.  Também, setamos a variável
        // g_RightMouseButtonPressed como true, para saber que o usuário está
        // com o botão esquerdo pressionado.
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_RightMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    {
        // Quando o usuário soltar o botão esquerdo do mouse, atualizamos a
        // variável abaixo para false.
        g_RightMouseButtonPressed = false;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
    {
        // Se o usuário pressionou o botão esquerdo do mouse, guardamos a
        // posição atual do cursor nas variáveis g_LastCursorPosX e
        // g_LastCursorPosY.  Também, setamos a variável
        // g_MiddleMouseButtonPressed como true, para saber que o usuário está
        // com o botão esquerdo pressionado.
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_MiddleMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
    {
        // Quando o usuário soltar o botão esquerdo do mouse, atualizamos a
        // variável abaixo para false.
        g_MiddleMouseButtonPressed = false;
    }
}

// Função callback chamada sempre que o usuário movimentar o cursor do mouse em
// cima da janela OpenGL.
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    // Abaixo executamos o seguinte: caso o botão esquerdo do mouse esteja
    // pressionado, computamos quanto que o mouse se movimento desde o último
    // instante de tempo, e usamos esta movimentação para atualizar os
    // parâmetros que definem a posição da câmera dentro da cena virtual.
    // Assim, temos que o usuário consegue controlar a câmera.

    if (g_LeftMouseButtonPressed)
    {
        // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
        float dx = xpos - g_LastCursorPosX;
        float dy = ypos - g_LastCursorPosY;

        // Atualizamos parâmetros da câmera com os deslocamentos
        g_CameraTheta -= 0.01f*dx;
        g_CameraPhi   += 0.01f*dy;

        // Em coordenadas esféricas, o ângulo phi deve ficar entre -pi/2 e +pi/2.
        float phimax = 3.141592f/2;
        float phimin = -phimax;

        if (g_CameraPhi > phimax)
            g_CameraPhi = phimax;

        if (g_CameraPhi < phimin)
            g_CameraPhi = phimin;

        // Atualizamos as variáveis globais para armazenar a posição atual do
        // cursor como sendo a última posição conhecida do cursor.
        g_LastCursorPosX = xpos;
        g_LastCursorPosY = ypos;
    }

    if (g_RightMouseButtonPressed)
    {
        // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
        float dx = xpos - g_LastCursorPosX;
        float dy = ypos - g_LastCursorPosY;

        // Atualizamos parâmetros da antebraço com os deslocamentos
        g_ForearmAngleZ -= 0.01f*dx;
        g_ForearmAngleX += 0.01f*dy;

        // Atualizamos as variáveis globais para armazenar a posição atual do
        // cursor como sendo a última posição conhecida do cursor.
        g_LastCursorPosX = xpos;
        g_LastCursorPosY = ypos;
    }

    if (g_MiddleMouseButtonPressed)
    {
        // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
        float dx = xpos - g_LastCursorPosX;
        float dy = ypos - g_LastCursorPosY;

        // Atualizamos parâmetros da antebraço com os deslocamentos
        g_TorsoPositionX += 0.01f*dx;
        g_TorsoPositionY -= 0.01f*dy;

        // Atualizamos as variáveis globais para armazenar a posição atual do
        // cursor como sendo a última posição conhecida do cursor.
        g_LastCursorPosX = xpos;
        g_LastCursorPosY = ypos;
    }
}

// Função callback chamada sempre que o usuário movimenta a "rodinha" do mouse.
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    // Atualizamos a distância da câmera para a origem utilizando a
    // movimentação da "rodinha", simulando um ZOOM.
    g_CameraDistance -= 0.1f*yoffset;

    // Uma câmera look-at nunca pode estar exatamente "em cima" do ponto para
    // onde ela está olhando, pois isto gera problemas de divisão por zero na
    // definição do sistema de coordenadas da câmera. Isto é, a variável abaixo
    // nunca pode ser zero. Versões anteriores deste código possuíam este bug,
    // o qual foi detectado pelo aluno Vinicius Fraga (2017/2).
    const float verysmallnumber = std::numeric_limits<float>::epsilon();
    if (g_CameraDistance < verysmallnumber)
        g_CameraDistance = verysmallnumber;
}

// Definição da função que será chamada sempre que o usuário pressionar alguma
// tecla do teclado. Veja http://www.glfw.org/docs/latest/input_guide.html#input_key
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mod)
{

    // Se o usuário pressionar a tecla ESC, fechamos a janela.
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_C && action == GLFW_PRESS)
        firstpCamera = !firstpCamera;

    // NOVO
    // Se o usuário apertar a tecla R, recarregamos os shaders dos arquivos "shader_fragment.glsl" e "shader_vertex.glsl".
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        LoadShadersFromFiles();
        fprintf(stdout,"Shaders recarregados!\n");
        fflush(stdout);
    }

}

// Definimos o callback para impressão de erros da GLFW no terminal
void ErrorCallback(int error, const char* description)
{
    fprintf(stderr, "ERROR: GLFW: %s\n", description);
}

// Escrevemos na tela o número de quadros renderizados por segundo (frames per
// second).
void TextRendering_ShowFramesPerSecond(GLFWwindow* window)
{
    if ( !g_ShowInfoText )
        return;

    // Variáveis estáticas (static) mantém seus valores entre chamadas
    // subsequentes da função!
    static float old_seconds = (float)glfwGetTime();
    static int   ellapsed_frames = 0;
    static char  buffer[20] = "?? fps";
    static int   numchars = 7;

    ellapsed_frames += 1;

    // Recuperamos o número de segundos que passou desde a execução do programa
    float seconds = (float)glfwGetTime();

    // Número de segundos desde o último cálculo do fps
    float ellapsed_seconds = seconds - old_seconds;

    if ( ellapsed_seconds > 1.0f )
    {
        numchars = snprintf(buffer, 20, "%.2f fps", ellapsed_frames / ellapsed_seconds);

        old_seconds = seconds;
        ellapsed_frames = 0;
    }

    float lineheight = TextRendering_LineHeight(window);
    float charwidth = TextRendering_CharWidth(window);

    TextRendering_PrintString(window, buffer, 1.0f-(numchars + 1)*charwidth, 1.0f-lineheight, 1.0f);
}

GLuint LoadTextureFromFile(const char* filename)
{
    int width, height, channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 0);
    if (!data) {
        fprintf(stderr, "Erro ao carregar textura: %s\n", filename);
        return 0;
    }
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(data);
    return tex;
}
glm::vec3 BezierCubic(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t)
{
    float u = 1.0f - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;

    glm::vec3 p = uuu * p0; // (1-t)^3 * P0
    p += 3 * uu * t * p1;   // 3*(1-t)^2 * t * P1
    p += 3 * u * tt * p2;   // 3*(1-t) * t^2 * P2
    p += ttt * p3;          // t^3 * P3
    return p;
}
void TextRendering_ShowScore(GLFWwindow* window)
{
    if (!g_ShowInfoText)
        return;

    char buffer[80];
    snprintf(buffer, 80, "Score: %d", score);

    float lineheight = TextRendering_LineHeight(window);
    float charwidth = TextRendering_CharWidth(window);

    TextRendering_PrintString(window, buffer, -1.0f+charwidth, 1.0f-lineheight, 1.0f);
}