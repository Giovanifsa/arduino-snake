#include <Arduino.h>
#include <LedControl.h>

/*
    Código fonte de: Giovani Frigo e Jackson Renato
    Técnico em Informática 2015
*/

//#define MATRIX_VCC 13
//#define MATRIX_GND 12
#define MATRIX_DIN 7
#define MATRIX_CS 8
#define MATRIX_CLK 9
#define MATRIX_QUANT 1
#define TAMANHO_X 8
#define TAMANHO_Y 8
#define BOTAO_CIMA 3
#define BOTAO_BAIXO 5
#define BOTAO_ESQUERDA 2
#define BOTAO_DIREITA 6
#define BUZZER 1
#define VELOCIDADE 250L
#define FIM_JOGO_PISCADAS 6
#define TEMPO_DESATIVAR_BUZZER 20

struct PosicaoCorpo {
    bool aceso;
    byte posX;
    byte posY;
};

struct PosicaoComida {
    bool aceso;
    byte posX;
    byte posY;
};

enum DirecaoMovimento {
    ESQUERDA,
    CIMA,
    DIREITA,
    BAIXO
};

//Controle do mundo
PosicaoCorpo posicoesSnake[TAMANHO_X * TAMANHO_Y];
byte tamanhoCorpo = 0;
PosicaoComida posicaoComida;
DirecaoMovimento direcaoMovimentoAtual;
DirecaoMovimento direcaoMovimentoAlterar;

bool fimJogo = false;
bool fimJogoAceso = true;
int piscadasFimJogo = 0;

//Controle botões e velocidade
unsigned long ultimoPasso = 0;
unsigned long tempoBuzzerAtivado = 0;
bool botaoBaixo = false;
bool botaoCima = false;
bool botaoEsquerda = false;
bool botaoDireita = false;
bool buzzerAtivo = true;

//Controle led
LedControl ledControl(MATRIX_DIN, MATRIX_CLK, MATRIX_CS, MATRIX_QUANT);

void gerarComida() {
    byte x, y;
    bool encontrado;

    do {
        encontrado = false;

        x = random(0, TAMANHO_X);
        y = random(0, TAMANHO_Y);

        for (int i = 0; i < tamanhoCorpo; i++) {
            if (posicoesSnake[i].posX == x && posicoesSnake[i].posY == y) {
                encontrado = true;
            }
        }
    } while (encontrado);

    posicaoComida.posX = x;
    posicaoComida.posY = y;
    posicaoComida.aceso = true;
}

void ativarBuzzer() {
    digitalWrite(BUZZER, HIGH);
    buzzerAtivo = true;

    tempoBuzzerAtivado = millis();
}

void reset() {
    ledControl.shutdown(0, false);
    ledControl.setIntensity(0, 3);
    ledControl.clearDisplay(0);

    tamanhoCorpo = 0;
    fimJogo = false;
    fimJogoAceso = true;
    piscadasFimJogo = 0;

    posicoesSnake[tamanhoCorpo].posX = random(0, TAMANHO_X);
    posicoesSnake[tamanhoCorpo].posY = random(0, TAMANHO_Y);
    posicoesSnake[tamanhoCorpo++].aceso = true;
    direcaoMovimentoAtual = ESQUERDA;
    direcaoMovimentoAlterar = ESQUERDA;
    
    gerarComida();
}

void setup() {
    //pinMode(MATRIX_VCC, OUTPUT);
    //pinMode(MATRIX_GND, OUTPUT);
    pinMode(MATRIX_DIN, OUTPUT);
    pinMode(MATRIX_CS, OUTPUT);
    pinMode(MATRIX_CLK, OUTPUT);
    pinMode(BOTAO_CIMA, INPUT_PULLUP);
    pinMode(BOTAO_BAIXO, INPUT_PULLUP);
    pinMode(BOTAO_ESQUERDA, INPUT_PULLUP);
    pinMode(BOTAO_DIREITA, INPUT_PULLUP);
    pinMode(BUZZER, OUTPUT);

    //digitalWrite(MATRIX_VCC, HIGH);
    //digitalWrite(MATRIX_GND, LOW);

    reset();
}

