#!/bin/bash

# Funzione per determinare quale terminale usare
get_terminal() {
    if [[ "$OSTYPE" == "darwin"* ]]; then
        echo "osascript"
    elif command -v xterm >/dev/null 2>&1; then
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
    
    if [[ "$terminal" == "osascript" ]]; then
        osascript -e "tell application \"Terminal\" to do script \"$command\""
    else
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
                echo "Nessun terminale supportato trovato."
                exit 1
                ;;
        esac
    fi
}

# Imposta la directory di lavoro
cd /Users/salvatoretortora/Desktop/TicTacToe_LSO2024/Progetto/

# Compila il server
cd server
make clean
make

# Compila il client
cd ../client
make clean
make

# Torna alla directory principale
cd ..

# Avvia il server e i client in nuovi terminali
launch_terminal "Server" "cd /Users/salvatoretortora/Desktop/TicTacToe_LSO2024/Progetto/server && ./server"
sleep 1
launch_terminal "Client 1" "cd /Users/salvatoretortora/Desktop/TicTacToe_LSO2024/Progetto/client && ./client"
launch_terminal "Client 2" "cd /Users/salvatoretortora/Desktop/TicTacToe_LSO2024/Progetto/client && ./client"
