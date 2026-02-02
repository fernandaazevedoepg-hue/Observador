#include "../include/Organismo.hpp"
#include <cmath>
#include <cstdlib>
#include <iostream>

Organismo::Organismo(TipoOrganismo t, ZonaPlaneta z, float angulo, float raio) 
    : tipo(t), zona(z), anguloOrbital(angulo), raioOrbital(raio), 
      vivo(true), idade(0), ciclosVida(0), ciclosReproducao(0) {
    inicializarAtributos();
    atualizarAparencia();
}

Organismo::~Organismo() {}

void Organismo::inicializarAtributos() {
    // Inicializar atributos baseado no tipo
    switch(tipo) {
        // NÚCLEO - PLANTAS
        case TipoOrganismo::PYROSYNTH:
            categoria = TipoCategoria::PLANTA;
            energiaMaxima = 100.0f;
            energia = energiaMaxima;
            resistenciaTermica = 0.95f;
            velocidadeOrbital = 0.1f;
            eficienciaMetabolica = 0.7f;
            break;
            
        case TipoOrganismo::RUBRAFLORA:
            categoria = TipoCategoria::PLANTA;
            energiaMaxima = 80.0f;
            energia = energiaMaxima;
            resistenciaTermica = 0.85f;
            velocidadeOrbital = 0.15f;
            eficienciaMetabolica = 0.6f;
            break;
            
        // NÚCLEO - REAGENTES
        case TipoOrganismo::IGNIVAR:
            categoria = TipoCategoria::REAGENTE;
            energiaMaxima = 90.0f;
            energia = energiaMaxima;
            resistenciaTermica = 0.80f;
            velocidadeOrbital = 0.8f;
            eficienciaMetabolica = 0.4f;
            break;
            
        case TipoOrganismo::VOLTREX:
            categoria = TipoCategoria::REAGENTE;
            energiaMaxima = 110.0f;
            energia = energiaMaxima;
            resistenciaTermica = 0.85f;
            velocidadeOrbital = 0.6f;
            eficienciaMetabolica = 0.5f;
            break;
            
        // ZONA HABITÁVEL - PLANTAS
        case TipoOrganismo::LUMIVINE:
            categoria = TipoCategoria::PLANTA;
            energiaMaxima = 95.0f;
            energia = energiaMaxima;
            resistenciaTermica = 0.60f;
            velocidadeOrbital = 0.2f;
            eficienciaMetabolica = 0.8f;
            break;
            
        case TipoOrganismo::AEROFLORA:
            categoria = TipoCategoria::PLANTA;
            energiaMaxima = 85.0f;
            energia = energiaMaxima;
            resistenciaTermica = 0.55f;
            velocidadeOrbital = 0.5f;
            eficienciaMetabolica = 0.65f;
            break;
            
        // ZONA HABITÁVEL - REAGENTES
        case TipoOrganismo::ORBITON:
            categoria = TipoCategoria::REAGENTE;
            energiaMaxima = 100.0f;
            energia = energiaMaxima;
            resistenciaTermica = 0.60f;
            velocidadeOrbital = 0.7f;
            eficienciaMetabolica = 0.75f;
            break;
            
        case TipoOrganismo::SYNAPSEX:
            categoria = TipoCategoria::REAGENTE;
            energiaMaxima = 105.0f;
            energia = energiaMaxima;
            resistenciaTermica = 0.58f;
            velocidadeOrbital = 0.5f;
            eficienciaMetabolica = 0.85f;
            break;
            
        // PERIFERIA - PLANTAS
        case TipoOrganismo::CRYOMOSS:
            categoria = TipoCategoria::PLANTA;
            energiaMaxima = 75.0f;
            energia = energiaMaxima;
            resistenciaTermica = 0.90f;
            velocidadeOrbital = 0.1f;
            eficienciaMetabolica = 0.9f;
            break;
            
        case TipoOrganismo::GELIBLOOM:
            categoria = TipoCategoria::PLANTA;
            energiaMaxima = 80.0f;
            energia = energiaMaxima;
            resistenciaTermica = 0.85f;
            velocidadeOrbital = 0.15f;
            eficienciaMetabolica = 0.75f;
            break;
            
        // PERIFERIA - REAGENTES
        case TipoOrganismo::NULLWALKER:
            categoria = TipoCategoria::REAGENTE;
            energiaMaxima = 60.0f;
            energia = energiaMaxima;
            resistenciaTermica = 0.80f;
            velocidadeOrbital = 0.4f;
            eficienciaMetabolica = 0.95f;
            break;
            
        case TipoOrganismo::HUSKLING:
            categoria = TipoCategoria::REAGENTE;
            energiaMaxima = 70.0f;
            energia = energiaMaxima;
            resistenciaTermica = 0.88f;
            velocidadeOrbital = 0.3f;
            eficienciaMetabolica = 0.65f;
            break;
    }
}

