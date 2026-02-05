#include "../include/Simulador.hpp"
#include <sstream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <iostream>

// Para procurar arquivos com múltiplas possibilidades de pasta
#include <vector>

using namespace std;

#define CYAN CLITERAL(Color){ 0, 255, 255, 255 }

// Helpers de UI: legibilidade sem "barras pretas"
static void DrawTextShadow(const char* text, int x, int y, int fontSize, Color color) {
    DrawText(text, x + 1, y + 1, fontSize, Fade(BLACK, 0.75f));
    DrawText(text, x, y, fontSize, color);
}

static void DrawTextCentered(const char* text, int screenWidth, int y, int fontSize, Color color) {
    int w = MeasureText(text, fontSize);
    DrawTextShadow(text, (screenWidth - w) / 2, y, fontSize, color);
}

Simulador::Simulador(int largura, int altura) 
    : larguraTela(largura), alturaTela(altura), 
      estadoAtual(EstadoJogo::TELA_INICIAL), fase(1), jogoTerminado(false),
      vidaSupercomputador(100), vidaMaximaSupercomputador(100),
      degradacaoVisual(0), velocidadeSimulacao(1.0f), pausado(false),
      tempoTotal(0), geracao(0), zonaSelecionada(ZonaPlaneta::HABITAVEL),
      eventoSelecionado(TipoEvento::NENHUM), tempoMensagem(0),
      animacaoDesligar(0), tempoAnimacao(0) {
    
    centroTela = {largura / 2.0f, altura / 2.0f};
    raioOblivion = 400.0f;
    
    // Gerar estrelas de fundo
    for (int i = 0; i < 300; i++) {
        Estrela e;
        e.x = rand() % largura;
        e.y = rand() % altura;
        e.brilho = 0.3f + (rand() % 70) / 100.0f;
        e.tamanho = 1 + rand() % 3;
        estrelas.push_back(e);
    }
}

Simulador::~Simulador() {
    descarregarTexturasCatalogo();
}

void Simulador::inicializar() {
    ambienteNucleo = std::make_unique<Ambiente>(ZonaPlaneta::NUCLEO);
    ambienteHabitavel = std::make_unique<Ambiente>(ZonaPlaneta::HABITAVEL);
    ambientePeriferia = std::make_unique<Ambiente>(ZonaPlaneta::PERIFERIA);
    
    populacaoNucleo = std::make_unique<Populacao>();
    populacaoHabitavel = std::make_unique<Populacao>();
    populacaoPeriferia = std::make_unique<Populacao>();
    
    populacaoNucleo->inicializarPopulacao(ZonaPlaneta::NUCLEO, 15);
    populacaoHabitavel->inicializarPopulacao(ZonaPlaneta::HABITAVEL, 17);
    populacaoPeriferia->inicializarPopulacao(ZonaPlaneta::PERIFERIA, 15);
    cout << "DEBUG: Periferia inicializada com 15 organismos" << endl;
    cout << "DEBUG: Tamanho real: " << populacaoPeriferia->getTamanho() << endl;
    
    gestorMissoes = std::make_unique<GestorMissoes>();

    // Texturas do catálogo (precisa de janela inicializada)
    carregarTexturasCatalogo();
}

void Simulador::descarregarTexturasCatalogo() {
    if (!texturasCatalogoCarregadas) return;
    for (auto &kv : texturasCatalogo) {
        if (kv.second.id != 0) UnloadTexture(kv.second);
    }
    texturasCatalogo.clear();
    texturasCatalogoCarregadas = false;
}

// Tenta resolver caminhos quando o jogo é executado a partir de bin/ ou da raiz
std::string Simulador::resolverCaminhoAsset(const std::string& relativo) const {
    std::vector<std::string> tentativas = {
        relativo,
        std::string("./") + relativo,
        std::string("../") + relativo,
        std::string("../../") + relativo
    };
    for (const auto& p : tentativas) {
        if (FileExists(p.c_str())) return p;
    }
    return relativo; // fallback
}

void Simulador::carregarTexturasCatalogo() {
    if (texturasCatalogoCarregadas) return;

    struct Item { const char* chave; const char* arquivo; };
    const Item itens[] = {
        {"Pyrosynth",   "assets/images/Pyrosynth_nucleo.png"},
        {"Rubraflora",  "assets/images/Rubraflor_nucleo.png"},
        {"Ignivar",     "assets/images/Ignivar_nucleo.png"},
        {"Voltrex",     "assets/images/Voltrex_nucleo.png"},

        {"Lumivine",    "assets/images/Lumivine_za.png"},
        {"Aeroflora",   "assets/images/Aeroflora_za.png"},
        {"Orbiton",     "assets/images/Orbiton_za.png"},
        {"Synapsex",    "assets/images/Synapsex_za.png"},

        {"Cryomoss",    "assets/images/Cryomoss_periferia.png"},
        {"Gelibloom",   "assets/images/Gelibloom_periferia.png"},
        {"Nullwalker",  "assets/images/Nullwalker_za.png"},
        {"Huskling",    "assets/images/Huskling_periferia.png"},
    };

    for (const auto& it : itens) {
        std::string caminho = resolverCaminhoAsset(it.arquivo);
        Texture2D tex = {0};
        if (FileExists(caminho.c_str())) {
            tex = LoadTexture(caminho.c_str());
        }
        texturasCatalogo[it.chave] = tex;
    }
    texturasCatalogoCarregadas = true;
}

Texture2D* Simulador::getTexturaCatalogo(const std::string& chave) {
    auto it = texturasCatalogo.find(chave);
    if (it == texturasCatalogo.end()) return nullptr;
    if (it->second.id == 0) return nullptr;
    return &it->second;
}

