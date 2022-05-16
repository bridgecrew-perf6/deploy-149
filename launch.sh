#!/bin/bash
shopt -s expand_aliases
source /home/cpelisseverdoux/.bash_aliases

if ! command -v mojitos &> /dev/null
then
	echo "mojitos was not found, please install mojitos (https://gitlab.irit.fr/sepia-pub/mojitos) and use an alias, example:"
    echo "echo \"alias mojitos='/home/$(whoami)/mojitos'\" >> /home/$(whoami)/.bash_aliases"
    echo "Where /home/$(whoami)/mojitos is the directory wich contain the mojitos binary file"
    echo "Don't forget to reload: source ~/.bash_aliases"
    exit
else	
	echo "Execution of mojitos"
	mojitos -r -c -o $(pwd)/Results.csv -e $(pwd)/examples/test_dot    

	
fi
