#!/bin/bash
########################
# Labor 1
# Author Yannick Schilling
# Date April 18th 2024
########################

function show_help {
    echo "Usage: email_analysis.sh [-h|--help] [-c|--company] [FILE] [-e|--email] [FILE]"
    echo ""
    echo "Analyses the mailing list according to the commands as follows:"
    echo ""
    echo "  -h|--help: Show this help and quit"
    echo "  -c|--company: Show count of occurrences of each sender company"
    echo "  -e|--email: Show address and count of occurrences of given email address"
    echo ""
}

# Function to count occurrences of email addresses
count_mail(){
  grep "^From .*@" "$FILE" | cut -d " " -f 2 | sort -s | uniq -c | sort -n
}

# Function to count occurrences of sender companies
count_company(){
  grep "^From .*@" "$FILE" | cut -d "@" -f 2 | cut -d " " -f 1 | sort -s | uniq -c | sort -n
}

# Parse command line options


case "$1" in
*.txt)
    FILE="$1"
    count_mail
    exit
    ;;
-h|--help)
    show_help
    exit
    ;;
-e|--email)
    FILE="$2"
    count_mail
    exit
    ;;
-c|--company)
    FILE="$2"
    count_company
    exit
    ;;

*) echo "Error: Illegal option $1"
        show_help
        exit 1
    ;;
esac
