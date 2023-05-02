#!/bin/bash

# Check if a C file was provided as an argument
if [ $# -ne 1 ]; then
  echo "Usage: $0 <C file>"
  exit 1
fi

# Compile the C file and redirect stderr to a temporary file
gcc -o /dev/null "$1" 2> /tmp/compiler_output

# Count the number of errors and warnings
errors=$(grep -c "error:" /tmp/compiler_output)
warnings=$(grep -c "warning:" /tmp/compiler_output)

# echo "Compilation complete"
echo "$errors"
echo "$warnings"

# Clean up the temporary file
rm /tmp/compiler_output
exit 0