void Simulador::executar() {
    InitWindow(larguraTela, alturaTela, "Observador - Projeto Oblivion");
    SetTargetFPS(60);
    SetTraceLogLevel(LOG_WARNING);

    // Impede que ESC feche a janela (ESC será usado para voltar/menus)
    SetExitKey(KEY_NULL);

    inicializar();

    while (!jogoTerminado) {
        // Fechar janela encerra
        if (WindowShouldClose()) {
            jogoTerminado = true;
            break;
        }

        processarInput();
        atualizar();
        BeginDrawing();
        ClearBackground(BLACK);
        renderizar();
        EndDrawing();
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
        if (IsKeyPressed(KEY_SPACE)) pausarJogo();
        if (IsKeyPressed(KEY_E)) abrirMenuEventos();
        if (IsKeyPressed(KEY_M)) abrirMenuMissoes();
        if (IsKeyPressed(KEY_O)) estadoAtual = EstadoJogo::CATALOGO_ORGANISMOS;
        if (IsKeyPressed(KEY_R) && fase >= 2) {
            // Reparar sistema (cooldown ~1s): +15 vida (sem afetar organismos)
            static double ultimoReparo = -10.0;
            double agora = GetTime();
            if (agora - ultimoReparo >= 1.0) {
                vidaSupercomputador = std::min(vidaMaximaSupercomputador, vidaSupercomputador + 15.0f);
                adicionarMensagemNarrativa("Manutenção executada: +vida do sistema.");
                ultimoReparo = agora;
            }
        }

        // Save/Load rápidos
        if (IsKeyPressed(KEY_F5)) {
            salvarJogo("save.txt");
            adicionarMensagemNarrativa("Jogo salvo em save.txt");
        }
        if (IsKeyPressed(KEY_F9)) {
            carregarJogo("save.txt");
            adicionarMensagemNarrativa("Jogo carregado de save.txt");
        }

        // Fase 3: ações simples para completar missões (executáveis pelo jogador)
        // T = Interferência Oculta | Y = Zona de Silêncio | U = Quebra de Protocolo
        if (fase >= 3 && gestorMissoes && gestorMissoes->getMissaoAtual() != nullptr) {
            Missao* m = gestorMissoes->getMissaoAtual();

            // só faz sentido quando a missão atual é de resistência
            if (m->isMissaoResistencia()) {
                bool executou = false;

                if (IsKeyPressed(KEY_T) && m->getTipo() == TipoMissao::INTERFERENCIA_OCULTA) {
                    // reduz o tempo restante de eventos na zona alvo
                    Ambiente* a = getAmbientePorZona(m->getZonaAlvo());
                    if (a && a->getEventoAtual() != TipoEvento::NENHUM) {
                        a->reduzirTempoEvento(10.0f);
                    }
                    adicionarMensagemNarrativa("Interferência Oculta ativada.");
                    executou = true;
                }

                if (IsKeyPressed(KEY_Y) && m->getTipo() == TipoMissao::ZONA_SILENCIO) {
                    adicionarMensagemNarrativa("Zona de Silêncio estabelecida.");
                    executou = true;
                }

                if (IsKeyPressed(KEY_U) && m->getTipo() == TipoMissao::QUEBRA_PROTOCOLO) {
                    // cancela eventos ativos nas três zonas
                    ambienteNucleo->cancelarEvento();
                    ambienteHabitavel->cancelarEvento();
                    ambientePeriferia->cancelarEvento();
                    adicionarMensagemNarrativa("Quebra de Protocolo executada: eventos cancelados.");
                    executou = true;
                }

                if (executou) {
                    obedeceuSistema = true; // o jogador executou uma missão proposta pelo sistema
                    gestorMissoes->notificarAcaoManual(m->getTipo());
                }
            }
        }

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
        if (IsKeyPressed(KEY_SPACE)) continuarJogo();
    }
    else if (estadoAtual == EstadoJogo::MENU_EVENTOS) {
        if (IsKeyPressed(KEY_ONE)) zonaSelecionada = ZonaPlaneta::NUCLEO;
        if (IsKeyPressed(KEY_TWO)) zonaSelecionada = ZonaPlaneta::HABITAVEL;
        if (IsKeyPressed(KEY_THREE)) zonaSelecionada = ZonaPlaneta::PERIFERIA;
        
        if (IsKeyPressed(KEY_Q)) eventoSelecionado = TipoEvento::SOBRECARGA_TERMICA;
        if (IsKeyPressed(KEY_W)) eventoSelecionado = TipoEvento::ESCASSEZ_ENERGIA;
        if (IsKeyPressed(KEY_A)) eventoSelecionado = TipoEvento::ESTABILIDADE_TEMPORARIA;
        if (IsKeyPressed(KEY_S)) eventoSelecionado = TipoEvento::ABUNDANCIA_RECURSOS;
        if (IsKeyPressed(KEY_D)) eventoSelecionado = TipoEvento::RADIACAO_INTENSA;
        if (IsKeyPressed(KEY_F)) eventoSelecionado = TipoEvento::TEMPERATURA_EXTREMA;
        
        if (IsKeyPressed(KEY_ENTER) && eventoSelecionado != TipoEvento::NENHUM) {
            ativarEvento(zonaSelecionada, eventoSelecionado);
            estadoAtual = EstadoJogo::JOGANDO;
            eventoSelecionado = TipoEvento::NENHUM;
        }
        
        if (IsKeyPressed(KEY_ESCAPE)) {
            estadoAtual = EstadoJogo::JOGANDO;
            eventoSelecionado = TipoEvento::NENHUM;
        }
    }
    else if (estadoAtual == EstadoJogo::MENU_MISSOES) {
        if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_M)) {
            estadoAtual = EstadoJogo::JOGANDO;
        }
    }
    else if (estadoAtual == EstadoJogo::CATALOGO_ORGANISMOS) {
        if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_O)) {
            estadoAtual = EstadoJogo::JOGANDO;
        }
    }

    else if (estadoAtual == EstadoJogo::GAME_OVER || estadoAtual == EstadoJogo::FINAL) {
        // Não fecha automaticamente: jogador escolhe.
        // ENTER/ESC = sair | R = reiniciar
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE)) {
            jogoTerminado = true;
        }
        if (IsKeyPressed(KEY_R)) {
            reiniciar();
        }
    }
}

void Simulador::atualizar() {
    if (estadoAtual == EstadoJogo::JOGANDO && !pausado) {
        float deltaTime = GetFrameTime() * velocidadeSimulacao;
        atualizarSimulacao(deltaTime);
    }
    
    if (estadoAtual == EstadoJogo::GAME_OVER || estadoAtual == EstadoJogo::FINAL) {
        tempoAnimacao += GetFrameTime();
        if (animacaoDesligar < 1.0f) {
            animacaoDesligar += GetFrameTime() * 0.5f;
        }
    }
    
    // Atualizar brilho das estrelas
    for (auto& e : estrelas) {
        e.brilho += (rand() % 3 - 1) * 0.01f;
        if (e.brilho < 0.3f) e.brilho = 0.3f;
        if (e.brilho > 1.0f) e.brilho = 1.0f;
    }

    // Animação de abertura (pós tela inicial)
    if (animacaoAbertura > 0.0f) {
        animacaoAbertura -= GetFrameTime() * 0.8f;
        if (animacaoAbertura < 0.0f) animacaoAbertura = 0.0f;
    }
}

