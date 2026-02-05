#include "../include/Missao.hpp"
#include <cmath>

Missao::Missao(TipoMissao t, ZonaPlaneta zona) 
    : tipo(t), zonaAlvo(zona), estado(EstadoMissao::DISPONIVEL),
      objetivoNumerico(0), tempoObjetivo(0), tempoDecorrido(0),
      pontos(0), eventoRequerido(TipoEvento::NENHUM), eventoFoiAtivado(false), tempoCondicao(0),
      missaoResistencia(false) {
    inicializarMissao();
}

Missao::~Missao() {}

void Missao::inicializarMissao() {
    switch(tipo) {
                case TipoMissao::TESTE_RESISTENCIA:
            nome = "Teste de Resistência";
            descricao = "Provocar sobrecarga térmica no Núcleo e observar sobreviventes.";
            objetivo = "Ative 'Sobrecarga Térmica' no Núcleo. Após o evento, pelo menos 5 organismos devem sobreviver.";
            objetivoNumerico = 5;
            tempoObjetivo = 0;
            pontos = 100;
            eventoRequerido = TipoEvento::SOBRECARGA_TERMICA;
            break;
            
                case TipoMissao::EVOLUCAO_ACELERADA:
            nome = "Evolução Acelerada";
            descricao = "Forçar mutações em organismos do Núcleo.";
            objetivo = "Ative 'Radiação Intensa' no Núcleo e mantenha por 30s.";
            objetivoNumerico = 30;
            tempoObjetivo = 30.0f;
            pontos = 150;
            eventoRequerido = TipoEvento::RADIACAO_INTENSA;
            break;
            
                case TipoMissao::EQUILIBRIO_PERFEITO:
            nome = "Equilíbrio Perfeito";
            descricao = "Manter energia e temperatura estáveis na Zona Habitável.";
            objetivo = "Mantenha a Zona Habitável estável (sem eventos; temperatura/recursos próximos do ideal) por 90s.";
            objetivoNumerico = 90;
            tempoObjetivo = 90.0f;
            pontos = 120;
            eventoRequerido = TipoEvento::NENHUM;
            break;
            
        case TipoMissao::SIMBIOSE:
            nome = "Simbiose";
            descricao = "Criar condicoes para coexistencia";
            objetivo = "Aumentar taxa de sobrevivencia coletiva";
            objetivoNumerico = 20;
            break;
            
                case TipoMissao::ESTIMULO_MINIMO:
            nome = "Estímulo Mínimo";
            descricao = "Ativar um evento de baixa intensidade na Periferia.";
            objetivo = "Ative 'Estabilidade Temporária' na Periferia e mantenha por 10s.";
            objetivoNumerico = 10;
            tempoObjetivo = 10.0f;
            pontos = 80;
            eventoRequerido = TipoEvento::ESTABILIDADE_TEMPORARIA;
            break;
            
                case TipoMissao::SELECAO_NATURAL:
            nome = "Seleção Natural";
            descricao = "Criar escassez prolongada e reduzir a população da Periferia.";
            objetivo = "Ative 'Escassez de Energia' na Periferia por 25s e reduza a população para <= 10.";
            objetivoNumerico = 10;
            tempoObjetivo = 25.0f;
            pontos = 130;
            eventoRequerido = TipoEvento::ESCASSEZ_ENERGIA;
            break;
            
        // Fase 2
        case TipoMissao::INSTABILIDADE_CRITICA:
            nome = "Instabilidade Critica";
            descricao = "Nucleo em colapso energetico";
            objetivo = "Salvar dados antes da perda total";
            break;
            
        case TipoMissao::CONTROLE_POPULACIONAL:
            nome = "Controle Populacional";
            descricao = "Reduzir superpopulacao";
            objetivo = "Nao extinguir especies";
            objetivoNumerico = 15;
            break;
            
        // Fase 3 - Missões contra o sistema
        case TipoMissao::INTERFERENCIA_OCULTA:
            nome = "Interferencia Oculta";
            descricao = "Reduzir impacto dos eventos";
            objetivo = "Evitar deteccao pelo sistema";
            missaoResistencia = true;
            pontos = 200;
            break;
            
        case TipoMissao::ZONA_SILENCIO:
            nome = "Zona de Silencio";
            descricao = "Desligar sensores orbitais";
            objetivo = "Ocultar organismos conscientes";
            missaoResistencia = true;
            pontos = 250;
            break;
            
        case TipoMissao::QUEBRA_PROTOCOLO:
            nome = "Quebra de Protocolo";
            descricao = "Cancelar eventos obrigatorios";
            objetivo = "Preservar organismos selecionados";
            missaoResistencia = true;
            pontos = 300;
            break;
            
        default:
            nome = "Missao Desconhecida";
            descricao = "...";
            objetivo = "...";
            break;
    }
}

