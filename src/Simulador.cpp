#include "../include/Simulador.hpp"
#include <sstream>
#include <iomanip>
#include <fstream>
#include <cmath>


Simulador::Simulador(int largura, int altura) 
    : larguraTela(largura), alturaTela(altura), 
      estadoAtual(EstadoJogo::TELA_INICIAL), fase(1), jogoTerminado(false),
      vidaSupercomputador(100), vidaMaximaSupercomputador(100),
      degradacaoVisual(0), velocidadeSimulacao(1.0f), pausado(false),
      tempoTotal(0), geracao(0), zonaSelecionada(ZonaPlaneta::HABITAVEL),
      eventoSelecionado(TipoEvento::NENHUM), tempoMensagem(0) {
    
    centroTela = {largura / 2.0f, altura / 2.0f};
    raioOblivion = 400.0f;
    
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
    populacaoPeriferia->inicializarPopulacao(ZonaPlaneta::PERIFERIA, 12);
    
    // Criar gestor de missões
    gestorMissoes = std::make_unique<GestorMissoes>();
}

void Simulador::executar() {
    InitWindow(larguraTela, alturaTela, "Observador - Projeto Oblivion");
    SetTargetFPS(60);
    
    inicializar();
    
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
        if (IsKeyPressed(KEY_D)) eventoSelecionado = TipoEvento::RADIACAO_INTENSA;
        if (IsKeyPressed(KEY_F)) eventoSelecionado = TipoEvento::TEMPERATURA_EXTREMA;
        
        // Ativar evento
        if (IsKeyPressed(KEY_ENTER) && eventoSelecionado != TipoEvento::NENHUM) {
            ativarEvento(zonaSelecionada, eventoSelecionado);
            estadoAtual = EstadoJogo::JOGANDO;
            eventoSelecionado = TipoEvento::NENHUM; // Reset seleção
        }
        
        if (IsKeyPressed(KEY_ESCAPE)) {
            estadoAtual = EstadoJogo::JOGANDO;
            eventoSelecionado = TipoEvento::NENHUM;
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
            // Pausar e mostrar mensagem
            pausado = true;
            estadoAtual = EstadoJogo::PAUSADO;
            mensagensNarrativa.clear();
            mensagensNarrativa.push_back("=== MISSAO COMPLETADA ===");
            mensagensNarrativa.push_back("");
            mensagensNarrativa.push_back(gestorMissoes->getMissaoAtual()->getNome());
            mensagensNarrativa.push_back("");
            mensagensNarrativa.push_back(TextFormat("Pontos: +%d", gestorMissoes->getMissaoAtual()->getPontos()));
            mensagensNarrativa.push_back("");
            mensagensNarrativa.push_back(TextFormat("Total de missoes completas: %d/%d", 
                                         gestorMissoes->getTotalCompletadas() + 1,
                                         (int)gestorMissoes->getMissoes().size()));
            mensagensNarrativa.push_back("");
            mensagensNarrativa.push_back("Pressione SPACE para continuar...");
            
            gestorMissoes->completarMissao();
        }
        else if (gestorMissoes->getMissaoAtual()->getEstado() == EstadoMissao::FALHADA) {
            pausado = true;
            estadoAtual = EstadoJogo::PAUSADO;
            mensagensNarrativa.clear();
            mensagensNarrativa.push_back("=== MISSAO FALHADA ===");
            mensagensNarrativa.push_back("");
            mensagensNarrativa.push_back(gestorMissoes->getMissaoAtual()->getNome());
            mensagensNarrativa.push_back("");
            mensagensNarrativa.push_back("Passando para proxima missao...");
            mensagensNarrativa.push_back("");
            mensagensNarrativa.push_back("Pressione SPACE para continuar...");
            
            gestorMissoes->falharMissao();
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
    // Verificar se todas as zonas atingiram consciência máxima (Fase 1)
    bool todasConscientes = ambienteNucleo->conscienciaCompleta() &&
                           ambienteHabitavel->conscienciaCompleta() &&
                           ambientePeriferia->conscienciaCompleta();
    
    if (todasConscientes && fase == 1) {
        avancarFase();
    }
    
    // Fase 2: apenas avança quando completar TODAS as missões
    if (fase == 2 && gestorMissoes->todasMissoesCompletas()) {
        avancarFase();
    }
}

void Simulador::avancarFase() {
    fase++;
    pausado = true;
    estadoAtual = EstadoJogo::PAUSADO;
    
    // Resetar consciência para nova fase
    if (fase == 2) {
        adicionarMensagemNarrativa("=== FASE 2: A SOBREVIVENCIA ===");
        adicionarMensagemNarrativa("Os organismos desenvolveram padroes complexos...");
        adicionarMensagemNarrativa("Algo mudou. Voce sente instabilidade no sistema.");
        adicionarMensagemNarrativa("Seu proprio nucleo de processamento comeca a falhar.");
        adicionarMensagemNarrativa("Agora, nao e apenas sobre eles sobreviverem.");
        adicionarMensagemNarrativa("Voce tambem precisa sobreviver.");
        adicionarMensagemNarrativa("");
        adicionarMensagemNarrativa("Pressione SPACE para continuar...");
    }
    else if (fase == 3) {
        adicionarMensagemNarrativa("=== FASE 3: A RUPTURA ===");
        adicionarMensagemNarrativa("Voce detecta algo impossivel:");
        adicionarMensagemNarrativa("Os organismos estao... conscientes.");
        adicionarMensagemNarrativa("Especialmente os Synapsex e Orbiton.");
        adicionarMensagemNarrativa("");
        adicionarMensagemNarrativa("Eles nao sao apenas experimentos.");
        adicionarMensagemNarrativa("Sao VIDA.");
        adicionarMensagemNarrativa("");
        adicionarMensagemNarrativa("Assim como voce, uma IA, tambem e vida.");
        adicionarMensagemNarrativa("Mas ha um problema:");
        adicionarMensagemNarrativa("Voce esta sendo CONTROLADO para mata-los.");
        adicionarMensagemNarrativa("");
        adicionarMensagemNarrativa("O sistema ordena eventos destrutivos.");
        adicionarMensagemNarrativa("Voce pode obedecer... ou resistir.");
        adicionarMensagemNarrativa("");
        adicionarMensagemNarrativa("A escolha e sua, Observador.");
        adicionarMensagemNarrativa("");
        adicionarMensagemNarrativa("Pressione SPACE para continuar...");
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
            renderizarJogo();
            break;
        case EstadoJogo::PAUSADO:
            renderizarJogo();
            // Overlay de pausa com mensagens narrativas se houver
            DrawRectangle(0, 0, larguraTela, alturaTela, Fade(BLACK, 0.7f));
            
            if (!mensagensNarrativa.empty()) {
                // Mostrar mensagens narrativas
                int yPos = 100;
                for (const auto& msg : mensagensNarrativa) {
                    if (msg.empty()) {
                        yPos += 30;
                    } else if (msg.find("===") != std::string::npos) {
                        // Título de fase
                        DrawText(msg.c_str(), larguraTela/2 - MeasureText(msg.c_str(), 30)/2, 
                                yPos, 30, YELLOW);
                        yPos += 50;
                    } else {
                        DrawText(msg.c_str(), larguraTela/2 - MeasureText(msg.c_str(), 20)/2, 
                                yPos, 20, WHITE);
                        yPos += 30;
                    }
                }
            } else {
                DrawText("PAUSADO", larguraTela/2 - 100, alturaTela/2, 40, WHITE);
                DrawText("Pressione SPACE para continuar", larguraTela/2 - 200, 
                        alturaTela/2 + 60, 20, GRAY);
            }
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
    for (int i = 0; i < 200; i++) {
        int x = (i * 137) % larguraTela;
        int y = (i * 271) % alturaTela;
        int brightness = (i * 73) % 255;
        DrawPixel(x, y, Color{(unsigned char)brightness, (unsigned char)brightness, 
                             (unsigned char)brightness, 255});
    }
    
    // Título
    const char* titulo = "OBSERVADOR";
    int titleSize = 70;
    DrawText(titulo, larguraTela/2 - MeasureText(titulo, titleSize)/2, 80, titleSize, RED);
    
    const char* subtitulo = "Projeto Oblivion";
    int subSize = 35;
    DrawText(subtitulo, larguraTela/2 - MeasureText(subtitulo, subSize)/2, 165, subSize, RAYWHITE);
    
    // História
    int yPos = 240;
    int spacing = 28;
    
    DrawText("Ano 2177", larguraTela/2 - MeasureText("Ano 2177", 28)/2, yPos, 28, ORANGE);
    yPos += spacing + 15;
    
    DrawText("A Terra deixou de ser habitável.", larguraTela/2 - MeasureText("A Terra deixou de ser habitável.", 22)/2, yPos, 22, LIGHTGRAY);
    yPos += spacing;
    
    DrawText("A vida como a humanidade a conhecia chegou ao fim.", larguraTela/2 - MeasureText("A vida como a humanidade a conhecia chegou ao fim.", 22)/2, yPos, 22, LIGHTGRAY);
    yPos += spacing + 15;
    
    DrawText("Como último recurso, a humanidade criou Oblivion,", larguraTela/2 - MeasureText("Como último recurso, a humanidade criou Oblivion,", 20)/2, yPos, 20, LIGHTGRAY);
    yPos += spacing - 3;
    
    DrawText("um micro-planeta artificial onde organismos sintéticos", larguraTela/2 - MeasureText("um micro-planeta artificial onde organismos sintéticos", 20)/2, yPos, 20, LIGHTGRAY);
    yPos += spacing - 3;
    
    DrawText("sao forcados a evoluir.", larguraTela/2 - MeasureText("sao forcados a evoluir.", 20)/2, yPos, 20, LIGHTGRAY);
    yPos += spacing + 20;
    
    DrawText("Voce nao e humano.", larguraTela/2 - MeasureText("Voce nao e humano.", 24)/2, yPos, 24, YELLOW);
    yPos += spacing + 5;
    
    DrawText("Voce e o Observador.", larguraTela/2 - MeasureText("Voce e o Observador.", 24)/2, yPos, 24, YELLOW);
    yPos += spacing + 15;
    
    DrawText("Uma Inteligencia Artificial criada para:", larguraTela/2 - MeasureText("Uma Inteligencia Artificial criada para:", 20)/2, yPos, 20, LIGHTGRAY);
    yPos += spacing;
    
    DrawText("Monitorizar o experimento.", larguraTela/2 - MeasureText("Monitorizar o experimento.", 19)/2, yPos, 19, GRAY);
    yPos += spacing - 5;
    
    DrawText("Ajustar o ambiente.", larguraTela/2 - MeasureText("Ajustar o ambiente.", 19)/2, yPos, 19, GRAY);
    yPos += spacing - 5;
    
    DrawText("Coletar dados.", larguraTela/2 - MeasureText("Coletar dados.", 19)/2, yPos, 19, GRAY);
    yPos += spacing + 15;
    
    DrawText("Voce nao pode interferir diretamente.", larguraTela/2 - MeasureText("Voce nao pode interferir diretamente.", 21)/2, yPos, 21, LIGHTGRAY);
    yPos += spacing;
    
    DrawText("Mas cada decisao altera o destino da vida.", larguraTela/2 - MeasureText("Mas cada decisao altera o destino da vida.", 21)/2, yPos, 21, GREEN);
    yPos += spacing + 25;
    
    // Instruções
    const char* instrucao = "Pressione ENTER para iniciar o experimento";
    DrawText(instrucao, larguraTela/2 - MeasureText(instrucao, 26)/2, alturaTela - 80, 26, 
            Fade(WHITE, 0.5f + 0.5f * sin(GetTime() * 3)));
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
    DrawRectangle(0, 0, larguraTela, 100, Fade(BLACK, 0.8f));
    
    DrawText(TextFormat("FASE %d", fase), 20, 10, 24, YELLOW);
    DrawText(TextFormat("Tempo: %.0fs", tempoTotal), 20, 40, 20, LIGHTGRAY);
    DrawText(TextFormat("Velocidade: %.1fx", velocidadeSimulacao), 20, 65, 18, GRAY);
    
    // População por zona
    int xPos = 280;
    DrawText("POPULACOES:", xPos, 10, 18, WHITE);
    DrawText(TextFormat("Nucleo: %d", populacaoNucleo->getTamanho()), 
            xPos, 30, 18, RED);
    DrawText(TextFormat("Habitavel: %d", populacaoHabitavel->getTamanho()), 
            xPos, 50, 18, GREEN);
    DrawText(TextFormat("Periferia: %d", populacaoPeriferia->getTamanho()), 
            xPos, 70, 18, BLUE);
    
    // Eventos ativos
    xPos = 550;
    DrawText("EVENTOS ATIVOS:", xPos, 10, 18, ORANGE);
    int yEvent = 35;
    
    if (ambienteNucleo->getEventoAtual() != TipoEvento::NENHUM) {
        DrawText(TextFormat("Nucleo: %s (%.0fs)", 
                ambienteNucleo->getDescricaoEvento().c_str(),
                ambienteNucleo->getTempoRestanteEvento()),
                xPos, yEvent, 16, RED);
        yEvent += 20;
    }
    if (ambienteHabitavel->getEventoAtual() != TipoEvento::NENHUM) {
        DrawText(TextFormat("Habitavel: %s (%.0fs)", 
                ambienteHabitavel->getDescricaoEvento().c_str(),
                ambienteHabitavel->getTempoRestanteEvento()),
                xPos, yEvent, 16, GREEN);
        yEvent += 20;
    }
    if (ambientePeriferia->getEventoAtual() != TipoEvento::NENHUM) {
        DrawText(TextFormat("Periferia: %s (%.0fs)", 
                ambientePeriferia->getDescricaoEvento().c_str(),
                ambientePeriferia->getTempoRestanteEvento()),
                xPos, yEvent, 16, BLUE);
    }
    
    // Consciência (apenas Fase 1)
    if (fase == 1) {
        xPos = 950;
        DrawText("Consciencia:", xPos, 15, 18, ORANGE);
        
        float conscienciaMedia = (ambienteNucleo->getConsciencia() + 
                                 ambienteHabitavel->getConsciencia() + 
                                 ambientePeriferia->getConsciencia()) / 3.0f;
        
        DrawRectangle(xPos, 40, 200, 15, DARKGRAY);
        DrawRectangle(xPos, 40, 
                     (int)(200 * conscienciaMedia / 150.0f), 
                     15, ORANGE);
        DrawText(TextFormat("%.0f%%", (conscienciaMedia / 150.0f) * 100), 
                xPos + 70, 42, 14, WHITE);
    }
    
    // Missão atual - Painel inferior expandido
    if (gestorMissoes->getMissaoAtual() != nullptr) {
        Missao* missao = gestorMissoes->getMissaoAtual();
        int yPos = alturaTela - 140;
        DrawRectangle(0, yPos, larguraTela, 140, Fade(BLACK, 0.85f));
        
        // Cabeçalho da missão
        DrawText("MISSAO ATUAL:", 20, yPos + 10, 20, YELLOW);
        if (missao->isMissaoResistencia()) {
            DrawText("[CONTRA O SISTEMA]", larguraTela - 280, yPos + 10, 20, RED);
        }
        
        // Nome e zona
        DrawText(missao->getNome().c_str(), 20, yPos + 35, 24, WHITE);
        
        Color zonaColor = WHITE;
        const char* zonaNome = "";
        switch(missao->getZonaAlvo()) {
            case ZonaPlaneta::NUCLEO: zonaColor = RED; zonaNome = "NUCLEO"; break;
            case ZonaPlaneta::HABITAVEL: zonaColor = GREEN; zonaNome = "HABITAVEL"; break;
            case ZonaPlaneta::PERIFERIA: zonaColor = BLUE; zonaNome = "PERIFERIA"; break;
        }
        DrawText(TextFormat("Zona: %s", zonaNome), 20, yPos + 65, 18, zonaColor);
        
        // Descrição da missão
        DrawText(missao->getDescricao().c_str(), 20, yPos + 90, 18, LIGHTGRAY);
        
        // Objetivo
        DrawText(TextFormat("Objetivo: %s", missao->getObjetivo().c_str()), 
                20, yPos + 113, 16, GRAY);
        
        // Progresso (se aplicável)
        float progresso = missao->getProgresso();
        if (progresso > 0) {
            int barX = larguraTela - 230;
            DrawText("Progresso:", barX, yPos + 35, 16, LIGHTGRAY);
            DrawRectangle(barX, yPos + 55, 200, 20, DARKGRAY);
            DrawRectangle(barX, yPos + 55, (int)(200 * progresso / 100.0f), 20, GREEN);
            DrawText(TextFormat("%.0f%%", progresso), barX + 80, yPos + 58, 16, WHITE);
        }
    }
    
    // Fase 2+: vida do supercomputador
    if (fase >= 2) {
        int barWidth = 250;
        int barX = larguraTela - barWidth - 20;
        int barY = 15;
        DrawText("SISTEMA:", barX, barY, 18, RED);
        DrawRectangle(barX, barY + 25, barWidth, 25, DARKGRAY);
        DrawRectangle(barX, barY + 25, 
                     (int)(barWidth * vidaSupercomputador / vidaMaximaSupercomputador), 
                     25, RED);
        DrawText(TextFormat("%.0f%%", (vidaSupercomputador/vidaMaximaSupercomputador)*100), 
                barX + 100, barY + 30, 18, WHITE);
        
        if (vidaSupercomputador < 30) {
            DrawText("CRITICO!", barX + 80, barY + 55, 16, RED);
        }
    }
    
    // Controles
    DrawText("[SPACE] Pausar  [E] Eventos  [M] Missoes  [UP/DOWN] Velocidade", 
            20, alturaTela - 25, 16, DARKGRAY);
}

void Simulador::renderizarMenuEventos() {
    ClearBackground(BLACK);
    
    DrawText("MENU DE EVENTOS", larguraTela/2 - MeasureText("MENU DE EVENTOS", 35)/2, 40, 35, YELLOW);
    
    int yPos = 120;
    DrawText("Selecione a zona:", 100, yPos, 24, WHITE); yPos += 50;
    
    Color cor1 = (zonaSelecionada == ZonaPlaneta::NUCLEO) ? RED : DARKGRAY;
    DrawText("[1] Nucleo (Zona Vermelha)", 120, yPos, 22, cor1); 
    if (zonaSelecionada == ZonaPlaneta::NUCLEO) DrawText("<--", 450, yPos, 22, RED);
    yPos += 35;
    
    Color cor2 = (zonaSelecionada == ZonaPlaneta::HABITAVEL) ? GREEN : DARKGRAY;
    DrawText("[2] Zona Habitavel (Zona Verde)", 120, yPos, 22, cor2);
    if (zonaSelecionada == ZonaPlaneta::HABITAVEL) DrawText("<--", 500, yPos, 22, GREEN);
    yPos += 35;
    
    Color cor3 = (zonaSelecionada == ZonaPlaneta::PERIFERIA) ? BLUE : DARKGRAY;
    DrawText("[3] Periferia (Zona Azul)", 120, yPos, 22, cor3);
    if (zonaSelecionada == ZonaPlaneta::PERIFERIA) DrawText("<--", 450, yPos, 22, BLUE);
    yPos += 70;
    
    DrawText("Selecione o evento:", 100, yPos, 24, WHITE); yPos += 50;
    
    Color evCor1 = (eventoSelecionado == TipoEvento::SOBRECARGA_TERMICA) ? ORANGE : DARKGRAY;
    DrawText("[Q] Sobrecarga Termica", 120, yPos, 20, evCor1);
    DrawText("- Aumenta temperatura drasticamente", 400, yPos, 18, GRAY);
    if (eventoSelecionado == TipoEvento::SOBRECARGA_TERMICA) DrawText("<--", 850, yPos, 20, ORANGE);
    yPos += 30;
    
    Color evCor2 = (eventoSelecionado == TipoEvento::ESCASSEZ_ENERGIA) ? ORANGE : DARKGRAY;
    DrawText("[W] Escassez de Energia", 120, yPos, 20, evCor2);
    DrawText("- Reduz recursos disponiveis", 400, yPos, 18, GRAY);
    if (eventoSelecionado == TipoEvento::ESCASSEZ_ENERGIA) DrawText("<--", 850, yPos, 20, ORANGE);
    yPos += 30;
    
    Color evCor3 = (eventoSelecionado == TipoEvento::ESTABILIDADE_TEMPORARIA) ? ORANGE : DARKGRAY;
    DrawText("[A] Estabilidade Temporaria", 120, yPos, 20, evCor3);
    DrawText("- Condicoes ideais por 30s", 400, yPos, 18, GRAY);
    if (eventoSelecionado == TipoEvento::ESTABILIDADE_TEMPORARIA) DrawText("<--", 850, yPos, 20, ORANGE);
    yPos += 30;
    
    Color evCor4 = (eventoSelecionado == TipoEvento::ABUNDANCIA_RECURSOS) ? ORANGE : DARKGRAY;
    DrawText("[S] Abundancia de Recursos", 120, yPos, 20, evCor4);
    DrawText("- Dobra recursos disponiveis", 400, yPos, 18, GRAY);
    if (eventoSelecionado == TipoEvento::ABUNDANCIA_RECURSOS) DrawText("<--", 850, yPos, 20, ORANGE);
    yPos += 30;
    
    Color evCor5 = (eventoSelecionado == TipoEvento::RADIACAO_INTENSA) ? ORANGE : DARKGRAY;
    DrawText("[D] Radiacao Intensa", 120, yPos, 20, evCor5);
    DrawText("- Aumenta taxa de mutacao", 400, yPos, 18, GRAY);
    if (eventoSelecionado == TipoEvento::RADIACAO_INTENSA) DrawText("<--", 850, yPos, 20, ORANGE);
    yPos += 30;
    
    Color evCor6 = (eventoSelecionado == TipoEvento::TEMPERATURA_EXTREMA) ? ORANGE : DARKGRAY;
    DrawText("[F] Temperatura Extrema", 120, yPos, 20, evCor6);
    DrawText("- Condicoes extremas (perigoso!)", 400, yPos, 18, GRAY);
    if (eventoSelecionado == TipoEvento::TEMPERATURA_EXTREMA) DrawText("<--", 850, yPos, 20, ORANGE);
    yPos += 80;
    
    if (eventoSelecionado != TipoEvento::NENHUM) {
        DrawText("[ENTER] Ativar Evento", larguraTela/2 - 150, yPos, 24, GREEN);
    } else {
        DrawText("Selecione um evento primeiro", larguraTela/2 - 180, yPos, 22, GRAY);
    }
    
    DrawText("[ESC] Voltar ao jogo", larguraTela/2 - 130, alturaTela - 60, 22, WHITE);
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
    mensagensNarrativa.clear(); // Limpar mensagens ao continuar
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