void Organismo::atualizar(float deltaTime, float temperaturaZona, float recursosZona) {
    if (!vivo) return;
    
    ciclosVida++;
    
    // Consumo de energia base
    float consumoBase = (2.0f - eficienciaMetabolica) * deltaTime;
    consumirEnergia(consumoBase);
    
    // Efeito da temperatura
    float diferencaTermica = std::abs(temperaturaZona - resistenciaTermica);
    if (diferencaTermica > 0.3f) {
        consumirEnergia(diferencaTermica * 5.0f * deltaTime);
    }
    
    // Absorção de recursos
    if (categoria == TipoCategoria::PLANTA) {
        energia += recursosZona * eficienciaMetabolica * deltaTime * 0.5f;
        if (energia > energiaMaxima) energia = energiaMaxima;
    }
    
    // Movimento
    mover(deltaTime);
    
    // Envelhecimento
    if (ciclosVida % 60 == 0) {
        envelhecer();
    }
    
    // Morte por falta de energia
    if (energia <= 0) {
        vivo = false;
    }
    
    // Atualizar aparência
    atualizarAparencia();
}

void Organismo::consumirEnergia(float quantidade) {
    energia -= quantidade;
    if (energia < 0) energia = 0;
}

void Organismo::envelhecer() {
    idade++;
    
    // Organismos mais velhos são menos eficientes
    eficienciaMetabolica *= 0.99f;
    
    // Morte por velhice (probabilidade aumenta com idade)
    int idadeMaxima = 0;
    switch(tipo) {
        case TipoOrganismo::PYROSYNTH:
        case TipoOrganismo::IGNIVAR:
            idadeMaxima = 50;
            break;
        case TipoOrganismo::RUBRAFLORA:
        case TipoOrganismo::VOLTREX:
        case TipoOrganismo::LUMIVINE:
        case TipoOrganismo::AEROFLORA:
            idadeMaxima = 100;
            break;
        case TipoOrganismo::ORBITON:
        case TipoOrganismo::SYNAPSEX:
            idadeMaxima = 150;
            break;
        case TipoOrganismo::CRYOMOSS:
        case TipoOrganismo::GELIBLOOM:
            idadeMaxima = 200;
            break;
        default:
            idadeMaxima = 120;
            break;
    }
    
    if (idade > idadeMaxima && (rand() % 100) < 20) {
        vivo = false;
    }
}

bool Organismo::podeReproduzir() const {
    return vivo && energia > energiaMaxima * 0.6f && idade > 10 && ciclosReproducao > 100;
}

Organismo* Organismo::reproduzir() {
    if (!podeReproduzir()) return nullptr;
    
    // Consumir energia para reprodução
    energia *= 0.7f;
    ciclosReproducao = 0;
    
    // Criar novo organismo próximo
    float novoAngulo = anguloOrbital + ((rand() % 60 - 30) * PI / 180.0f);
    float novoRaio = raioOrbital + ((rand() % 20 - 10));
    
    Organismo* filho = new Organismo(tipo, zona, novoAngulo, novoRaio);
    
    // Aplicar mutação
    filho->aplicarMutacao();
    
    return filho;
}

void Organismo::aplicarMutacao() {
    // Mutações aleatórias pequenas
    float mutacao = (rand() % 21 - 10) / 100.0f; // -0.1 a +0.1
    
    eficienciaMetabolica += mutacao * 0.1f;
    if (eficienciaMetabolica < 0.1f) eficienciaMetabolica = 0.1f;
    if (eficienciaMetabolica > 1.0f) eficienciaMetabolica = 1.0f;
    
    resistenciaTermica += mutacao * 0.05f;
    if (resistenciaTermica < 0.3f) resistenciaTermica = 0.3f;
    if (resistenciaTermica > 1.0f) resistenciaTermica = 1.0f;
    
    velocidadeOrbital += mutacao * 0.2f;
    if (velocidadeOrbital < 0.05f) velocidadeOrbital = 0.05f;
    if (velocidadeOrbital > 1.0f) velocidadeOrbital = 1.0f;
    
    energiaMaxima += mutacao * 10.0f;
    if (energiaMaxima < 30.0f) energiaMaxima = 30.0f;
    if (energiaMaxima > 150.0f) energiaMaxima = 150.0f;
}

