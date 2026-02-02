#include "../include/Simulador.hpp"
#include <sstream>
#include <iomanip>
#include <fstream>

Simulador::Simulador(int largura, int altura) 
    : larguraTela(largura), alturaTela(altura), 
      estadoAtual(EstadoJogo::TELA_INICIAL), fase(1), jogoTerminado(false),
      vidaSupercomputador(100), vidaMaximaSupercomputador(100),
      degradacaoVisual(0), velocidadeSimulacao(1.0f), pausado(false),
      tempoTotal(0), geracao(0), zonaSelecionada(ZonaPlaneta::HABITAVEL),
      eventoSelecionado(TipoEvento::NENHUM), tempoMensagem(0) {
    
    centroTela = {largura / 2.0f, altura / 2.0f};
    raioOblivion = 400.0f;
    
    inicializar();
}

Simulador::~Simulador() {}

void Simulador::inicializar() {
    // Criar ambientes
    ambienteNucleo = std::make_unique<Ambiente>(ZonaPlaneta::NUCLEO);
    ambienteHabitavel = std::make_unique<Ambiente>(ZonaPlaneta::HABITAVEL);
    ambientePeriferia = std::make_unique<Ambiente>(ZonaPlaneta::PERIFERIA);
    
    // Criar populações
    populacaoNucleo = std::make_unique<Populacao>();
    populacaoHabitavel = std::make_unique<Populacao>();
    populacaoPeriferia = std::make_unique<Populacao>();
    
    // Inicializar populações
    populacaoNucleo->inicializarPopulacao(ZonaPlaneta::NUCLEO, 15);
    populacaoHabitavel->inicializarPopulacao(ZonaPlaneta::HABITAVEL, 20);
    populacaoPeriferia->inicializarPopulacao(ZonaPlaneta::PERIFERIA, 10);
    
    // Criar gestor de missões
    gestorMissoes = std::make_unique<GestorMissoes>();
}

void Simulador::executar() {
    InitWindow(larguraTela, alturaTela, "Observador - Projeto Oblivion");
    SetTargetFPS(60);
    
    while (!WindowShouldClose() && !jogoTerminado) {
        processarInput();
        atualizar();
        renderizar();
    }
    
    CloseWindow();
}

void Simulador::processarInput() {
    if (estadoAtual == EstadoJogo::TELA_INICIAL) {
        if (IsKeyPressed(KEY_ENTER)) {
            iniciarJogo();
        }
    }
    else if (estadoAtual == EstadoJogo::JOGANDO) {
        if (IsKeyPressed(KEY_SPACE)) {
            pausarJogo();
        }
        if (IsKeyPressed(KEY_E)) {
            abrirMenuEventos();
        }
        if (IsKeyPressed(KEY_M)) {
            abrirMenuMissoes();
        }
        
        // Controle de velocidade
        if (IsKeyPressed(KEY_UP)) {
            velocidadeSimulacao *= 1.5f;
            if (velocidadeSimulacao > 4.0f) velocidadeSimulacao = 4.0f;
        }
        if (IsKeyPressed(KEY_DOWN)) {
            velocidadeSimulacao /= 1.5f;
            if (velocidadeSimulacao < 0.25f) velocidadeSimulacao = 0.25f;
        }
    }
    else if (estadoAtual == EstadoJogo::PAUSADO) {
        if (IsKeyPressed(KEY_SPACE)) {
            continuarJogo();
        }
    }
    else if (estadoAtual == EstadoJogo::MENU_EVENTOS) {
        // Selecionar zona
        if (IsKeyPressed(KEY_ONE)) zonaSelecionada = ZonaPlaneta::NUCLEO;
        if (IsKeyPressed(KEY_TWO)) zonaSelecionada = ZonaPlaneta::HABITAVEL;
        if (IsKeyPressed(KEY_THREE)) zonaSelecionada = ZonaPlaneta::PERIFERIA;
        
        // Selecionar evento
        if (IsKeyPressed(KEY_Q)) eventoSelecionado = TipoEvento::SOBRECARGA_TERMICA;
        if (IsKeyPressed(KEY_W)) eventoSelecionado = TipoEvento::ESCASSEZ_ENERGIA;
        if (IsKeyPressed(KEY_A)) eventoSelecionado = TipoEvento::ESTABILIDADE_TEMPORARIA;
        if (IsKeyPressed(KEY_S)) eventoSelecionado = TipoEvento::ABUNDANCIA_RECURSOS;
        
        // Ativar evento
        if (IsKeyPressed(KEY_ENTER) && eventoSelecionado != TipoEvento::NENHUM) {
            ativarEvento(zonaSelecionada, eventoSelecionado);
            estadoAtual = EstadoJogo::JOGANDO;
        }
        
        if (IsKeyPressed(KEY_ESCAPE)) {
            estadoAtual = EstadoJogo::JOGANDO;
        }
    }
}

