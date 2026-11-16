# Vespera engine and game: roadmap design

Date: 2026-11-16
Status: draft, awaiting approval

## Intent

Grow this repository into a game engine and the first game built on it: **Vespera**,
an on-rails VR shooter. Visually it sits between Wipeout and Thumper: abstract neon
geometry, glowing track lines running into the distance, a chrome hero object,
heavy bloom, bold graphic UI, and the whole world pulsing with the music. The player
flies over neon landscapes and city blocks drawn as wireframes. Particles streak
through the air to sell the speed.

Play: a **third-person chase view** in both VR and flat mode, with **twin-stick
controls**. The left stick moves the ship across the plane perpendicular to the rail;
the right stick aims. Combat is **dense**: bullet-hell density, with thousands of
bullets on screen, laid out on the beat so it stays readable. It plays like Star Fox: a ship flies a fixed path while enemy
waves and bullet patterns come at it. A drum and bass soundtrack and the gameplay
shape each other, synced to the beat. The game is original; those games are
reference points for the feel only.

Why this way:
- **Solo developer, maximum understanding.** The owner writes most of the C++.
  The assistant explains each step: the design, the data flow, ownership, threading,
  and why each boundary sits where it does. It then reviews and tests. The owner knows
  every component.
- **Mostly procedural assets** with good authoring tools, because one person cannot
  hand-make a game's worth of art and music.
- **A public blog.** It is for other solo developers who want to learn how an engine
  is wired together, cohesive and manageable while staying fast. It is not a C++
  tutorial.
- **Mostly for Vespera, reusable for future VR games.** Build what Vespera needs, in a
  form a second VR game can reuse.

## Goals

1. Vespera playable in a Steam Frame **through SteamVR streaming** from a Windows or
   Linux x64 PC, using OpenXR.
2. Vespera playable **without VR**, on a monitor with a gamepad or keyboard and mouse.
3. Windows and Linux as equal first-class platforms. Both build in CI and pass tests.
4. A steady VR frame rate. The target is 90 Hz (11.1 ms per frame for everything),
   configurable upward. In VR a dropped frame is a comfort bug, not a cosmetic one.
5. Music time is the game's master clock. Gameplay events land on musical positions,
   and gameplay feeds back into the music.
6. Procedural generators for level geometry, ships, enemies, palettes, drum one-shots
   and synth sounds. Each is deterministic from a seed and parameters, and live-editable
   in an in-engine editor.
7. One blog post per milestone, drafted from notes kept while building.

## Non-goals (for Vespera; may return for a later game)

- Running natively on the headset (Linux ARM64 standalone). CI keeps building and
  testing on a native ARM64 runner (`ubuntu-26.04-arm`), so the engine keeps
  compiling for ARM64. Shipping on the
  headset waits for a game that needs it.
- D3D12, Metal, WebGPU, macOS, mobile.
- Deferred shading, PBR material pipelines, ray tracing, shaped-viewer at runtime.
- A general-purpose scene editor or imported-art pipeline. glTF import only if needed.
- Networking and multiplayer.
- SDL3, or any windowing or audio library besides the OS APIs.

## Anti-goals (would make this a failure even if it works)

- Engine features no game uses, written "for later".
- An abstraction layer with one implementation. The platform seam is the exception,
  because it always has two: Windows and Linux.
- Frame-time spikes or judder in VR. Any system that can't hold the frame budget gets
  redesigned, not tolerated.
- Engine code that knows about Vespera. Game specifics live in the game target.
- A codebase the owner can't explain without the assistant.

## Constraints

- C++23, CMake 3.28 or later, Ninja; presets `win-clang`, `linux-clang` and
  `linux-clang-asan`; ARM64 is the `linux-clang` preset on a native ARM64 machine
  (the qemu cross preset was dropped 2026-11-16). Warnings are errors.
