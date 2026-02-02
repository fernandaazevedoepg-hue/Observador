#ifndef AMBIENTE_HPP
#define AMBIENTE_HPP

#include "Organismo.hpp"
#include <string>

enum class TipoEvento {
    NENHUM,
    SOBRECARGA_TERMICA,
    ESCASSEZ_ENERGIA,
    ESTABILIDADE_TEMPORARIA,
    RADIACAO_INTENSA,
    ABUNDANCIA_RECURSOS,
    TEMPERATURA_EXTREMA
};

class Ambiente {
private:
    ZonaPlaneta zona;
    
    // Condições ambientais
    float temperatura;
    float temperaturaBase;
    float nivelRecursos;
    float nivelRecursosBase;
    float taxaConsumo;
    
    // Evento atual
    TipoEvento eventoAtual;
    float duracaoEvento;
    float tempoRestanteEvento;
    
    // Consciência da zona
    float consciencia;
    float conscienciaMaxima;
    
    // Estatísticas
    int populacaoAtual;
    int mortesRecentes;
    
public:
    Ambiente(ZonaPlaneta z);
    ~Ambiente();
    
    // Getters
    ZonaPlaneta getZona() const { return zona; }
    float getTemperatura() const { return temperatura; }
    float getNivelRecursos() const { return nivelRecursos; }
    float getTaxaConsumo() const { return taxaConsumo; }
    TipoEvento getEventoAtual() const { return eventoAtual; }
    float getTempoRestanteEvento() const { return tempoRestanteEvento; }
    float getConsciencia() const { return consciencia; }
    float getConscienciaMaxima() const { return conscienciaMaxima; }
    int getPopulacaoAtual() const { return populacaoAtual; }
    
    // Setters
    void setPopulacaoAtual(int pop) { populacaoAtual = pop; }
    void incrementarMortes() { mortesRecentes++; }
    
    // Comportamentos
    void atualizar(float deltaTime);
    void ativarEvento(TipoEvento evento, float duracao);
    void cancelarEvento();
    void aumentarConsciencia(float quantidade);
    bool conscienciaCompleta() const;
    
    // Modificadores ambientais
    void alterarTemperatura(float delta);
    void alterarRecursos(float delta);
    
    // Utilidades
    std::string getNomeZona() const;
    std::string getDescricaoEvento() const;
    Color getCorZona() const;
    
private:
    void aplicarEfeitosEvento();
    void restaurarCondicoesBase();
};

#endif // AMBIENTE_HPP