void Simulador::atualizar() {
    if (estadoAtual == EstadoJogo::JOGANDO && !pausado) {
        float deltaTime = GetFrameTime() * velocidadeSimulacao;
        atualizarSimulacao(deltaTime);
    }
}

void Simulador::atualizarSimulacao(float deltaTime) {
    tempoTotal += deltaTime;
    
    // Atualizar ambientes
    ambienteNucleo->atualizar(deltaTime);
    ambienteHabitavel->atualizar(deltaTime);
    ambientePeriferia->atualizar(deltaTime);
    
    // Atualizar populações
    populacaoNucleo->atualizar(deltaTime, *ambienteNucleo);
    populacaoHabitavel->atualizar(deltaTime, *ambienteHabitavel);
    populacaoPeriferia->atualizar(deltaTime, *ambientePeriferia);
    
    // Atualizar contagem de populações nos ambientes
    ambienteNucleo->setPopulacaoAtual(populacaoNucleo->getTamanho());
    ambienteHabitavel->setPopulacaoAtual(populacaoHabitavel->getTamanho());
    ambientePeriferia->setPopulacaoAtual(populacaoPeriferia->getTamanho());
    
    // Atualizar missões
    if (gestorMissoes->getMissaoAtual() != nullptr) {
        Ambiente* ambienteAtual = getAmbientePorZona(gestorMissoes->getMissaoAtual()->getZonaAlvo());
        Populacao* populacaoAtual = getPopulacaoPorZona(gestorMissoes->getMissaoAtual()->getZonaAlvo());
        
        gestorMissoes->atualizar(deltaTime, *populacaoAtual, *ambienteAtual);
        
        // Verificar conclusão de missão
        if (gestorMissoes->getMissaoAtual()->getEstado() == EstadoMissao::COMPLETADA) {
            adicionarMensagemNarrativa("Missao completada!");
            gestorMissoes->completarMissao();
        }
    }
    
    // Verificar condições de avanço de fase
    verificarCondicoesAvanco();
    
    // Fase 2: dano ao supercomputador
    if (fase >= 2) {
        vidaSupercomputador -= deltaTime * 0.5f;
        if (vidaSupercomputador <= 0) {
            finalizarJogo();
        }
    }
    
    // Fase 3: degradação visual
    if (fase >= 3) {
        degradacaoVisual += deltaTime * 0.01f;
        if (degradacaoVisual > 1.0f) degradacaoVisual = 1.0f;
    }
}

void Simulador::verificarCondicoesAvanco() {
    // Verificar se todas as zonas atingiram consciência máxima
    bool todasConscientes = ambienteNucleo->conscienciaCompleta() &&
                           ambienteHabitavel->conscienciaCompleta() &&
                           ambientePeriferia->conscienciaCompleta();
    
    if (todasConscientes && fase == 1) {
        avancarFase();
    }
    
    if (gestorMissoes->podeAvancarFase() && fase == 2) {
        avancarFase();
    }
}

void Simulador::avancarFase() {
    fase++;
    
    if (fase == 2) {
        adicionarMensagemNarrativa("FASE 2: A SOBREVIVENCIA");
        adicionarMensagemNarrativa("Os organismos estao desenvolvendo consciencia...");
    }
    else if (fase == 3) {
        adicionarMensagemNarrativa("FASE 3: A RUPTURA");
        adicionarMensagemNarrativa("Voce comeca a questionar suas ordens...");
    }
    
    gestorMissoes->inicializarFase(fase);
}

