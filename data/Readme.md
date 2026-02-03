# 🎮 Versão 1.2 - Melhorias Visuais e de Gameplay

## ✨ Novidades e Correções

### 🎨 Melhorias Visuais

#### Diferenciação de Organismos
- **Plantas** agora são representadas por **quadrados/retângulos**
- **Reagentes** continuam **circulares** mas com cauda de movimento
- Efeitos visuais únicos por tipo:
  - **Pyrosynth**: Aura de fogo (laranja)
  - **Ignivar**: Energia instável pulsante
  - **Lumivine**: Brilho verde expansivo
  - **Synapsex**: Pulso de consciência
  - **Cryomoss**: Cristais de gelo ao redor

#### Tela Inicial Renovada
- Fonte maior e mais legível
- Texto centralizado
- História mais clara e envolvente
- Animação de pulsação no "Pressione ENTER"
- Fundo estrelado melhorado (200 estrelas com brilho variado)

#### Interface de Jogo
- **Painel superior expandido** (100px altura)
- **Seção de Eventos Ativos** mostrando:
  - Qual zona está com evento
  - Nome do evento
  - Tempo restante
- **Painel de Missões expandido** (140px altura) com:
  - Nome da missão em destaque
  - Zona alvo com cor correspondente
  - Descrição completa
  - Objetivo detalhado
  - Barra de progresso visual
  - Indicador de missão de resistência

### 🎯 Sistema de Missões

#### Transição de Fases Corrigida
- **Fase 1 → 2**: Quando consciência das 3 zonas = 150
- **Fase 2 → 3**: Quando **TODAS** as missões da Fase 2 forem completadas
- Não avança apenas com metade das missões

#### Narrativa Entre Fases
- **Tela de pausa narrativa** ao trocar de fase
- **Fase 2**: Texto explicativo sobre a sobrevivência do supercomputador
- **Fase 3**: Revelação sobre consciência dos organismos e quebra da quarta parede
- Mensagens centralizadas e formatadas
- Pressione SPACE para continuar após ler

#### Feedback de Missões
- **Tela de conclusão** ao completar missão:
  - "MISSÃO COMPLETADA"
  - Nome da missão
  - Pontos ganhos
  - Progresso total (X/Y missões)
- **Tela de falha** se missão falhar
- Jogo pausa automaticamente para você ler

### 🌍 Correções de Bugs

#### Periferia sem Organismos
- ✅ **CORRIGIDO**: Periferia agora inicia com 12 organismos
- Distribuição balanceada de plantas e reagentes

#### Eventos Não Funcionando
- ✅ **CORRIGIDO**: Todos os 6 eventos agora funcionam corretamente
- Eventos adicionais mapeados:
  - **[D]** Radiação Intensa
  - **[F]** Temperatura Extrema

#### Mensagens de Eventos
- ✅ **CORRIGIDO**: Agora mostra claramente:
  - Qual zona tem evento ativo
  - Nome do evento
  - Tempo restante (em segundos)
- Seção dedicada no topo da tela

#### Barra de Consciência
- ✅ **CORRIGIDO**: Barra só aparece na **Fase 1**
- Escondida automaticamente a partir da Fase 2

### 📋 Menu de Eventos Melhorado

- Fonte maior e mais legível
- Indicadores visuais claros:
  - Zona selecionada: `<--`
  - Evento selecionado: `<--` em laranja
- Descrição de cada evento:
  - "Aumenta temperatura drasticamente"
  - "Reduz recursos disponíveis"
  - etc.
- Cores corretas (zona selecionada destaca em vermelho/verde/azul)
- Aviso se nenhum evento foi selecionado

### 🎨 Outras Melhorias

- **Fundo estrelado dinâmico** com variação de brilho
- **Texto "PAUSADO"** mais visível quando o jogo está pausado
- **Mensagens narrativas** em tela cheia quando há transições
- **Cores mais vibrantes** em toda interface
- **Fontes maiores** para melhor legibilidade

## 🎮 Como Jogar (Lembretes)

### Controles Básicos
- **ENTER**: Iniciar jogo / Continuar após pausa narrativa
- **SPACE**: Pausar/Despausar
- **E**: Abrir menu de eventos
- **↑/↓**: Controlar velocidade da simulação
- **ESC**: Voltar de menus

