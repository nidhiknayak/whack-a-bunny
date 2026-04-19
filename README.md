# 🐰 Whack-a-Bunny

A fun 3D Whack-a-Mole style game built with **OpenGL** and **FreeGLUT** in C++. Bunnies pop up from holes — click them before they disappear to score points!


---

## 🎮 Gameplay

- **6 holes** are laid out on a green field
- A bunny randomly pops out of one hole at a time
- **Click the bunny** before it disappears to earn **+10 points**
- You have **30 seconds** — score as high as you can!
- Missing a bunny flashes the screen red
- Press **R** to restart after game over

---

## 🖼️ Features

- 3D rendered bunnies and holes using OpenGL lighting and shading
- Animated hammer cursor that follows your mouse and swings on click
- Score and countdown timer HUD
- +10 floating score popup on successful hits
- Miss flash feedback (red screen)
- Dual-light setup for warm and cool lighting

---

## 🛠️ Requirements

- **C++ compiler** (MSVC, GCC, or Clang)
- **OpenGL** (comes with most systems)
- **FreeGLUT** — [download here](http://freeglut.sourceforge.net/)
- **GLU** (usually bundled with OpenGL)

---

## 🚀 Building & Running

### Visual Studio (Windows)

1. Open the project in **Visual Studio**
2. Make sure **FreeGLUT** is linked:
   - Add FreeGLUT's `include/` folder to *Additional Include Directories*
   - Add FreeGLUT's `lib/` folder to *Additional Library Directories*
   - Add `freeglut.lib` and `opengl32.lib` to *Additional Dependencies*
3. Place `freeglut.dll` in the same folder as the built `.exe`
4. Hit **Build → Run** (or press `F5`)

### Linux / GCC

```bash
g++ main.cpp -o whack-a-bunny -lGL -lGLU -lglut
./whack-a-bunny
```

### macOS

```bash
g++ main.cpp -o whack-a-bunny -framework OpenGL -framework GLUT
./whack-a-bunny
```

---

## 🕹️ Controls

| Input | Action |
|-------|--------|
| **Mouse move** | Aim the hammer |
| **Left click** | Whack! |
| **R** | Restart (after game over) |
| **ESC** | Quit |

---

## 📁 Project Structure

```
whack-a-bunny/
├── main.cpp        # All game source code
└── README.md       # You're reading it!
```

---

## 🔧 Possible Improvements

- [ ] Add sound effects on hit/miss
- [ ] Difficulty scaling (bunnies appear faster over time)
- [ ] High score tracking
- [ ] Multiple bunny types with different point values
- [ ] Animated bunny pop-up/pop-down motion

---

## 📄 License

This project is open source. Feel free to use, modify, and share it.