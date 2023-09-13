# Chesserver
Chess client and server to play with other players over a network

# Architettura
# Server
Server centrale a cui gli utenti si connettono per giocare tra di loro.
Il server si appoggia a un database SQLite se c'è bisogno (non dovrebbe essercene)

# Client
Il client si connette al server e gli viene presentato un menù:
- Creare una partita (massimo 32 caratteri)
- Connettersi a una stanza
- Disconnettersi


# Funzioni
## Stanza
Una partita viene creata con un nome univoco. Se un utente si unisce alla partita, il gioco inizia.

A fine partita gli utenti vengono disconnessi

## Partita
A inizio partita viene scelto chi dei due giocatori inizi per primo. Dopodiché si procede a turni, ogni giocatore ha un timer che scorre solo durante il suo turno.

Ogni mossa che proviene da un giocatore viene validata e inoltrata a entrambi.


# Implementazione
## Server
La server socket principale accetta le connessioni e fa partire un thread col menù. A ogni connessione associa e invia un token se non è già presente

Alla creazione della partita viene creato un thread (con parametro la connessione), che sta fermo.

All'unione alla partita, viene inviato un messaggio al thread e comincia l'esecuzione.


## Client
Il client chiede l'ip e la porta del server.
Alla connessione il menù, da cui scegliere se creare o unirsi alla stanza.

Alla creazione della stanza, sta in attesa finché non si unisce un altro giocatore.

All'unione alla stanza, se è il primo diventa giocatore, e il creatore della stanza viene notificato. Se non è il primo ad unirsi diventa uno spettatore.

All'inizio partita si avvia l'interfaccia che prevede l'input per i giocatori.

### Interfaccia
Board graphic interface:

r n b q k b n r 
p p p p p p p p 
. . . . . . . . 
. . . . . . . . 
. . . . . . . . 
. . . . . . . . 
P P P P P P P P 
R N B Q K B N R 

Input:
formato algebrico e.g.: e6, Nc3
oppure scelta casella e destinazione se non riesco (esempio: a2 a4)

# Tecnologie
- multithreading
- mutex e lock
- semafori
- logica scacchi


## Strutture dati

typedef struct player{
    char *token;
    int timer;
    int socket_fd;
    struct player *next_player;
} player;


typedef struct match_data{
    player *players;
    unsigned int timer_lenght;
    unsigned short int num_players; //usato per scorrere la lista dei giocatori
} match_data;

typedef struct game{
    char name[GAME_NAME_MAX_LENGHT];
    pthread_t tid;
    match_data *match_data;
    UT_hash_handle hh;
} game;

typedef struct games_struct{
    game *hashmap;
    pthread_rwlock_t rwlock;
} games_struct;

Per l'hashmap è stata usata la seguente implementazione: https://troydhanson.github.io/uthash.

## Funzionalità da aggiungere
- Token del client:
    - Creazione sul server e gestione quando un nuovo client si connette
    - Salvataggio sul client

- Timer per la partita

- Tutta la parte degli scacchi:
    - scacchiera
    - pezzi
    - ricezione e parsing mosse
    - validazione mosse
    - log delle mosse

- Visualizzazione scacchiera sul client

- Dump e ripristino della struttura games

- Due sistemi di log sul server. Uno per gli eventi (creazione partita, rimozione, connessione giocatore) e l'altro per le partite (le mosse ecc ecc)

## Modifiche da fare:
### client.c
    - linea 39, non fare chiudere il processo immediatamente, prova prima un paio di volte

### game_handling.h
    match_data non ha senso:
    - players magari diventa current_player

    game_handling dovrebbe inoltre riferirsi alla parte di gestione delle strutture dati

### game_handling.c
    insert_game():
    - linea 22: nella creazione del thread andrebbe passata la struct game

    run_game():
    - Controlla i giocatori finché non ci sono entrambi (parte dalla testa e itera controllando che ci siano due elementi)
    - sceglie casualmente uno dei giocatori per iniziare avvia il timer e comunica ai client che la partita è iniziata
    - aspetta la mossa, usando il timer come timeout. Se va in timeout partita finita. Se arriva la mossa si aggiorna il timer, si valida la mossa e se è valida il timer si stoppa e si comunica con i client, prossimo giocatore.
    - A termine partita eliminare il thread e l'entry del match, ricreare i worker per i client (notifica che la partita è finita)


### server_connection
    Dovrebbe separare le funzioni di utility da quelle del worker (per interagire col client direttamente)

    join_game:
    - dovrebbe semplicemente allocare al game un player