### Menu de Eventos
1. Pressione **E** para abrir
2. Selecione zona: **1** (Núcleo), **2** (Habitável), **3** (Periferia)
3. Selecione evento: **Q/W/A/S/D/F**
4. Pressione **ENTER** para ativar
5. Veja o evento acontecer em tempo real!

### Progressão
- **Fase 1**: Complete missões e aumente consciência
- **Fase 2**: Gerencie vida do supercomputador + missões
- **Fase 3**: Escolha entre obedecer ou resistir

## 📊 Resumo das Melhorias

| Categoria | Antes | Depois |
|-----------|-------|--------|
| **Organismos** | Todos círculos iguais | Plantas quadradas, reagentes circulares + efeitos |
| **Eventos** | Sem feedback visual | Painel dedicado com tempo restante |
| **Missões** | Só nome | Nome + descrição + objetivo + progresso |
| **Transição Fases** | Sem narrativa | Tela cheia com história |
| **Tela Inicial** | Texto pequeno | Fonte grande, centralizada, animada |
| **Periferia** | 10 organismos | 12 organismos (mais balanceado) |
| **Consciência** | Sempre visível | Só Fase 1 |

## 🚀 Próximos Passos Sugeridos

Para continuar melhorando:
1. ✅ Sons e música de fundo
2. ✅ Partículas visuais nos eventos
3. ✅ Zoom/pan da câmera
4. ✅ Tutorial interativo
5. ✅ Mais tipos de organismos

---

**Versão**: 1.2  
**Data**: Atualização de melhorias visuais  
**Compatibilidade**: 100% com versão anterior (salvamentos compatíveis)

Divirta-se observando a evolução! 🌌

## Contexto

No ano 2177, a Terra perdeu a capacidade de sustentar vida complexa.
Cientistas criaram Oblivion, um micro-planeta artificial colocado em órbita, para testar a sobrevivência de organismos geneticamente instáveis.
Esses organismos não são animais nem plantas conhecidas, são formas de vida sintéticas, projetadas para evoluir rapidamente.
Você é o Observador, uma IA responsável por monitorizar a evolução, ajustar o ambiente e decidir se a vida merece continuar.


## O planeta Oblivion

O planeta tem 3 zonas ambientais:

## Núcleo (Zona Vermelha) 
    Muita energia
    Temperatura extrema
    Só organismos resistentes sobrevivem

## Zona Habitável (Zona Verde)
    Energia equilibrada
    Maior taxa de reprodução
    Zona ideal

## Periferia (Zona Azul)
    Fria
    Poucos recursos
    Organismos lentos sobrevivem melhor


## Organismos

Os organismos se chamam Reagentes.

## Atributos:
    Energia
    Idade
    Resistência térmica
    Velocidade orbital
    Eficiência metabólica

## Evolução:
    Reagentes adaptam-se à zona onde vivem
    Os que não se adaptam morrem
    Os adaptados reproduzem-se
    Filhos herdam + mutação

## Organismos núcleo
    → Plantas
        Pyrosynth
            Absorve calor extremo para gerar energia
            Cresce em pulsos
            Se energia cair, entra em combustão e morre

        Atributos
            Energia: Alta (absorve calor extremo)
            Idade: Curta (cresce em pulsos e morre se energia cair)
            Resistência térmica: Muito alta
            Velocidade orbital: Baixa (fixa no local)
            Eficiência metabólica: Média-alta (rápida absorção, mas risco de combustão)
        
        Rubraflora
            Usa radiação para mutação rápida
            Pode gerar descendentes muito diferentes entre si
            Alta chance de falha genética

        Atributos
            Energia: Média (usa radiação para mutação)
            Idade: Curta-média
            Resistência térmica: Alta
            Velocidade orbital: Baixa
            Eficiência metabólica: Média (alta chance de falha genética, reprodução instável)

    → Reagentes (“animais”)
        Ignivar
            Corpo energético instável
            Movimenta-se rapidamente
            Vive pouco, mas se reproduz muito

        Atributos
            Energia: Média-alta
            Idade: Curta
            Resistência térmica: Alta
            Velocidade orbital: Muito alta
            Eficiência metabólica: Baixa (gasta muita energia para se mover rápido)

        Voltrex
            Alimenta-se de surtos de energia
            Pode causar eventos caóticos no Núcleo
        
        Atributos
            Energia: Alta (absorve surtos)
            Idade: Média
            Resistência térmica: Alta
            Velocidade orbital: Média-alta
            Eficiência metabólica: Média (cria instabilidade local, mas sobrevive razoavelmente)

