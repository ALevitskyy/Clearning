if [ -z "$1" ]; then
  echo "Usage: $0 <source_file.c>"
  exit 1
fi

# Get the file name without the extension
filename=$(basename -- "$1")
filename="${filename%.*}"

# Compile the C file
clang -o "$filename" "$1"

# Check if compilation was successful
if [ $? -eq 0 ]; then
  # Execute the binary
  ./"$filename"
  # Remove the binary
  rm "$filename"
else
  echo "Compilation failed."
  exit 1
fi
