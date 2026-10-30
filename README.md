# Lunar Lander

This project is a minimal Entity-based Lunar Lander scaffold.

## Current foundation

- Raylib window and player/platform rendering
- `Entity` class for player and platform state
- Gravity through an acceleration value
- Delta-time measurement
- Fixed-timestep updates
- Horizontal acceleration with drift
- Static platforms and collision
- Hazard and goal platforms
- Mission result messages

The current image assets are:

- `assets/game/idle.png` for the ship
- `assets/game/pad_safe.png` for the safe platform
- `assets/game/pad_hazard.png` for the hazard platform
- `assets/game/pad_goal.png` for the goal platform

The next components to add are:

1. A moving platform
2. Fuel and a fuel display
3. Optional sprite animation

## Development

Install the JavaScript development dependencies and check supported files with:

```sh
npm install
npm run format:check
```

Run `npm run format` to format Markdown and configuration files. C++ files use
the repository's `.clang-format` configuration through the Microsoft C/C++
extension for VS Code.