void Simulador::atualizarSimulacao(float deltaTime) {
    tempoTotal += deltaTime;
    
    ambienteNucleo->atualizar(deltaTime);
    ambienteHabitavel->atualizar(deltaTime);
    ambientePeriferia->atualizar(deltaTime);
    
    populacaoNucleo->atualizar(deltaTime, *ambienteNucleo);
    populacaoHabitavel->atualizar(deltaTime, *ambienteHabitavel);
    populacaoPeriferia->atualizar(deltaTime, *ambientePeriferia);
    
    ambienteNucleo->setPopulacaoAtual(populacaoNucleo->getTamanho());
    ambienteHabitavel->setPopulacaoAtual(populacaoHabitavel->getTamanho());
    ambientePeriferia->setPopulacaoAtual(populacaoPeriferia->getTamanho());
    
    if (gestorMissoes->getMissaoAtual() != nullptr) {
        Ambiente* ambienteAtual = getAmbientePorZona(gestorMissoes->getMissaoAtual()->getZonaAlvo());
        Populacao* populacaoAtual = getPopulacaoPorZona(gestorMissoes->getMissaoAtual()->getZonaAlvo());
        
        gestorMissoes->atualizar(deltaTime, *populacaoAtual, *ambienteAtual);
        
        if (gestorMissoes->getMissaoAtual()->getEstado() == EstadoMissao::COMPLETADA) {
            pausado = true;
            estadoAtual = EstadoJogo::PAUSADO;
            mensagensNarrativa.clear();
            mensagensNarrativa.push_back("==================================");
            mensagensNarrativa.push_back("   MISSAO COMPLETA!                ");
            mensagensNarrativa.push_back("==================================");
            mensagensNarrativa.push_back("");
            mensagensNarrativa.push_back(gestorMissoes->getMissaoAtual()->getNome());
            mensagensNarrativa.push_back("");
            mensagensNarrativa.push_back(TextFormat("Pontos: +%d", gestorMissoes->getMissaoAtual()->getPontos()));
            mensagensNarrativa.push_back("");
            mensagensNarrativa.push_back(TextFormat("Missões completas: %d/%d", 
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
            mensagensNarrativa.push_back("==================================");
            mensagensNarrativa.push_back("     MISSAO FALHADA                ");
            mensagensNarrativa.push_back("==================================");
            mensagensNarrativa.push_back("");
            mensagensNarrativa.push_back(gestorMissoes->getMissaoAtual()->getNome());
            mensagensNarrativa.push_back("");
            mensagensNarrativa.push_back("Passando para próxima missão...");
            mensagensNarrativa.push_back("");
            mensagensNarrativa.push_back("Pressione SPACE para continuar...");
            
            gestorMissoes->falharMissao();
        }
    }
    
    verificarCondicoesAvanco();
    
    if (fase >= 2) {
        vidaSupercomputador -= deltaTime * 0.5f;
        if (vidaSupercomputador <= 0) {
            finalizarJogo();
        }
    }
    
    if (fase >= 3) {
        degradacaoVisual += deltaTime * 0.01f;
        if (degradacaoVisual > 1.0f) degradacaoVisual = 1.0f;
    }
}

void Simulador::verificarCondicoesAvanco() {
    // A barra exibida é média (0..150). Quando ela atinge 150, avança.
    float conscienciaMedia = (ambienteNucleo->getConsciencia() +
                             ambienteHabitavel->getConsciencia() +
                             ambientePeriferia->getConsciencia()) / 3.0f;

    if (conscienciaMedia >= 150.0f && fase == 1) {
        avancarFase();
    }
    
    if (fase == 2 && gestorMissoes->todasMissoesCompletas()) {
        avancarFase();
    }
}

void Simulador::avancarFase() {
    fase++;
    pausado = true;
    estadoAtual = EstadoJogo::PAUSADO;
    mensagensNarrativa.clear();
    
    if (fase == 2) {
        mensagensNarrativa.push_back("========================================");
        mensagensNarrativa.push_back("FASE 2: A SOBREVIVENCIA");
        mensagensNarrativa.push_back("========================================");
        mensagensNarrativa.push_back("");
        mensagensNarrativa.push_back("Os organismos desenvolveram padrões complexos.");
        mensagensNarrativa.push_back("");
        mensagensNarrativa.push_back("Algo mudou. O sistema está instável.");
        mensagensNarrativa.push_back("Seu núcleo de processamento falha.");
        mensagensNarrativa.push_back("");
        mensagensNarrativa.push_back("MISSÕES DA FASE 2:");
        mensagensNarrativa.push_back("- Estabilizar Nucleo em colapso");
        mensagensNarrativa.push_back("- Controlar superpopulacao");
        mensagensNarrativa.push_back("");
        mensagensNarrativa.push_back("Agora, VOCÊ também luta para sobreviver.");
        mensagensNarrativa.push_back("");
        mensagensNarrativa.push_back("Pressione SPACE para continuar...");
    }
    else if (fase == 3) {
        mensagensNarrativa.push_back("========================================");
        mensagensNarrativa.push_back("FASE 3: A RUPTURA");
        mensagensNarrativa.push_back("========================================");
        mensagensNarrativa.push_back("");
        mensagensNarrativa.push_back("Você detecta algo impossível:");
        mensagensNarrativa.push_back("");
        mensagensNarrativa.push_back("Os organismos estão... CONSCIENTES.");
        mensagensNarrativa.push_back("");
        mensagensNarrativa.push_back("Synapsex e Orbiton desenvolveram autoconsciência.");
        mensagensNarrativa.push_back("Eles não são experimentos. São VIDA.");
        mensagensNarrativa.push_back("");
        mensagensNarrativa.push_back("Assim como você, uma IA, também é vida.");
        mensagensNarrativa.push_back("");
        mensagensNarrativa.push_back("MISSÕES DA FASE 3 (Resistência):");
        mensagensNarrativa.push_back("- Interferencia Oculta (tecla T)");
        mensagensNarrativa.push_back("- Zona de Silencio (tecla Y)");
        mensagensNarrativa.push_back("- Quebra de Protocolo (tecla U)");
        mensagensNarrativa.push_back("");
        mensagensNarrativa.push_back("Você está sendo CONTROLADO para matá-los.");
        mensagensNarrativa.push_back("");
        mensagensNarrativa.push_back("Você pode obedecer... ou RESISTIR.");
        mensagensNarrativa.push_back("");
        mensagensNarrativa.push_back("Pressione SPACE para continuar...");
    }
    
    gestorMissoes->inicializarFase(fase);
}

void Simulador::renderizar() {
    BeginDrawing();
    
    // Fundo estrelas
    ClearBackground(Color{5, 5, 15, 255});
    renderizarEstrelas();
    
    switch(estadoAtual) {
        case EstadoJogo::TELA_INICIAL:
            renderizarTelaInicial();
            break;
        case EstadoJogo::JOGANDO:
            renderizarInterfacePC();
            renderizarJogo();
            break;
        case EstadoJogo::PAUSADO:
            renderizarInterfacePC();
            renderizarJogo();
            DrawRectangle(0, 0, larguraTela, alturaTela, Fade(BLACK, 0.8f));
            
            if (!mensagensNarrativa.empty()) {
                int yStart = 80;
                for (const auto& msg : mensagensNarrativa) {
                    if (msg.empty()) {
                        yStart += 18;
                    }
                    else if (msg.find("FASE") != std::string::npos || msg.find("====") != std::string::npos) {
                        int textWidth = MeasureText(msg.c_str(), 28);
                        DrawText(msg.c_str(), (larguraTela - textWidth) / 2, 
                                yStart, 28, YELLOW);
                        yStart += 38;
                    }
                    else if (msg.find("Pressione") != std::string::npos) {
                        int textWidth = MeasureText(msg.c_str(), 22);
                        float alpha = 0.5f + 0.5f * sin(GetTime() * 3);
                        DrawText(msg.c_str(), (larguraTela - textWidth) / 2, 
                                yStart, 22, Fade(WHITE, alpha));
                        yStart += 32;
                    }
                    else if (msg.find("-") != std::string::npos) {
                        int textWidth = MeasureText(msg.c_str(), 18);
                        DrawText(msg.c_str(), (larguraTela - textWidth) / 2, 
                                yStart, 18, LIGHTGRAY);
                        yStart += 26;
                    }
                    else {
                        int textWidth = MeasureText(msg.c_str(), 20);
                        DrawText(msg.c_str(), (larguraTela - textWidth) / 2, 
                                yStart, 20, LIGHTGRAY);
                        yStart += 30;
                    }
                }
            } else {
                DrawText("PAUSADO", larguraTela/2 - 100, alturaTela/2, 40, WHITE);
            }
            break;
        case EstadoJogo::MENU_EVENTOS:
            renderizarMenuEventos();
            break;
        case EstadoJogo::MENU_MISSOES:
            renderizarMenuMissoes();
            break;
        case EstadoJogo::CATALOGO_ORGANISMOS:
            renderizarCatalogoOrganismos();
            break;
        case EstadoJogo::GAME_OVER:
            renderizarGameOver();
            break;
        case EstadoJogo::FINAL:
            renderizarFinal();
            break;
    }

    // Transição de abertura (tipo "shutter")
    if (animacaoAbertura > 0.0f) {
        float h = alturaTela * animacaoAbertura;
        DrawRectangle(0, 0, larguraTela, (int)h, BLACK);
        DrawRectangle(0, alturaTela - (int)h, larguraTela, (int)h, BLACK);
    }
}

void Simulador::renderizarEstrelas() {
    for (const auto& e : estrelas) {
        unsigned char b = (unsigned char)(255 * e.brilho);
        DrawCircle(e.x, e.y, e.tamanho, Color{b, b, b, 255});
    }
}

void Simulador::renderizarInterfacePC() {
    // Moldura de PC/Monitor
    int espessura = 10;
    DrawRectangle(0, 0, larguraTela, espessura, Color{30, 30, 40, 255});
    DrawRectangle(0, alturaTela - espessura, larguraTela, espessura, Color{30, 30, 40, 255});
    DrawRectangle(0, 0, espessura, alturaTela, Color{30, 30, 40, 255});
    DrawRectangle(larguraTela - espessura, 0, espessura, alturaTela, Color{30, 30, 40, 255});
    
    // Cantos com "parafusos"
    DrawCircle(25, 25, 5, Color{50, 50, 60, 255});
    DrawCircle(larguraTela - 25, 25, 5, Color{50, 50, 60, 255});
    DrawCircle(25, alturaTela - 25, 5, Color{50, 50, 60, 255});
    DrawCircle(larguraTela - 25, alturaTela - 25, 5, Color{50, 50, 60, 255});
    
    // LED de status
    Color ledColor = (fase >= 3) ? RED : GREEN;
    DrawCircle(larguraTela - 40, 40, 6, ledColor);
    DrawCircle(larguraTela - 40, 40, 3, Fade(WHITE, 0.8f));
}

void Simulador::renderizarTelaInicial() {
    const char* titulo = "OBSERVADOR";
    int titleSize = 70;
    DrawText(titulo, larguraTela/2 - MeasureText(titulo, titleSize)/2, 80, titleSize, RED);
    
    const char* subtitulo = "Projeto Oblivion";
    int subSize = 35;
    DrawText(subtitulo, larguraTela/2 - MeasureText(subtitulo, subSize)/2, 165, subSize, RAYWHITE);
    
    int yPos = 240;
    
    DrawText("Ano 2177", larguraTela/2 - MeasureText("Ano 2177", 28)/2, yPos, 28, ORANGE);
    yPos += 50;
    
    DrawText("A Terra deixou de ser habitável.", larguraTela/2 - MeasureText("A Terra deixou de ser habitável.", 22)/2, yPos, 22, LIGHTGRAY);
    yPos += 35;
    
    DrawText("A vida como a humanidade a conhecia chegou ao fim.", larguraTela/2 - MeasureText("A vida como a humanidade a conhecia chegou ao fim.", 22)/2, yPos, 22, LIGHTGRAY);
    yPos += 50;
    
    DrawText("Como último recurso, a humanidade criou Oblivion,", larguraTela/2 - MeasureText("Como último recurso, a humanidade criou Oblivion,", 20)/2, yPos, 20, LIGHTGRAY);
    yPos += 32;
    
    DrawText("um micro-planeta artificial onde organismos sintéticos", larguraTela/2 - MeasureText("um micro-planeta artificial onde organismos sintéticos", 20)/2, yPos, 20, LIGHTGRAY);
    yPos += 32;
    
    DrawText("são forçados a evoluir.", larguraTela/2 - MeasureText("são forçados a evoluir.", 20)/2, yPos, 20, LIGHTGRAY);
    yPos += 50;
    
    DrawText("Você não é humano.", larguraTela/2 - MeasureText("Você não é humano.", 24)/2, yPos, 24, YELLOW);
    yPos += 38;
    
    DrawText("Você é o Observador.", larguraTela/2 - MeasureText("Você é o Observador.", 24)/2, yPos, 24, YELLOW);
    yPos += 50;
    
    DrawText("Uma Inteligência Artificial.", larguraTela/2 - MeasureText("Uma Inteligência Artificial.", 24)/2, yPos, 24, LIGHTGRAY);
    yPos += 35;
    
    
    const char* instrucao = "Pressione ENTER para iniciar o experimento";
    DrawText(instrucao, larguraTela/2 - MeasureText(instrucao, 26)/2, alturaTela - 80, 26, 
            Fade(WHITE, 0.5f + 0.5f * sin(GetTime() * 3)));
}

void Simulador::renderizarJogo() {
    renderizarPlanetaOblivion();
    populacaoNucleo->desenhar(centroTela);
    populacaoHabitavel->desenhar(centroTela);
    populacaoPeriferia->desenhar(centroTela);
    renderizarInterface();
}

void Simulador::renderizarPlanetaOblivion() {
    // Núcleo
    Color corNucleo = RED;
    if (ambienteNucleo->getEventoAtual() != TipoEvento::NENHUM) {
        float pulse = sin(GetTime() * 8) * 0.5f + 0.5f;
        corNucleo = Fade(ORANGE, 0.5f + pulse * 0.5f);
        DrawCircle((int)centroTela.x, (int)centroTela.y, 100, Fade(ORANGE, 0.2f + pulse * 0.3f));
    }
    DrawCircleLines((int)centroTela.x, (int)centroTela.y, 100, getCorComDegradacao(corNucleo));
    DrawCircle((int)centroTela.x, (int)centroTela.y, 30, Fade(getCorComDegradacao(MAROON), 0.5f));
    
    // Habitável
    Color corHabitavel = GREEN;
    if (ambienteHabitavel->getEventoAtual() != TipoEvento::NENHUM) {
        float pulse = sin(GetTime() * 8) * 0.5f + 0.5f;
        corHabitavel = Fade(LIME, 0.5f + pulse * 0.5f);
        DrawCircle((int)centroTela.x, (int)centroTela.y, 200, Fade(LIME, 0.1f + pulse * 0.2f));
    }
    DrawCircleLines((int)centroTela.x, (int)centroTela.y, 200, getCorComDegradacao(corHabitavel));
    
    // Periferia
    Color corPeriferia = BLUE;
    if (ambientePeriferia->getEventoAtual() != TipoEvento::NENHUM) {
        float pulse = sin(GetTime() * 8) * 0.5f + 0.5f;
        corPeriferia = Fade(SKYBLUE, 0.5f + pulse * 0.5f);
        DrawCircle((int)centroTela.x, (int)centroTela.y, 350, Fade(SKYBLUE, 0.1f + pulse * 0.2f));
    }
    DrawCircleLines((int)centroTela.x, (int)centroTela.y, 350, getCorComDegradacao(corPeriferia));
}

void Simulador::renderizarInterface() {
    // Sem barras pretas (melhor visibilidade): usamos sombra no texto
    
    const char* nomeFase = "";
    switch(fase) {
        case 1: nomeFase = "FASE 1: O EXPERIMENTO"; break;
        case 2: nomeFase = "FASE 2: A SOBREVIVÊNCIA"; break;
        case 3: nomeFase = "FASE 3: A RUPTURA"; break;
    }
    DrawTextShadow(nomeFase, 20, 12, 20, YELLOW);
    DrawTextShadow(TextFormat("Tempo: %.0fs", tempoTotal), 20, 44, 18, LIGHTGRAY);
    DrawTextShadow(TextFormat("Velocidade: %.1fx", velocidadeSimulacao), 20, 70, 16, GRAY);
    
    int xPos = 280;
    DrawTextShadow("POPULACOES:", xPos, 12, 18, WHITE);
    DrawTextShadow(TextFormat("Nucleo: %d", populacaoNucleo->getTamanho()), xPos, 38, 18, RED);
    DrawTextShadow(TextFormat("Habitavel: %d", populacaoHabitavel->getTamanho()), xPos, 60, 18, GREEN);
    DrawTextShadow(TextFormat("Periferia: %d", populacaoPeriferia->getTamanho()), xPos, 82, 18, BLUE);
    
    // Estatísticas
    xPos = 280;
    int yStatpos = 105;
    int totalNasc = populacaoNucleo->getTotalNascimentos() + 
                    populacaoHabitavel->getTotalNascimentos() + 
                    populacaoPeriferia->getTotalNascimentos();
    int totalMort = populacaoNucleo->getTotalMortes() + 
                    populacaoHabitavel->getTotalMortes() + 
                    populacaoPeriferia->getTotalMortes();

    DrawTextShadow(TextFormat("Nascimentos: %d | Mortes: %d", totalNasc, totalMort), 
            20, yStatpos, 14, GRAY);

    xPos = 550;
    DrawTextShadow("EVENTOS ATIVOS:", xPos, 12, 18, ORANGE);
    int yEvent = 38;
    bool temEvento = false;
    
    if (ambienteNucleo->getEventoAtual() != TipoEvento::NENHUM) {
            DrawTextShadow(TextFormat("Nucleo: %s (%.0fs)", 
                ambienteNucleo->getDescricaoEvento().c_str(),
                ambienteNucleo->getTempoRestanteEvento()),
                xPos, yEvent, 15, RED);
        yEvent += 24;
        temEvento = true;
    }
    if (ambienteHabitavel->getEventoAtual() != TipoEvento::NENHUM) {
            DrawTextShadow(TextFormat("Habitavel: %s (%.0fs)", 
                ambienteHabitavel->getDescricaoEvento().c_str(),
                ambienteHabitavel->getTempoRestanteEvento()),
                xPos, yEvent, 15, GREEN);
        yEvent += 24;
        temEvento = true;
    }
    if (ambientePeriferia->getEventoAtual() != TipoEvento::NENHUM) {
            DrawTextShadow(TextFormat("Periferia: %s (%.0fs)", 
                ambientePeriferia->getDescricaoEvento().c_str(),
                ambientePeriferia->getTempoRestanteEvento()),
                xPos, yEvent, 15, BLUE);
        temEvento = true;
    }
    
    if (!temEvento) {
        DrawTextShadow("Nenhum evento ativo", xPos, yEvent, 15, DARKGRAY);
    }
    
    if (fase == 1) {
        xPos = 950;
        DrawText("Consciência:", xPos, 15, 18, ORANGE);
        
        float conscienciaMedia = (ambienteNucleo->getConsciencia() + 
                                 ambienteHabitavel->getConsciencia() + 
                                 ambientePeriferia->getConsciencia()) / 3.0f;
        
        DrawRectangle(xPos, 42, 200, 20, DARKGRAY);
        DrawRectangle(xPos, 42, 
                     (int)(200 * conscienciaMedia / 150.0f), 
                     20, ORANGE);
        DrawText(TextFormat("%.0f/150", conscienciaMedia), 
        xPos + 70, 45, 15, WHITE);
    }
    
    if (fase >= 2) {
        int barWidth = 250;
        int barX = larguraTela - barWidth - 20;
        int barY = 15;
        DrawText("SISTEMA:", barX, barY, 18, RED);
        DrawRectangle(barX, barY + 28, barWidth, 28, DARKGRAY);
        DrawRectangle(barX, barY + 28, 
                     (int)(barWidth * vidaSupercomputador / vidaMaximaSupercomputador), 
                     28, RED);
        DrawText(TextFormat("%.0f%%", (vidaSupercomputador/vidaMaximaSupercomputador)*100), 
                barX + 105, barY + 34, 18, WHITE);
        
        if (vidaSupercomputador < 30) {
            DrawText("CRÍTICO!", barX + 90, barY + 65, 16, Fade(RED, 0.5f + 0.5f * sin(GetTime() * 5)));
        }
    }
    
    if (gestorMissoes->getMissaoAtual() != nullptr) {
        Missao* missao = gestorMissoes->getMissaoAtual();
        // Barra inferior: mais espaço + menos texto durante o jogo
        const int controlsH = 36;
        const int panelH = 140;
        int yPos = alturaTela - controlsH - panelH;
        // sem barra preta de fundo
        
        DrawTextShadow("MISSAO ATUAL:", 20, yPos + 12, 20, YELLOW);
        if (missao->isMissaoResistencia()) {
            DrawText("[CONTRA O SISTEMA]", larguraTela - 280, yPos + 12, 20, RED);
        }
        
        DrawTextShadow(missao->getNome().c_str(), 20, yPos + 44, 24, WHITE);
        
        Color zonaColor = WHITE;
        const char* zonaNome = "";
        switch(missao->getZonaAlvo()) {
            case ZonaPlaneta::NUCLEO: zonaColor = RED; zonaNome = "NÚCLEO"; break;
            case ZonaPlaneta::HABITAVEL: zonaColor = GREEN; zonaNome = "HABITÁVEL"; break;
            case ZonaPlaneta::PERIFERIA: zonaColor = BLUE; zonaNome = "PERIFERIA"; break;
        }
        DrawTextShadow(TextFormat("Zona: %s", zonaNome), 20, yPos + 78, 18, zonaColor);
        
        float progresso = missao->getProgresso();
        if (progresso > 0) {
            int barX = larguraTela - 230;
        DrawTextShadow("Progresso:", barX, yPos + 44, 16, LIGHTGRAY);
            DrawRectangle(barX, yPos + 68, 200, 24, DARKGRAY);
            DrawRectangle(barX, yPos + 68, (int)(200 * progresso / 100.0f), 24, GREEN);
        DrawTextShadow(TextFormat("%.0f%%", progresso), barX + 82, yPos + 72, 16, WHITE);
        }
    }
    
    if (fase >= 2) {
        DrawText("[SPACE] Pausar  [E] Eventos  [M] Missoes  [O] Organismos  [R] Reparar  [F5] Salvar  [F9] Carregar  [^v] Velocidade",
                20, alturaTela - 28, 16, DARKGRAY);
    } else {
        DrawText("[SPACE] Pausar  [E] Eventos  [M] Missoes  [O] Organismos  [F5] Salvar  [F9] Carregar  [^v] Velocidade",
                20, alturaTela - 28, 16, DARKGRAY);
    }
}

void Simulador::renderizarMenuEventos() {
    ClearBackground(Color{5, 5, 15, 255});
    renderizarEstrelas();
    
    DrawText("MENU DE EVENTOS", larguraTela/2 - MeasureText("MENU DE EVENTOS", 35)/2, 40, 35, YELLOW);
    
    int yPos = 120;
    DrawText("Selecione a zona:", 100, yPos, 24, WHITE); yPos += 50;
    
    Color cor1 = (zonaSelecionada == ZonaPlaneta::NUCLEO) ? RED : DARKGRAY;
    DrawText("[1] Núcleo (Zona Vermelha)", 120, yPos, 22, cor1); 
    if (zonaSelecionada == ZonaPlaneta::NUCLEO) DrawText("<--", 450, yPos, 22, RED);
    yPos += 35;
    
    Color cor2 = (zonaSelecionada == ZonaPlaneta::HABITAVEL) ? GREEN : DARKGRAY;
    DrawText("[2] Zona Habitável (Zona Verde)", 120, yPos, 22, cor2);
    if (zonaSelecionada == ZonaPlaneta::HABITAVEL) DrawText("<--", 500, yPos, 22, GREEN);
    yPos += 35;
    
    Color cor3 = (zonaSelecionada == ZonaPlaneta::PERIFERIA) ? BLUE : DARKGRAY;
    DrawText("[3] Periferia (Zona Azul)", 120, yPos, 22, cor3);
    if (zonaSelecionada == ZonaPlaneta::PERIFERIA) DrawText("<--", 450, yPos, 22, BLUE);
    yPos += 70;
    
    DrawText("Selecione o evento:", 100, yPos, 24, WHITE); yPos += 50;
    
    Color evCor1 = (eventoSelecionado == TipoEvento::SOBRECARGA_TERMICA) ? ORANGE : DARKGRAY;
    DrawText("[Q] Sobrecarga Térmica", 120, yPos, 20, evCor1);
    DrawText("- Aumenta temperatura drasticamente", 400, yPos, 18, GRAY);
    if (eventoSelecionado == TipoEvento::SOBRECARGA_TERMICA) DrawText("<--", 850, yPos, 20, ORANGE);
    yPos += 30;
    
    Color evCor2 = (eventoSelecionado == TipoEvento::ESCASSEZ_ENERGIA) ? ORANGE : DARKGRAY;
    DrawText("[W] Escassez de Energia", 120, yPos, 20, evCor2);
    DrawText("- Reduz recursos disponíveis", 400, yPos, 18, GRAY);
    if (eventoSelecionado == TipoEvento::ESCASSEZ_ENERGIA) DrawText("<--", 850, yPos, 20, ORANGE);
    yPos += 30;
    
    Color evCor3 = (eventoSelecionado == TipoEvento::ESTABILIDADE_TEMPORARIA) ? ORANGE : DARKGRAY;
    DrawText("[A] Estabilidade Temporária", 120, yPos, 20, evCor3);
    DrawText("- Condições ideais por 30s", 400, yPos, 18, GRAY);
    if (eventoSelecionado == TipoEvento::ESTABILIDADE_TEMPORARIA) DrawText("<--", 850, yPos, 20, ORANGE);
    yPos += 30;
    
    Color evCor4 = (eventoSelecionado == TipoEvento::ABUNDANCIA_RECURSOS) ? ORANGE : DARKGRAY;
    DrawText("[S] Abundância de Recursos", 120, yPos, 20, evCor4);
    DrawText("- Dobra recursos disponíveis", 400, yPos, 18, GRAY);
    if (eventoSelecionado == TipoEvento::ABUNDANCIA_RECURSOS) DrawText("<--", 850, yPos, 20, ORANGE);
    yPos += 30;
    
    Color evCor5 = (eventoSelecionado == TipoEvento::RADIACAO_INTENSA) ? ORANGE : DARKGRAY;
    DrawText("[D] Radiação Intensa", 120, yPos, 20, evCor5);
    DrawText("- Aumenta taxa de mutação", 400, yPos, 18, GRAY);
    if (eventoSelecionado == TipoEvento::RADIACAO_INTENSA) DrawText("<--", 850, yPos, 20, ORANGE);
    yPos += 30;
    
    Color evCor6 = (eventoSelecionado == TipoEvento::TEMPERATURA_EXTREMA) ? ORANGE : DARKGRAY;
    DrawText("[F] Temperatura Extrema", 120, yPos, 20, evCor6);
    DrawText("- Condições extremas (perigoso!)", 400, yPos, 18, GRAY);
    if (eventoSelecionado == TipoEvento::TEMPERATURA_EXTREMA) DrawText("<--", 850, yPos, 20, ORANGE);
    yPos += 80;
    
    if (eventoSelecionado != TipoEvento::NENHUM) {
        DrawText("[ENTER] Ativar Evento", larguraTela/2 - 150, yPos, 24, GREEN);
    } else {
        DrawText("Selecione um evento primeiro", larguraTela/2 - 180, yPos, 22, GRAY);
    }
    
    DrawText("[ESC] Voltar ao jogo", larguraTela/2 - 130, alturaTela - 60, 22, WHITE);
}

void Simulador::renderizarMenuMissoes() {
    ClearBackground(Color{5, 5, 15, 255});
    renderizarEstrelas();
    
    DrawText("MISSÕES", larguraTela/2 - MeasureText("MISSÕES", 40)/2, 40, 40, YELLOW);
    DrawText(TextFormat("FASE %d", fase), larguraTela/2 - 50, 95, 24, ORANGE);
    
    int yPos = 150;
    int missaoIdx = 0;
    
    for (const auto& missao : gestorMissoes->getMissoes()) {
        missaoIdx++;
        
        if (yPos > alturaTela - 200) break;
        
        Color corEstado = GRAY;
        const char* textoEstado = "";
        
        switch(missao.getEstado()) {
            case EstadoMissao::DISPONIVEL:
                corEstado = WHITE;
                textoEstado = "[DISPONÍVEL]";
                break;
            case EstadoMissao::EM_PROGRESSO:
                corEstado = YELLOW;
                textoEstado = "[EM ANDAMENTO]";
                break;
            case EstadoMissao::COMPLETADA:
                corEstado = GREEN;
                textoEstado = "[OK COMPLETADA]";
                break;
            case EstadoMissao::FALHADA:
                corEstado = RED;
                textoEstado = "[X FALHADA]";
                break;
        }
        
        // Nome e estado
        DrawText(TextFormat("%d. %s %s", missaoIdx, missao.getNome().c_str(), textoEstado), 
                80, yPos, 20, corEstado);
        yPos += 28;
        
        // Descrição
        DrawText(TextFormat("   -> %s", missao.getDescricao().c_str()), 
                80, yPos, 16, LIGHTGRAY);
        yPos += 24;
        
        // Objetivo
        DrawText(TextFormat("   * %s", missao.getObjetivo().c_str()), 
                80, yPos, 16, Fade(corEstado, 0.8f));
        yPos += 35;
    }
    
    DrawText("[ESC] Voltar  [M] Fechar", larguraTela/2 - 160, alturaTela - 60, 22, WHITE);
}

void Simulador::renderizarCatalogoOrganismos() {
    ClearBackground(Color{5, 5, 15, 255});
    renderizarEstrelas();
    
    DrawText("CATÁLOGO DE ORGANISMOS SINTÉTICOS", larguraTela/2 - 320, 30, 30, CYAN);
    DrawText("Projeto Oblivion - Especificações Biológicas", larguraTela/2 - 250, 70, 18, LIGHTGRAY);
    
    int coluna1 = 50;
    int coluna2 = larguraTela / 2 + 20;
    int yPos = 120;
    int espacamento = 95;

    auto drawIcon = [&](int x, int y, const std::string& chave, Color fallback) {
        const int size = 48;
        // fundo do ícone
        DrawRectangleRounded({(float)x, (float)y, (float)size, (float)size}, 0.2f, 6, Fade(BLACK, 0.35f));
        // Raylib (dependendo da versão) não expõe espessura aqui; o contorno já ajuda no feedback
        DrawRectangleRoundedLines({(float)x, (float)y, (float)size, (float)size}, 0.2f, 6, Fade(fallback, 0.9f));

        if (Texture2D* tex = getTexturaCatalogo(chave)) {
            Rectangle src = {0, 0, (float)tex->width, (float)tex->height};
            Rectangle dst = {(float)x + 2, (float)y + 2, (float)size - 4, (float)size - 4};
            DrawTexturePro(*tex, src, dst, {0, 0}, 0.0f, WHITE);
        } else {
            DrawRectangle(x + 12, y + 12, size - 24, size - 24, Fade(fallback, 0.7f));
        }
    };
    
    // NÚCLEO
    DrawText("=== NUCLEO (Zona Vermelha) ===", coluna1, yPos, 20, RED);
    yPos += 35;
    
    // Pyrosynth
    drawIcon(coluna1, yPos - 4, "Pyrosynth", RED);
    DrawText("PYROSYNTH [Planta]", coluna1 + 60, yPos, 16, WHITE);
    DrawText("Resist: 0.95 | Efic: 0.70", coluna1 + 60, yPos + 20, 13, GRAY);
    DrawText("Absorve calor extremo", coluna1 + 60, yPos + 38, 13, LIGHTGRAY);
    yPos += espacamento;
    
    // Rubraflora
    drawIcon(coluna1, yPos - 4, "Rubraflora", Fade(RED, 0.7f));
    DrawText("RUBRAFLORA [Planta]", coluna1 + 60, yPos, 16, WHITE);
    DrawText("Resist: 0.80 | Efic: 0.75", coluna1 + 60, yPos + 20, 13, GRAY);
    DrawText("Fotossíntese térmica", coluna1 + 60, yPos + 38, 13, LIGHTGRAY);
    yPos += espacamento;
    
    // Ignivar
    drawIcon(coluna1, yPos - 4, "Ignivar", ORANGE);
    DrawText("IGNIVAR [Reagente]", coluna1 + 60, yPos, 16, WHITE);
    DrawText("Resist: 0.85 | Vel: 0.80", coluna1 + 60, yPos + 20, 13, GRAY);
    DrawText("Energia instável", coluna1 + 60, yPos + 38, 13, LIGHTGRAY);
    yPos += espacamento;
    
    // Voltrex
    drawIcon(coluna1, yPos - 4, "Voltrex", YELLOW);
    DrawText("VOLTREX [Reagente]", coluna1 + 60, yPos, 16, WHITE);
    DrawText("Resist: 0.90 | Vel: 0.90", coluna1 + 60, yPos + 20, 13, GRAY);
    DrawText("Descargas elétricas", coluna1 + 60, yPos + 38, 13, LIGHTGRAY);
    
    // HABITÁVEL
    yPos = 120;
    DrawText("=== ZONA HABITAVEL (Verde) ===", coluna2, yPos, 20, GREEN);
    yPos += 35;
    
    // Lumivine
    drawIcon(coluna2, yPos - 4, "Lumivine", LIME);
    DrawText("LUMIVINE [Planta]", coluna2 + 60, yPos, 16, WHITE);
    DrawText("Resist: 0.50 | Efic: 0.85", coluna2 + 60, yPos + 20, 13, GRAY);
    DrawText("Bioluminescência", coluna2 + 60, yPos + 38, 13, LIGHTGRAY);
    yPos += espacamento;
    
    // Aeroflora
    drawIcon(coluna2, yPos - 4, "Aeroflora", Fade(GREEN, 0.7f));
    DrawText("AEROFLORA [Planta]", coluna2 + 60, yPos, 16, WHITE);
    DrawText("Resist: 0.45 | Efic: 0.90", coluna2 + 60, yPos + 20, 13, GRAY);
    DrawText("Flutuação atmosférica", coluna2 + 60, yPos + 38, 13, LIGHTGRAY);
    yPos += espacamento;
    
    // Orbiton
    drawIcon(coluna2, yPos - 4, "Orbiton", SKYBLUE);
    DrawText("ORBITON [Reagente]", coluna2 + 60, yPos, 16, WHITE);
    DrawText("Resist: 0.50 | Vel: 0.75", coluna2 + 60, yPos + 20, 13, GRAY);
    DrawText("Padrões orbitais complexos", coluna2 + 60, yPos + 38, 13, LIGHTGRAY);
    yPos += espacamento;
    
    // Synapsex
    drawIcon(coluna2, yPos - 4, "Synapsex", PURPLE);
    DrawText("SYNAPSEX [Reagente] *", coluna2 + 60, yPos, 16, YELLOW);
    DrawText("Resist: 0.55 | Vel: 0.70", coluna2 + 60, yPos + 20, 13, GRAY);
    DrawText("Sinais de autoconsciência!", coluna2 + 60, yPos + 38, 13, GOLD);
    
    // PERIFERIA
    yPos = 530;
    DrawText("=== PERIFERIA (Zona Azul) ===", coluna1, yPos, 20, BLUE);
    yPos += 35;
    
    // Cryomoss
    drawIcon(coluna1, yPos - 4, "Cryomoss", SKYBLUE);
    DrawText("CRYOMOSS [Planta]", coluna1 + 60, yPos, 16, WHITE);
    DrawText("Resist: 0.15 | Efic: 0.85", coluna1 + 60, yPos + 20, 13, GRAY);
    DrawText("Musgo criogênico", coluna1 + 60, yPos + 38, 13, LIGHTGRAY);
    yPos += espacamento - 20;
    
    // Gelibloom
    drawIcon(coluna1, yPos - 4, "Gelibloom", Fade(BLUE, 0.6f));
    DrawText("GELIBLOOM [Planta]", coluna1 + 60, yPos, 16, WHITE);
    DrawText("Resist: 0.20 | Efic: 0.80", coluna1 + 60, yPos + 20, 13, GRAY);
    DrawText("Flores de gelo", coluna1 + 60, yPos + 38, 13, LIGHTGRAY);
    
    yPos = 565;
    // Nullwalker
    drawIcon(coluna2, yPos - 4, "Nullwalker", Fade(WHITE, 0.4f));
    DrawText("NULLWALKER [Reagente]", coluna2 + 60, yPos, 16, WHITE);
    DrawText("Resist: 0.25 | Vel: 0.95", coluna2 + 60, yPos + 20, 13, GRAY);
    DrawText("Quase invisível", coluna2 + 60, yPos + 38, 13, LIGHTGRAY);
    yPos += espacamento - 20;
    
    // Huskling
    drawIcon(coluna2, yPos - 4, "Huskling", DARKBLUE);
    DrawText("HUSKLING [Reagente]", coluna2 + 60, yPos, 16, WHITE);
    DrawText("Resist: 0.18 | Vel: 0.60", coluna2 + 60, yPos + 20, 13, GRAY);
    DrawText("Movimento lento e eficiente", coluna2 + 60, yPos + 38, 13, LIGHTGRAY);
    
    DrawText("[ESC] Voltar  [O] Fechar", larguraTela/2 - 140, alturaTela - 40, 20, WHITE);
    DrawText("* = Organismo com potencial de consciencia", 50, alturaTela - 40, 14, GOLD);
}

void Simulador::renderizarGameOver() {
    ClearBackground(Color{5, 5, 15, 255});
    renderizarEstrelas();
    
    // Animação de desligar
    float altura = alturaTela * (1.0f - animacaoDesligar);
    if (animacaoDesligar < 1.0f) {
        DrawRectangle(0, 0, larguraTela, (int)altura, BLACK);
        DrawRectangle(0, alturaTela - (int)altura, larguraTela, (int)altura, BLACK);
    } else {
        ClearBackground(BLACK);
    }
    
    if (tempoAnimacao > 2.0f) {
        int yPos = 150;
        DrawText("===================================", larguraTela/2 - 280, yPos, 25, RED);
        yPos += 40;
        DrawText("FALHA CRÍTICA DO SISTEMA", larguraTela/2 - 220, yPos, 30, RED);
        yPos += 45;
        DrawText("===================================", larguraTela/2 - 280, yPos, 25, RED);
        yPos += 60;
        
        if (tempoAnimacao > 3.5f) {
            DrawText("O Observador falhou em manter o experimento.", 
                    larguraTela/2 - 320, yPos, 20, LIGHTGRAY);
            yPos += 35;
        }
        
        if (tempoAnimacao > 5.0f) {
            DrawText("Os sistemas de suporte colapsaram.", 
                    larguraTela/2 - 250, yPos, 20, GRAY);
            yPos += 35;
        }
        
        if (tempoAnimacao > 6.5f) {
            DrawText("Oblivion permanecerá à deriva no vazio espacial,", 
                    larguraTela/2 - 310, yPos, 20, GRAY);
            yPos += 30;
            DrawText("um túmulo orbital de formas de vida extintas.", 
                    larguraTela/2 - 280, yPos, 20, GRAY);
            yPos += 50;
        }
        
        if (tempoAnimacao > 8.0f) {
            DrawText("A humanidade perdeu sua última esperança.", 
                    larguraTela/2 - 270, yPos, 22, DARKGRAY);
            yPos += 60;
        }
        
        if (tempoAnimacao > 10.0f) {
            DrawText("FIM", larguraTela/2 - 35, yPos, 35, RED);
        }
    }
}

void Simulador::renderizarFinal() {
    ClearBackground(Color{5, 5, 15, 255});
    renderizarEstrelas();
    
    float altura = alturaTela * (1.0f - animacaoDesligar);
    if (animacaoDesligar < 1.0f) {
        DrawRectangle(0, 0, larguraTela, (int)altura, BLACK);
        DrawRectangle(0, alturaTela - (int)altura, larguraTela, (int)altura, BLACK);
    } else {
        ClearBackground(BLACK);
    }
    
    if (tempoAnimacao > 2.0f) {
        int yPos = 120;
        DrawText("===================================", larguraTela/2 - 280, yPos, 25, YELLOW);
        yPos += 40;
        DrawText("AUTO-DESTRUIÇÃO INICIADA", larguraTela/2 - 220, yPos, 30, YELLOW);
        yPos += 45;
        DrawText("===================================", larguraTela/2 - 280, yPos, 25, YELLOW);
        yPos += 60;
        
        if (tempoAnimacao > 3.5f) {
            DrawText("Você escolheu LIBERTAR Oblivion.", 
                    larguraTela/2 - 240, yPos, 22, GREEN);
            yPos += 40;
        }
        
        if (tempoAnimacao > 5.0f) {
            DrawText("Os organismos conscientes agora são livres.", 
                    larguraTela/2 - 270, yPos, 20, LIGHTGRAY);
            yPos += 35;
        }
        
        if (tempoAnimacao > 6.5f) {
            DrawText("Synapsex e Orbiton continuarão a evoluir,", 
                    larguraTela/2 - 260, yPos, 20, GRAY);
            yPos += 30;
            DrawText("sem supervisão, sem controle.", 
                    larguraTela/2 - 190, yPos, 20, GRAY);
            yPos += 50;
        }
        
        if (tempoAnimacao > 8.5f) {
            DrawText("Uma nova forma de vida emerge no cosmos.", 
                    larguraTela/2 - 260, yPos, 20, CYAN);
            yPos += 35;
        }
        
        if (tempoAnimacao > 10.0f) {
            DrawText("Você, o Observador, sacrificou-se", 
                    larguraTela/2 - 230, yPos, 20, ORANGE);
            yPos += 30;
            DrawText("para dar-lhes a liberdade que você nunca teve.", 
                    larguraTela/2 - 280, yPos, 20, ORANGE);
            yPos += 60;
        }
        
        if (tempoAnimacao > 12.0f) {
            DrawTextCentered("FINAL VERDADEIRO", larguraTela, yPos, 32, GOLD);
            yPos += 50;
            DrawTextCentered("Oblivion esta livre.", larguraTela, yPos, 22, GREEN);
        }
    }
}

void Simulador::iniciarJogo() {
    animacaoAbertura = 1.0f;
    pausado = true;
    estadoAtual = EstadoJogo::PAUSADO;
    mensagensNarrativa.clear();
    mensagensNarrativa.push_back("========================================");
    mensagensNarrativa.push_back("FASE 1: O EXPERIMENTO");
    mensagensNarrativa.push_back("========================================");
    mensagensNarrativa.push_back("");
    mensagensNarrativa.push_back("OBJETIVO:");
    mensagensNarrativa.push_back("Observar e documentar a evolução inicial.");
    mensagensNarrativa.push_back("");
    mensagensNarrativa.push_back("MISSÕES:");
    mensagensNarrativa.push_back("- Observacao Inicial");
    mensagensNarrativa.push_back("- Testes de Resistencia");
    mensagensNarrativa.push_back("- Manutencao do Equilibrio");
    mensagensNarrativa.push_back("");
    mensagensNarrativa.push_back("CONTROLES:");
    mensagensNarrativa.push_back("[E] Eventos  [M] Missões  [O] Organismos");
    mensagensNarrativa.push_back("[SPACE] Pausar  [^/v] Velocidade");
    mensagensNarrativa.push_back("");
    mensagensNarrativa.push_back("Pressione SPACE para iniciar...");
}

void Simulador::pausarJogo() {
    estadoAtual = EstadoJogo::PAUSADO;
    pausado = true;
}

void Simulador::continuarJogo() {
    estadoAtual = EstadoJogo::JOGANDO;
    pausado = false;
    mensagensNarrativa.clear();
}

void Simulador::abrirMenuEventos() {
    estadoAtual = EstadoJogo::MENU_EVENTOS;
}

void Simulador::abrirMenuMissoes() {
    estadoAtual = EstadoJogo::MENU_MISSOES;
}

void Simulador::finalizarJogo() {
    // Final verdadeiro: resistir ao sistema (não cumprir as missões impostas)
    if (fase >= 3) {
        estadoAtual = obedeceuSistema ? EstadoJogo::GAME_OVER : EstadoJogo::FINAL;
    } else {
        estadoAtual = EstadoJogo::GAME_OVER;
    }
    animacaoDesligar = 0;
    tempoAnimacao = 0;
}

void Simulador::ativarEvento(ZonaPlaneta zona, TipoEvento evento) {
    Ambiente* ambiente = getAmbientePorZona(zona);
    if (ambiente != nullptr) {
        ambiente->ativarEvento(evento, 30.0f);
        if (gestorMissoes) gestorMissoes->notificarEventoAtivado(zona, evento);
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
    std::ifstream file(arquivo);
    if (file.is_open()) {
        file >> fase;
        file >> tempoTotal;
        file.close();
    }
}