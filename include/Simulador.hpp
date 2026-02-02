#ifndef SIMULADOR_HPP
#define SIMULADOR_HPP

#include "Organismo.hpp"
#include "Ambiente.hpp"
#include "Populacao.hpp"
#include "Missao.hpp"
#include <raylib.h>
#include <memory>
#include <string>

enum class EstadoJogo {
    TELA_INICIAL,
    JOGANDO,
    PAUSADO,
    MENU_EVENTOS,
    MENU_MISSOES,
    GAME_OVER,
    FINAL
};

class Simulador {
private:
    // Estados do jogo
    EstadoJogo estadoAtual;
    int fase;
    bool jogoTerminado;
    
    // Componentes principais
    std::unique_ptr<Ambiente> ambienteNucleo;
    std::unique_ptr<Ambiente> ambienteHabitavel;
    std::unique_ptr<Ambiente> ambientePeriferia;
    
    std::unique_ptr<Populacao> populacaoNucleo;
    std::unique_ptr<Populacao> populacaoHabitavel;
    std::unique_ptr<Populacao> populacaoPeriferia;
    
    std::unique_ptr<GestorMissoes> gestorMissoes;
    
    // Mecânicas do jogo
    float vidaSupercomputador;
    float vidaMaximaSupercomputador;
    float degradacaoVisual;
    
    // Controle de tempo
    float velocidadeSimulacao;
    bool pausado;
    float tempoTotal;
    int geracao;
    
    // Interface
    Vector2 centroTela;
    float raioOblivion;
    int larguraTela;
    int alturaTela;
    
    // Seleção de zona/evento
    ZonaPlaneta zonaSelecionada;
    TipoEvento eventoSelecionado;
    
    // Mensagens narrativas
    std::vector<std::string> mensagensNarrativa;
    float tempoMensagem;
    
public:
    Simulador(int largura, int altura);
    ~Simulador();
    
    // Inicialização
    void inicializar();
    void reiniciar();
    
    // Loop principal
    void executar();
    void atualizar();
    void renderizar();
    void processarInput();
    
    // Transições de estado
    void iniciarJogo();
    void pausarJogo();
    void continuarJogo();
    void abrirMenuEventos();
    void abrirMenuMissoes();
    void finalizarJogo();
    
    // Gestão de fases
    void avancarFase();
    void verificarCondicoesAvanco();
    
    // Eventos do jogador
    void ativarEvento(ZonaPlaneta zona, TipoEvento evento);
    void completarMissao();
    
    // Renderização de componentes
    void renderizarTelaInicial();
    void renderizarJogo();
    void renderizarPlanetaOblivion();
    void renderizarInterface();
    void renderizarMenuEventos();
    void renderizarMenuMissoes();
    void renderizarGameOver();
    void renderizarFinal();
    
    // Utilidades
    void adicionarMensagemNarrativa(const std::string& mensagem);
    void salvarJogo(const std::string& arquivo);
    void carregarJogo(const std::string& arquivo);
    
private:
    void atualizarSimulacao(float deltaTime);
    void verificarConscienciaTotal();
    void aplicarDegradacaoVisual();
    Ambiente* getAmbientePorZona(ZonaPlaneta zona);
    Populacao* getPopulacaoPorZona(ZonaPlaneta zona);
    Color getCorComDegradacao(Color cor) const;
};

#endif // SIMULADOR_HPP