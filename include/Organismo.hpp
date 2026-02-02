#ifndef ORGANISMO_HPP
#define ORGANISMO_HPP

#include <string>
#include <raylib.h>

enum class TipoOrganismo {
    // Núcleo (Zona Vermelha)
    PYROSYNTH,
    RUBRAFLORA,
    IGNIVAR,
    VOLTREX,
    
    // Zona Habitável (Zona Verde)
    LUMIVINE,
    AEROFLORA,
    ORBITON,
    SYNAPSEX,
    
    // Periferia (Zona Azul)
    CRYOMOSS,
    GELIBLOOM,
    NULLWALKER,
    HUSKLING
};

enum class TipoCategoria {
    PLANTA,
    REAGENTE
};

enum class ZonaPlaneta {
    NUCLEO,      // Zona Vermelha
    HABITAVEL,   // Zona Verde
    PERIFERIA    // Zona Azul
};

class Organismo {
private:
    TipoOrganismo tipo;
    TipoCategoria categoria;
    ZonaPlaneta zona;
    
    // Atributos do organismo
    float energia;
    float energiaMaxima;
    int idade;
    float resistenciaTermica;
    float velocidadeOrbital;
    float eficienciaMetabolica;
    bool vivo;
    
    // Posição orbital
    float anguloOrbital;
    float raioOrbital;
    
    // Características visuais
    Color cor;
    float tamanho;
    
    // Reprodução
    int ciclosVida;
    int ciclosReproducao;
    
public:
    Organismo(TipoOrganismo t, ZonaPlaneta z, float angulo, float raio);
    ~Organismo();
    
    // Getters
    TipoOrganismo getTipo() const { return tipo; }
    TipoCategoria getCategoria() const { return categoria; }
    ZonaPlaneta getZona() const { return zona; }
    float getEnergia() const { return energia; }
    float getEnergiaMaxima() const { return energiaMaxima; }
    int getIdade() const { return idade; }
    float getResistenciaTermica() const { return resistenciaTermica; }
    float getVelocidadeOrbital() const { return velocidadeOrbital; }
    float getEficienciaMetabolica() const { return eficienciaMetabolica; }
    bool isVivo() const { return vivo; }
    float getAnguloOrbital() const { return anguloOrbital; }
    float getRaioOrbital() const { return raioOrbital; }
    Color getCor() const { return cor; }
    float getTamanho() const { return tamanho; }
    
    // Setters
    void setEnergia(float e);
    void setVivo(bool v) { vivo = v; }
    void setAnguloOrbital(float a) { anguloOrbital = a; }
    
    // Comportamentos
    void atualizar(float deltaTime, float temperaturaZona, float recursosZona);
    void consumirEnergia(float quantidade);
    void envelhecer();
    bool podeReproduzir() const;
    Organismo* reproduzir();
    void aplicarMutacao();
    void mover(float deltaTime);
    
    // Renderização
    void desenhar(Vector2 centroTela) const;
    
    // Utilidades
    std::string getNome() const;
    std::string getDescricao() const;
    
private:
    void inicializarAtributos();
    void atualizarAparencia();
    Color getCorPorTipo() const;
};

#endif // ORGANISMO_HPP