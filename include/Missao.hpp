#ifndef MISSAO_HPP
#define MISSAO_HPP

#include "Ambiente.hpp"
#include "Populacao.hpp"
#include <string>
#include <vector>

enum class TipoMissao {
    // Fase 1 - O Experimento
    TESTE_RESISTENCIA,
    EVOLUCAO_ACELERADA,
    EQUILIBRIO_PERFEITO,
    SIMBIOSE,
    ESTIMULO_MINIMO,
    SELECAO_NATURAL,
    
    // Fase 2 - A Sobrevivência
    INSTABILIDADE_CRITICA,
    CONTROLE_POPULACIONAL,
    ECOS_INVISIVEIS,
    FALHA_SUPORTE,
    MANUTENCAO_NUCLEO,
    ATAQUE_EXTERNO,
    
    // Fase 3 - A Ruptura
    INTENSIFICACAO_ENERGETICA,
    PURIFICACAO_TERMICA,
    REEQUILIBRIO_POPULACIONAL,
    ACELERACAO_EVOLUTIVA,
    CORRECAO_ESTRUTURAL,
    FILTRAGEM_FINAL,
    INTERFERENCIA_OCULTA,
    ZONA_SILENCIO,
    QUEBRA_PROTOCOLO
};

enum class EstadoMissao {
    DISPONIVEL,
    EM_PROGRESSO,
    COMPLETADA,
    FALHADA
};

class Missao {
private:
    TipoMissao tipo;
    ZonaPlaneta zonaAlvo;
    EstadoMissao estado;
    
    std::string nome;
    std::string descricao;
    std::string objetivo;
    
    // Condições de conclusão
    int objetivoNumerico;
    float tempoObjetivo;
    float tempoDecorrido;
    
    // Recompensas
    int pontos;
    
    // Contra o sistema?
    bool missaoResistencia;
    
public:
    Missao(TipoMissao t, ZonaPlaneta zona);
    ~Missao();
    
    // Getters
    TipoMissao getTipo() const { return tipo; }
    ZonaPlaneta getZonaAlvo() const { return zonaAlvo; }
    EstadoMissao getEstado() const { return estado; }
    std::string getNome() const { return nome; }
    std::string getDescricao() const { return descricao; }
    std::string getObjetivo() const { return objetivo; }
    int getPontos() const { return pontos; }
    bool isMissaoResistencia() const { return missaoResistencia; }
    float getProgresso() const;
    
    // Setters
    void setEstado(EstadoMissao e) { estado = e; }
    
    // Comportamentos
    void iniciar();
    void atualizar(float deltaTime, const Populacao& pop, const Ambiente& ambiente);
    bool verificarConclusao(const Populacao& pop, const Ambiente& ambiente);
    bool verificarFalha(const Populacao& pop, const Ambiente& ambiente);
    
private:
    void inicializarMissao();
};

class GestorMissoes {
private:
    std::vector<Missao> missoes;
    Missao* missaoAtual;
    int fase;
    
    int totalCompletadas;
    int pontuacaoTotal;
    
public:
    GestorMissoes();
    ~GestorMissoes();
    
    // Gestão de missões
    void inicializarFase(int numeroFase);
    void proximaMissao();
    void completarMissao();
    void falharMissao();
    
    // Getters
    Missao* getMissaoAtual() { return missaoAtual; }
    int getFase() const { return fase; }
    int getTotalCompletadas() const { return totalCompletadas; }
    int getPontuacaoTotal() const { return pontuacaoTotal; }
    const std::vector<Missao>& getMissoes() const { return missoes; }
    
    // Atualização
    void atualizar(float deltaTime, const Populacao& pop, const Ambiente& ambiente);
    
    // Verificação de progressão
    bool todasMissoesCompletas() const;
    bool podeAvancarFase() const;
    
private:
    void criarMissoesFase1();
    void criarMissoesFase2();
    void criarMissoesFase3();
};

#endif // MISSAO_HPP