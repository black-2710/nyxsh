#  Nyxsh — The Custom Minimal Shell

> _"Simple. Scriptable. Stealthy."_  
> Designed for hackers, minimalists, and those who prefer to build their tools from the ground up.

---

## ✨ Overview

**Nyxsh** is a lightweight custom Unix shell handcrafted in C.  
It embraces simplicity, a beautiful CLI aesthetic, and extensibility through dotfiles and modular design.

This shell is not meant to replace Bash or Zsh—  
It’s meant to be **yours.**

---

## 🛠️ Features

- Custom colored prompt (e.g., `┌───(user@host)-[dir]` & `└─$`)
- Command execution using `fork()` and `execvp()`
- Background job handling via `&`
- Built-in commands: `cd`, `exit`, `clear`
- Persistent shell history (`~/.nyxhistory`)
- Configurable startup script (`~/.nyxrc`)
- Readline-based input with arrow key history navigation
- SIGCHLD handler for zombie process cleanup
  
---

## 🔥 .nyxrc Support

Place your custom aliases, banners, and commands in:

```bash
~/.nyxrc
