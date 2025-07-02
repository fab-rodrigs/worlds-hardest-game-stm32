#include "game_entities.h"
#include "game_map.h"
#include "st7735.h"

// Inicializa o objeto
void InitObject(GameObject *obj, int32_t start_x, int32_t start_y, int32_t vel_x, int32_t vel_y, uint16_t obj_width, uint16_t obj_height, uint16_t obj_color) {
    obj->x = start_x;
    obj->y = start_y;
    obj->dx = vel_x;
    obj->dy = vel_y;
    obj->width = obj_width;
    obj->height = obj_height;
    obj->color = obj_color;
}

// Verifica rebote entre dois limites no eixo X
void CheckReboundX(GameObject *obj, int32_t x_min, int32_t x_max) {
    if (obj->x < x_min) {
        obj->x = x_min;
        obj->dx *= -1;
    }
    if (obj->x + obj->width > x_max) {
        obj->x = x_max - obj->width;
        obj->dx *= -1;
    }
}

// Atualiza a posição do objeto apenas no eixo X com rebote
void UpdateObject(GameObject *obj) {
    // Apaga o objeto antigo
    ST7735_FillRectangle(obj->x, obj->y, obj->width, obj->height, ST7735_BLACK);

    // Atualiza apenas o eixo X
    obj->x += obj->dx;

    // Rebote apenas entre x = 20 e x = 100 (exemplo)
    CheckReboundX(obj, 40, 120);

    // Desenha na nova posição
    DrawObject(obj);
}

// Desenha o objeto na tela
void DrawObject(GameObject *obj) {
    ST7735_FillRectangle(obj->x, obj->y, obj->width, obj->height, obj->color);
}
