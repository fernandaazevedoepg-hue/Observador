#ifndef POPULACAO_HPP
#define POPULACAO_HPP

#include "Organismo.hpp"
#include "Ambiente.hpp"
#include <vector>
#include <memory>

class Populacao {
private:
    std::vector<std::unique_ptr<Organismo>> organismos;
    std::vector<std::unique_ptr<Organismo>> novaGeracao;
    
    // Estatísticas
    int totalNascimentos;
    int totalMortes;
    int geracao;
    
    // Limites por zona
    int populacaoMaxima;
    
public:
    Populacao();
    ~Populacao();
    
    // Gestão de organismos
    void adicionarOrganismo(TipoOrganismo tipo, ZonaPlaneta zona, float angulo, float raio);
    void removerMortos();
    void processarReproducao();
    void aplicarSelecaoNatural(const Ambiente& ambiente);
    
    // Atualização
    void atualizar(float deltaTime, const Ambiente& ambiente);
    
    // Getters
    int getTamanho() const { return organismos.size(); }
    int getTotalNascimentos() const { return totalNascimentos; }
    int getTotalMortes() const { return totalMortes; }
    int getGeracao() const { return geracao; }
    const std::vector<std::unique_ptr<Organismo>>& getOrganismos() const { return organismos; }
    
    // Estatísticas
    int contarPorZona(ZonaPlaneta zona) const;
    int contarPorTipo(TipoOrganismo tipo) const;
    float energiaMedia() const;
    float idadeMedia() const;
    
    // Inicialização
    void inicializarPopulacao(ZonaPlaneta zona, int quantidade);
    void limpar();
    
    // Renderização
    void desenhar(Vector2 centroTela) const;
    
private:
    TipoOrganismo getOrganismoAleatorio(ZonaPlaneta zona) const;
};

#endif // POPULACAO_HPP