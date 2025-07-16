/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  * @aluno			: Fabricio Rodrigues de Santana
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdlib.h>
#include "stdio.h"
#include "atraso.h"
#include "defPrincipais.h"
#include "PRNG_LFSR.h"
#include "st7735.h"

#include "game_map.h"   // Para definições do mapa e dimensões do LCD
#include "game_logic.h" // Para lógica do jogo e colisões
#include "game_entities.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

SPI_HandleTypeDef hspi1;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* USER CODE BEGIN PV */

uint16_t ADC_buffer[2];
uint16_t valor_ADC[2];
uint16_t deaths = 0;
char buffer[32];
uint16_t wins = 0;
char buffer2[32];

int32_t player_x;
int32_t player_y;
#define NUM_BALLS 4
GameObject red_balls[NUM_BALLS];
GameObject red_balls2[NUM_BALLS];
const uint8_t PLAYER_SIZE = 8;
extern uint8_t current_level;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SPI1_Init(void);
static void MX_ADC1_Init(void);
void StartDefaultTask(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	    valor_ADC[0]=ADC_buffer[0];
		valor_ADC[1]=ADC_buffer[1];
}

//---------------------------------------------------------------------------------------------------

void vTask_Move_Balls(void *pvParameters)
{
    while (1)
    {
        for (int i = 0; i < NUM_BALLS; i++)
        {
        	if(current_level == 1){
        		UpdateObjectX(&red_balls[i]);
        		DrawObject(&red_balls[i]);
                if (CheckCollision(player_x, player_y, PLAYER_SIZE, PLAYER_SIZE,
    			   red_balls[i].x, red_balls[i].y, red_balls[i].width, red_balls[i].height))
                {

                	deaths++;
                	Game_GameOver();
                }
        	}
        	if(current_level == 2){
				UpdateObjectY(&red_balls2[i]);
				DrawObject(&red_balls2[i]);
	            if (CheckCollision(player_x, player_y, PLAYER_SIZE, PLAYER_SIZE,
				   red_balls2[i].x, red_balls2[i].y, red_balls2[i].width, red_balls2[i].height))
	            {
	            	deaths++;
	            	Game_GameOver();
	            }
			}


        }
        vTaskDelay(20);
    }
}


