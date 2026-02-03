#include "../include/Missao.hpp"

Missao::Missao(TipoMissao t, ZonaPlaneta zona) 
    : tipo(t), zonaAlvo(zona), estado(EstadoMissao::DISPONIVEL),
      objetivoNumerico(0), tempoObjetivo(0), tempoDecorrido(0),
      pontos(100), missaoResistencia(false) {
    inicializarMissao();
}

Missao::~Missao() {}

void Missao::inicializarMissao() {
    switch(tipo) {
        case TipoMissao::TESTE_RESISTENCIA:
            nome = "Teste de Resistencia";
            descricao = "Provocar sobrecarga termica no Nucleo";
            objetivo = "Sobrevivam pelo menos 5 organismos";
            objetivoNumerico = 5;
            break;
            
        case TipoMissao::EVOLUCAO_ACELERADA:
            nome = "Evolucao Acelerada";
            descricao = "Forcar mutacoes no Nucleo";
            objetivo = "Observar variacoes extremas";
            tempoObjetivo = 60.0f;
            break;
            
        case TipoMissao::EQUILIBRIO_PERFEITO:
            nome = "Equilibrio Perfeito";
            descricao = "Manter energia estavel por 5 minutos";
            objetivo = "Analisar crescimento populacional";
            tempoObjetivo = 300.0f;
            break;
            
        case TipoMissao::SIMBIOSE:
            nome = "Simbiose";
            descricao = "Criar condicoes para coexistencia";
            objetivo = "Aumentar taxa de sobrevivencia coletiva";
            objetivoNumerico = 20;
            break;
            
        case TipoMissao::ESTIMULO_MINIMO:
            nome = "Estimulo Minimo";
            descricao = "Ativar evento de baixa intensidade";
            objetivo = "Forcar reacao em ambiente hostil";
            break;
            
        case TipoMissao::SELECAO_NATURAL:
            nome = "Selecao Natural";
            descricao = "Criar escassez prolongada";
            objetivo = "Reduzir populacao para niveis sustentaveis";
            objetivoNumerico = 10;
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
}

void Missao::atualizar(float deltaTime, const Populacao& pop, const Ambiente& ambiente) {
    if (estado != EstadoMissao::EM_PROGRESSO) return;
    
    tempoDecorrido += deltaTime;
    
    if (verificarConclusao(pop, ambiente)) {
        estado = EstadoMissao::COMPLETADA;
    } else if (verificarFalha(pop, ambiente)) {
        estado = EstadoMissao::FALHADA;
    }
}

bool Missao::verificarConclusao(const Populacao& pop, const Ambiente& ambiente) {
    switch(tipo) {
        case TipoMissao::TESTE_RESISTENCIA:
            return pop.getTamanho() >= objetivoNumerico;
            
        case TipoMissao::EVOLUCAO_ACELERADA:
            return tempoDecorrido >= tempoObjetivo;
            
        case TipoMissao::EQUILIBRIO_PERFEITO:
            return tempoDecorrido >= tempoObjetivo && pop.getTamanho() > 15;
            
        case TipoMissao::SIMBIOSE:
            return pop.getTamanho() >= objetivoNumerico;
            
        default:
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