- Dependencies are kept minimal:
  - **shaped-core** (MIT), the foundation. Its libraries used here:
    - `clean-core` (containers, strings, memory, assertions)
    - `typed-geometry` (math)
    - `shaped-graphics` (`sg`) with the **Vulkan** backend
    - **SGL** (`shaped-graphics-language`), the only shading language. It compiles
      natively to SPIR-V and depends only on clean-core.
    - Its vendored Dear ImGui, for tools.
    - `blob-cache` for caching generated assets.
    - `babel-serializer` where a file format is needed.
  - **OpenXR loader** (Khronos, Apache 2.0). Unavoidable for VR.
  - **Vulkan SDK / loader.**
  - **OS APIs only** for the window, input and audio:
    - Windows: Win32, Raw Input, XInput, WASAPI.
    - Linux: X11 through xcb, evdev, ALSA.
- Minimum CPU: x86-64-v3 (AVX2), shaped-core's default `SC_X64_LEVEL`. Decided
  2026-11-16.
- shaped-core is early-stage and its scope is still growing. Pin it to a known commit,
  and upgrade on purpose.
  - The owner can't change it directly, but its authors are colleagues.
  - Gaps go to them as requests, with a minimal reproduction from Vespera.
  - Until a gap is fixed, Vespera works around it in a thin, clearly marked adapter
    in its own code. No fork.
- SGL gaps to plan around:
  - no separate compilation
  - a nesting-depth limit in long expressions
  - native SPIR-V output has open performance issues compared with DXC
- Steam Frame streaming: SteamVR is the OpenXR runtime, and Valve does foveated
  streaming in the encoder, so the game doesn't need eye-tracking code.

## Approaches considered

- **A. Bottom-up layers.** Finish each layer before starting the next. Clean, but
  nothing is playable for a long time, and integration problems surface last.
- **B. Thin vertical slice, then deepen.** Get a crude but complete Vespera running
  end to end first, then improve one system per milestone. The wiring, which is the
  blog's subject, is built first and tested continuously.
- **C. Tools first.** Excellent authoring, but it designs tools before anyone knows
  what VR play needs.

**Chosen: B via A.** The slice is the goal. The path to it is built layer by layer,
bottom-up, with each layer only as deep as the slice needs. After the slice, each
milestone deepens one system without breaking the whole.

For sound, a hybrid was chosen over composed stems or fully synthesized audio:
- the engine's own sequencer and synthesizers for bass, pads and stabs
- a sampler playing drum one-shots and sliced breaks, either recorded or synthesized
  offline

## Architecture

### Repository layout

```
src/core/       platform detection, assert, log, arena, time, threads (exists in part)
src/platform/   window + input + audio device: win32/, linux/ (one header, two impls)
src/gfx/        sg device/swapchain ownership, frame loop, render passes, SGL shader loading
src/xr/         OpenXR instance/session/swapchains, views, actions
src/audio/      audio thread, mixer, sampler, synth voices, sequencer, music clock
src/gen/        procedural generators (geometry, palettes, audio one-shots)
src/editor/     ImGui tools; compiled out of shipping builds
game/vespera/   the game: rails, ship, enemies, waves, bullet patterns, levels, music
shaders/        .sgl sources
docs/blog/      post drafts and per-milestone notes
```

