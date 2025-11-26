# 🐱 CatRoad

> Um jogo 2D em C inspirado em Crossy Road: ajude o gato a atravessar a rodovia e ir o mais longe possível em 35 segundos!

## ✨ Visão Geral

**CatRoad** é um jogo arcade 2D desenvolvido em C utilizando a biblioteca gráfica **raylib**. O objetivo é controlar um gato que precisa atravessar pistas movimentadas, desviando de carros e obstáculos.

O jogo opera contra o relógio: você tem **35 segundos** para avançar o máximo de "faixas" possível. Além da pressão do tempo, o jogo possui um sistema de **dificuldade progressiva**: conforme o jogador avança, a velocidade e frequência dos carros aumentam, escalando até um limite máximo de **3x** a dificuldade inicial.

## 🎮 Regras do Jogo

* **Tempo:** O cronômetro inicia em 35 segundos. Se o tempo acabar, é fim de jogo.
* **Pontuação:** Cada passo para frente (nova faixa alcançada) = +1 ponto no score.
* **Colisão:** Se o gato for atingido por um carro, é Game Over imediato.
* **Dificuldade:** Quanto mais longe você vai, mais rápidos os carros ficam (até o limite de 3x).
* **Recorde:** O jogo salva automaticamente a sua maior distância alcançada.

## 🛠️ Tecnologias e Conceitos Utilizados

Este projeto demonstra o domínio de conceitos fundamentais e complexos da linguagem C aplicados ao desenvolvimento de jogos:

* **Lista Encadeada:** Estrutura de dados central para gerenciar os obstáculos (carros). Como os veículos entram e saem da tela constantemente, a lista encadeada permite a inserção e remoção dinâmica e eficiente dos nós.
* **Matriz:** Utilizada para a lógica do mapa, renderização do asfalto e cálculo de posições na grade.
* **Structs:** Definição das entidades do jogo (o Jogador/Gato e os Inimigos/Carros) para organizar seus atributos (posição X/Y, velocidade, hitbox, tipo).
* **Ponteiros e Alocação Dinâmica (malloc):** Gerenciamento manual da memória, essencial para o funcionamento da lista encadeada e otimização de recursos durante a execução.
* **Manipulação de Arquivos:** Sistema de persistência que lê e escreve o recorde (High Score) em um arquivo externo, mantendo os dados mesmo após fechar o jogo.

## 🕹️ Controles

O jogo suporta teclado para movimentação e controle de fluxo:

| Tecla/Ação | Comando Primário | Comando Secundário |
| :--- | :---: | :---: |
| **Mover para Cima** | W | Seta ↑ |
| **Mover para Baixo** | S | Seta ↓ |
| **Mover para Esquerda** | A | Seta ← |
| **Mover para Direita** | D | Seta → |
| **Reiniciar Jogo** | R | - |
| **Sair** | ESC | - |

## 🚀 Como Executar

### Pré-requisitos
1.  Um compilador C (ex: GCC, Clang).
2.  A biblioteca **raylib** instalada e configurada no seu ambiente de desenvolvimento.
