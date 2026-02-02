#include "../include/Populacao.hpp"
#include <algorithm>
#include <cstdlib>

Populacao::Populacao() 
    : totalNascimentos(0), totalMortes(0), geracao(0), populacaoMaxima(200) {}

Populacao::~Populacao() {
    limpar();
}

void Populacao::adicionarOrganismo(TipoOrganismo tipo, ZonaPlaneta zona, float angulo, float raio) {
    if (organismos.size() < populacaoMaxima) {
        organismos.push_back(std::make_unique<Organismo>(tipo, zona, angulo, raio));
        totalNascimentos++;
    }
}

void Populacao::removerMortos() {
    auto it = std::remove_if(organismos.begin(), organismos.end(),
        [this](const std::unique_ptr<Organismo>& org) {
            if (!org->isVivo()) {
                totalMortes++;
                return true;
            }
            return false;
        });
    
    organismos.erase(it, organismos.end());
}

void Populacao::processarReproducao() {
    novaGeracao.clear();
    
    for (auto& org : organismos) {
        if (org->podeReproduzir()) {
            Organismo* filho = org->reproduzir();
            if (filho != nullptr) {
                novaGeracao.push_back(std::unique_ptr<Organismo>(filho));
            }
        }
    }
    
    // Adicionar nova geração
    for (auto& filho : novaGeracao) {
        if (organismos.size() < populacaoMaxima) {
            organismos.push_back(std::move(filho));
            totalNascimentos++;
        }
    }
    
    if (novaGeracao.size() > 0) {
        geracao++;
    }
}

void Populacao::aplicarSelecaoNatural(const Ambiente& ambiente) {
    // Organismos mal adaptados têm maior chance de morrer
    for (auto& org : organismos) {
        if (!org->isVivo()) continue;
        
        float adaptacao = std::abs(org->getResistenciaTermica() - ambiente.getTemperatura());
        
        // Se muito mal adaptado, chance de morte
        if (adaptacao > 0.4f) {
            if ((rand() % 100) < (adaptacao * 50)) {
                org->setVivo(false);
            }
        }
    }
}

void Populacao::atualizar(float deltaTime, const Ambiente& ambiente) {
    // Atualizar todos os organismos
    for (auto& org : organismos) {
        org->atualizar(deltaTime, ambiente.getTemperatura(), ambiente.getNivelRecursos());
    }
    
    // Aplicar seleção natural
    aplicarSelecaoNatural(ambiente);
    
    // Processar reprodução
    processarReproducao();
    
    // Remover mortos
    removerMortos();
}

int Populacao::contarPorZona(ZonaPlaneta zona) const {
    int count = 0;
    for (const auto& org : organismos) {
        if (org->getZona() == zona && org->isVivo()) {
            count++;
        }
    }
    return count;
}

int Populacao::contarPorTipo(TipoOrganismo tipo) const {
    int count = 0;
    for (const auto& org : organismos) {
        if (org->getTipo() == tipo && org->isVivo()) {
            count++;
        }
    }
    return count;
}

float Populacao::energiaMedia() const {
    if (organismos.empty()) return 0;
    
    float total = 0;
    int count = 0;
    for (const auto& org : organismos) {
        if (org->isVivo()) {
            total += org->getEnergia();
            count++;
        }
    }
    return count > 0 ? total / count : 0;
}

float Populacao::idadeMedia() const {
    if (organismos.empty()) return 0;
    
    float total = 0;
    int count = 0;
    for (const auto& org : organismos) {
        if (org->isVivo()) {
            total += org->getIdade();
            count++;
        }
    }
    return count > 0 ? total / count : 0;
}

void Populacao::inicializarPopulacao(ZonaPlaneta zona, int quantidade) {
    for (int i = 0; i < quantidade; i++) {
        TipoOrganismo tipo = getOrganismoAleatorio(zona);
        float angulo = (rand() % 360) * PI / 180.0f;
        
        float raio = 0;
        switch(zona) {
            case ZonaPlaneta::NUCLEO:
                raio = 50 + (rand() % 50);
                break;
            case ZonaPlaneta::HABITAVEL:
                raio = 150 + (rand() % 100);
                break;
            case ZonaPlaneta::PERIFERIA:
                raio = 300 + (rand() % 100);
                break;
        }
        
        adicionarOrganismo(tipo, zona, angulo, raio);
    }
}

void Populacao::limpar() {
    organismos.clear();
    novaGeracao.clear();
    totalNascimentos = 0;
    totalMortes = 0;
    geracao = 0;
}

void Populacao::desenhar(Vector2 centroTela) const {
    for (const auto& org : organismos) {
        org->desenhar(centroTela);
    }
}

TipoOrganismo Populacao::getOrganismoAleatorio(ZonaPlaneta zona) const {
    switch(zona) {
        case ZonaPlaneta::NUCLEO: {
            int r = rand() % 4;
            switch(r) {
                case 0: return TipoOrganismo::PYROSYNTH;
                case 1: return TipoOrganismo::RUBRAFLORA;
                case 2: return TipoOrganismo::IGNIVAR;
                default: return TipoOrganismo::VOLTREX;
            }
        }
        case ZonaPlaneta::HABITAVEL: {
            int r = rand() % 4;
            switch(r) {
                case 0: return TipoOrganismo::LUMIVINE;
                case 1: return TipoOrganismo::AEROFLORA;
                case 2: return TipoOrganismo::ORBITON;
                default: return TipoOrganismo::SYNAPSEX;
            }
        }
        case ZonaPlaneta::PERIFERIA: {
            int r = rand() % 4;
            switch(r) {
                case 0: return TipoOrganismo::CRYOMOSS;
                case 1: return TipoOrganismo::GELIBLOOM;
                case 2: return TipoOrganismo::NULLWALKER;
                default: return TipoOrganismo::HUSKLING;
            }
        }
    }
    return TipoOrganismo::LUMIVINE;
}