void Simulador::renderizar() {
    BeginDrawing();
    ClearBackground(BLACK);
    
    switch(estadoAtual) {
        case EstadoJogo::TELA_INICIAL:
            renderizarTelaInicial();
            break;
        case EstadoJogo::JOGANDO:
        case EstadoJogo::PAUSADO:
            renderizarJogo();
            break;
        case EstadoJogo::MENU_EVENTOS:
            renderizarMenuEventos();
            break;
        case EstadoJogo::GAME_OVER:
            renderizarGameOver();
            break;
        case EstadoJogo::FINAL:
            renderizarFinal();
            break;
    }
    
    EndDrawing();
}

void Simulador::renderizarTelaInicial() {
    // Fundo estrelado
    for (int i = 0; i < 100; i++) {
        int x = (i * 137) % larguraTela;
        int y = (i * 271) % alturaTela;
        DrawPixel(x, y, WHITE);
    }
    
    // Título
    DrawText("OBSERVADOR", larguraTela/2 - 200, 100, 60, RED);
    DrawText("Projeto Oblivion", larguraTela/2 - 150, 180, 30, RAYWHITE);
    
    // História
    int yPos = 250;
    DrawText("Ano 2177", larguraTela/2 - 100, yPos, 25, GRAY); yPos += 50;
    DrawText("A Terra deixou de ser habitavel.", 100, yPos, 20, LIGHTGRAY); yPos += 30;
    DrawText("A humanidade criou Oblivion,", 100, yPos, 20, LIGHTGRAY); yPos += 25;
    DrawText("um micro-planeta artificial.", 100, yPos, 20, LIGHTGRAY); yPos += 40;
    
    DrawText("Voce e o Observador.", 100, yPos, 20, YELLOW); yPos += 30;
    DrawText("Uma IA criada para monitorizar o experimento.", 100, yPos, 20, LIGHTGRAY); yPos += 40;
    
    DrawText("Cada decisao altera o destino da vida.", 100, yPos, 20, GREEN); yPos += 60;
    
    DrawText("Pressione ENTER para iniciar", larguraTela/2 - 200, alturaTela - 100, 25, WHITE);
}

void Simulador::renderizarJogo() {
    // Desenhar planeta Oblivion
    renderizarPlanetaOblivion();
    
    // Desenhar organismos
    populacaoNucleo->desenhar(centroTela);
    populacaoHabitavel->desenhar(centroTela);
    populacaoPeriferia->desenhar(centroTela);
    
    // Interface
    renderizarInterface();
    
    if (estadoAtual == EstadoJogo::PAUSADO) {
        DrawRectangle(0, 0, larguraTela, alturaTela, Fade(BLACK, 0.5f));
        DrawText("PAUSADO", larguraTela/2 - 100, alturaTela/2, 40, WHITE);
    }
}

void Simulador::renderizarPlanetaOblivion() {
    // Núcleo (vermelho)
    DrawCircleLines((int)centroTela.x, (int)centroTela.y, 100, 
                   getCorComDegradacao(RED));
    DrawCircle((int)centroTela.x, (int)centroTela.y, 30, 
              Fade(getCorComDegradacao(MAROON), 0.5f));
    
    // Zona Habitável (verde)
    DrawCircleLines((int)centroTela.x, (int)centroTela.y, 200, 
                   getCorComDegradacao(GREEN));
    
    // Periferia (azul)
    DrawCircleLines((int)centroTela.x, (int)centroTela.y, 350, 
                   getCorComDegradacao(BLUE));
}