## Organismos zona habitável
    → Plantas
        Lumivine
            Fotossíntese energética
            Produz energia extra que beneficia Ecos próximos
            Favorece simbiose
    
        Atributos
            Energia: Média-alta (produz energia extra)
            Idade: Média
            Resistência térmica: Média
            Velocidade orbital: Baixa (fixa no local, mas influencia vizinhos)
            Eficiência metabólica: Alta (favorece simbiose)
        
        Aeroflora
            Flutua ao redor do planeta
            Espalha “sementes orbitais”
            Ajuda na colonização de novas áreas
        Atributos
            Energia: Média
            Idade: Média
            Resistência térmica: Média
            Velocidade orbital: Média-alta (flutua ao redor)
            Eficiência metabólica: Média (espalha sementes orbitais)

    → Reagentes (“animais”)
        Orbiton
            Move-se em padrões orbitais perfeitos
            Alta inteligência adaptativa
            Pode desenvolver comportamentos coletivos
        
        Atributos
            Energia: Média
            Idade: Média-longa
            Resistência térmica: Média
            Velocidade orbital: Alta (padrões precisos)
            Eficiência metabólica: Alta (inteligência adaptativa, comportamentos coletivos)
        
        Synapsex
            Aprende com o ambiente
            Filhotes herdam comportamentos, não só atributos
            Primeiros sinais de “consciência”
        
        Atributos
            Energia: Média
            Idade: Média-longa
            Resistência térmica: Média
            Velocidade orbital: Média
            Eficiência metabólica: Alta (aprende com ambiente, sinais iniciais de consciência)

## Organismos periferia
    → Plantas
        Cryomoss
            Cresce lentamente
            Armazena energia por longos períodos
            Pode sobreviver sem reprodução por gerações
        
        Atributos
            Energia: Média-baixa (armazena energia por longos períodos)
            Idade: Longa
            Resistência térmica: Muito alta (suporta frio extremo)
            Velocidade orbital: Baixa (quase fixa)
            Eficiência metabólica: Alta (sobrevive mesmo sem reprodução)
        
        Gelibloom
            Floresce apenas quando há escassez extrema
            Muito resistente, mas rara
        
        Atributos
            Energia: Média
            Idade: Longa
            Resistência térmica: Alta
            Velocidade orbital: Baixa
            Eficiência metabólica: Média-alta (sobrevive em condições extremas)

    → Reagentes (“animais”)
        Nullwalker
            Quase invisível
            Consome o mínimo de energia possível
            Fica invisível quando há eventos
        
        Atributos
            Energia: Baixa
            Idade: Média
            Resistência térmica: Alta
            Velocidade orbital: Baixa-média
            Eficiência metabólica: Muito alta (economiza energia, fica invisível quando há eventos)
        
        Huskling
            Nasce frágil
            A maioria morre cedo
            Os poucos que sobrevivem tornam-se quase indestrutíveis
            Carregam marcas físicas do ambiente
        
        Atributos
            Energia: Média-baixa
            Idade: Curta (a maioria morre cedo)
            Resistência térmica: Alta
            Velocidade orbital: Média-baixa
            Eficiência metabólica: Média (os poucos sobreviventes tornam-se quase indestrutíveis e carregam marcas do ambiente)

## Jogador

O jogador não controla diretamente os Reagentes.
    Observa os organismos
    Pausa o tempo
    Acelera a simulação
    Provoca eventos: Sobrecarga térmica, escassez de energia, estabilidade temporária…


