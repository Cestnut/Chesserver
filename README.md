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
    unsigned short int num_players; //usato per scorrere la lista dei giocatori
} match_data;

typedef struct game{
    char name[GAME_NAME_MAX_LENGTH];
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


## Controllo riempimento stanza
In run_game(), il controllo che la stanza sia piena viene fatto confrontando connected_players e max_players.
Per non avere attesa attiva tra un confronto e l'altro è stata utilizzata una condition variable come meccanismo di sincronizzazione, che viene segnalata ogni volta che un nuovo giocatore si connette. 

Il motivo per cui è stato usato questo meccanismo e non un semaforo inizializzato a max players è che in questo caso non sarebbe più stato necessario il confronto tra le due variabili, mentre l'approccio con la condition variable rende più coerente e intuitiva l'implementazione, anche dato che il confronto tra connected_players e max_players viene fatto ogni volta che si vuole verificare lo stato di riempimento della stanza.

## Stallo, scacco e scaccomatto

La condizione di stallo va effettuata a ogni turno. Lo scaccomatto sussiste se si è in stallo (nessuna mossa valida fattibile) e si è sotto scacco. Quindi a ogni turno avremo questo tipo di controllo:

if(mosse_valide == NULL){
    if(sotto_scacco){
        scacco_matto();
    }
    else{
        stallo();
    }
}

Essere sottoscacco è un controllo che va fatto per ogni mossa come uno dei controlli. Se la mossa che sta per essere effettuata metterebbe il giocatore sotto scacco allora non è una mossa valida.

I controlli sullo scacco vanno effettuati quindi in due casi:
- a inizio round per vedere se ci sono mosse valide e se si è sottoscacco
- dopo l'esecuzione di una mossa per verificare se porterebbe a uno scacco

Le nuove strutture dati e funzioni sarebbero

typedef struct Position{
    int row, col;
} Position;

//Ritorna una lista di punti destinazione in cui il pezzo a src_point può andare
Position *valid_moves(board *board, piece_color player_color, Position src_point);

//Ritorna TRUE o FALSE, se il pezzo in posizione src_point può andare in posizione dst_point
//Per non duplicare tutte le funzioni e dato che non costa molto, viene chiamato valid_moves() su src_point
//e si controlla se dst_point è dentro la lista ritornata
int is_move_valid(board *board, piece_color player_color, Position src_point, Position dst_point);

//Controlla se il giocatore player_color è sotto scacco.
//Iterando tutti i pezzi della scacchiera, controllo se appartengono all'avversario e in caso se possono raggiungere il re del giocatore

int is_in_check(board *board, piece_color player_color, Position king_position);

//Controlla se esistono mosse valide per il giocatore. Vengono iterati tutti i suoi pezzi sulla scacchiera e per ognuno di essi viene chiamato valid_moves() se la lista non è vuota si ritorna TRUE, altrimenti alla fine si ritorna FALSE. Non dovrebbe costare molto dato che è difficile che solo pochi dei pezzi del giocatore abbiano poche mosse
int exists_valid_move(board *board, piece_color player_color);


Può spostarsi(una flag che indica se controllare una mossa specifica o una mossa qualunque):
- Rispetta il pattern
- Il percorso non è bloccato, la destinazione non è un pezzo del giocatore che sta facendo la mossa

Giocatore sottoscacco:
- Per ogni pezzo del giocatore avversario:
 - Il pezzo può spostarsi alla casella del re del giocatore

Mossa valida:
    - Il pezzo può spostarsi
    - La mossa non metterebbe il giocatore sottoscacco



### Scambio messaggi per la partita:
server->client - Inizio partita contente il colore del giocatore

client->server - Contenente la mossa che si vuole fare
server->client - Contente la mossa fatta dal giocatore

server->client - nuovo stato partita