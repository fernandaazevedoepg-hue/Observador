#include "../include/Ambiente.hpp"
#include <cmath>

Ambiente::Ambiente(ZonaPlaneta z) 
    : zona(z), eventoAtual(TipoEvento::NENHUM), duracaoEvento(0), 
      tempoRestanteEvento(0), consciencia(0), conscienciaMaxima(150), 
      populacaoAtual(0), mortesRecentes(0) {
    
    // Configurar condições base por zona
    switch(zona) {
        case ZonaPlaneta::NUCLEO:
            temperaturaBase = 0.9f;
            nivelRecursosBase = 0.8f;
            taxaConsumo = 1.5f;
            break;
            
        case ZonaPlaneta::HABITAVEL:
            temperaturaBase = 0.5f;
            nivelRecursosBase = 1.0f;
            taxaConsumo = 1.0f;
            break;
            
        case ZonaPlaneta::PERIFERIA:
            temperaturaBase = 0.2f;
            nivelRecursosBase = 0.4f;
            taxaConsumo = 0.7f;
            break;
    }
    
    temperatura = temperaturaBase;
    nivelRecursos = nivelRecursosBase;
}

Ambiente::~Ambiente() {}

void Ambiente::atualizar(float deltaTime) {
    // Aumentar consciência com o tempo
    consciencia += 1.0f * deltaTime;
    
    // Processar evento atual
    if (eventoAtual != TipoEvento::NENHUM) {
        tempoRestanteEvento -= deltaTime;
        
        // Aumentar consciência durante eventos
        consciencia += 2.0f * deltaTime;
        
        aplicarEfeitosEvento();
        
        if (tempoRestanteEvento <= 0) {
            cancelarEvento();
        }
    } else {
        // Restaurar condições base gradualmente
        restaurarCondicoesBase();
    }
    
    // Limitar valores
    if (temperatura < 0) temperatura = 0;
    if (temperatura > 1) temperatura = 1;
    if (nivelRecursos < 0) nivelRecursos = 0;
    if (nivelRecursos > 2) nivelRecursos = 2;
}

void Ambiente::ativarEvento(TipoEvento evento, float duracao) {
    eventoAtual = evento;
    duracaoEvento = duracao;
    tempoRestanteEvento = duracao;
}

void Ambiente::cancelarEvento() {
    eventoAtual = TipoEvento::NENHUM;
    tempoRestanteEvento = 0;
}

void Ambiente::reduzirTempoEvento(float segundos) {
    if (eventoAtual == TipoEvento::NENHUM) return;
    if (segundos <= 0) return;

    tempoRestanteEvento -= segundos;
    if (tempoRestanteEvento <= 0.0f) {
        cancelarEvento();
    }
}

void Ambiente::aumentarConsciencia(float quantidade) {
    consciencia += quantidade;
    if (consciencia > conscienciaMaxima) {
        consciencia = conscienciaMaxima;
    }
}

bool Ambiente::conscienciaCompleta() const {
    return consciencia >= conscienciaMaxima;
}

void Ambiente::alterarTemperatura(float delta) {
    temperatura += delta;
}

void Ambiente::alterarRecursos(float delta) {
    nivelRecursos += delta;
}

void Ambiente::aplicarEfeitosEvento() {
    switch(eventoAtual) {
        case TipoEvento::SOBRECARGA_TERMICA:
            temperatura = temperaturaBase + 0.5f;
            break;
            
        case TipoEvento::ESCASSEZ_ENERGIA:
            nivelRecursos = nivelRecursosBase * 0.1f;
            break;
            
        case TipoEvento::ESTABILIDADE_TEMPORARIA:
            temperatura = temperaturaBase;
            nivelRecursos = nivelRecursosBase * 1.5f;
            break;
            
        case TipoEvento::RADIACAO_INTENSA:
            temperatura = temperaturaBase + 0.2f;
            nivelRecursos = nivelRecursosBase * 0.7f;
            break;
            
        case TipoEvento::ABUNDANCIA_RECURSOS:
            nivelRecursos = nivelRecursosBase * 2.0f;
            break;
            
        case TipoEvento::TEMPERATURA_EXTREMA:
            if (zona == ZonaPlaneta::NUCLEO)
                temperatura = 1.0f;
            else if (zona == ZonaPlaneta::PERIFERIA)
                temperatura = 0.0f;
            else
                temperatura = temperaturaBase +0.4f;
            break;
            
        default:
            break;
    }
}

void Ambiente::restaurarCondicoesBase() {
    float velocidade = 0.01f;
    
    if (temperatura > temperaturaBase)
        temperatura -= velocidade;
    else if (temperatura < temperaturaBase)
        temperatura += velocidade;
        
    if (nivelRecursos > nivelRecursosBase)
        nivelRecursos -= velocidade;
    else if (nivelRecursos < nivelRecursosBase)
        nivelRecursos += velocidade;
}

std::string Ambiente::getNomeZona() const {
    switch(zona) {
        case ZonaPlaneta::NUCLEO: return "Nucleo (Zona Vermelha)";
        case ZonaPlaneta::HABITAVEL: return "Zona Habitavel (Verde)";
        case ZonaPlaneta::PERIFERIA: return "Periferia (Zona Azul)";
        default: return "Desconhecida";
    }
}

std::string Ambiente::getDescricaoEvento() const {
    switch(eventoAtual) {
        case TipoEvento::SOBRECARGA_TERMICA: return "Sobrecarga Termica Ativa";
        case TipoEvento::ESCASSEZ_ENERGIA: return "Escassez de Energia";
        case TipoEvento::ESTABILIDADE_TEMPORARIA: return "Estabilidade Temporaria";
        case TipoEvento::RADIACAO_INTENSA: return "Radiacao Intensa";
        case TipoEvento::ABUNDANCIA_RECURSOS: return "Abundancia de Recursos";
        case TipoEvento::TEMPERATURA_EXTREMA: return "Temperatura Extrema";
        default: return "Nenhum evento ativo";
    }
}

Color Ambiente::getCorZona() const {
    switch(zona) {
        case ZonaPlaneta::NUCLEO: return RED;
        case ZonaPlaneta::HABITAVEL: return GREEN;
        case ZonaPlaneta::PERIFERIA: return BLUE;
        default: return WHITE;
    }
}