## Jogo

    Fase 1 -  O experimento

        Missões
        No início jogo aparecerão missões para induzir o jogador a provocar eventos:

        → Missões Núcleo  
            Teste de Resistência
                Provocar sobrecarga térmica no Núcleo
                Objetivo: sobreviverem pelo menos 5 Ecos
                Falha aceitável: mortes em massa

            Evolução Acelerada
                Forçar mutações em organismos do Núcleo
                Observar variações extremas
        
        → Missões Zona Habitável
            Equilíbrio Perfeito
                Manter energia estável por 5 minutos
                Analisar crescimento populacional

            Simbiose
                Criar condições para plantas e Reagentes coexistirem
                Aumentar taxa de sobrevivência coletiva
        
        → Missões Periferia
            Estímulo Mínimo
                Ativar evento de baixa intensidade
                Forçar reação em ambiente hostil

            Seleção Natural

                Criar escassez prolongada
                Reduzir população para níveis sustentáveis
                
        Mecânica

            Cada zona do planeta possui uma barra que não é visível para o utilizador que cresce conforme o tempo passa (aumenta 1 de consciência a cada 1 segundo). Toda vez que o observador (jogador) provocar um evento a barra aumentará 2 de consciência enquanto o evento não terminar.  Essa barra representa a consciência dos reagentes, quando ela chegar ao fim (150 de consciência) ativará outra fase do jogo.

    Fase 2 - A sobrevivência

        Missões

        → Missões Núcleo  
            Instabilidade Crítica
                Núcleo entra em colapso energético
                Salvar dados antes da perda total

        → Missões Zona Habitável
            Controle Populacional
                Reduzir superpopulação sem extinguir espécies

        → Missões Periferia
            Ecos Invisíveis
                Detectar Ecos que desapareceram dos sensores
                Alguns não querem ser encontrados

            Falha de Suporte
                Sistema proíbe eventos positivos na Periferia
                Ecos devem sobreviver sozinhos
        → Missões Supercomputador
            Manutenção do Núcleo

            Desviar energia das zonas
                Afeta diretamente os Reagentes

            Ataque Externo
                Defender o sistema de desligamento forçado

            Mecânica

            Nessa nova fase o jogador terá que lutar para sobreviver. Será adicionada uma barra de vida que representa a resistência do supercomputador que nos armazena, afinal somos uma inteligência artificial. Agora, além das missões padrão, temos que fazer a manutenção do nosso computador e localizar os reagentes resistentes. Quando o jogador completar as missões da fase dois inicia a fase 3.

    Fase 3 - A ruptura

        Missões

        → Missões Núcleo  
            Intensificação Energética
                Aumentar fluxo de energia do Núcleo
                Forçar seleção extrema de Reagentes

            Purificação Térmica
                Ativar sobrecarga térmica

            Eliminar organismos instáveis

        → Missões Zona Habitável

            Reequilíbrio Populacional
                Provocar escassez controlada
                Reduzir densidade biológica

            Aceleração Evolutiva
                 Ativar eventos sucessivos
                 Forçar adaptação rápida

        → Missões Periferia
            Correção Estrutural
                Ativar evento ambiental severo
                Forçar adaptação em ambiente negligenciado

            Filtragem Final
                Criar escassez extrema
                Manter apenas Reagentes altamente eficientes

        → Missões Contra o sistema
            Interferência Oculta
                Reduzir impacto dos eventos
                Evitar detecção pelo sistema

            Zona de Silêncio
                Desligar sensores orbitais
                Ocultar Reagentes conscientes

            Quebra de Protocolo
                Cancelar eventos obrigatórios
                Preservar organismos selecionados

            Mecânica

            Com o passar do tempo o observador vai começar a questionar se o que ele está fazendo é certo, porque apesar dos reagentes serem formas de vidas artificiais eles não deixam ser vidas, assim como ele que também é uma vida artificial. Neste momento o jogador vai começar a ter dúvidas se deve fazer as missões que são atribuídas para cada zona, agora vão entrar as missões contra o sistema, missões que vão ajudar os reagentes a serem livres. Com a história percebemos que nós, o observador, também estamos sendo controlados, neste momento quebramos a quarta parede. O jogo vai terminar com o observador se autodestruindo de modo que mais ninguém tenha acesso ao planeta Oblivion. 