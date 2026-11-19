# The Vespera project

A long time ago I was working on a step-by-step walkthrough of making a custom game engine, both to explain in detail the steps involved and to clarify the architecture to myself. This is a continuation of that work, where I return to diving into all the steps involved in making a fully custom game engine, with corresponding high performance targets.

Recently I've acquired a Steam Frame VR headset, which is a good target platform to keep in mind - VR has very tight constraints on the frame rate. I still would like to have pretty shiny graphics, so I'm going to target it via the streaming wifi adapter from a desktop computer. I'm treating this the way a solo developer would, which mostly has consequences for what the asset pipeline looks like - heavily lean on procedural generation, but I'd still like to be able to be in control and author stuff as needed. For VR the guideline is to achieve a framerate of 90 fps, which is challenging.

The theme of control also applies to the library stack we'll use - keep a pretty minimal list of dependencies, although I would like to be relatively flexible in what platform to target. Recently I've become aware of the shaped-core libraries and its new SGL shading language and this is a good opportunity to become acquainted with it. At the very least I'd like to target Windows and Linux platforms, which leads us to the following set of dependencies:
- shaped-core (which includes SGL, Dear ImGui and OpenXR support)
- OS API's  (for window, input and audio)

Everything else will be implemented by hand, which should be very informative. I'm going to use C++23 with all of the latest tricks that enables, which should be interesting and educational.

As for the game that will be developed on top of this engine - it's going to be a VR-first bullet hell. A kind of mix between Tempest, StarFox, Wipeout, Thumper and Returnal. It's something I haven't really seen before, and sounds pretty fun to me. Tempest has some excellent aesthetics that could use a more modern take, and I've always enjoyed the general atmosphere of Wipeout. Importantly, the models involved are relatively simple so we don't have to implement character animation and such. From StarFox I'd like to borrow the concept of an on-rails shooter. Thumper pioneered a kind of rhythmic pulsing gameplay that I'd like to try and replicate, with a sense of speed that was an excellent experience in VR. Finally the bullets as used in Returnal should make for an interesting challenge both in game design and in programming. I'm not entirely sure where this will end up, but it feels like this could be something good.

## Goals
- Vespera should be playable in a Steam Frame via SteamVR streaming, from a Windows or Linux PC using OpenXR
- It should also be playable without VR, on a monitor with a gamepad, or keyboard+mouse
- Both Windows and Linux are treated as equal first-class platforms. Both are built and tested in CI.
- In VR we should have a steady 90 Hz framerate (11.1 ms per frame), configurable upward. Dropped frames are comfort bugs.
- Music time is used as the game master clock - gameplay events land on musical positions and gameplay feeds back into the music.
- We'll make use of procedural generators for level geometry, ships, enemies, palettes, drum one-shots and synth sounds. Each is deterministic from a seed plus parameters, and live-tunable in an in-engine editor.
- Every milestone is reported on as a blog post

## Non-goals
- We're not aiming to run natively on the headset itself
- D3D12, Metal, WebGPU, macOS, mobile rendering targets are not in scope
- We don't need a general-purpose scene editor or imported-art pipeline. Maybe we'll need a glTF importer
- We're not doing multiplayer, no networking

## Anti-goals
- We'll only develop engine features that are actually used (YAGNI)
- We're not making abstraction layers with just one implementation.
- Frame-time spikes or judder is unacceptable in VR. Any system that can't hold the frame budget is redesigned.
- The engine should not be aware of the game itself. Game specifics live in the game target
- The codebase must always remain fully explainable without an AI assistant

## Constraints
- The main programming language is C++23
- The build system is CMake 3.28 or higher
- Dependencies are kept minimal:
  - **shaped-core** (MIT) as a foundational library
  - **Vulkan SDK / loader**
  - **OS APIs** for the window, input and audio
    - Windows: Win32, Raw Input, XInput, WASAPI
    - Linux: X11 through xcb, evdev, ALSA
- Minimum CPU target: x86-64-v3 (which enables AVX2 instructions)

At this point we have a tech stack, a set of platform requirements and a rough game outline. Now the overall approach - I'd like to have a clean layered architecture for the game engine itself, but at the same time I'd like to have something playable as soon as possible. That's the approach - build minimal layers to achieve a vertical game slice, then widen as needed to improve the final product. I'm going to use AI models mostly to check my work and verify that it's going in the right direction, but all of the C++ code will be hand-written so that I deeply understand everything that was written; I've delegated the build plumbing and CI scripts.

With that we make a roadmap:
- M0 Foundation
  - Create repository layout
  - Set up the vespera_engine as a static library, with an executable game vespera
  - Set up unit tests, CI workflow
  - Bring in shaped-core as a submodule
  - Make sure to have windows and linux platform layers
  - Provide initial basis: arena allocator, engine entry point, initialization/shutdown, assertion, logging
- M1 Window and input
  - Create a native window on Win32 and xcb
  - Handle keyboard, mouse and gamepad: XInput on windows, evdev on linux
  - Run a fixed-step main loop
- M2 Pixels on the screen
  - Create a vulkan device and swapchain on the main window
  - Clear the screen, draw a triangle using an SGL shader compiled to SPIR-V
  - Set up shader hot-reloading
  - Draw an imgui overlay through a custom platform backend
- M3 Flat slice
  - Make a chase camera that follows the ship
  - Make a sline rail that guides the level
  - Set up a procedural wireframe landscape ~ terrain from heightfield plus a grid of city blocks
  - Enable twin-stick steering and aiming through an action layer
  - Make a single enemy that fires a dense bullet pattern
    - Store the bullets in a structure-of-arrays for at least 10k live bullets
    - Draw all bullets in a single instanced draw call
    - Perform collision detection against the ships' hitbox
    - Player shots are tested only against enemies
    - Bullet processing should have a timing budget of around 1 ms
- M4 Sound and clock
  - Set up audio output: WASAPI on Windows, ALSA on Linux
  - Create an audio thread and mixer
  - Create a sampler playing one drum loop
  - Set up a music clock, with latency calibration
  - Make the enemy fire on the beat
- M5 The VR slice
  - Start an OpenXR session through SteamVR
  - Draw both eyes in a single view-instanced pass
  - Map the twinstick controls onto the frame controllers' thumbsticks
  - Work through comfort basics for a chase cam
  - Make sure the vertical slice is fully playable in the Steam Frame
- M6 Deepening: graphics
- M7 Deepening: sound
- M8 Deepening: authoring
- M9 Deepening: gameplay
- M10 Deepening: performance
- M11 Polish and shipping

Enjoy the ride, we're going to have some fun here :)