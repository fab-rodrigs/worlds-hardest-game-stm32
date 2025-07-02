// game_logic.h

#ifndef __GAME_LOGIC_H
#define __GAME_LOGIC_H

#include <stdint.h>
#include "game_entities.h"

// --- FUNÇÕES DE LÓGICA DO JOGO ---

// Verifica se o jogador (retângulo definido por px, py, p_width, p_height)
// colidiu com uma parede (TILE_WALL) no mapa.
// Retorna 1 se houver colisão, 0 caso contrário.
uint8_t CheckWallCollision(int32_t px, int32_t py, uint8_t p_width, uint8_t p_height);

// Verifica se o jogador atingiu a área de objetivo (TILE_GOAL).
// Retorna 1 se atingiu, 0 caso contrário.
uint8_t CheckGoal(int32_t px, int32_t py, uint8_t p_width, uint8_t p_height);

// Verifica se o jogador tocou em uma área de perigo (TILE_DANGER).
// Retorna 1 se tocou, 0 caso contrário.
uint8_t CheckDanger(int32_t px, int32_t py, uint8_t p_width, uint8_t p_height);

// --- FUNÇÕES DE GERENCIAMENTO DE ESTADO DO JOGO ---

// Reinicia o nível atual, movendo o jogador de volta para o ponto inicial.
void Game_RestartLevel(void);

// Avança para o próximo nível (futuramente).
void Game_NextLevel(void);

// Lida com a condição de Game Over.
void Game_GameOver(void);

uint8_t CheckCollision(int32_t x1, int32_t y1, uint16_t w1, uint16_t h1,
                       int32_t x2, int32_t y2, uint16_t w2, uint16_t h2);

#endif // __GAME_LOGIC_H
