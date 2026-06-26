g++ -o cli assignment.cpp || clang++ -o cli assignment.cpp

if [ $? -eq 0 ]; then
    ./cli
else
    echo "Compilation failed."
    exit 1
fi