void vTask_Move_Soldado(void *pvParameters)
{
	int32_t current_player_x, current_player_y; // Guarda a posição atual do jogador
	int32_t next_player_x, next_player_y;     // Posição para onde o jogador tentará se mover

	while(1)
	{
		current_player_x = player_x; // Salva a posição X atual antes de calcular a próxima
		current_player_y = player_y; // Salva a posição Y atual antes de calcular a próxima

		next_player_x = player_x; // Começa a próxima posição como a atual
		next_player_y = player_y; // Começa a próxima posição como a atual

		// Apaga o player na posição anterior (pinta de preto).
		//ST7735_FillRectangle(current_player_x, current_player_y, PLAYER_SIZE, PLAYER_SIZE, ST7735_BLACK);
		ST7735_FillRectangle(current_player_x - 1, current_player_y - 1, PLAYER_SIZE + 1, PLAYER_SIZE + 1, ST7735_BLACK);

		// Leitura do Joystick
		uint32_t dif_eixoX = valor_ADC[1];
		uint32_t dif_eixoY = valor_ADC[0];

		// Ajuste os thresholds (1800, 3800) conforme a sensibilidade do seu joystick
		if(dif_eixoX < 1800)
		{
			next_player_x--;
		}
		else if(dif_eixoX > 3800)
		{
			next_player_x++;
		}
		// ------------------------------------------------------------------------
		if(dif_eixoY < 1800)
		{
			next_player_y++;
		}
		else if(dif_eixoY > 3800)
		{
			next_player_y--;
		}
		// ------------------------------------------------------------------------

		// Limites da Tela: Garante que o jogador não tente sair da área visível do LCD
		if (next_player_x < 0) next_player_x = 0;
		if (next_player_x > (ST7735_WIDTH - PLAYER_SIZE)) next_player_x = (ST7735_WIDTH - PLAYER_SIZE);
		if (next_player_y < 0) next_player_y = 0;
		if (next_player_y > (ST7735_HEIGHT - PLAYER_SIZE)) next_player_y = (ST7735_HEIGHT - PLAYER_SIZE);

        // --- LÓGICA DE JOGO: COLISÕES E ESTADO ---
		// Paredes bloqueiam o movimento.
		if (CheckWallCollision(next_player_x-1, next_player_y-1, PLAYER_SIZE+1, PLAYER_SIZE+1)) {
			player_x = current_player_x; // Volta para a posição anterior
			player_y = current_player_y; // Volta para a posição anterior
		} else {
			// Se não colidir com parede, a nova posição é válida
			player_x = next_player_x;
			player_y = next_player_y;
		}

        // 2. Colisão com Áreas de Perigo (Tiles vermelhos)
		//UpdateObject(red_balls);
		//DrawObject(red_balls);
		// A colisão com esses objetos causa o Game Over.
        if (CheckDanger(player_x, player_y, PLAYER_SIZE, PLAYER_SIZE)) {
            Game_GameOver();
            deaths++;
            sprintf(buffer, "%d", deaths);
			ST7735_WriteString(133, 4, buffer, Font_7x10, ST7735_WHITE, ST7735_BLACK);

			sprintf(buffer2, "%d", wins);
			ST7735_WriteString(18, 4, buffer2, Font_7x10, ST7735_WHITE, ST7735_BLACK);
        }

        // 3. Chegou ao Objetivo (Tiles verdes)
        // A colisão com esses objetos passa o jogador para outro nível
        if (CheckGoal(player_x, player_y, PLAYER_SIZE, PLAYER_SIZE)) {
            Game_NextLevel();
            wins++;
            sprintf(buffer2, "%d", wins);
			ST7735_WriteString(18, 4, buffer2, Font_7x10, ST7735_WHITE, ST7735_BLACK);
            sprintf(buffer, "%d", deaths);
			ST7735_WriteString(133, 4, buffer, Font_7x10, ST7735_WHITE, ST7735_BLACK);
        }

		// Redesenha o soldado na nova posição
		ST7735_draw_figure(player_x, player_y, soldado, ST7735_CYAN);

		vTaskDelay(20); // 50 quadros por segundo (ajuste para fluidez vs. processamento)
	}
}
//---------------------------------------------------------------------------------------------------
/*void vTask_Print_Soldado(void *pvParameters)
{
	while(1)
	{
		// ST7735_draw_figure(9, 30, soldado, ST7735_BLUE); // Removido
		// ST7735_draw_figure(9, 70, soldado, ST7735_GREEN); // Removido
		vTaskDelay(500); // Apenas um atraso para evitar CPU ociosa se a task ainda existir
	}
}*/
//---------------------------------------------------------------------------------------------------
void vTask_Print_ValorADC(void *pvParameters)
{
	while(1)
	{
        // Esta task pode ser útil para depuração, mas não faz parte do jogo.
		ST7735_WriteString(75, 50, "     ", Font_7x10, ST7735_GREEN, ST7735_BLACK);
		ST7735_write_nr(75, 50, valor_ADC[0], Font_7x10, ST7735_GREEN, ST7735_BLACK);

		ST7735_WriteString(75, 60, "     ", Font_7x10, ST7735_GREEN, ST7735_BLACK);
		ST7735_write_nr(75, 60, valor_ADC[1], Font_7x10, ST7735_GREEN, ST7735_BLACK);

		vTaskDelay(500);
	}
}
//---------------------------------------------------------------------------------------------------
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  uint32_t semente_PRNG=1;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI1_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */

	// inicializa LCD

  HAL_Delay(100);
  ST7735_Init();

  // --- PREPARAÇÃO DO JOGO ---
  ST7735_FillScreen(ST7735_BLACK); // Limpa a tela antes de desenhar o mapa,


  // Você pode ter uma tela de introdução antes do jogo começar.
  //ST7735_draw_figure(0, 8, fig_campo_minado, ST7735_WHITE); // Sua figura existente

  // aqui
  DrawMenu();

  //ST7735_WriteString(20, 28,"World's Hardest Game", Font_7x10, ST7735_CYAN, ST7735_BLACK);
  //ST7735_WriteString(32, 48,"Pressione Start!", Font_7x10, ST7735_CYAN, ST7735_BLACK);


  // --------------------------------------------------------------------------------------
  // inicializa nr. aleatorio e segue programção

  while(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15))// quando pressionar a tecla comea o jogo
  {
	  semente_PRNG++;
  }
  init_LFSR(semente_PRNG);	// inicializacao para geracao de numeros pseudoaleatorios

  HAL_ADC_Start_DMA(&hadc1,(uint32_t*)ADC_buffer,2);

  // --- INICIA O JOGO AQUI ---
  // O mapa 'game_map' já está pronto porque foi inicializado como 'const' na Flash.
  // Não há necessidade de chamar InitGameMap() aqui.

  // Desenha o mapa do jogo pela primeira vez

  DrawGameMap();
  sprintf(buffer2, "%d", wins);
  ST7735_WriteString(18, 4, buffer2, Font_7x10, ST7735_WHITE, ST7735_BLACK);
  sprintf(buffer, "%d", deaths);
  ST7735_WriteString(133, 4, buffer, Font_7x10, ST7735_WHITE, ST7735_BLACK);

  // Inicializa bolinhas vermelhas
  InitObject(&red_balls[0], 60, 60,  -1,  1, 5, 5, ST7735_RED);
  InitObject(&red_balls[1], 60, 75, 1,  1, 5, 5, ST7735_RED);
  InitObject(&red_balls[2], 60, 45,  1, 1, 5, 5, ST7735_RED);
  InitObject(&red_balls[3], 60, 90, -1,  1, 5, 5, ST7735_RED);

  InitObject(&red_balls2[0], 50, 60,  1,  -1, 5, 5, ST7735_RED);
  InitObject(&red_balls2[1], 70, 60, 1,  1, 5, 5, ST7735_RED);
  InitObject(&red_balls2[2], 90, 60,  1, -1, 5, 5, ST7735_RED);
  InitObject(&red_balls2[3], 110, 60, 1,  1, 5, 5, ST7735_RED);


  // Define a posição inicial do jogador
  player_x = PLAYER_START_X;
  player_y = PLAYER_START_Y;

  /*ST7735_draw_figure(player_x, player_y, soldado, ST7735_GREEN);

  DrawObject(red_balls);

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */

  // --- CRIAÇÃO DAS TASKS ---

  xTaskCreate(vTask_Move_Soldado, "SoldadoAnda", 256, NULL, osPriorityAboveNormal, NULL);
  xTaskCreate(vTask_Move_Balls, "Bolinhas", 256, NULL, osPriorityNormal, NULL);


  // xTaskCreate(vTask_Print_Soldado, "soldado", 256, NULL, osPriorityNormal, NULL);

  // xTaskCreate(vTask_Print_ValorADC, "ADC_Joystick_Debug", 256, NULL, osPriorityNormal, NULL);

  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  // Dentro de vTask_Move_Soldado ou uma nova task de GameLogic

  // Declaração de objetos (como fizemos para enemy_balls)
  GameObject meuObjeto;

  // No início da task (após InitGameMap e DrawGameMap):
  InitObject(&meuObjeto, 50, 50, 2, 1, 10, 10, ST7735_RED); // Inicializa um quadrado vermelho 10x10

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	    // ... (Seu código de movimento do jogador) ...

	    // 1. Apaga a posição antiga do jogador (já está lá)
	    // 2. Apaga a posição antiga do(s) objeto(s) em movimento
	    //    (UpdateObject() já faz isso, mas se tiver muitos, pode ser melhor antes de todos updates)

	    // 3. Atualiza a lógica de cada objeto em movimento
	  //UpdateObject(&meuObjeto); // Atualiza a posição da bolinha/objeto

	    // 4. Desenha o objeto em sua nova posição
	  //DrawObject(&meuObjeto); // Desenha a bolinha/objeto

	    // 5. Verifica colisões do jogador com o objeto
	    // if (CheckPlayerObjectCollision(player_x, player_y, PLAYER_SIZE, &meuObjeto)) {
	    //    Game_GameOver();
	    // }

	    // ... (Suas outras verificações de colisão do jogador) ...

	    // Redesenha o jogador (já está lá)
	  //ST7735_draw_figure(player_x, player_y, soldado, ST7735_GREEN);

	  //vTaskDelay(20);
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 2;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, CS_Pin|RST_Pin|DC_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PC15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : CS_Pin RST_Pin DC_Pin */
  GPIO_InitStruct.Pin = CS_Pin|RST_Pin|DC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
	while(1)
	{
		  HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

		  osDelay(200);
	}
  /* USER CODE END 5 */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM4 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM4) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  * where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