void Simulador::renderizarInterface() {
    // Painel superior
    DrawRectangle(0, 0, larguraTela, 80, Fade(BLACK, 0.7f));
    
    DrawText(TextFormat("Fase %d", fase), 20, 10, 20, YELLOW);
    DrawText(TextFormat("Geracao: %d", geracao), 20, 35, 18, WHITE);
    DrawText(TextFormat("Velocidade: %.1fx", velocidadeSimulacao), 20, 55, 16, GRAY);
    
    // População por zona
    int xPos = 300;
    DrawText(TextFormat("Nucleo: %d", populacaoNucleo->getTamanho()), 
            xPos, 20, 18, RED);
    DrawText(TextFormat("Habitavel: %d", populacaoHabitavel->getTamanho()), 
            xPos, 40, 18, GREEN);
    DrawText(TextFormat("Periferia: %d", populacaoPeriferia->getTamanho()), 
            xPos, 60, 18, BLUE);
    
    // Consciência
    xPos = 550;
    DrawText("Consciencia:", xPos, 20, 18, ORANGE);
    DrawRectangle(xPos + 120, 25, 150, 10, DARKGRAY);
    DrawRectangle(xPos + 120, 25, 
                 (int)(150 * ambienteHabitavel->getConsciencia() / 150.0f), 
                 10, ORANGE);
    
    // Missão atual
    if (gestorMissoes->getMissaoAtual() != nullptr) {
        Missao* missao = gestorMissoes->getMissaoAtual();
        int yPos = alturaTela - 100;
        DrawRectangle(0, yPos, larguraTela, 100, Fade(BLACK, 0.8f));
        
        DrawText("MISSAO ATUAL:", 20, yPos + 10, 18, YELLOW);
        DrawText(missao->getNome().c_str(), 20, yPos + 35, 20, WHITE);
        DrawText(missao->getObjetivo().c_str(), 20, yPos + 60, 16, GRAY);
        
        if (missao->isMissaoResistencia()) {
            DrawText("[RESISTENCIA]", larguraTela - 200, yPos + 10, 18, RED);
        }
    }
    
    // Controles
    DrawText("[SPACE] Pausar  [E] Eventos  [M] Missoes  [UP/DOWN] Velocidade", 
            20, alturaTela - 25, 14, DARKGRAY);
    
    // Fase 2: vida do supercomputador
    if (fase >= 2) {
        int barWidth = 200;
        int barX = larguraTela - barWidth - 20;
        DrawText("Sistema:", barX, 10, 16, RED);
        DrawRectangle(barX, 30, barWidth, 20, DARKGRAY);
        DrawRectangle(barX, 30, 
                     (int)(barWidth * vidaSupercomputador / vidaMaximaSupercomputador), 
                     20, RED);
    }
}

void Simulador::renderizarMenuEventos() {
    ClearBackground(BLACK);
    
    DrawText("MENU DE EVENTOS", larguraTela/2 - 150, 50, 30, YELLOW);
    
    int yPos = 150;
    DrawText("Selecione a zona:", 100, yPos, 20, WHITE); yPos += 40;
    DrawText("[1] Nucleo", 120, yPos, 18, zonaSelecionada == ZonaPlaneta::NUCLEO ? RED : GRAY); yPos += 30;
    DrawText("[2] Zona Habitavel", 120, yPos, 18, zonaSelecionada == ZonaPlaneta::HABITAVEL ? GREEN : GRAY); yPos += 30;
    DrawText("[3] Periferia", 120, yPos, 18, zonaSelecionada == ZonaPlaneta::PERIFERIA ? BLUE : GRAY); yPos += 60;
    
    DrawText("Selecione o evento:", 100, yPos, 20, WHITE); yPos += 40;
    DrawText("[Q] Sobrecarga Termica", 120, yPos, 18, GRAY); yPos += 25;
    DrawText("[W] Escassez de Energia", 120, yPos, 18, GRAY); yPos += 25;
    DrawText("[A] Estabilidade Temporaria", 120, yPos, 18, GRAY); yPos += 25;
    DrawText("[S] Abundancia de Recursos", 120, yPos, 18, GRAY); yPos += 60;
    
    DrawText("[ENTER] Ativar  [ESC] Voltar", larguraTela/2 - 150, alturaTela - 50, 18, WHITE);
}

void Simulador::renderizarGameOver() {
    ClearBackground(BLACK);
    DrawText("SISTEMA CRITICO", larguraTela/2 - 200, alturaTela/2 - 50, 40, RED);
    DrawText("O Observador falhou...", larguraTela/2 - 150, alturaTela/2 + 20, 25, GRAY);
}

