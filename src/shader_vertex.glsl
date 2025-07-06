#version 330 core

// Atributos de vértice recebidos como entrada ("in") pelo Vertex Shader.
// Veja a função BuildTrianglesAndAddToVirtualScene() em "main.cpp".
layout (location = 0) in vec4 model_coefficients;
layout (location = 1) in vec4 normal_coefficients;
layout (location = 2) in vec2 texture_coefficients;

// Matrizes computadas no código C++ e enviadas para a GPU
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
// NOVO
uniform int object_id;
uniform vec4 camera_position;

// Atributos de vértice que serão gerados como saída ("out") pelo Vertex Shader.
// ** Estes serão interpolados pelo rasterizador! ** gerando, assim, valores
// para cada fragmento, os quais serão recebidos como entrada pelo Fragment
// Shader. Veja o arquivo "shader_fragment.glsl".
out vec4 position_world;
out vec4 normal;
out vec2 texcoords;
// NOVO
out vec4 camera_pos;
out vec4 position_model;
out vec4 shading_gourard_color;
uniform sampler2D TextureImage6;

#define CARROT 7

void main()
{
    // A variável gl_Position define a posição final de cada vértice
    // OBRIGATORIAMENTE em "normalized device coordinates" (NDC), onde cada
    // coeficiente estará entre -1 e 1 após divisão por w.
    // Veja {+NDC2+}.
    //
    // O código em "main.cpp" define os vértices dos modelos em coordenadas
    // locais de cada modelo (array model_coefficients). Abaixo, utilizamos
    // operações de modelagem, definição da câmera, e projeção, para computar
    // as coordenadas finais em NDC (variável gl_Position). Após a execução
    // deste Vertex Shader, a placa de vídeo (GPU) fará a divisão por W. Veja
    // slides 41-67 e 69-86 do documento Aula_09_Projecoes.pdf.

    gl_Position = projection * view * model * model_coefficients;

    // Como as variáveis acima  (tipo vec4) são vetores com 4 coeficientes,
    // também é possível acessar e modificar cada coeficiente de maneira
    // independente. Esses são indexados pelos nomes x, y, z, e w (nessa
    // ordem, isto é, 'x' é o primeiro coeficiente, 'y' é o segundo, ...):
    //
    //     gl_Position.x = model_coefficients.x;
    //     gl_Position.y = model_coefficients.y;
    //     gl_Position.z = model_coefficients.z;
    //     gl_Position.w = model_coefficients.w;
    //

    // Agora definimos outros atributos dos vértices que serão interpolados pelo
    // rasterizador para gerar atributos únicos para cada fragmento gerado.

    // Posição do vértice atual no sistema de coordenadas global (World).
    position_world = model * model_coefficients;
    // NOVO
    position_model = model_coefficients;
    // Normal do vértice atual no sistema de coordenadas global (World).
    // Veja slides 123-151 do documento Aula_07_Transformacoes_Geometricas_3D.pdf.
    normal = inverse(transpose(model)) * normal_coefficients;
    normal.w = 0.0;


// Parâmetros que definem as propriedades espectrais da superfície
    vec3 Kd;
    vec3 Ks; 
    vec3 Ka; 
    float q;

    if (object_id == CARROT){
        Kd = vec3(1.0, 1.0, 1.0);
        Ks = vec3(0.2, 0.2, 0.2);
        Ka = Kd * 0.2;
        q = 16.0;
    }

    //Calculos para a cor em shading gourard
    
    camera_pos = camera_position;
    vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 camera_position = inverse(view) * origin;
    normal = normalize(normal);
    vec4 l = normalize(camera_position - position_world);
    vec4 v = normalize(camera_position - position_world);
    vec4 r = normalize(reflect(-l, normal));
    vec4 h = normalize(v + l);
    vec3 I = vec3(1.0,1.0,1.0);
    vec3 Ia = vec3(0.2,0.2,0.2);

    vec3 ambient_term = Ka * Ia;
    vec3 lambert_diffuse_term = Kd * I * max(0.0, dot(normalize(normal.xyz), normalize(l.xyz)));
    vec3 blinn_phong_specular_term = Ks * I * pow(max(0.0, dot(normal.xyz, h.xyz)), q);

    float U = 0.0;
    float V = 0.0;
    vec3 tex_obj;
    
    texcoords = texture_coefficients;
    U = texcoords.x;
    V = texcoords.y;
    tex_obj = texture(TextureImage6, vec2(U,V)).rgb;

    vec3 textured_ambient = tex_obj * ambient_term;
    vec3 textured_diffuse = tex_obj * lambert_diffuse_term;
    shading_gourard_color.rgb = textured_ambient + textured_diffuse + blinn_phong_specular_term;
}
