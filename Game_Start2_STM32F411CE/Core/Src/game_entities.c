// Inclua o cabeçalho onde GameObject é definido
#include "game_entities.h"
#include "game_map.h" // Para checar colisões com o mapa
#include "st7735.h"   // Para funções de desenho

// Função para inicializar um objeto
void InitObject(GameObject *obj, int32_t start_x, int32_t start_y, int32_t vel_x, int32_t vel_y, uint16_t obj_width, uint16_t obj_height, uint16_t obj_color) {
    obj->x = start_x;
    obj->y = start_y;
    obj->dx = vel_x;
    obj->dy = vel_y;
    obj->width = obj_width;
    obj->height = obj_height;
    obj->color = obj_color;
    // Se você tiver sprites, inicializaria obj->sprite aqui
}

// Função para atualizar a posição de um objeto
void UpdateObject(GameObject *obj) {
    // 1. Apaga o objeto na posição antiga (necessário se o fundo não for preto)
    // Para um fundo preto, ST7735_FillRectangle(old_x, old_y, width, height, ST7735_BLACK) funciona.
    // Para mapas com cores, você precisa redesenhar os tiles que estavam por baixo.
    ST7735_FillRectangle(obj->x, obj->y, obj->width, obj->height, ST7735_BLACK); // Apaga

    // 2. Calcula a nova posição
    obj->x += obj->dx;
    obj->y += obj->dy;

    // 3. Lógica de Colisão com as Bordas da Tela (exemplo)
    if (obj->x < 0 || obj->x + obj->width > ST7735_WIDTH) {
        obj->dx *= -1; // Inverte a direção X
        // Ajusta a posição para não ficar fora da tela
        if (obj->x < 0) obj->x = 0;
        if (obj->x + obj->width > ST7735_WIDTH) obj->x = ST7735_WIDTH - obj->width;
    }
    if (obj->y < 0 || obj->y + obj->height > ST7735_HEIGHT) {
        obj->dy *= -1; // Inverte a direção Y
        // Ajusta a posição para não ficar fora da tela
        if (obj->y < 0) obj->y = 0;
        if (obj->y + obj->height > ST7735_HEIGHT) obj->y = ST7735_HEIGHT - obj->height;
    }

    // 4. Lógica de Colisão com o Mapa (Paredes, etc.)
    // Você pode usar suas funções CheckWallCollision, CheckDanger aqui, mas para o objeto.
    // Isso é mais complexo, pois pode precisar de um "ricochete" ou um caminho predefinido.
    // Exemplo simplificado de colisão com parede:
    if (CheckWallCollision(obj->x, obj->y, obj->width, obj->height)) {
        obj->dx *= -1; // Inverte a direção
        obj->dy *= -1; // Inverte a direção
        // Recua um pouco para não ficar preso na parede
        obj->x -= obj->dx;
        obj->y -= obj->dy;
    }
}

// Função para desenhar um objeto
void DrawObject(GameObject *obj) {
    // Desenha o objeto na sua nova posição
    ST7735_FillRectangle(obj->x, obj->y, obj->width, obj->height, obj->color);
    // Se você estiver usando um sprite:
    // ST7735_draw_figure(obj->x, obj->y, *(obj->sprite), obj->color);
}