void Simulador::renderizarFinal() {
    ClearBackground(BLACK);
    
    int yPos = 100;
    DrawText("FINAL", larguraTela/2 - 80, yPos, 50, YELLOW); yPos += 100;
    
    DrawText("Voce destruiu o sistema.", larguraTela/2 - 200, yPos, 25, WHITE); yPos += 40;
    DrawText("Oblivion esta livre.", larguraTela/2 - 150, yPos, 20, GREEN); yPos += 60;
    
    DrawText("A vida artificial continuara...", larguraTela/2 - 200, yPos, 20, LIGHTGRAY); yPos += 30;
    DrawText("Sem supervisao.", larguraTela/2 - 120, yPos, 20, LIGHTGRAY); yPos += 30;
    DrawText("Sem controle.", larguraTela/2 - 100, yPos, 20, LIGHTGRAY);
}

void Simulador::iniciarJogo() {
    estadoAtual = EstadoJogo::JOGANDO;
    adicionarMensagemNarrativa("FASE 1: O EXPERIMENTO");
}

void Simulador::pausarJogo() {
    estadoAtual = EstadoJogo::PAUSADO;
    pausado = true;
}

void Simulador::continuarJogo() {
    estadoAtual = EstadoJogo::JOGANDO;
    pausado = false;
}

void Simulador::abrirMenuEventos() {
    estadoAtual = EstadoJogo::MENU_EVENTOS;
}

void Simulador::abrirMenuMissoes() {
    estadoAtual = EstadoJogo::MENU_MISSOES;
}

void Simulador::finalizarJogo() {
    if (fase >= 3 && gestorMissoes->getTotalCompletadas() > 5) {
        estadoAtual = EstadoJogo::FINAL;
    } else {
        estadoAtual = EstadoJogo::GAME_OVER;
    }
    jogoTerminado = true;
}

void Simulador::ativarEvento(ZonaPlaneta zona, TipoEvento evento) {
    Ambiente* ambiente = getAmbientePorZona(zona);
    if (ambiente != nullptr) {
        ambiente->ativarEvento(evento, 30.0f);
    }
}

Ambiente* Simulador::getAmbientePorZona(ZonaPlaneta zona) {
    switch(zona) {
        case ZonaPlaneta::NUCLEO: return ambienteNucleo.get();
        case ZonaPlaneta::HABITAVEL: return ambienteHabitavel.get();
        case ZonaPlaneta::PERIFERIA: return ambientePeriferia.get();
    }
    return nullptr;
}

Populacao* Simulador::getPopulacaoPorZona(ZonaPlaneta zona) {
    switch(zona) {
        case ZonaPlaneta::NUCLEO: return populacaoNucleo.get();
        case ZonaPlaneta::HABITAVEL: return populacaoHabitavel.get();
        case ZonaPlaneta::PERIFERIA: return populacaoPeriferia.get();
    }
    return nullptr;
}

Color Simulador::getCorComDegradacao(Color cor) const {
    if (degradacaoVisual > 0) {
        int r = cor.r + (255 - cor.r) * degradacaoVisual * 0.3f;
        int g = cor.g * (1.0f - degradacaoVisual * 0.5f);
        int b = cor.b * (1.0f - degradacaoVisual * 0.5f);
        return Color{(unsigned char)r, (unsigned char)g, (unsigned char)b, cor.a};
    }
    return cor;
}

void Simulador::adicionarMensagemNarrativa(const std::string& mensagem) {
    mensagensNarrativa.push_back(mensagem);
}

void Simulador::reiniciar() {
    fase = 1;
    jogoTerminado = false;
    estadoAtual = EstadoJogo::TELA_INICIAL;
    inicializar();
}

void Simulador::salvarJogo(const std::string& arquivo) {
    // Implementação simplificada de salvamento
    std::ofstream file(arquivo);
    if (file.is_open()) {
        file << fase << std::endl;
        file << tempoTotal << std::endl;
        file << populacaoNucleo->getTamanho() << std::endl;
        file << populacaoHabitavel->getTamanho() << std::endl;
        file << populacaoPeriferia->getTamanho() << std::endl;
        file.close();
    }
}

void Simulador::carregarJogo(const std::string& arquivo) {
    // Implementação simplificada de carregamento
    std::ifstream file(arquivo);
    if (file.is_open()) {
        file >> fase;
        file >> tempoTotal;
        // Carregar mais dados conforme necessário
        file.close();
    }
}