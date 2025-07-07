# Trabalho Final da disciplina de Fundamentos de Computação Gráfica.

## Divisão de contribuições entre os membros
### Alef:
- Colisãos do tipo Sphere-Box, Box-Plane e Sphere-Sphere (tentativa de implementar a Box-Box Rotacionada, mas não foi possível);
- Texturas;
- Iluminacao com Gourard Shading para determinados modelos;
- Ambient Mapping;
- Desenho da hitbox de modelos para debug;
### Henrique:
- Construção do circuito: disposição, textura e colisão das caixas. Pontuação, disposição, textura e colisão das cenouras;
- Movimentação/Pulo e Aumento/Diminuição do Coelho;
- Câmeras;
- AABB;
- Aplicação da bola de futebol como objeto animado pelo tempo com movimentação por uma curva de Bezier;
- Blinn-phong e lambert para alguns modelos;


## Relato de uso de ferramentas de IA (ChatGPT, Copilot, etc.):
### Alef:
utilizei principalmente o Gemini para saber a melhor maneira de implementar uma funcionalidade (e.g Ambient Mapping), para revisar/debugar (e.g box-box collision) e para criar funcionalidades (e.g sphere-box collision). Neste último caso, sempre tentei compreender o código gerado. Dentro da IDE, usei para completar códigos "óbvios" (e.g "printf(x = %f)" já completava com as demais coordenadas do objeto).
### Henrique:
uso de IA para o auxílio na construção de uma parcela do percurso em espiral, auxílio na dinâmica do pulo do coelho, função de mostrar score na tela. Questões de sintaxe das bibliotecas OpenGL/GLM;

## Descrição dos conceitos de CG implementados:
No total, os conceitos da disciplinas que foram implementados foram:
- Transformações geométricas;
- Sistemas de coordenadas e transformações de um para outro;
- Projeções;
- Curvas de Bézier aplicadas em animação;
- Cor;
- Iluminação (de objeto e mundo);
- Colisões;
- Texturas;

## Imagens do jogo:

## Como jogar:
- WASD para movimento do coelho;
- Espaço para pular;
- Mouse para rotacionar a câmera;
- Scroll do mouse para zoom;
- Tecla "C" para alternar entre primeira e terceira pessoa;
- Tecla "," e "." para aumentar e diminuir o tamanho do coelho, respectivamente;

## Instruções de compilação e execução:
As mesmas dos laboratórios
