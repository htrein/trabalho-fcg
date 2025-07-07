#version 330 core

// Atributos de fragmentos recebidos como entrada ("in") pelo Fragment Shader.
// Neste exemplo, este atributo foi gerado pelo rasterizador como a
// interpolação da posição global e a normal de cada vértice, definidas em
// "shader_vertex.glsl" e "main.cpp".
in vec4 position_world;
in vec4 normal;
in vec2 texcoords;
//NOVO
in vec4 camera_pos;
in vec4 position_model;
in vec4 shading_gourard_color;

// Matrizes computadas no código C++ e enviadas para a GPU
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

//NOVO
uniform sampler2D TextureImage0;
uniform sampler2D TextureImage1;
uniform sampler2D TextureImage2;
uniform sampler2D TextureImage3;
uniform sampler2D TextureImage4;
uniform sampler2D TextureImage5;
uniform sampler2D TextureImage6;
uniform sampler2D TextureImage7;

// Identificador que define qual objeto está sendo desenhado no momento
#define SPHERE 0
#define BUNNY  1
#define PLANE  2
#define CHAIR 3
// NOVO
#define SKY_SPHERE 4
#define BOX 5
#define SOCCER_BALL 6
#define CARROT 7
#define BOOK 8

uniform int object_id;

// O valor de saída ("out") de um Fragment Shader é a cor final do fragmento.
out vec4 color;

// NOVO
// Constantes
#define M_PI   3.14159265358979323846
#define M_PI_2 1.57079632679489661923