`app/` becomes `game/vespera/`. The `vespera` engine library is renamed when we get
there (assistant's call: `vespera_engine`), so the engine and game names don't
collide.

### Frame and time model

- **Main thread:** OS events, input, and the game update at a fixed simulation step.
  It fills a render packet.
- **Render:** sg Vulkan command recording on the main thread at first. It moves to a
  render thread only when profiling says so.
- **Audio thread:** its own high-priority thread. It runs the mixer, sequencer and
  voices. It never locks, never allocates and never waits on the main thread.
  Communication goes through lock-free single-producer, single-consumer queues:
  commands in, events and clock positions out.
- **Music clock:** the audio thread publishes "sample position S at host time T". The
  game converts it to bars and beats for any frame, compensating for output latency.
  Gameplay schedules events at musical positions. The sequencer quantizes
  gameplay-driven changes to the next beat or bar.
- **Views:** the renderer draws a list of views. A flat game has one view and
  VR has two, rendered with multiview. The renderer has no other VR-specific
  knowledge.
- **Input:** an action layer (fire, steer, boost, and so on). Bindings map XR
  controllers, gamepad, keyboard and mouse to the same actions, so flat and VR play
  share all game code.

### Procedural content model

Every asset is either a **recipe** or a **sample**:
- A recipe is a generator id plus a seed plus parameters. It is serialized as data,
  edited live in ImGui, and its output is cached by hash in `blob-cache`.
- A sample is an imported drum break or one-shot, the deliberate exception.

Generators are pure functions, meaning the same recipe always produces the same
output, so caching, hot reload and determinism all come for free.

Planned generators:
- spline-swept corridor and track meshes (cross-section profiles along the rail)
- placement of modular megastructure pieces
- ship and enemy hulls built from primitives by rules
- palettes and neon trim lines
- drum one-shots synthesized offline
- synth patches

## Milestones

Each milestone ends with something you can run, a blog post, and its own detailed
plan written when we reach it.

**Path to the slice, bottom-up:**

- **M0 Foundation.**
  - Integrate shaped-core: FetchContent pinned to a commit, or a git submodule. The
    assistant's call is a submodule, so the source is browsable.
  - Reconcile the overlap with `clean-core`: arena, assert, logger. Read `cc` first,
    then keep a Vespera piece only where it does something `cc` doesn't.
  - Create the repository layout.
  - Write blog post 0, the why.
- **M1 Window and input.**
  - Create a native window on Win32 and xcb.
  - Handle keyboard, mouse and gamepad: XInput on Windows, evdev on Linux.
  - Run a fixed-step main loop.
- **M2 First pixels.**
  - Create an sg Vulkan device and a swapchain on the native window.
  - Clear the screen, then draw a triangle from an SGL shader compiled to SPIR-V.
  - Add shader hot reload.
  - Draw an ImGui overlay through a platform backend you write yourself.
- **M3 Flat slice.**
  - A chase camera behind the ship, on a spline rail.
  - A procedural wireframe landscape: terrain from a heightfield, plus a grid of
    city blocks.
  - Twin-stick steering and aiming through the action layer.
  - One enemy firing a dense bullet pattern.
  - **Bullets:**
    - Stored as structure-of-arrays on the CPU, sized for at least 10,000 live
      bullets.
    - Drawn with one instanced draw call.
    - Collision only against the ship's hitbox; player shots are tested only against
      enemies.
    - Budget: about 1 ms of CPU time per step.
- **M4 Sound and clock.**
  - Audio output: WASAPI on Windows, ALSA on Linux.
  - The audio thread and mixer, plus a sampler playing one drum loop.
  - The music clock, with latency calibration.
  - The enemy fires on the beat.
- **M5 VR slice.**
  - An OpenXR session through SteamVR using `sxr`, with both eyes drawn in one
    view-instanced pass.
  - Twin-stick controls on the VR controllers' thumbsticks.
  - Comfort basics for a chase camera:
    - constant forward speed and smooth rail curves
    - the camera never rolls with the ship, and the horizon stays level
    - a stable frame of reference around the player
  - Played in the Steam Frame. **The vertical slice is complete.**

**Deepening, one system each:**

- **M6 Look:**
  - **wireframe lines that hold still in motion.** Bloom hides jagged edges, but
    not lines thinner than a pixel flickering from frame to frame, and bloom then
    spreads that flicker wider.
    - Minimum fix: give lines at least about 1.5 pixels of width, and fade a line
      by its coverage instead of letting it get thinner.
    - Fuller edge smoothing only if the headset still shows crawling lines after
      that.
  - streaking speed particles, stretched along their velocity
  - emissive neon materials and bloom
  - cheap chrome reflections from a procedural environment map
  - fog and depth cues, MSAA
  - geometry and track lights pulsing on the beat
  - a comfort and photosensitivity setting that limits flash intensity and frequency,
    because strobing in VR is harsher than on a monitor
- **M7 Music:** synth voices (Reese/sub bass, pad, FM stab), the pattern sequencer,
  a sampler with break slicing, reactive layering, and an editor for patterns and
  the mixer.
- **M8 Generators and authoring:** generators for corridors, structures, hulls and
  palettes; the recipe editor; caching.
- **M9 Combat content:** a data language for bullet patterns, wave timelines laid out
  on musical time, enemy types, a boss.
- **M10 Performance:** profiling markers, a job system if profiling demands one, frame
  pacing, and moving the bullet simulation to the GPU if the CPU version breaks its
  1 ms budget.
- **M11 Ship:** Steamworks integration, packaging for Windows and Linux, and the Steam
  page.

## Working model

1. At the start of each milestone the assistant explains the design. That covers
   what the system is responsible for, its data flow and ownership, which threads
   touch it, its performance budget, and how it connects to the existing pieces.
   Together you agree the interfaces.
2. You write the code. The assistant answers questions, reviews the code against the
   design, writes or suggests one runnable check per non-trivial piece, and keeps CI
   green.
3. The assistant writes code only when you ask it to, or for scaffolding you approve
   (build files, CI, boilerplate).
4. Notes collected during the milestone go to `docs/blog/`, and the post is drafted
   from them at the end.

## Calls made by the assistant (owner may override)

- **sg and OpenXR:** settled by reading shaped-core at commit `8c853df`. It already has
  **`shaped-xr` (`sxr`)**:
  - `sxr::system` creates the OpenXR instance, and `system->create_context()`
    creates the sg context on the runtime's GPU, to the runtime's spec.
  - On Vulkan, sg renders into the runtime's own images.
  - Both eyes are drawn in one pass with view instancing.
  - Actions and poses come in tg's coordinate frame.

  It depends on shaped-rendering, whose SDL3 window backend is optional, so Vespera
  can use `sxr` without SDL3. M5 uses `sxr` and doesn't write OpenXR bindings from
  scratch. Consequence for M2: the renderer takes an `sg::context` from outside
  (from `sg` directly when flat, from `sxr` in VR), and never creates one itself.
- **Native windows in sg:** settled. `sg::native_window` accepts `win32` (HWND) and
  `xcb` (connection plus window id) directly.
- **VR platform for the slice:** Windows. Linux is flat-only until SteamVR streaming
  to Steam Frame is verified on Linux hardware.
- **Time:**
  - The simulation runs at a fixed step.
  - Each step's musical position comes from the audio clock, corrected for
    output latency.
  - In VR, the render interpolates to OpenXR's predicted display time.
  - With no audio device, a silent clock that advances with system time replaces
    the audio clock.
  - Pausing freezes the music clock and the simulation together.
  - Randomness is seeded and update order is fixed, so a run replays the same on one
    machine.
  - Bit-identical results across platforms are a non-goal, and so are replays and
    leaderboards, until M11 asks for them.
- **Generators run at load time on the player's machine** and are cached in
  `blob-cache`. Their output doesn't need to be bit-identical across platforms. Caches
  are per platform. `src/gen` ships in the game; `src/editor` doesn't.
- **ARM64 CI** builds and tests natively on `ubuntu-26.04-arm`. Once hardware
  backends exist, the ARM64 job may be limited to the code without hardware
  dependencies (`src/core`, `src/gen`), until a game targets the headset natively.
- **Editor in VR:** no. While in VR, a desktop mirror window shows the game view and
  the ImGui tools. A floating ImGui panel inside the headset is a later option.

## Left to the builder (decided per milestone)

- Exact file formats for recipes and levels. Text-based, so diffs stay reviewable.
- Whether to add a Wayland backend later. xcb works on X11 and on Wayland through
  XWayland, so it's enough for now.
- The blog's hosting and format.
