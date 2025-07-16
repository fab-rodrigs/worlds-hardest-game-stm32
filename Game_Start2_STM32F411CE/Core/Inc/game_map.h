// game_map.h

#ifndef __GAME_MAP_H
#define __GAME_MAP_H

#include <stdint.h>

// --- DIMENSÕES DO LCD (AJUSTE CONFORME SEU SETUP) ---
// POR FAVOR, VERIFIQUE ESTES VALORES COM O SEU ST7735.H OU O DATASHEET DO SEU LCD.
// Se sua tela é 128 de altura e 160 de largura (como no mapa que você colou),
// estes defines estão corretos.
#ifndef ST7735_WIDTH
#define ST7735_WIDTH   160 // <--- VERIFIQUE E AJUSTE ISTO SE NECESSÁRIO
#endif
#ifndef ST7735_HEIGHT
#define ST7735_HEIGHT  128 // <--- VERIFIQUE E AJUSTE ISTO SE NECESSÁRIO
#endif

// --- TIPOS DE TILES (BLOCOS) DO MAPA ---
// Esses defines devem estar SEM o sinal de igual (=)
#define TILE_EMPTY   0
#define TILE_WALL    1
#define TILE_GOAL    2
#define TILE_START   3
#define TILE_DANGER  4

// --- POSIÇÃO INICIAL DO JOGADOR ---
#define PLAYER_START_X 20
#define PLAYER_START_Y 65

// --- VARIÁVEIS GLOBAIS DO JOGO ---
// O mapa agora é 'const' e lido diretamente da Flash.
extern const uint8_t game_map[ST7735_HEIGHT][ST7735_WIDTH];

// --- FUNÇÕES RELACIONADAS AO MAPA ---
// A função InitGameMap() NÃO é mais necessária se o mapa é 'const'
// e já está preenchido na declaração.
// void InitGameMap(void); // <--- REMOVA OU COMENTE ESTA LINHA

// Desenha todo o mapa na tela do LCD.
void DrawGameMap(void);

// Retorna o tipo de tile em uma coordenada (x, y) específica.
uint8_t GetTileType(int x, int y);

#endif // __GAME_MAP_H
