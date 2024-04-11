#!/bin/sh
########################
#Labor 1
#Author Yannick Schilling
########################

function show_help {
    echo "Usage: email_analysis.sh [-h|--help] [-c|--company] [-e|--email]"
    echo ""
    echo "Analyses the mailing list according to the commands as follows:"
    echo ""
    echo "  -h|--help: Show this help and quit"
    echo "  -c|--company: Show count of occurences of each sender company"
    echo "  -e|--email: Show adress and count of ocurrences of given mail-adress"
    echo ""
}

#search for headers including the mail-adress. Space after "From" and including "@"
count_mail(){
  grep "^From .*@" gcc_2024.02.txt | cut -d " " -f 2 | cut -d " " -f 1 | sort -s | uniq -c | sort -rnr
}

count_company(){
  grep "^From .*@" gcc_2024.02.txt | cut -d "@" -f 2 | cut -d " " -f 1 | sort -s | uniq -c | sort -rnr
}

while test $# -gt 0 ; do #while Argument > 0; do
    case "$1" in
    -h|--help)
        show_help
        exit
        ;;
    -e|--email)
        count_mail
        ;;
    -c|--company)
        count_company
        ;;
    *)#exception for illegal input
        echo "Error: Illegal option $1"
        show_help
        exit 1
        ;;
    esac
    shift
done




