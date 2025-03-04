#!/bin/bash

# Funzione per determinare quale terminale è disponibile
get_terminal() {
    if command -v xterm >/dev/null 2>&1; then
        echo "xterm"
    elif command -v gnome-terminal >/dev/null 2>&1; then
        echo "gnome-terminal"
    elif command -v konsole >/dev/null 2>&1; then
        echo "konsole"
    elif command -v xfce4-terminal >/dev/null 2>&1; then
        echo "xfce4-terminal"
    else
        echo "NONE"
    fi
}

# Funzione per avviare un programma nel terminale appropriato
launch_terminal() {
    local title="$1"
    local command="$2"
    local terminal=$(get_terminal)
    
    case $terminal in
        "xterm")
            xterm -T "$title" -e "$command" &
            ;;
        "gnome-terminal")
            gnome-terminal --title="$title" -- bash -c "$command" &
            ;;
        "konsole")
            konsole --title "$title" -e bash -c "$command" &
            ;;
        "xfce4-terminal")
            xfce4-terminal --title="$title" -e "$command" &
            ;;
        *)
            echo "Nessun terminale supportato trovato. Installare xterm con:"
            echo "sudo apt-get install xterm"
            exit 1
            ;;
    esac
}

# Compila il server
cd Progetto/Server
make clean
make

# Compila il client
cd ../Client
make clean
make

# Torna alla directory principale
cd ../..

# Avvia il server e i client
launch_terminal "Server" "cd Progetto/Server && ./server; exec bash"
sleep 1
launch_terminal "Client 1" "cd Progetto/Client && ./client; exec bash"
launch_terminal "Client 2" "cd Progetto/Client && ./client; exec bash"