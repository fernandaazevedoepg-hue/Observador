#ifndef SIMULADOR_HPP
#define SIMULADOR_HPP

#include "Organismo.hpp"
#include "Ambiente.hpp"
#include "Populacao.hpp"
#include "Missao.hpp"
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <raylib.h>

enum class EstadoJogo {
    TELA_INICIAL,
    JOGANDO,
    PAUSADO,
    MENU_EVENTOS,
    MENU_MISSOES,
    CATALOGO_ORGANISMOS,  // NOVO!
    GAME_OVER,
    FINAL
};

struct Estrela {
    int x, y;
    float brilho;
    int tamanho;
};

class Simulador {
private:
    // Dimensões da tela
    int larguraTela, alturaTela;
    Vector2 centroTela;
    float raioOblivion;
    
    // Estado do jogo
    EstadoJogo estadoAtual;
    int fase;
    bool jogoTerminado;
    bool pausado;
    
    // Componentes principais
    std::unique_ptr<Ambiente> ambienteNucleo;
    std::unique_ptr<Ambiente> ambienteHabitavel;
    std::unique_ptr<Ambiente> ambientePeriferia;
    
    std::unique_ptr<Populacao> populacaoNucleo;
    std::unique_ptr<Populacao> populacaoHabitavel;
    std::unique_ptr<Populacao> populacaoPeriferia;
    
    std::unique_ptr<GestorMissoes> gestorMissoes;
    
    // Variáveis de jogo
    float tempoTotal;
    int geracao;
    float velocidadeSimulacao;
    
    // Fase 2
    float vidaSupercomputador;
    float vidaMaximaSupercomputador;
    
    // Fase 3
    float degradacaoVisual;

    // Decisão narrativa (obedecer ou resistir ao sistema)
    bool obedeceuSistema = false;
    
    // Menus
    ZonaPlaneta zonaSelecionada;
    TipoEvento eventoSelecionado;
    
    // Narrativa
    std::vector<std::string> mensagensNarrativa;
    float tempoMensagem;
    
    // Visual NOVO!
    std::vector<Estrela> estrelas;
    float animacaoDesligar;
    float tempoAnimacao;

    // Transição de abertura (ao sair da tela inicial)
    float animacaoAbertura = 0.0f; // 1.0 = fechado, 0.0 = aberto

    // Catálogo: texturas dos organismos (carregadas uma vez)
    std::unordered_map<std::string, Texture2D> texturasCatalogo;
    bool texturasCatalogoCarregadas = false;
    
    // Métodos privados
    void atualizarSimulacao(float deltaTime);
    void verificarCondicoesAvanco();
    void avancarFase();
    
    // Renderização
    void renderizarTelaInicial();
    void renderizarJogo();
    void renderizarInterface();
    void renderizarPlanetaOblivion();
    void renderizarMenuEventos();
    void renderizarMenuMissoes();
    void renderizarCatalogoOrganismos();  // NOVO!
    void renderizarGameOver();
    void renderizarFinal();
    void renderizarEstrelas();  // NOVO!
    void renderizarInterfacePC();  // NOVO!

    // Helpers
    Ambiente* getAmbientePorZona(ZonaPlaneta zona);
    Populacao* getPopulacaoPorZona(ZonaPlaneta zona);
    Color getCorComDegradacao(Color cor) const;
    void adicionarMensagemNarrativa(const std::string& mensagem);

    // Catálogo
    void carregarTexturasCatalogo();
    void descarregarTexturasCatalogo();
    Texture2D* getTexturaCatalogo(const std::string& chave);
    std::string resolverCaminhoAsset(const std::string& relativo) const;
    
public:
    Simulador(int largura, int altura);
    ~Simulador();
    
    // Loop principal
    void executar();
    void inicializar();
    void processarInput();
    void atualizar();
    void renderizar();
    
    // Controles
    void iniciarJogo();
    void pausarJogo();
    void continuarJogo();
    void abrirMenuEventos();
    void abrirMenuMissoes();
    void finalizarJogo();
    void reiniciar();
    
    // Eventos
    void ativarEvento(ZonaPlaneta zona, TipoEvento evento);
    
    // Save/Load
    void salvarJogo(const std::string& arquivo);
    void carregarJogo(const std::string& arquivo);
};

#endif // SIMULADOR_HPP