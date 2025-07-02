// game_logic.c

#include "game_logic.h"
#include "game_map.h"   // Para acessar o mapa e os tipos de tiles
#include "game_entities.h"
#include "st7735.h"     // Para funções de desenho de texto no LCD
#include "main.h"       // Para acessar player_x, player_y e HAL_Delay

// --- Variáveis globais do jogador (declaradas em main.c, acessadas aqui como extern) ---
extern int32_t player_x;
extern int32_t player_y;
extern const uint8_t PLAYER_SIZE;

// --- FUNÇÕES DE LÓGICA DO JOGO ---

// Helper para verificar um único pixel do jogador contra um tipo de tile
static uint8_t CheckTileAt(int32_t px, int32_t py, uint8_t tile_type_to_check) {
    if (GetTileType(px, py) == tile_type_to_check) {
        return 1;
    }
    return 0;
}

// Verifica se o jogador (caixa) colide com um tile de parede.
// Verifica os 4 cantos do quadrado do jogador.
uint8_t CheckWallCollision(int32_t px, int32_t py, uint8_t p_width, uint8_t p_height) {
    // Canto superior esquerdo
    if (CheckTileAt(px, py, TILE_WALL)) return 1;
    // Canto superior direito
    if (CheckTileAt(px + p_width - 1, py, TILE_WALL)) return 1;
    // Canto inferior esquerdo
    if (CheckTileAt(px, py + p_height - 1, TILE_WALL)) return 1;
    // Canto inferior direito
    if (CheckTileAt(px + p_width - 1, py + p_height - 1, TILE_WALL)) return 1;

    return 0; // Nenhuma colisão com parede
}

// Verifica se o jogador (caixa) colide com um tile de objetivo.
uint8_t CheckGoal(int32_t px, int32_t py, uint8_t p_width, uint8_t p_height) {
    // Canto superior esquerdo
    if (CheckTileAt(px, py, TILE_GOAL)) return 1;
    // Canto superior direito
    if (CheckTileAt(px + p_width - 1, py, TILE_GOAL)) return 1;
    // Canto inferior esquerdo
    if (CheckTileAt(px, py + p_height - 1, TILE_GOAL)) return 1;
    // Canto inferior direito
    if (CheckTileAt(px + p_width - 1, py + p_height - 1, TILE_GOAL)) return 1;

    return 0; // Nenhuma colisão com objetivo
}

// Verifica se o jogador (caixa) colide com um tile de perigo.
uint8_t CheckDanger(int32_t px, int32_t py, uint8_t p_width, uint8_t p_height) {
    // Canto superior esquerdo
    if (CheckTileAt(px, py, TILE_DANGER)) return 1;
    // Canto superior direito
    if (CheckTileAt(px + p_width - 1, py, TILE_DANGER)) return 1;
    // Canto inferior esquerdo
    if (CheckTileAt(px, py + p_height - 1, TILE_DANGER)) return 1;
    // Canto inferior direito
    if (CheckTileAt(px + p_width - 1, py + p_height - 1, TILE_DANGER)) return 1;

    return 0; // Nenhuma colisão com perigo
}

// --- FUNÇÕES DE GERENCIAMENTO DE ESTADO DO JOGO ---

void Game_RestartLevel(void) {
    // Mostra mensagem e reinicia o jogador para a posição inicial.
    ST7735_FillScreen(ST7735_BLACK);
    ST7735_WriteString(20, (ST7735_HEIGHT / 2) - 10, "Reiniciando...", Font_7x10, ST7735_RED, ST7735_BLACK);
    HAL_Delay(500); // Pequena pausa para a mensagem
    player_x = PLAYER_START_X;
    player_y = PLAYER_START_Y;
    DrawGameMap(); // Redesenha o mapa completo
}

void Game_NextLevel(void) {
    // Lógica para avançar para o próximo nível.
    // Por enquanto, apenas mostra uma mensagem e reinicia o nível atual.
    ST7735_FillScreen(ST7735_BLACK);
    ST7735_WriteString(20, (ST7735_HEIGHT / 2) - 10, "Nivel Completo!", Font_7x10, ST7735_GREEN, ST7735_BLACK);
    HAL_Delay(1000); // Pausa para a mensagem
    Game_RestartLevel(); // Por enquanto, apenas reinicia o nível atual
    // No futuro, carregar um novo mapa e novos inimigos.
}

void Game_GameOver(void) {
    // Lógica para Game Over.
    ST7735_FillScreen(ST7735_BLACK);
    ST7735_WriteString(20, (ST7735_HEIGHT / 2) - 20, "GAME OVER", Font_11x18, ST7735_RED, ST7735_BLACK);
    ST7735_WriteString(10, (ST7735_HEIGHT / 2), "Tocou no Perigo!", Font_7x10, ST7735_RED, ST7735_BLACK);
    HAL_Delay(2000); // Pausa para a mensagem
    Game_RestartLevel(); // Reinicia o nível após Game Over
}

uint8_t CheckCollision(int32_t x1, int32_t y1, uint16_t w1, uint16_t h1,
                       int32_t x2, int32_t y2, uint16_t w2, uint16_t h2)
{
    return !(x1 + w1 <= x2 || x1 >= x2 + w2 ||
             y1 + h1 <= y2 || y1 >= y2 + h2);
}


