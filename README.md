# Chesserver
Chess client and server to play with other players over a network

# Architettura
# Server
Server centrale a cui gli utenti si connettono per giocare tra di loro.
Il server si appoggia a un database SQLite

# Client
Il client si connette al server e gli viene presentato un menù:
- Creare una partita
- Connettersi a una stanza
- Disconnettersi


# Funzioni
## Stanza
Una partita viene creata con un nome univoco. Se un utente si unisce alla partita, il gioco inizia.
Tutti gli utenti che si connettono dopo faranno da spettatori.

A fine partita gli utenti vengono disconnessi

## Partita
A inizio partita viene scelto chi dei due giocatori inizi per primo. Dopodiché si procede a turni, ogni giocatore ha un timer che scorre solo durante il suo turno.

Ogni mossa che proviene da un giocatore viene validata e inoltrata a entrambi.


# Implementazione
## Server
La server socket principale accetta le connessioni e fa partire un thread col menù. A ogni connessione associa e invia un token se non è già presente

Alla creazione della partita viene creato un thread (con parametro la connessione), che sta fermo.

All'unione alla partita, viene inviato un messaggio al thread e comincia l'esecuzione.
Unioni successive sono considerati come spettatori e non influiscono sull'esecuzione.


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

typedef struct spectator{
    int socket_fd;
} spectator;

typedef struct match_data{
    player *players;
    spectator *spectators;
    unsigned int timer_lenght;
    unsigned short int num_players; //usato per scorrere la lista dei giocatori
    unsigned int max_spectators;
} match_data;