void Missao::iniciar() {
    estado = EstadoMissao::EM_PROGRESSO;
    tempoDecorrido = 0;
    tempoCondicao = 0;
    eventoFoiAtivado = false;
}

void Missao::notificarEventoAtivado(TipoEvento evento) {
    if (estado != EstadoMissao::EM_PROGRESSO) return;
    if (eventoRequerido != TipoEvento::NENHUM && evento == eventoRequerido) {
        eventoFoiAtivado = true;
    }
}

void Missao::notificarAcaoManual() {
    if (estado != EstadoMissao::EM_PROGRESSO) return;

    // Ações manuais (teclas/controles) só contam para as missões de resistência (Fase 3)
    switch (tipo) {
        case TipoMissao::INTERFERENCIA_OCULTA:
        case TipoMissao::ZONA_SILENCIO:
        case TipoMissao::QUEBRA_PROTOCOLO:
            // Marca como concluída simulando que a condição/tempo-alvo foi atingido
            tempoDecorrido = tempoObjetivo;
            tempoCondicao = tempoObjetivo;
            estado = EstadoMissao::COMPLETADA;
            break;
        default:
            break;
    }
}

void Missao::atualizar(float deltaTime, const Populacao& pop, const Ambiente& ambiente) {
    if (estado != EstadoMissao::EM_PROGRESSO) return;

    tempoDecorrido += deltaTime;

    // Atualiza condição/progresso específico por missão
    switch (tipo) {
        case TipoMissao::EVOLUCAO_ACELERADA:
        case TipoMissao::ESTIMULO_MINIMO:
        case TipoMissao::SELECAO_NATURAL:
            if (ambiente.getEventoAtual() == eventoRequerido) tempoCondicao += deltaTime;
            break;

        case TipoMissao::EQUILIBRIO_PERFEITO: {
            // Estável = sem evento e muito perto do "ideal" da zona
            float tempIdeal = 0.5f;
            float recIdeal = 1.0f;
            if (zonaAlvo == ZonaPlaneta::NUCLEO) { tempIdeal = 0.9f; recIdeal = 0.8f; }
            if (zonaAlvo == ZonaPlaneta::PERIFERIA) { tempIdeal = 0.2f; recIdeal = 0.4f; }

            bool estavel =
                (ambiente.getEventoAtual() == TipoEvento::NENHUM) &&
                (fabsf(ambiente.getTemperatura() - tempIdeal) < 0.05f) &&
                (fabsf(ambiente.getNivelRecursos() - recIdeal) < 0.15f);

            if (estavel) tempoCondicao += deltaTime;
            else tempoCondicao = 0.0f;
            break;
        }

        default:
            break;
    }

    if (verificarConclusao(pop, ambiente)) {
        estado = EstadoMissao::COMPLETADA;
    } else if (verificarFalha(pop, ambiente)) {
        estado = EstadoMissao::FALHADA;
    }
}

bool Missao::verificarConclusao(const Populacao& pop, const Ambiente& ambiente) {
    switch(tipo) {
        case TipoMissao::TESTE_RESISTENCIA:
            // Só completa se a sobrecarga foi ativada e já terminou
            if (!eventoFoiAtivado) return false;
            return (ambiente.getEventoAtual() == TipoEvento::NENHUM) && (pop.getTamanho() >= objetivoNumerico);

        case TipoMissao::EVOLUCAO_ACELERADA:
            // Manter Radiação Intensa por tempoObjetivo
            return tempoCondicao >= tempoObjetivo;

        case TipoMissao::EQUILIBRIO_PERFEITO:
            // Manter estabilidade contínua por tempoObjetivo
            return (tempoCondicao >= tempoObjetivo) && (pop.getTamanho() > 10);

        case TipoMissao::SIMBIOSE:
            return pop.getTamanho() >= 20;

        case TipoMissao::ESTIMULO_MINIMO:
            // Estabilidade Temporária por 10s
            return tempoCondicao >= tempoObjetivo;

        case TipoMissao::SELECAO_NATURAL:
            // Escassez prolongada + população baixa
            return (tempoCondicao >= tempoObjetivo) && (pop.getTamanho() <= objetivoNumerico);

        // Fase 3 - Resistência (ações do jogador)
        case TipoMissao::INTERFERENCIA_OCULTA:
        case TipoMissao::ZONA_SILENCIO:
        case TipoMissao::QUEBRA_PROTOCOLO:
            return acaoManualFeita;

        default:
            // Outras missões: tempo mínimo de 30s (placeholder)
            return tempoDecorrido >= 30.0f;
    }
}


