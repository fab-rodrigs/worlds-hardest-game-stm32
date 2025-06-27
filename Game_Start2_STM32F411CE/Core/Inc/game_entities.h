// game_entities.h
#ifndef __GAME_ENTITIES_H
#define __GAME_ENTITIES_H

#include <stdint.h>
#include "st7735.h" // Se precisar de dimensões da tela aqui

// Estrutura genérica para um objeto em movimento
typedef struct {
    int32_t x;       // Posição X (coordenada horizontal)
    int32_t y;       // Posição Y (coordenada vertical)
    int32_t dx;      // Velocidade/direção no eixo X (delta X)
    int32_t dy;      // Velocidade/direção no eixo Y (delta Y)
    uint16_t width;  // Largura do objeto (se for um quadrado/sprite)
    uint16_t height; // Altura do objeto
    uint16_t color;  // Cor do objeto (para desenho simples)
    // Se for uma imagem (sprite), você teria:
    // FigDef *sprite; // Ponteiro para a definição da figura (FigDef)
} GameObject;

// Você pode ter arrays de GameObjects, por exemplo:
// #define MAX_OBJECTS 10
// extern GameObject moving_objects[MAX_OBJECTS];

// Funções para gerenciar esses objetos (declaradas aqui)
void InitObject(GameObject *obj, int32_t start_x, int32_t start_y, int32_t vel_x, int32_t vel_y, uint16_t obj_width, uint16_t obj_height, uint16_t obj_color);
void UpdateObject(GameObject *obj);
void DrawObject(GameObject *obj);

#endif // __GAME_ENTITIES_H
