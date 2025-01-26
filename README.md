# TicTacToe_LSO2024
Progetto 1: "La partita di Tris" per il corso di laurea di Laboratorio di Sistemi Operativi 2024-2025

Questo progetto è una simulazione di un gioco Tic Tac Toe multi-client, sviluppato in linguaggio C e gestito tramite un'architettura client-server. Utilizza Docker per facilitare il deploy e l'esecuzione.

## Funzionalità principali
- Gestione multi-client con sincronizzazione del server.
- Comunicazione socket TCP.
- Logica completa di gestione delle partite, incluse vittorie, pareggi e turni.

## Struttura del progetto
- `client/`: Contiene il codice e le implementazioni del client.
- `server/`: Contiene il codice e le implementazioni del server.
- `doc/`: Documentazione e istruzioni per l'utilizzo.

## Come iniziare
1. Costruisci l'ambiente con Docker.
2. Avvia il server e connetti i client.
3. Gioca a Tic Tac Toe!

#

## Struttura del Progetto

```graphql
├── LICENSE                      # Licenza del progetto
├── README.md                    # Guida rapida al progetto
├── client
│   ├── client.c                 # Punto di ingresso principale per il client
│   ├── client_include           # Directory con i file header (.h) del client
│   │   ├── client_models.h      # Strutture dati e costanti usate nel client
│   │   └── gui.h                # Dichiarazioni delle funzioni della GUI
│   ├── client_models_impl       # Directory con l'implementazione delle funzioni
│   │   ├── connection.c         # Funzioni per la connessione al server
│   │   ├── gui.c                # Funzioni per l'interfaccia utente (GUI)
│   │   └── receive.c            # Funzioni per ricevere e gestire i messaggi dal server
│   ├── dockerfile               # Dockerfile per creare il container del client
│   └── makefile                 # Makefile per costruire il client
│
├── doc
│   ├── documentazione.pdf       # Documentazione del progetto
│   ├── istruzioni_docker_compose.md  # Istruzioni per Docker Compose
│   └── tracce_progetti.pdf      # Tracce o specifiche del progetto
│
├── docker-compose.yml           # File docker-compose per avviare server e client
│
└── server
    ├── server.c                 # Punto di ingresso principale per il server
    ├── dockerfile               # Dockerfile per il container del server
    ├── makefile                 # Makefile per costruire il server
    ├── server_include           # Directory con i file header (.h) del server
    │   ├── colors.h             # Colori ANSI per i log del server
    │   └── server_models.h      # Strutture dati e costanti del server
    └── server_models_impl       # Directory con l'implementazione delle funzioni
        ├── connection.c         # Funzioni di gestione delle connessioni TCP
        ├── game.c               # **Funzioni principali della partita di Tris**
        ├── player.c             # Funzioni per gestire i giocatori (player)
        └── session.c            # Funzioni per la gestione delle partite attive

```