void main()
{
    // Obtemos a posição da câmera utilizando a inversa da matriz que define o
    // sistema de coordenadas da câmera.
    vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 camera_position = inverse(view) * origin;

    // O fragmento atual é coberto por um ponto que percente à superfície de um
    // dos objetos virtuais da cena. Este ponto, p, possui uma posição no
    // sistema de coordenadas global (World coordinates). Esta posição é obtida
    // através da interpolação, feita pelo rasterizador, da posição de cada
    // vértice.
    vec4 p = position_world;

    // Normal do fragmento atual, interpolada pelo rasterizador a partir das
    // normais de cada vértice.
    vec4 n = normalize(normal);

    // Vetor que define o sentido da fonte de luz em relação ao ponto atual.
    vec4 l = normalize(camera_position - p);

    // Vetor que define o sentido da câmera em relação ao ponto atual.
    vec4 v = normalize(camera_position - p);

    // Vetor que define o sentido da reflexão especular ideal.
    vec4 r = normalize(reflect(-l, n));

    // Parâmetros que definem as propriedades espectrais da superfície
    vec3 Kd; // Refletância difusa
    vec3 Ks; // Refletância especular
    vec3 Ka; // Refletância ambiente
    float q; // Expoente especular para o modelo de iluminação de Phong

    vec4 h = normalize(v + l); //vetor para BLinn-Phong

    if ( object_id == SPHERE )
    {
        // Propriedades espectrais da esfera
        Kd = vec3(0.8,0.4,0.08);
        Ks = vec3(0.0,0.0,0.0);
        Ka = Kd * 0.5;
        q = 1.0;
    }
    else if ( object_id == BUNNY )
    {
        // Propriedades espectrais do coelho
        Kd = vec3(1.0, 1.0, 1.0);
        Ks = vec3(0.3, 0.3, 0.3);
        Ka = vec3(0.2, 0.2, 0.2);
        q = 16.0;

    }
    else if ( object_id == PLANE )
    {
        // Propriedades espectrais do plano
        Kd = vec3(1.0, 1.0, 1.0);
        Ks = vec3(0.3,0.3,0.3);
        Ka = vec3(0.2,0.2,0.2);
        q = 20.0;
    }
    else if (object_id == CHAIR){
        Kd = vec3(1.0, 1.0, 1.0);
        Ks = vec3(0.3, 0.3, 0.3);
        Ka = Kd * 0.2;
        q = 16.0;
    }
    else if (object_id == BOX){
        Kd = vec3(1.0, 1.0, 1.0);
        Ks = vec3(0.1, 0.1, 0.1);
        Ka = Kd * 0.2;
        q = 8.0;
    }
    else if (object_id == SOCCER_BALL){
        Kd = vec3(1.0, 1.0, 1.0);
        Ks = vec3(0.5, 0.5, 0.5);
        Ka = Kd * 0.2;
        q = 40.0;
    }
    else if (object_id == CARROT){
        Kd = vec3(1.0, 1.0, 1.0);
        Ks = vec3(0.2, 0.2, 0.2);
        Ka = Kd * 0.2;
        q = 16.0;
    }
    else if (object_id == BOOK){
        Kd = vec3(1.0, 1.0, 1.0);
        Ks = vec3(0.2, 0.2, 0.2);
        Ka = Kd * 0.2;
        q = 16.0;
    }
    else // Objeto desconhecido = preto
    {
        Kd = vec3(0.0,0.0,0.0);
        Ks = vec3(0.0,0.0,0.0);
        Ka = vec3(0.0,0.0,0.0);
        q = 1.0;
    }

    // Espectro da fonte de iluminação
    vec3 I = vec3(1.0,1.0,1.0);

    // Espectro da luz ambiente
    vec3 Ia = vec3(0.2,0.2,0.2);

    // Termo difuso utilizando a lei dos cossenos de Lambert
    vec3 lambert_diffuse_term = Kd * I * max(0.0, dot(normalize(n.xyz), normalize(l.xyz)));

    // Termo ambiente
    vec3 ambient_term = Ka * Ia;

    // Termo especular utilizando Blinn-Phong
    vec3 blinn_phong_specular_term = Ks * I * pow(max(0.0, dot(n.xyz, h.xyz)), q);

    // NOTE: Se você quiser fazer o rendering de objetos transparentes, é
    // necessário:
    // 1) Habilitar a operação de "blending" de OpenGL logo antes de realizar o
    //    desenho dos objetos transparentes, com os comandos abaixo no código C++:
    //      glEnable(GL_BLEND);
    //      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // 2) Realizar o desenho de todos objetos transparentes *após* ter desenhado
    //    todos os objetos opacos; e
    // 3) Realizar o desenho de objetos transparentes ordenados de acordo com
    //    suas distâncias para a câmera (desenhando primeiro objetos
    //    transparentes que estão mais longe da câmera).
    // Alpha default = 1 = 100% opaco = 0% transparente
    color.a = 1;

    vec3 tex_obj;
    vec2 final_texcoords = texcoords;

    if (object_id == PLANE)
    {
        // Para o plano, usamos as coordenadas de mundo para que a textura pareça estática.
        // O fator 0.1 serve para escalar a textura (deixe-a maior).
        final_texcoords = position_world.xz * 0.1;
    }

    if (object_id == BUNNY) {
        tex_obj = texture(TextureImage0, final_texcoords).rgb;
    } else if ( object_id == CHAIR){
        tex_obj = texture(TextureImage1, final_texcoords).rgb;
    } else if (object_id == SKY_SPHERE) {
        float sphere_rho = 1;
        vec4 camera_pos_a = vec4(camera_pos.x, camera_pos.y, camera_pos.z, 1);
        vec4 p_line = camera_pos_a + sphere_rho*normalize(position_model - camera_pos_a);
        vec4 p_vector = p_line - camera_pos;
        float sphere_theta = atan(p_vector[0], p_vector[2]);
        float sphere_psi = asin(p_vector[1]/sphere_rho);
        tex_obj = texture(TextureImage2, vec2((sphere_theta + M_PI) / (2*M_PI), (sphere_psi + M_PI_2)/ M_PI)).rgb;
    } else if (object_id == BOX) {
        tex_obj = texture(TextureImage3, final_texcoords).rgb;
    } else if (object_id == SOCCER_BALL) {
        tex_obj = texture(TextureImage4, final_texcoords).rgb;
    } else if(object_id == PLANE){
        tex_obj = texture(TextureImage5, final_texcoords).rgb;
    } else if (object_id == CARROT) {
        tex_obj = texture(TextureImage6, final_texcoords).rgb;
    } else if (object_id == BOOK) {
        tex_obj = texture(TextureImage7, final_texcoords).rgb; 
    }

    if (object_id == SKY_SPHERE)
    {
        color.rgb = tex_obj;
    }
    else if (object_id == SPHERE)
    {
        color.rgb = ambient_term + lambert_diffuse_term + blinn_phong_specular_term;
    }
    else
    {
        vec3 textured_ambient = tex_obj * ambient_term;
        vec3 textured_diffuse = tex_obj * lambert_diffuse_term;
        color.rgb = textured_ambient + textured_diffuse + blinn_phong_specular_term;
    }

    if (object_id == CARROT) {
        color = shading_gourard_color;
    }
    // Cor final com correção gamma, considerando monitor sRGB.
    // Veja https://en.wikipedia.org/w/index.php?title=Gamma_correction&oldid=751281772#Windows.2C_Mac.2C_sRGB_and_TV.2Fvideo_standard_gammas
    color.rgb = pow(color.rgb, vec3(1.0,1.0,1.0)/2.2);
}