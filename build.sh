mkdir -p bin
clang ./core_midi_stuff.c -o ./bin/core_midi_stuff -framework CoreMIDI -framework CoreFoundation