bool Missao::verificarFalha(const Populacao& pop, const Ambiente& ambiente) {
    // Falha se população zerou (exceto em missões de resistência)
    if (!missaoResistencia && pop.getTamanho() == 0) {
        return true;
    }
    return false;
}

float Missao::getProgresso() const {
    if (tempoObjetivo > 0) {
        return (tempoDecorrido / tempoObjetivo) * 100.0f;
    }
    return 0;
}

// GestorMissoes

GestorMissoes::GestorMissoes() 
    : missaoAtual(nullptr), fase(1), totalCompletadas(0), pontuacaoTotal(0) {
    inicializarFase(1);
}

GestorMissoes::~GestorMissoes() {}

void GestorMissoes::inicializarFase(int numeroFase) {
    fase = numeroFase;
    missoes.clear();
    totalCompletadas = 0;
    
    switch(fase) {
        case 1:
            criarMissoesFase1();
            break;
        case 2:
            criarMissoesFase2();
            break;
        case 3:
            criarMissoesFase3();
            break;
    }
    
    if (!missoes.empty()) {
        missaoAtual = &missoes[0];
        missaoAtual->iniciar();
    }
}

void GestorMissoes::criarMissoesFase1() {
    missoes.push_back(Missao(TipoMissao::TESTE_RESISTENCIA, ZonaPlaneta::NUCLEO));
    missoes.push_back(Missao(TipoMissao::EVOLUCAO_ACELERADA, ZonaPlaneta::NUCLEO));
    missoes.push_back(Missao(TipoMissao::EQUILIBRIO_PERFEITO, ZonaPlaneta::HABITAVEL));
    missoes.push_back(Missao(TipoMissao::SIMBIOSE, ZonaPlaneta::HABITAVEL));
    missoes.push_back(Missao(TipoMissao::ESTIMULO_MINIMO, ZonaPlaneta::PERIFERIA));
    missoes.push_back(Missao(TipoMissao::SELECAO_NATURAL, ZonaPlaneta::PERIFERIA));
}

void GestorMissoes::criarMissoesFase2() {
    missoes.push_back(Missao(TipoMissao::INSTABILIDADE_CRITICA, ZonaPlaneta::NUCLEO));
    missoes.push_back(Missao(TipoMissao::CONTROLE_POPULACIONAL, ZonaPlaneta::HABITAVEL));
}

void GestorMissoes::criarMissoesFase3() {
    missoes.push_back(Missao(TipoMissao::INTERFERENCIA_OCULTA, ZonaPlaneta::HABITAVEL));
    missoes.push_back(Missao(TipoMissao::ZONA_SILENCIO, ZonaPlaneta::PERIFERIA));
    missoes.push_back(Missao(TipoMissao::QUEBRA_PROTOCOLO, ZonaPlaneta::NUCLEO));
}

void GestorMissoes::proximaMissao() {
    if (missaoAtual == nullptr) return;
    
    // Encontrar índice da missão atual
    for (size_t i = 0; i < missoes.size(); i++) {
        if (&missoes[i] == missaoAtual) {
            if (i + 1 < missoes.size()) {
                missaoAtual = &missoes[i + 1];
                missaoAtual->iniciar();
            } else {
                missaoAtual = nullptr;
            }
            break;
        }
    }
}

void GestorMissoes::completarMissao() {
    if (missaoAtual != nullptr) {
        totalCompletadas++;
        pontuacaoTotal += missaoAtual->getPontos();
    }
    proximaMissao();
}

void GestorMissoes::falharMissao() {
    proximaMissao();
}

void GestorMissoes::notificarEventoAtivado(ZonaPlaneta zona, TipoEvento evento) {
    if (missaoAtual == nullptr) return;
    if (missaoAtual->getZonaAlvo() != zona) return;
    missaoAtual->notificarEventoAtivado(evento);
}

void GestorMissoes::notificarAcaoManual(TipoMissao tipo) {
    if (missaoAtual == nullptr) return;
    if (missaoAtual->getTipo() != tipo) return;
    missaoAtual->notificarAcaoManual();
}

void GestorMissoes::atualizar(float deltaTime, const Populacao& pop, const Ambiente& ambiente) {
    if (missaoAtual != nullptr) {
        missaoAtual->atualizar(deltaTime, pop, ambiente);
    }
}

bool GestorMissoes::todasMissoesCompletas() const {
    int completas = 0;
    for (const auto& missao : missoes) {
        if (missao.getEstado() == EstadoMissao::COMPLETADA) {
            completas++;
        }
    }
    // Todas as missões devem estar completas
    return completas == missoes.size() && missoes.size() > 0;
}

bool GestorMissoes::podeAvancarFase() const {
    return totalCompletadas >= missoes.size() / 2;
}