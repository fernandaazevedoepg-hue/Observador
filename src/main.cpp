#include "Simulador.hpp"
#include <raylib.h>
#include <ctime>
#include <cstdlib>

int main() {
    // Inicializar seed aleatória
    srand(static_cast<unsigned int>(time(nullptr)));
    
    // Configurações da janela
    const int LARGURA_TELA = 1280;
    const int ALTURA_TELA = 720;
    
    // Criar e executar o simulador
    Simulador simulador(LARGURA_TELA, ALTURA_TELA);
    simulador.executar();
    
    return 0;
}