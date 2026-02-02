# 🚀 Guia Rápido de Início

## Instalação e Execução Rápida

### 1. Instalar Dependências (Ubuntu/Debian)

```bash
# Tornar o script executável
chmod +x install_dependencies.sh

# Executar instalador
sudo ./install_dependencies.sh
```

**OU manualmente:**

```bash
sudo apt update
sudo apt install build-essential libraylib-dev git cmake
```

---

### 2. Compilar o Jogo

```bash
# Na pasta do projeto
make
```

---

### 3. Executar

```bash
# Opção 1: Via make
make run

# Opção 2: Diretamente
cd bin
./Observador
```

---

## ⚠️ Problemas Comuns

### Erro: "raylib.h: No such file or directory"

**Solução:**
```bash
sudo apt install libraylib-dev
```

### Erro: "cannot find -lraylib"

**Solução:** Instalar Raylib do código-fonte:
```bash
cd /tmp
git clone https://github.com/raysan5/raylib.git
cd raylib/src
make PLATFORM=PLATFORM_DESKTOP
sudo make install
```

### Erro de permissão ao executar

**Solução:**
```bash
chmod +x bin/Observador
```

---

## 🎮 Controles Básicos

**ENTER** - Iniciar jogo (na tela inicial)
**SPACE** - Pausar/Continuar
**E** - Abrir menu de eventos
**M** - Ver missões
**↑/↓** - Controlar velocidade
**ESC** - Voltar

---

## 📂 Estrutura de Arquivos

```
Observador/
├── src/          → Código-fonte (.cpp)
├── include/      → Headers (.hpp)
├── bin/          → Executável (após compilar)
├── obj/          → Objetos (após compilar)
├── Makefile      → Script de compilação
└── README.md     → Documentação completa
```

---

## 🔧 Comandos Make Úteis

```bash
make           # Compilar
make run       # Compilar e executar
make clean     # Limpar arquivos compilados
make rebuild   # Limpar e recompilar
make help      # Ver ajuda
```

---

## 📖 Próximos Passos

1. Leia o **README.md** para história completa
2. Consulte **DESIGN.md** para detalhes dos organismos
3. Jogue e experimente diferentes estratégias!

---

## 💬 Dicas de Jogo

- **Fase 1:** Foque em completar missões básicas
- **Fase 2:** Gerencie a vida do supercomputador
- **Fase 3:** Questione suas ordens - o jogo muda aqui!

---

**Divirta-se sendo o Observador!** 🌌