# 📚 Quiz CLI

A powerful, minimal command-line flashcard and spaced repetition learning tool.  
Easily create, manage, and review flashcards directly from your terminal.

---

## ✨ Features

- ⚡ Fast and lightweight CLI experience
- 🗂️ Organize flashcards into sets
- 🔄 Spaced repetition scheduling (SM-2 inspired)
- 🎲 Shuffle, reveal, skip, or auto-grade cards
- 📖 Simple text-based interface — no GUI needed
- 🛠️ Extensible command system

---

## 📦 Installation

```bash
# Clone the repository
git clone https://github.com/yourusername/quiz-cli.git
cd quiz-cli

# Build with CMake
cmake -S . -B build
cmake --build build --config Release
cmake --install build
```
-> The executable will be in `install/bin/` at the parent folder

---

## ⚠️ Adding path to Environment Variables Guide (for Windows users)

  To run `$ quiz` from anywhere in your terminal, do the followings: 
  - After downloading a release, extract the file.
  - Copy the address of the `bin/` folder.
  - Type on the search bar and find `Edit the system environment variables`.
  - Click on `Environmental variables`.
  - At `System variables`, scroll and find `Path`, then click `Edit...`.
  - Click on `New`, and paste the copied `bin/` address, then hit `Ok`.
  - And that's it. Enjoy the program!

## 🚀 Usage

### General Help
  NOTE: Keywords like `--set`,  `--item` can be abbreviated to `-s`, `-i`.

```bash
$ quiz help
```

### Create a new Set
```bash
$ quiz new --set French
```

### Create a new Card in a Set
```bash
$ quiz new --set French --front "Bonjour" --back "Hello"
```

### Learn a Set
```bash
$ quiz learn --set French
```

### Delete an Item in Set
```bash
$ quiz delete --set French --item 0
```

---

## 🎮 Learning Controls

When reviewing a card:

- **Auto-mode** → `Enter`
- **Reveal Back** → `reveal`, or `r`
- **Grade** (after reveal):
  - `1` → Again
  - `2` → Hard
  - `3` → Good
  - `4` → Easy
- **Skip (before reveal)** → `skip` / `s`
- **Next (after reveal)** → `next` / `n`
- **Bubble** (reprint current card) → `bubble` / `b`
- **Help** → `help` / `h`
- **Quit** → `quit` / `q`

---

## 📂 Project Structure

```text
.
├── cli/               # App class object
├── commands/          # Command handlers (help, learn, etc.)
├── data/card/         # Card models and spaced repetition logic
├── data/dataHandler/  # JSON file handler 
├── CMakeLists.txt     # Build configuration
└── main.cpp           # Entry point
```

---

## 🛠️ Development

### Add a New Command
- Define the command in `commands/`
- Add nodes and their dependencies in `Command` constructor
- Update `quiz_help()` to document it
- Register in the command dispatcher

---

## 🤝 Contributing

Contributions are welcome!  
1. Fork the repo  
2. Create a feature branch (`git checkout -b feature/new-command`)  
3. Commit your changes (`git commit -m 'Add new command'`)  
4. Push the branch (`git push origin feature/new-command`)  
5. Open a Pull Request  

---

## 📜 License

MIT License © 2025 Nguyen Danh Khai (github.com/danhkhai07)

---

## 🌟 Acknowledgments

- Inspired by [Anki](https://apps.ankiweb.net/) and SM-2 algorithms
- Built with ❤️ in C++