void loop() {
    if ((ultimoPasso + VELOCIDADE) <= millis()) {
        if (!fimJogo) {
            byte proximoX = posicoesSnake[0].posX;
            byte proximoY = posicoesSnake[0].posY;

            if (direcaoMovimentoAlterar != direcaoMovimentoAtual) {
                direcaoMovimentoAtual = direcaoMovimentoAlterar;
            }

            if (direcaoMovimentoAtual== ESQUERDA) {
                if (proximoX == 0) {
                    proximoX = TAMANHO_X - 1;
                }

                else {
                    proximoX--;
                }
            }

            else if (direcaoMovimentoAtual == CIMA) {
                if (proximoY == (TAMANHO_Y - 1)) {
                    proximoY = 0;
                }

                else {
                    proximoY++;
                }
            }

            else if (direcaoMovimentoAtual == DIREITA) {
                if (proximoX == (TAMANHO_X - 1)) {
                    proximoX = 0;
                }

                else {
                    proximoX++;
                }
            }

            else if (direcaoMovimentoAtual == BAIXO) {
                if (proximoY == 0) {
                    proximoY = TAMANHO_Y - 1;
                }

                else {
                    proximoY--;
                }
            }

            if (!fimJogo) {
                bool crescer = false;

                if (proximoX == posicaoComida.posX && proximoY == posicaoComida.posY) {
                    crescer = true;

                    ativarBuzzer();
                }

                if (tamanhoCorpo > 1) {
                    byte ultimoX = posicoesSnake[0].posX, ultimoY = posicoesSnake[0].posY;
                    byte ultimoXLoop, ultimoYLoop;

                    posicoesSnake[0].posX = proximoX;
                    posicoesSnake[0].posY = proximoY;

                    for (int i = 1; i < tamanhoCorpo; i++) {
                        ultimoXLoop = posicoesSnake[i].posX;
                        ultimoYLoop = posicoesSnake[i].posY;

                        posicoesSnake[i].posX = ultimoX;
                        posicoesSnake[i].posY = ultimoY;

                        ultimoX = ultimoXLoop;
                        ultimoY = ultimoYLoop;
                    }

                    if (crescer) {
                        posicoesSnake[tamanhoCorpo].posX = ultimoX;
                        posicoesSnake[tamanhoCorpo].posY = ultimoY;
                        posicoesSnake[tamanhoCorpo++].aceso = true;

                        gerarComida();
                    }
                }

                else {
                    if (crescer) {
                        posicoesSnake[tamanhoCorpo].posX = posicoesSnake[0].posX;
                        posicoesSnake[tamanhoCorpo].posY = posicoesSnake[0].posY;
                        posicoesSnake[tamanhoCorpo++].aceso = true;

                        gerarComida();
                    }

                    posicoesSnake[0].posX = proximoX;
                    posicoesSnake[0].posY = proximoY;
                }

                ledControl.clearDisplay(0);

                ledControl.setLed(0, posicaoComida.posX, posicaoComida.posY, posicaoComida.aceso);

                for (int i = 0; i < tamanhoCorpo; i++) {
                    ledControl.setLed(0, posicoesSnake[i].posX, posicoesSnake[i].posY, posicoesSnake[i].aceso);
                    
                    if (i != 0 && posicoesSnake[0].posX == posicoesSnake[i].posX && posicoesSnake[0].posY == posicoesSnake[i].posY) {
                        fimJogo = true;
                    }
                }

                if (tamanhoCorpo == (TAMANHO_X * TAMANHO_Y)) {
                    fimJogo = true;
                }
            }
        }

        else {
            if (piscadasFimJogo <= FIM_JOGO_PISCADAS) {
                for (int i = 0; i < tamanhoCorpo; i++) {
                    ledControl.setLed(0, posicoesSnake[i].posX, posicoesSnake[i].posY, (piscadasFimJogo % 2 == 0));
                }

                piscadasFimJogo++;
            }

            else {
                reset();
            }
        }

        ultimoPasso = millis();
    }

    if (!digitalRead(BOTAO_CIMA)) {
        if (!botaoCima) {
            if (direcaoMovimentoAtual != BAIXO) {
                direcaoMovimentoAlterar = CIMA;
            }

            botaoCima = true;
        }
    }

    else if (botaoCima) {
        botaoCima = false;
    }

    if (!digitalRead(BOTAO_BAIXO)) {
        if (!botaoBaixo) {
            if (direcaoMovimentoAtual != CIMA) {
                direcaoMovimentoAlterar = BAIXO;
            }

            botaoBaixo = true;
        }
    }

    else if (botaoBaixo) {
        botaoBaixo = false;
    }

    if (!digitalRead(BOTAO_ESQUERDA)) {
        if (!botaoEsquerda) {
            if (direcaoMovimentoAtual != DIREITA) {
                direcaoMovimentoAlterar = ESQUERDA;
            }

            botaoEsquerda = true;
        }
    }

    else if (botaoEsquerda) {
        botaoEsquerda = false;
    }

    if (!digitalRead(BOTAO_DIREITA)) {
        if (!botaoDireita) {
            if (direcaoMovimentoAtual != ESQUERDA) {
                direcaoMovimentoAlterar = DIREITA;
            }

            botaoDireita = true;
        }
    }

    else if (botaoDireita) {
        botaoDireita = false;
    }

    if (buzzerAtivo && millis() >= (tempoBuzzerAtivado + TEMPO_DESATIVAR_BUZZER)) {
        digitalWrite(BUZZER, LOW);

        buzzerAtivo = false;
        tempoBuzzerAtivado = 0;
    }
}
