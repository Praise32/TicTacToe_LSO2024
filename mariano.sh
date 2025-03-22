#!/bin/bash

# Enable error handling
set -e

# Enable debug logging
DEBUG=true
log() {
    if [ "$DEBUG" = true ]; then
        echo "[DEBUG] $1"
    fi
}

# Check if running on Windows
is_windows() {
    [[ "$(uname)" =~ "MINGW"|"MSYS"|"CYGWIN" ]]
}

# Check if required directories exist
check_directories() {
    if [ ! -d "Progetto/Server" ]; then
        echo "Error: Server directory not found"
        exit 1
    fi
    if [ ! -d "Progetto/Client" ]; then
        echo "Error: Client directory not found"
        exit 1
    fi
}

# Function to determine available terminal
get_terminal() {
    if is_windows; then
        echo "windows"
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

# Function to launch program in appropriate terminal
launch_terminal() {
    local title="$1"
    local command="$2"
    local terminal=$(get_terminal)
    
    log "Launching terminal: $terminal with title: $title"
    
    case $terminal in
        "windows")
            start "cmd.exe" /c "title $title && $command"
            ;;
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
            echo "No supported terminal found. On Windows, ensure you're running from Git Bash or similar."
            exit 1
            ;;
    esac
}

# Main execution
main() {
    log "Starting script execution"
    
    # Check directories
    check_directories
    
    # Compile server
    log "Compiling server"
    cd "Progetto/Server" || exit 1
    make clean
    make
    
    # Compile client
    log "Compiling client"
    cd "../Client" || exit 1
    make clean
    make
    
    # Return to main directory
    cd "../.." || exit 1
    
    # Launch server and clients
    log "Launching applications"
    launch_terminal "Server" "cd \"$(pwd)/Progetto/Server\" && ./server"
    sleep 1
    launch_terminal "Client 1" "cd \"$(pwd)/Progetto/Client\" && ./client"
    launch_terminal "Client 2" "cd \"$(pwd)/Progetto/Client\" && ./client"
}

# Run main function
main