void Organismo::mover(float deltaTime) {
    if (categoria == TipoCategoria::REAGENTE) {
        anguloOrbital += velocidadeOrbital * deltaTime * 0.1f;
        if (anguloOrbital > 2 * PI) anguloOrbital -= 2 * PI;
    }
    
    ciclosReproducao++;
}

void Organismo::desenhar(Vector2 centroTela) const {
    if (!vivo) return;
    
    // Calcular posição na tela
    float x = centroTela.x + raioOrbital * cos(anguloOrbital);
    float y = centroTela.y + raioOrbital * sin(anguloOrbital);
    
    // Desenhar organismo
    DrawCircle((int)x, (int)y, tamanho, cor);
    
    // Efeito visual para reagentes
    if (categoria == TipoCategoria::REAGENTE) {
        DrawCircleLines((int)x, (int)y, tamanho + 2, Fade(cor, 0.3f));
    }
}

void Organismo::atualizarAparencia() {
    // Tamanho baseado na energia
    tamanho = 3.0f + (energia / energiaMaxima) * 5.0f;
    
    // Cor baseada no tipo
    cor = getCorPorTipo();
    
    // Modificar cor baseado na saúde
    float saude = energia / energiaMaxima;
    if (saude < 0.5f) {
        cor = ColorAlpha(cor, 0.5f + saude);
    }
}

Color Organismo::getCorPorTipo() const {
    switch(tipo) {
        case TipoOrganismo::PYROSYNTH: return RED;
        case TipoOrganismo::RUBRAFLORA: return MAROON;
        case TipoOrganismo::IGNIVAR: return ORANGE;
        case TipoOrganismo::VOLTREX: return YELLOW;
        
        case TipoOrganismo::LUMIVINE: return GREEN;
        case TipoOrganismo::AEROFLORA: return LIME;
        case TipoOrganismo::ORBITON: return DARKGREEN;
        case TipoOrganismo::SYNAPSEX: return Color{100, 200, 100, 255};
        
        case TipoOrganismo::CRYOMOSS: return SKYBLUE;
        case TipoOrganismo::GELIBLOOM: return BLUE;
        case TipoOrganismo::NULLWALKER: return DARKBLUE;
        case TipoOrganismo::HUSKLING: return Color{150, 150, 200, 255};
        
        default: return WHITE;
    }
}

void Organismo::setEnergia(float e) {
    energia = e;
    if (energia > energiaMaxima) energia = energiaMaxima;
    if (energia < 0) energia = 0;
}

std::string Organismo::getNome() const {
    switch(tipo) {
        case TipoOrganismo::PYROSYNTH: return "Pyrosynth";
        case TipoOrganismo::RUBRAFLORA: return "Rubraflora";
        case TipoOrganismo::IGNIVAR: return "Ignivar";
        case TipoOrganismo::VOLTREX: return "Voltrex";
        case TipoOrganismo::LUMIVINE: return "Lumivine";
        case TipoOrganismo::AEROFLORA: return "Aeroflora";
        case TipoOrganismo::ORBITON: return "Orbiton";
        case TipoOrganismo::SYNAPSEX: return "Synapsex";
        case TipoOrganismo::CRYOMOSS: return "Cryomoss";
        case TipoOrganismo::GELIBLOOM: return "Gelibloom";
        case TipoOrganismo::NULLWALKER: return "Nullwalker";
        case TipoOrganismo::HUSKLING: return "Huskling";
        default: return "Desconhecido";
    }
}

std::string Organismo::getDescricao() const {
    switch(tipo) {
        case TipoOrganismo::PYROSYNTH: 
            return "Absorve calor extremo para gerar energia";
        case TipoOrganismo::IGNIVAR: 
            return "Corpo energetico instavel, movimenta-se rapidamente";
        case TipoOrganismo::LUMIVINE: 
            return "Fotossintese energetica, beneficia outros organismos";
        case TipoOrganismo::SYNAPSEX: 
            return "Aprende com o ambiente, primeiros sinais de consciencia";
        case TipoOrganismo::CRYOMOSS: 
            return "Cresce lentamente, armazena energia por longos periodos";
        case TipoOrganismo::NULLWALKER: 
            return "Quase invisivel, consome o minimo de energia possivel";
        default: 
            return "Organismo sintetico do projeto Oblivion";
    }
}