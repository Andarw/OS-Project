if [ $# -ne 1 ]; then
  echo "Usage: $0 <name_of_file>"
  exit 1
fi
# Create a file with the given name
touch "$1"
exit 0